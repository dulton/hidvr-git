
#include "spook.h"
#include "spook_debug.h"
#include "generic.h"
#include <sys/syscall.h>
#include <dirent.h>
#include <sys/stat.h>


#define SPOOK_PROBE_BUF_SIZE (1024)
#define SPOOK_MAX_SESSION_ENTRY (256)
#define SPOOK_ADDRESS_LIST_BACKLOG (32)

struct SPOOK_SESSION
{
	bool session_trigger;
	pthread_t session_tid;
	int sock;
	time_t read_pts;
};

struct SPOOK_SESSION_ENTRY
{
	char* name;
	SPOOK_TASK_PROBE_FUNC do_probe;
	SPOOK_TASK_LOOP_FUNC do_loop;
};

struct SPOOK_ADDRESS_LIST
{
	unsigned long s_addr[SPOOK_ADDRESS_LIST_BACKLOG]; 
};

struct SPOOK_SERVER
{
	uint32_t listen_port;
	char note_path[64];

	bool server_trigger;
	pthread_t server_tid;

	uint32_t n_entry;
	struct SPOOK_SESSION_ENTRY entry[SPOOK_MAX_SESSION_ENTRY];
	
	struct SPOOK_SESSION session[SPOOK_MAX_SESSION_ENTRY];

	bool black_list; // if true, the following address list is black list, if false, it's white list
	struct SPOOK_ADDRESS_LIST addr_list;
};
static struct SPOOK_SERVER * _spook_server = NULL;

static void server_sock_set_timeout(int sock, uint32_t timeout_s)
{
	if(timeout_s > 0){
		struct timeval tv;
		tv.tv_sec = timeout_s;
		tv.tv_usec = 0;
		setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	}
}

static int server_sock_init(uint16_t port, int backlog, uint32_t timeout_s)
{
	int ret;
	int sock_server = 0;
	int reuse_on = 1;
	struct sockaddr_in addr;


	// create socket
	sock_server = socket(AF_INET, SOCK_STREAM, 0);
	SPOOK_ASSERT((-1 != sock_server), "socket %s", strerror(errno));

	// port reuse active
	ret = setsockopt(sock_server, SOL_SOCKET, SO_REUSEADDR, &reuse_on, sizeof(int));
	SPOOK_ASSERT((-1 != ret), "setsockopt %s", strerror(errno));

	// bind
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	ret = bind(sock_server, (struct sockaddr *) &addr, sizeof(addr));
	SPOOK_ASSERT((-1 != ret), "bind %s", strerror(errno));

	// listen
	ret = listen(sock_server, backlog);
	SPOOK_ASSERT((-1 != ret), "listen %s", strerror(errno));

	// poll wait time
	server_sock_set_timeout(sock_server, timeout_s);

	return sock_server;
}

static void server_peek_mesg(const char* title, const char* peek_buf, ssize_t peek_buf_size)
{
	int i = 0;
	if(title){
		SPOOK_TRACE("%s :\r\n", title);
	}
	for(i = 0; i <  peek_buf_size; ++i){
		if(isalpha(peek_buf[i])){
			printf("%c", peek_buf[i]);
		}else{
			printf("\\%02x", peek_buf[i]);
		}
	}
}

static void spook_note_create_folder(const char* folder)
{
	if(folder && strlen(folder) > 0){
		char note_file[128] = {""};
		FILE* fid = NULL;
		remove(folder);
		mkdir(folder, 0);
		snprintf(note_file, sizeof(note_file), "%s/0.log", folder);
		fid = fopen(note_file, "w+b");
		if(fid){
			char buf[128] = {""};
			snprintf(buf, sizeof(buf), "|%8s |%8s |%8s |%20s |%8s |\r\n",
				"PID", "TID", "Name", "IP Address", "Port");
			fwrite(buf, 1, strlen(buf), fid);
			fclose(fid);
			fid = NULL;
		}
	}
}

