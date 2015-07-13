
#include "media_buf.h"
#include "media_pool.h"
#include "mediabuf_debug.h"

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

typedef struct _MEDIABUF_USER_ATTR
{
	int pool_ch;
	lpMEDIA_POOL_USER pool_user;
}stMEDIABUF_USER_ATTR, *lpMEDIABUF_USER_ATTR;

#define MEDIABUF_MAX_POOL_REF (256)
typedef struct MEDIA_BUF
{
	uint32_t n_pools;
	lpMEDIA_POOL pools[MEDIABUF_MAX_POOL_REF];
}stMEDIA_BUF, *lpMEDIA_BUF;
static stMEDIA_BUF _media_buf = {
	.n_pools = 0,
};
static lpMEDIA_BUF _p_media_buf = NULL;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

int MEDIABUF_init()
{
	if(!_p_media_buf){
		int i = 0;
		// alloc the handle
		_p_media_buf = &_media_buf;
		// alloc all the pools
		_p_media_buf->n_pools = 0;
		// clear the handler
		for(i = 0; i < MEDIABUF_MAX_POOL_REF; ++i){
			_p_media_buf->pools[i] = NULL;
		}
		// success
		MEDIABUF_TRACE("media buf init success");
		return 0;
	}
	return -1;
}

void MEDIABUF_destroy()
{
	if(_p_media_buf){
		int i = 0;
		for(i = 0; i < MEDIABUF_MAX_POOL_REF; ++i){
			if(_p_media_buf->pools[i]){
				MEDIAPOOL_release(_p_media_buf->pools[i]);
				_p_media_buf->pools[i] = NULL;
			}
		}
		_p_media_buf = NULL; // very important
	}
}

int MEDIABUF_new(size_t mem_size, const char* preferred_name, const char* alternate_name,
	int entry_available, int key_entry_available, int user_available)
{
	if(_p_media_buf->n_pools < MEDIABUF_MAX_POOL_REF){
		_p_media_buf->pools[_p_media_buf->n_pools] = MEDIAPOOL_create(mem_size, preferred_name, alternate_name,
			entry_available, key_entry_available, user_available);
		if(_p_media_buf->pools[_p_media_buf->n_pools]){
			_p_media_buf->n_pools++;
			return 0;
		}
	}
	return -1;
}

void MEDIABUF_hock_entry_in(int id, fMEDIA_BUF_ON_ENTRY_IN on_entry_in)
{
	if(id < _p_media_buf->n_pools){
		MEDIAPOOL_hock_entry_in(_p_media_buf->pools[id], on_entry_in);
	}
}

void MEDIABUF_hock_entry_out(int id, fMEDIA_BUF_ON_ENTRY_OUT on_entry_out)
{
	if(id < _p_media_buf->n_pools){
		MEDIAPOOL_hock_entry_out(_p_media_buf->pools[id], on_entry_out);
	}
}

void MEDIABUF_hock_entry_clear(int id, fMEDIA_BUF_ON_ENTRY_CLEAR on_entry_clear)
{
	if(id < _p_media_buf->n_pools){
		MEDIAPOOL_hock_entry_clear(_p_media_buf->pools[id], on_entry_clear);
	}
}

void MEDIABUF_hock_user_changed(int id, fMEDIA_BUF_ON_USER_CHANGED on_user_changed)
{
	if(id < _p_media_buf->n_pools){
		MEDIAPOOL_hock_user_changed(_p_media_buf->pools[id], on_user_changed);
	}
}

void MEDIABUF_hock_pool_exhausted(int id, fMEDIA_BUF_ON_POOL_EXHAUSTED on_pool_exhausted)
{
	if(id < _p_media_buf->n_pools){
		MEDIAPOOL_hock_pool_exhausted(_p_media_buf->pools[id], on_pool_exhausted);
	}
}

lpMEDIABUF_USER MEDIABUF_attach(int id)
{
	if(id < _p_media_buf->n_pools){
		lpMEDIA_POOL_USER const pool_user = MEDIAPOOL_user_attach(_p_media_buf->pools[id]);
		if(pool_user){
			lpMEDIABUF_USER const user = calloc(sizeof(stMEDIABUF_USER) + sizeof(stMEDIABUF_USER_ATTR), 1);
			lpMEDIABUF_USER_ATTR const user_attr = (lpMEDIABUF_USER_ATTR)(user + 1);
			user->forbidden_zero = 0;
			user_attr->pool_ch = id;
			user_attr->pool_user = pool_user;
			return user;
		}
	}
	return NULL;	
}

void MEDIABUF_detach(lpMEDIABUF_USER const user)
{
	if(user){
		lpMEDIABUF_USER_ATTR const user_attr = (lpMEDIABUF_USER_ATTR)(user + 1);
		MEDIAPOOL_user_detach(user_attr->pool_user);
		free(user);
	}
}

int MEDIABUF_sync(lpMEDIABUF_USER const user)
{
	lpMEDIABUF_USER_ATTR const user_attr = (lpMEDIABUF_USER_ATTR)(user + 1);
	return MEDIAPOOL_user_sync(user_attr->pool_user);
}

int MEDIABUF_in_request(int id, size_t data_sz, uint32_t key_flag)
{
	#if 1
	static int frame_interval[MEDIABUF_MAX_POOL_REF] = {0};	
	static int frame_interval_max[MEDIABUF_MAX_POOL_REF] = {0};	
	if(key_flag)
	{
		if(frame_interval_max[id] < frame_interval[id])
		{
			frame_interval_max[id] = frame_interval[id];
			MEDIABUF_TRACE("id=%d, i-frame interval=%d", id, frame_interval_max[id]);
		}
		frame_interval[id] = 0;
	}
	else
		frame_interval[id]++;
	#endif
		
	if(id < _p_media_buf->n_pools){
		return MEDIAPOOL_in_request(_p_media_buf->pools[id], data_sz, key_flag, false);
	}
	return -1;
}

int MEDIABUF_in_append(int id, const void* data, size_t data_sz)
{
	if(id < _p_media_buf->n_pools){
		return MEDIAPOOL_in_append(_p_media_buf->pools[id], data, data_sz);
	}
	return -1;
}

int MEDIABUF_in_commit(int id)
{
	if(id < _p_media_buf->n_pools){
		return MEDIAPOOL_in_commit(_p_media_buf->pools[id]);
	}
	return -1;
}

int MEDIABUF_in(int id, const void* data, size_t data_sz, uint32_t key_flag)
{
	if(id < _p_media_buf->n_pools){
		return MEDIAPOOL_in(_p_media_buf->pools[id], data, data_sz, key_flag, false);
	}
	return -1;
}

int MEDIABUF_empty(int id)
{
	if(id < _p_media_buf->n_pools){
		return MEDIAPOOL_emtpy(_p_media_buf->pools[id]);
	}
	return -1;
}

int MEDIABUF_out(lpMEDIABUF_USER user, void** ret_data_ptr, void* ret_data, size_t* ret_sz)
{
	lpMEDIABUF_USER_ATTR const user_attr = (lpMEDIABUF_USER_ATTR)(user + 1);
	return MEDIAPOOL_out(user_attr->pool_user, ret_data_ptr, ret_data, ret_sz);
}

int MEDIABUF_out_lock(lpMEDIABUF_USER user)
{
	lpMEDIABUF_USER_ATTR const user_attr = (lpMEDIABUF_USER_ATTR)(user + 1);
	return MEDIAPOOL_out_lock(user_attr->pool_user);
}

void MEDIABUF_out_unlock(lpMEDIABUF_USER user)
{
	lpMEDIABUF_USER_ATTR const user_attr = (lpMEDIABUF_USER_ATTR)(user + 1);
	MEDIAPOOL_out_unlock(user_attr->pool_user);
}

int MEDIABUF_lookup_byname(const char* name)
{
	int i = 0;
	// find following the preferred name
	for(i = 0; i < MEDIABUF_MAX_POOL_REF; ++i){
		if(_p_media_buf->pools[i]){
			const char* pool_name = MEDIAPOOL_preferred_name(_p_media_buf->pools[i]);
			if(0 == strcmp(pool_name, name) && strlen(pool_name) == strlen(name)){
				// bingo!!
				return i;
			}
		}
	}
	// find following the alternate name
	for(i = 0; i < MEDIABUF_MAX_POOL_REF; ++i){
		if(_p_media_buf->pools[i]){
			const char* pool_name = MEDIAPOOL_alternate_name(_p_media_buf->pools[i]);
			if(0 == strcmp(pool_name, name) && strlen(pool_name) == strlen(name)){
				// bingo!!
				return i;
			}
		}
	}
	return -1;
}

uint32_t MEDIABUF_in_speed(int id)
{
	if(id < _p_media_buf->n_pools){
		return MEDIAPOOL_in_speed(_p_media_buf->pools[id]);
	}
	return 0;
}

bool MEDIABUF_is_empty(lpMEDIABUF_USER user)
{
	lpMEDIABUF_USER_ATTR const user_attr = (lpMEDIABUF_USER_ATTR)(user + 1);
	return MEDIAPOOL_is_empty(user_attr->pool_user);
}