static void spook_note_release_folder(const char* folder)
{
	if(folder && strlen(folder) > 0){
		DIR* dir = NULL;
		struct dirent* entry = NULL;
		struct stat statbuf;
		dir = opendir(folder);
		if(NULL != dir){
			while(NULL != (entry = readdir(dir))){
				if(S_ISDIR(statbuf.st_mode)){
					// is a directory
					if(0 == strcmp(".", entry->d_name) || 0 == strcmp("..", entry->d_name)){
						// skip local path and upper path
						continue;
					}
					SPOOK_ASSERT(0, "Why exist folder \"%s\"", entry->d_name);
				}else{
					char file_note[128] = {""};
					snprintf(file_note, sizeof(file_note), "%s/%s", folder, entry->d_name);
					//SPOOK_TRACE("Deleting \"%s\"", file_note);
					unlink(file_note);
					remove(file_note);
				}
			}
			closedir(dir);
			dir = NULL;
		}
	}
}

static void spook_note(bool flag, int sock, const char* name)
{
	if(strlen(_spook_server->note_path) > 0){
		char note_file[128] = {""};
		uint32_t const this_pid = getpid();
		uint32_t const this_tid = syscall(SYS_gettid);
		snprintf(note_file, sizeof(note_file), "%s/pid%06d_tid%06d_%s.log",
			_spook_server->note_path, this_pid, this_tid, name);
		if(flag){
			FILE* fid = fopen(note_file, "w+b");
			if(fid){
				char buf[128] = {""};
				struct sockaddr_in client_addr;
				socklen_t client_addr_len = sizeof(client_addr);
				getpeername(sock, (struct sockaddr*)&client_addr, &client_addr_len);
				snprintf(buf, sizeof(buf), "|%8d |%8d |%8s |%20s |%8d |\r\n",
					this_pid, this_tid, name, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				fwrite(buf, 1, strlen(buf), fid);
				fclose(fid);
				fid = NULL;
			}
		}else{
			unlink(note_file);
			remove(note_file);
		}
	}
}

static void* server_session_probe(void* arg)
{
	struct SPOOK_SESSION* const session = (struct SPOOK_SESSION*)arg;
	SPOOK_SESSION_PROBE_t probe_ret = SPOOK_PROBE_NULL;
	int session_detect = 0;
	int session_hit = -1;
	void* peek_buf = NULL;
	ssize_t peek_buf_seen = 0;
	int match_retry = 0;

	pthread_detach(pthread_self());
	SPOOK_TRACE("pid=%u tid=%x begin", (uint32_t)getpid(), (uint32_t)pthread_self());
	peek_buf = calloc(SPOOK_PROBE_BUF_SIZE, 1);
	while(session->session_trigger)
	{
		if(SPOOK_PROBE_NULL == probe_ret || SPOOK_PROBE_UNDETERMIN == probe_ret){
			if(peek_buf_seen >= SPOOK_PROBE_BUF_SIZE){
				SPOOK_TRACE("protocol parse failed!");
				break;
			}
			// peek the message and check which session i should choose
			peek_buf_seen = recv(session->sock, peek_buf, SPOOK_PROBE_BUF_SIZE, MSG_PEEK);
			if(peek_buf_seen < 0){
				SPOOK_TRACE("socket-%d error, errno_cpy=%d",session->sock, errno);
				break;
			}
			session->read_pts = time(NULL); //
		}
		// matching protocol
		if(SPOOK_PROBE_NULL == probe_ret || SPOOK_PROBE_UNDETERMIN == probe_ret || SPOOK_PROBE_MISMATCH == probe_ret){
			probe_ret = _spook_server->entry[session_detect].do_probe(peek_buf, peek_buf_seen);
		}
		if(SPOOK_PROBE_UNDETERMIN == probe_ret){
			// wait more msg for confirming
			SPOOK_TRACE("Spook session(pid=0x%x tid=0x%x) is undeterminable, retry %ds", getpid(), (uint32_t)pthread_self(), match_retry);
			if(match_retry < 5){
				// retry again, and try 5 seconds
				++match_retry;
				sleep(1);
				continue;
			}
			break;
		}else if(SPOOK_PROBE_MATCH == probe_ret){
			session_hit = session_detect;
//			SPOOK_TRACE("session protocal hit @ %d", session_hit);
			break;
		}else if(SPOOK_PROBE_MISMATCH == probe_ret){
			// try other protocols
			if(session_detect < _spook_server->n_entry - 1){
				++session_detect;
				match_retry = 0;
				continue;
			}
			// no match protocal found
			server_peek_mesg("Unknown session", peek_buf, peek_buf_seen);
			break;
		}
	}

	// dont use any more
	free(peek_buf);
	peek_buf = NULL;
	peek_buf_seen = 0;
	
	if(session->session_trigger && session_hit >= 0){
		SPOOK_TRACE("session protocal @ %d, name %s", session_hit, _spook_server->entry[session_hit].name);
		spook_note(true, session->sock, _spook_server->entry[session_hit].name);
		// enter relative protocal task
		_spook_server->entry[session_hit].do_loop(&session->session_trigger, session->sock, &session->read_pts);
		spook_note(false, session->sock, _spook_server->entry[session_hit].name);
	}
	SPOOK_TRACE("Spook pid=%u tid=%x end", (uint32_t)getpid(), (uint32_t)pthread_self());
	// exit probe
	close(session->sock);
	session->sock = -1;
	session->session_trigger = false;
	pthread_exit(NULL);
}


static int spook_server_setup_session(int sock)
{
	int i = 0;
	for(i = 0; i < ARRAY_ITEM(_spook_server->session); ++i){
		struct SPOOK_SESSION* const session = _spook_server->session + i;
		if(!session->session_trigger){
			int ret = 0;
			// got a vocation
			session->sock = sock;
			session->read_pts = time(NULL);
			session->session_trigger = true;
			ret = pthread_create(&session->session_tid, NULL, server_session_probe, (void*)session);
			if(ret != 0)
			{
				SPOOK_TRACE("ret=%d", ret);
			}
			SPOOK_ASSERT(0 == ret, "pthread_create");
			return 0;
		}
	}
	return -1;
}

static void server_do_poll(int sock_server)
{
	int ret = 0;
	fd_set read_fds;
	struct timeval poll_wait;

	while(_spook_server->server_trigger)
	{
		FD_ZERO(&read_fds);
		FD_SET(sock_server, &read_fds);
		poll_wait.tv_sec = 2;
		poll_wait.tv_usec = 0;
		ret = select(sock_server + 1, &read_fds, NULL, NULL, &poll_wait);
		SPOOK_ASSERT((-1 != ret), "select %s", strerror(errno));
		if(ret > 0){
			int sock_session = 0;
			if(FD_ISSET(sock_server, &read_fds)){
				struct sockaddr_in client_addr;
				socklen_t client_addr_len = sizeof(client_addr);
				sock_session = accept(sock_server, (struct sockaddr*)&client_addr, &client_addr_len);
				SPOOK_ASSERT((-1 != sock_session), "accept %s", strerror(errno));
				//close(sock_server);
				#if 0
				if(client_addr.sin_addr.s_addr != inet_addr("192.168.1.36")){
					close(sock_session);
					continue;
				}
				#endif
				// check addr_list
				if((_spook_server->black_list && SPOOK_addrlist_check(inet_ntoa(client_addr.sin_addr)))
					|| (!_spook_server->black_list && !SPOOK_addrlist_check(inet_ntoa(client_addr.sin_addr)))){
					// refuse all the black list address
					SPOOK_TRACE("Refuse address %s!", inet_ntoa(client_addr.sin_addr));
					close(sock_session);
					continue;
				}

				// setup a new session
				server_sock_set_timeout(sock_session, 10);
				ret = spook_server_setup_session(sock_session);
				if(0 != ret){
					SPOOK_TRACE("setup a new session failed!");
					close(sock_session);
				}
			}
		}else{
//			SPOOK_TRACE("server alive!!");
		}
	}
}

static void* server_do_loop(void* _arg)
{
	int sock_server = 0;
	// setup server socket
	//sock_server = server_sock_init(SPOOK_SERVER_PORT, 5, 1);
	sock_server = server_sock_init(_spook_server->listen_port, 8, 1);
	server_do_poll(sock_server);
	close(sock_server);
	pthread_exit(NULL);
}

static int server_start(uint32_t port)
{
	if(!_spook_server->server_tid){
		int ret = 0;
		_spook_server->server_trigger = true;
		ret = pthread_create(&_spook_server->server_tid, NULL, server_do_loop, NULL);
		SPOOK_ASSERT(0 == ret, "pthread create err!");
		return 0;
	}
	return -1;
}

static void server_stop()
{
	if(_spook_server->server_tid){
		_spook_server->server_trigger = false;
		pthread_join(_spook_server->server_tid, NULL);
		_spook_server->server_tid = (pthread_t)NULL;
	}
}

int SPOOK_add_session(const char* name, SPOOK_TASK_PROBE_FUNC do_probe, SPOOK_TASK_LOOP_FUNC do_loop)
{
	if(_spook_server->n_entry < SPOOK_MAX_SESSION_ENTRY){
		int i = 0;
		const char* new_name = strdupa(name ?  name :  "uknown name"); // in stack
		// check unique name first
		for(i = 0; i < SPOOK_MAX_SESSION_ENTRY; ++i){
			struct SPOOK_SESSION_ENTRY* const entry = _spook_server->entry + i;
			if(entry->name && 0 == strcmp(entry->name, new_name) && strlen(entry->name) == strlen(new_name)){
				// override
				SPOOK_TRACE("Spook vocation is full (%d)!", SPOOK_MAX_SESSION_ENTRY);
				return -1;
			}
		}
		// add a new seesion
		for(i = 0; i < SPOOK_MAX_SESSION_ENTRY; ++i){
			struct SPOOK_SESSION_ENTRY* const entry = _spook_server->entry + i;
			if(NULL == entry->name){
				// emtpy pisition
				entry->name = strdup(new_name); // in heap
				entry->do_probe = do_probe;
				entry->do_loop = do_loop;
				++_spook_server->n_entry;
				SPOOK_TRACE("\"%s\" got a spook vocation @ %d!", entry->name, i);
				return 0;
			}
		}
		
	}
	return -1;
}

static void remove_session(struct SPOOK_SESSION_ENTRY* const entry)
{
	SPOOK_TRACE("Remove spook entry \"%s\"!", entry->name);
	free(entry->name);
	entry->name = NULL;
	entry->do_probe = NULL;
	entry->do_loop = NULL;
	--_spook_server->n_entry;
}

int SPOOK_remove_session(const char* name)
{
	if(_spook_server->n_entry > 0){
		int i = 0;
		for(i = 0; i < SPOOK_MAX_SESSION_ENTRY; ++i){
			struct SPOOK_SESSION_ENTRY* const entry = _spook_server->entry + i;
			if(name && 0 == strcmp(entry->name, name) && strlen(entry->name) == strlen(name)){
				// check exist
				remove_session(entry);
				return 0;
			}
		}
		SPOOK_TRACE("Spook entry \"%s\" not found!", name);
	}
	return -1;
}

void SPOOK_addrlist_as_black()
{
	_spook_server->black_list = true;
}

void SPOOK_addrlist_as_white()
{
	_spook_server->black_list = false;
}


bool SPOOK_addrlist_check(const char* addr)
{
	int i = 0;
	unsigned long const s_addr = inet_addr(addr);
	if(0 != s_addr){
		for(i = 0; i < ARRAY_ITEM(_spook_server->addr_list.s_addr); ++i){
			if(_spook_server->addr_list.s_addr[i] == s_addr){
				return true;
			}
		}
	}
	return false;
}

int SPOOK_addrlist_add(const char* addr)
{
	int i = 0;
	unsigned long const s_addr = inet_addr(addr);
	if(0 != s_addr){
		if(SPOOK_addrlist_check(addr)){
			SPOOK_TRACE("Address %s has existed in address list", addr);
		}else{
			for(i = 0; i < ARRAY_ITEM(_spook_server->addr_list.s_addr); ++i){
				if(0 == _spook_server->addr_list.s_addr[i]){
					SPOOK_TRACE("Add %s to address list", addr);
					_spook_server->addr_list.s_addr[i] = s_addr;
					return 0;
				}
			}
		}
	}
	return -1;
}

void SPOOK_addrlist_remove(const char* addr)
{
	int i = 0;
	unsigned long const s_addr = inet_addr(addr);
	if(0 != s_addr){
		for(i = 0; i < ARRAY_ITEM(_spook_server->addr_list.s_addr); ++i){
			if(_spook_server->addr_list.s_addr[i] == s_addr){
				SPOOK_TRACE("Remove %s from address list", addr);
				_spook_server->addr_list.s_addr[i] = 0;
				return;
			}
		}
	}
}

void SPOOK_addrlist_clear()
{
	STRUCT_ZERO(_spook_server->addr_list);
}

static void spook_init(uint32_t port)
{
	if(!_spook_server){
		// alloc space
		struct SPOOK_SERVER* const server = calloc(sizeof(struct SPOOK_SERVER), 1);
		server->listen_port = port;
		server->server_trigger = false;
		server->server_tid = (pthread_t)NULL;
		_spook_server = server;
		SPOOK_addrlist_as_black();
		SPOOK_addrlist_clear();
	}
}

static void spook_destroy()
{
	if(_spook_server){
		void* const tmp_server = (void*)_spook_server;
		_spook_server = NULL;
		free(tmp_server);
	}
}

void SPOOK_server_reload(uint32_t port)
{
	server_stop();
	_spook_server->listen_port = port;
	server_start(port);
}

int SPOOK_init(uint32_t port, const char *note_path)
{
	if(!_spook_server){
		spook_init(port);
		server_start(port);
		if(note_path){
			snprintf(_spook_server->note_path, sizeof(_spook_server->note_path), "%s%c", note_path, '\0');
			SPOOK_TRACE("Spook note path \"%s\"", _spook_server->note_path);
		}else{
			memset(_spook_server->note_path, 0, sizeof(_spook_server->note_path));
		}
		spook_note_release_folder(_spook_server->note_path);
		spook_note_create_folder(_spook_server->note_path);
		return 0;
	}
	return -1;
}

void SPOOK_destroy()
{
	if(_spook_server){
		spook_note_release_folder(_spook_server->note_path);
		memset(_spook_server->note_path, 0, sizeof(_spook_server->note_path));
		server_stop();
		do{
			int i = 0;
			for(i = 0; i < SPOOK_MAX_SESSION_ENTRY; ++i){
				// clear all the session entry
				remove_session(_spook_server->entry + i);
			}
		}while(0);
		spook_destroy();
	}
}

in_port_t SPOOK_listen_port()
{
	return _spook_server->listen_port;
}

int SPOOK_genVconAppAddr(struct sockaddr *serviceaddr)
{
	struct sockaddr_in spookaddr;
	spookaddr.sin_family = AF_INET;
	spookaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	spookaddr.sin_port = htons(_spook_server->listen_port);
	if (NULL != serviceaddr) {
		memcpy(serviceaddr, &spookaddr, sizeof(spookaddr));
		return 0;
	}
	SPOOK_TRACE("VconService_spook null addr\n");
	return -1;
}

