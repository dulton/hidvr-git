
#include "media_pool.h"
#include "mediabuf_debug.h"


// user interface
static stMEDIA_POOL_USER* user_create()
{
	stMEDIA_POOL_USER* const user = calloc(sizeof(stMEDIA_POOL_USER), 1);
	user->pool = NULL;
	user->entry_rd = 0;
	user->data_rd_id = 0;
	user->lock = NULL;
	return user;
}

static void user_release(stMEDIA_POOL_USER* const user)
{
	free(user);
}

static stMEDIA_POOL_USER* user_find_vacancy(lpMEDIA_POOL const pool)
{
	int i = 0;
	for(i = 0; i < pool->user_available; ++i){
		lpMEDIA_POOL_USER const user = pool->users[i];
		if(NULL == user->pool){
			user->pool = (void*)pool;
			user->entry_rd = pool->entry_key_history[0];
			user->data_rd_id = 0;
			user->lock = NULL;
			return user;
		}
	}
	return NULL;
}

static int user_attached_amount(lpMEDIA_POOL const pool)
{
	int i = 0, user_attached = 0;
	for(i = 0; i < pool->user_available; ++i){
		lpMEDIA_POOL_USER const user = pool->users[i];
		if(NULL != user->pool){
			++user_attached;
		}
	}
	return user_attached;
}

static void user_operate_enter_critical(lpMEDIA_POOL const pool)
{
	pool->user_op_lock->wrlock(pool->user_op_lock);
}

static void user_operate_leave_critical(lpMEDIA_POOL const pool)
{
	pool->user_op_lock->unlock(pool->user_op_lock);
}

lpMEDIA_POOL_USER MEDIAPOOL_user_attach(lpMEDIA_POOL const pool)
{
	user_operate_enter_critical(pool);
	stMEDIA_POOL_USER* const user = user_find_vacancy(pool);
	if(!user){
		MEDIABUF_TRACE("Media Pool(%s) is crowded(%d/%d)",
			pool->preferred_name, user_attached_amount(pool), pool->user_available);
	}else{
		user->timestamp = pool->timestamp;
	}
	if(pool->on_user_changed){
		pool->on_user_changed(pool->preferred_name, user_attached_amount(pool), pool->user_available);
	}
	user_operate_leave_critical(pool);
	return user;
}

void MEDIAPOOL_user_detach(lpMEDIA_POOL_USER const user)
{
	lpMEDIA_POOL const pool = user->pool;
	user_operate_enter_critical(pool);
	// clear this user
	user->pool = NULL;
	if(pool->on_user_changed){
		pool->on_user_changed(pool->preferred_name, user_attached_amount(pool), pool->user_available);
	}
	user_operate_leave_critical(pool);
}

int MEDIAPOOL_user_sync(lpMEDIA_POOL_USER const user)
{
	lpMEDIA_POOL const pool = user->pool;
	if(pool->timestamp > user->timestamp){
		user->timestamp = pool->timestamp;
		user->entry_rd = pool->entry_key_history[0];
		return 0;
	}
	return -1;
}

static void default_on_entry_in(const char *name, const void *entry_data, size_t entry_size, bool key_flag)
{
	//if(key_flag){
	//	MEDIABUF_TRACE("Media Pool(%s) in entry size(%d)", name, entry_size);
	//}
}

static void default_on_entry_out(const char *name, const void *entry_data, size_t entry_size, bool key_flag)
{
	//MEDIABUF_TRACE("Media Pool(%s) out entry size(%d)", name, entry_size);
}

static void default_on_entry_clear(const char *name, const void *entry_data, size_t entry_size, bool key_flag)
{
	//MEDIABUF_TRACE("Media Pool(%s) clear entry size(%d)", name, entry_size);
}

static void default_on_user_changed(const char *name, uint32_t user_number, uint32_t user_max)
{
	MEDIABUF_TRACE("Media Pool(%s) user changed(%d/%d)", name, user_number, user_max);
}

static void default_on_pool_exhausted(const char *name)
{
	MEDIABUF_TRACE("Media Pool(%s) emptying", name);
}

lpMEDIA_POOL MEDIAPOOL_create(size_t mem_size, const char *preferred_name, const char *alternate_name, 
	uint32_t entry_available, uint32_t entry_key_available, uint32_t user_available)
{
	int i = 0;
	///////////////////////////
	// create a pool
	lpMEDIA_POOL const pool = calloc(sizeof(stMEDIA_POOL), 1);

	if(mem_size > 0){
		pool->mem_size = mem_size;
		pool->mem_space = create_mspace_with_base(calloc(pool->mem_size, 1), mem_size, 1);
		pool->calloc = mspace_calloc;
		pool->malloc = mspace_malloc;
		pool->realloc = mspace_realloc;
		pool->free = mspace_free;
	}else{
		pool->mem_size = 0;
		pool->mem_space = NULL;
		pool->calloc = calloc;
		pool->malloc = malloc;
		pool->realloc = realloc;
		pool->free = free;
	}
	
	///////////////////////////
	// parameters
	if(preferred_name){
		MEDIABUF_ASSERT(strlen(preferred_name) < sizeof(pool->preferred_name), "Media Pool name is too long");
		strcpy(pool->preferred_name, preferred_name);
	}
	if(alternate_name){
		MEDIABUF_ASSERT(strlen(alternate_name) < sizeof(pool->alternate_name), "Media Pool name is too long");
		strcpy(pool->alternate_name, alternate_name);
	}

	// full flag default clear
	pool->full_flag = false;

	pool->dataflow_flag = false;
	pool->data_counter = 0;

	// some paramenter
	pool->entry_available = entry_available;
	pool->entry_available += (rand() % 5) * 2 + 1; // make the number somewhat special
	pool->entry_key_available = entry_key_available;
	pool->user_available = user_available;
	pool->entry_in_speed = 50000;

	// interval stats
	pool->entry_key_interval_min = 0;
	pool->entry_key_interval_max = 0;
	
	// callback
	#if 0
	MEDIAPOOL_hock_entry_in(pool, NULL);
	MEDIAPOOL_hock_entry_out(pool, NULL);
	MEDIAPOOL_hock_entry_clear(pool, NULL);
	MEDIAPOOL_hock_user_changed(pool, NULL);
	#else
	MEDIAPOOL_hock_entry_in(pool, default_on_entry_in);
	MEDIAPOOL_hock_entry_out(pool, default_on_entry_out);
	MEDIAPOOL_hock_entry_clear(pool, default_on_entry_clear);
	MEDIAPOOL_hock_user_changed(pool, default_on_user_changed);
	MEDIAPOOL_hock_pool_exhausted(pool, default_on_pool_exhausted);
	#endif
	
	// check the parameters
	MEDIABUF_ASSERT(0 != pool->entry_available, "Media Pool capacity depands the entry available");
	MEDIABUF_ASSERT(pool->entry_key_available < pool->entry_available, "Media Pool key entries should be less than the entry available");
	if(0 == pool->user_available){
		pool->user_available = 1;
	}
	///////////////////////////
	// create the entires
	pool->entries = calloc(sizeof(stMEDIA_ENTRY*), pool->entry_available);
	for(i = 0; i < pool->entry_available; ++i){
		pool->entries[i] = MEDIAENTRY_create();
	}
	pool->entry_in = 0;
	pool->entry_key_history = calloc(sizeof(int), pool->entry_key_available);
	for(i = 0; i < pool->entry_key_available; ++i){
		pool->entry_key_history[i] = 0;
	}
	
	// entries
	pool->entry_in_ctrl = calloc(sizeof(stMEDIA_POOL_IN_CTRL), 1);
	pool->entry_in_ctrl->data_size = 0;
	pool->entry_in_ctrl->in_size = 0;
	pool->entry_in_ctrl->ptr = NULL;
	pool->entry_in_ctrl->lock = RWLOCK_create();
	pool->entry_counter = 0; // counter how many entries have been input
	pool->timestamp = 0;
	// create the users
	pool->users = calloc(sizeof(stMEDIA_POOL_USER*), pool->user_available);
	for(i = 0; i < pool->user_available; ++i){
		pool->users[i] = user_create();
	}
	pool->user_op_lock = RWLOCK_create();
	///////////////////////////
	MEDIABUF_TRACE("create media buf %s", pool->preferred_name);
	return pool;
}

void MEDIAPOOL_hock_entry_in(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_ENTRY_IN on_entry_in)
{
	pool->on_entry_in = on_entry_in;
}

void MEDIAPOOL_hock_entry_out(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_ENTRY_OUT on_entry_out)
{
	pool->on_entry_out = on_entry_out;
}

void MEDIAPOOL_hock_entry_clear(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_ENTRY_CLEAR on_entry_clear)
{
	pool->on_entry_clear = on_entry_clear;
}

void MEDIAPOOL_hock_user_changed(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_USER_CHANGED on_user_changed)
{
	pool->on_user_changed = on_user_changed;
}

void MEDIAPOOL_hock_pool_exhausted(lpMEDIA_POOL const pool, fMEDIA_POOL_ON_POOL_EXHAUSTED on_pool_exhausted)
{
	pool->on_pool_exhausted = on_pool_exhausted;
}

void MEDIAPOOL_release(lpMEDIA_POOL const pool)
{
	if(pool){
		int i = 0;
		// destroy the users
		RWLOCK_release(pool->user_op_lock);
		pool->user_op_lock = NULL;
		for(i = 0; i < pool->user_available; ++i){
			user_release(pool->users[i]);
		}
		free(pool->users);
		// destroy the entires
		RWLOCK_release(pool->entry_in_ctrl->lock);
		free(pool->entry_in_ctrl);
		for(i = 0; i < pool->entry_available; ++i){
			MEDIAENTRY_destroy(pool->entries[i]);
		}
		free(pool->entries);
		//
		if(pool->mem_space){
			destroy_mspace(pool->mem_space);
			pool->mem_space = NULL;
			pool->mem_size = 0;
		}
		
		///////////////////////////
		// destroy the pool
		free(pool);
	}
}

uint32_t MEDIA_POOL_threadsafe(lpMEDIA_POOL const pool)
{
	return false;
}


int MEDIAPOOL_in_request(lpMEDIA_POOL const pool, size_t data_size, bool key_flag, bool wait_block)
{
	int ret = 0;
	int const entry_in = pool->entry_in;
	lpMEDIA_ENTRY const entry_in_p = pool->entries[entry_in];
	lpRW_LOCK const entry_in_lock = entry_in_p->rwlock;
	time_t tmp_time = 0;

	if(!data_size){
		return -1;
	}
	if(pool->full_flag){
		MEDIABUF_TRACE("Media Pool(%s) is full", pool->preferred_name);
		return -1;
	}

	if(MEDIA_POOL_threadsafe(pool)){
		// if thread safe
		// meida pool in interface could be called by serval threads
		// mutex control by pool::entry_in_ctrl::lock
		if(wait_block){
			RWLOCK_WRLOCK(pool->entry_in_ctrl->lock);
		}else{
			if(0 != RWLOCK_TRYWRLOCK(pool->entry_in_ctrl->lock)){
				return -1;
			}
		}
	}else if(NULL != pool->entry_in_ctrl->ptr){
		// some body is inputing data
		MEDIABUF_TRACE("Media Pool(%s) is inputting(%d)", pool->preferred_name, entry_in);
		MEDIAPOOL_rw_status(pool);
		return -1;
	}else if(entry_in_p->data_size != 0)
	{
		tmp_time = time(NULL);
		pool->discard_count++;
		#if 1
		if(abs(tmp_time - pool->full_last_time) > 60)
		{
			MEDIABUF_TRACE("Media Pool(%s) in(%d-%d) full, discard frame(%d)!", 
				pool->preferred_name, entry_in, *(pool->entry_key_history), pool->discard_count);
			//pool->discard_count = 0;
			pool->full_last_time = tmp_time;
		}
		#endif
		return -1;
	}


	if(pool->discard_count != 0)
	{
		MEDIABUF_TRACE("Media Pool(%s) in(%d-%d) restore, discard frame(%d)!", 
			pool->preferred_name, entry_in, *(pool->entry_key_history), pool->discard_count);
		pool->discard_count = 0;
	}
	// excluse the current entry
	ret = RWLOCK_WRLOCK(entry_in_lock);
	//MEDIABUF_ASSERT(0 == ret, "Media Pool(%s) lock failed, please check you have unlocked last time", pool->preferred_name);
	if( ret != 0)
	{
		MEDIABUF_TRACE("Media Pool(%s) lock failed, please check you have unlocked last time", pool->preferred_name);
		return -1;
	}

	// mark down the attribute
	entry_in_p->key_flag = key_flag;
	//MEDIABUF_ASSERT(0 == entry_in_p->data_size, "entry data got data = %d @ %d", entry_in_p->data_size, entry_in);
	if( 0 != entry_in_p->data_size )
	{
		MEDIABUF_TRACE("entry data got data = %d @ %d", entry_in_p->data_size, entry_in);
		return -1;
	}
	entry_in_p->data_size = data_size;
	//MEDIABUF_ASSERT(NULL == entry_in_p->data, "entry data not empty");
	if(entry_in_p->data)
	{
		MEDIABUF_TRACE(NULL == entry_in_p->data, "entry data not empty");
		return -1;
	}
	entry_in_p->data = calloc(data_size, 1);
	if(!entry_in_p->data)
	{
		MEDIABUF_TRACE("Media Pool calloc fail(%s, %d)", pool->preferred_name, data_size);
		ret = RWLOCK_UNLOCK(entry_in_lock);
		if( ret != 0 )
			MEDIABUF_TRACE("Media Pool(%s) input unlock(%d) failed", pool->preferred_name, entry_in);
		return -1;
	}
	pool->entry_in_ctrl->data_size = entry_in_p->data_size;
	pool->entry_in_ctrl->in_size = 0;
	pool->entry_in_ctrl->ptr = entry_in_p->data; // pointing to the beginning of the data

	// what's next is starting to store data
	return 0;
}

int MEDIAPOOL_in_append(lpMEDIA_POOL const pool, const void *data_piece, size_t data_piece_data)
{
	lpMEDIA_POOL_IN_CTRL const pool_in_ctrl = pool->entry_in_ctrl;
	MEDIABUF_ASSERT(pool_in_ctrl->ptr, "Media Pool(%s) never requested", pool->preferred_name);
	if(data_piece_data > 0 && NULL != data_piece){
		size_t raise_to_size = pool_in_ctrl->in_size + data_piece_data;
		// check the suitable append
		MEDIABUF_ASSERT(raise_to_size <= pool_in_ctrl->data_size, "Media Pool(%s) overload(%d/%d)",
			pool->preferred_name, raise_to_size, pool_in_ctrl->data_size);
		memcpy(pool_in_ctrl->ptr, data_piece, data_piece_data);
		pool_in_ctrl->ptr += data_piece_data;
		pool_in_ctrl->in_size += data_piece_data;
		return 0;
	}
	return -1;
}

int MEDIAPOOL_in_commit(lpMEDIA_POOL const pool)
{
	int i = 0;
	int ret = 0;
	int const entry_in = pool->entry_in;
	lpMEDIA_ENTRY const entry_in_p = pool->entries[entry_in];
	lpRW_LOCK const entry_in_lock = entry_in_p->rwlock;
	lpMEDIA_POOL_IN_CTRL const pool_in_ctrl = pool->entry_in_ctrl;

	MEDIABUF_ASSERT(pool_in_ctrl->in_size == pool_in_ctrl->data_size,
		"Media Pool(%s) commit error! The size is mismatch(%d/%d)",
		pool->preferred_name, pool_in_ctrl->in_size, pool_in_ctrl->data_size);

	// entry counter increase
	pool->entry_counter++;

	if(entry_in_p->key_flag){
		
		struct timeval tv_now;
		uint64_t timestamp_now = 0;
		int timestamp_diff = 0;
		
		// get the timestamp
		gettimeofday(&tv_now, NULL);
		timestamp_now = tv_now.tv_sec;
		timestamp_now *= 1000000;
		timestamp_now += tv_now.tv_usec;
		// stats average speed
		timestamp_diff = timestamp_now - pool->timestamp;
		if(timestamp_diff > 0){
			uint32_t entry_in_speed = (uint32_t)(timestamp_diff / pool->entry_counter);
			if(entry_in_speed > 1000000){
				entry_in_speed = 1000000;
			}
			pool->entry_in_speed = entry_in_speed;
		}
		// counter reset
		pool->entry_counter = 0;
		// update the timestamp
		pool->timestamp = timestamp_now;
		///////////////////////////////////////////////////////////////
		// dump the interval
		if(1){
			int entry_key_interval = 0;
			if(pool->entry_in < pool->entry_key_history[0]){
				entry_key_interval = pool->entry_in + pool->entry_available - pool->entry_key_history[0];
			}else{
				entry_key_interval = pool->entry_in - pool->entry_key_history[0];
			}
			// stats
			if(0 == pool->entry_key_interval_min || entry_key_interval < pool->entry_key_interval_min){
				// update the minimal interval
				pool->entry_key_interval_min = entry_key_interval;
			}
			if(entry_key_interval > pool->entry_key_interval_max){
				// update the maximal interval
				pool->entry_key_interval_max = entry_key_interval;
			}
			if(entry_key_interval > 0){
//				MEDIABUF_TRACE("Meida Pool(%s) key interval(%d) in [%d, %d]", pool->preferred_name, entry_key_interval,
//					pool->entry_key_interval_min, pool->entry_key_interval_max);
			}
		}
		// update the entry key histroy
		do{
			#if 0
			bool history_the_same = true;
			for(i = pool->entry_key_available - 1; i > 0; --i){
				if(pool->entry_key_history[i] != pool->entry_key_history[i - 1]){
					history_the_same = false;
					break;
				}
			}
			if(history_the_same){
				// update the all key history
				for(i = 0; i < pool->entry_key_available; ++i){
					pool->entry_key_history[i] = pool->entry_in;
				}
			}else
			#endif
			{
				// update the latest key history
				for(i = pool->entry_key_available - 1; i > 0; --i){
					pool->entry_key_history[i] = pool->entry_key_history[i - 1];
				}
				pool->entry_key_history[0] = pool->entry_in;
			}
		}while(0);
		///////////////////////////////////////////////////////////////
		// scan all the entries and try to clear some expires
		for(i = 0; i < pool->entry_available - 1; ++i){
			// -1 because exclude itself
			int const entry_clear = (i + pool->entry_in + 1) % pool->entry_available;
			lpMEDIA_ENTRY const entry_clear_p = pool->entries[entry_clear];
			// break until a whole cycle
			if(entry_clear == pool->entry_key_history[pool->entry_key_available - 1]){
				// clear all the time until the second latest key
				break;
			}
			// clear which entry there is data in
			if(entry_clear_p->data_size > 0){
				lpRW_LOCK const entry_clear_lock = entry_clear_p->rwlock;
				
				if(0 == RWLOCK_TRYWRLOCK(entry_clear_lock)){
					// lock success
					// this entry is free
					// clear it immediately please
					if(pool->on_entry_clear){
						pool->on_entry_clear(pool->preferred_name, entry_clear_p->data, entry_clear_p->data_size, entry_clear_p->key_flag);
					}
					MEDIAENTRY_clear(entry_clear_p, pool->entry_key_history[0]);
					RWLOCK_UNLOCK(entry_clear_lock);
				}else{
					// lock failed
					// this entry is being locked now
					// and we couldn't clear it at this time
					// forcely to change its next entry postion
					// then when read finish the user could find the current latest key automatically
					entry_clear_p->next = pool->entry_key_history[0];
				}
			}
		}
	}

	// find next entry in
	do{
		bool find_next_success = false;
		for(i = 0; i < pool->entry_available - 1; ++i){
			// -1 because exclude itself
			int const entry_next = (i + pool->entry_in + 1) % pool->entry_available;
			lpMEDIA_ENTRY const entry_next_p = pool->entries[entry_next];
			// check the entry whether it is empty
			if(NULL == entry_next_p->data){
				// bingo!!
				pool->entry_in = entry_next;
				// mark down the next entry of current entry
				entry_in_p->next = pool->entry_in; // cur_in ->|next|->next_in
				find_next_success = true;
				break;
			}
		}
		if(!find_next_success){
			// the pool is full
			// set full flag
			pool->full_flag = true;
			// force the next entry
			MEDIABUF_TRACE("Media Pool(%s) find next failed! Force(%d/%d)",
				pool->preferred_name, pool->entry_in, pool->entry_key_history[0]);
			pool->entry_in = pool->entry_key_history[0];
			entry_in_p->next = pool->entry_in;
			if(pool->on_pool_exhausted){
				pool->on_pool_exhausted(pool->preferred_name);
			}
		}
	}while(0);

	// finish being in data
	if(pool->on_entry_in){
		pool->on_entry_in(pool->preferred_name, entry_in_p->data, entry_in_p->data_size, entry_in_p->key_flag);
	}
	
	pool->dataflow_flag = true;
	pool->data_counter++;

	// unlock current entry write lock
	ret = RWLOCK_UNLOCK(entry_in_lock);
	MEDIABUF_ASSERT(0 == ret, "Media Pool(%s) input unlock(%d) failed", pool->preferred_name, entry_in);
	// thread safe unlock
	if(MEDIA_POOL_threadsafe(pool)){
		RWLOCK_UNLOCK(pool->entry_in_ctrl->lock);
	}
	// clear the control status
	pool_in_ctrl->data_size = 0;
	pool_in_ctrl->in_size = 0;
	pool_in_ctrl->ptr = NULL;

	return 0;
}

int MEDIAPOOL_in(lpMEDIA_POOL const pool, const void* data, size_t data_sz, bool keyflag, bool wait_block)
{
	int ret = 0;
	if(0 == MEDIAPOOL_in_request(pool, data_sz, keyflag, wait_block)){
		ret = MEDIAPOOL_in_append(pool, data, data_sz);
		MEDIABUF_ASSERT(0 == ret, "Media Pool append failed!");
		return MEDIAPOOL_in_commit(pool);
	}
	return -1;
}

int MEDIAPOOL_emtpy(lpMEDIA_POOL const pool)
{
	int i = 0;
	// try the best to empty all the entries
	for(i = 0; i < pool->entry_available; ++i){
		int const entry_empty = (i + pool->entry_in) % pool->entry_available;
		lpMEDIA_ENTRY const entry_empty_p = pool->entries[entry_empty];

		// clear which entry there is data in
		if(entry_empty_p->data_size > 0){
			lpRW_LOCK const entry_empty_lock = entry_empty_p->rwlock;
			
			if(0 == RWLOCK_TRYWRLOCK(entry_empty_lock)){
				if(pool->on_entry_clear){
					pool->on_entry_clear(pool->preferred_name, entry_empty_p->data, entry_empty_p->data_size, entry_empty_p->key_flag);
				}
				MEDIAENTRY_clear(entry_empty_p, pool->entry_key_history[0]);
				RWLOCK_UNLOCK(entry_empty_lock);
//				MEDIABUF_TRACE("Media Pool(%s) clear entry(%d)", pool->preferred_name, entry_empty);
			}
			else
			{
				MEDIABUF_TRACE("Media Pool(%s) clear entry(%d) fail!", pool->preferred_name, entry_empty);
			}
		}
	}
	
	// clear full flag
	pool->full_flag = false;
	return 0;
}

int MEDIAPOOL_out_lock(lpMEDIA_POOL_USER user)
{
	lpMEDIA_POOL const pool = (lpMEDIA_POOL)user->pool;
	lpMEDIA_ENTRY const entry_out_p = pool->entries[user->entry_rd];
	lpRW_LOCK const entry_out_lock = entry_out_p->rwlock;
//	if(0 == MediaPoolUser_sync(user)){
//		return MEDIAPOOL_out_lock(user);
//	}
	if(0 == RWLOCK_TRYRDLOCK(entry_out_lock)){
		// user must know what's the lock
		MEDIABUF_ASSERT(NULL == user->lock, "user->lock is using");
		user->lock = entry_out_lock;
		return 0;
	}
	return -1;
}

void MEDIAPOOL_out_unlock(lpMEDIA_POOL_USER user)
{
	int ret = 0;
	lpRW_LOCK const entry_out_lock = (lpRW_LOCK)user->lock;
	MEDIABUF_ASSERT(entry_out_lock, "media pool out unlock failed");
	ret = entry_out_lock->unlock(entry_out_lock);
	MEDIABUF_ASSERT(0 == ret, "media out unlock failed!");
	user->lock = NULL; // clear the lock
}

int MEDIAPOOL_find_nearest_key(lpMEDIA_POOL_USER user)
{
	int const entry_rd = user->entry_rd;
	lpMEDIA_POOL const pool = (lpMEDIA_POOL)user->pool;
	int i;
	int old_key, new_key;

	// first , find if is in [WP(KEY0), OLDEST_KEY]
	old_key = pool->entry_key_history[0];
	new_key = pool->entry_key_history[pool->entry_key_available - 1];
	if(old_key != new_key){
		if(new_key > old_key){
			if((entry_rd > old_key) && (entry_rd <= new_key)){
				return new_key;
			}
		}else{
			if(!((entry_rd >= new_key) && (entry_rd < old_key))){
				return new_key;
			}
		}
	}
	// find if is in key[n], key[n-1]  , n is in [entry_key_available-1, 1]
	for(i = pool->entry_key_available - 1; i > 0; i--){
		MEDIABUF_TRACE("%d old:%d->new:%d rd:%d key1:%d key0:%d", i, old_key, new_key, entry_rd,
			pool->entry_key_history[1] , pool->entry_key_history[0]);
		old_key = pool->entry_key_history[i];
		new_key = pool->entry_key_history[i - 1];
	
		if(old_key != new_key){
			if(new_key > old_key){
				if((entry_rd > old_key) && (entry_rd <= new_key)){
					MEDIABUF_TRACE("c old:%d->new:%d rd:%d key1:%d key0:%d", old_key, new_key, entry_rd,
						pool->entry_key_history[1] , pool->entry_key_history[0]);
					return new_key;
				}
			}else{
				if(!((entry_rd >= new_key) && (entry_rd < old_key))){
					MEDIABUF_TRACE("d old:%d->new:%d rd:%d key1:%d key0:%d", old_key, new_key, entry_rd,
						pool->entry_key_history[1] , pool->entry_key_history[0]);
					return new_key;
				}
			}
		}
	}

	return pool->entry_key_history[0];
}

int MEDIAPOOL_out(lpMEDIA_POOL_USER user, void** ret_data_ptr, void* ret_data, size_t* ret_sz)
{
	int entry_rd = user->entry_rd;
	lpMEDIA_POOL pool = (lpMEDIA_POOL)user->pool;
	lpMEDIA_ENTRY entry_out_p = pool->entries[entry_rd];

	if(user->entry_rd == pool->entry_in){
		//MEDIABUF_TRACE("read speed is higher %d/%d", user->entry_rd, pool->entry_in);
		return -1;
	}

	if(0 == entry_out_p->data_size){
		// data is not existed
		// an abnormal situation
		// the read speed is slower than write speed of pool
		user->timestamp = pool->timestamp;
		user->entry_rd = MEDIAPOOL_find_nearest_key(user);
		MEDIABUF_TRACE("user sync read entry = %d(%d) key1:%d key0:%d", user->entry_rd, entry_rd,
			((pool->entry_key_available > 1) ? pool->entry_key_history[1] : -1), pool->entry_key_history[0]);
		entry_rd = user->entry_rd;
		entry_out_p = pool->entries[entry_rd];
		if(0 == entry_out_p->data_size){
			// update the next read pos
			user->entry_rd = entry_out_p->next;
			return -1;
		}
	}
	// update the next read pos
	user->entry_rd = entry_out_p->next;


	if(ret_data_ptr){ *ret_data_ptr = entry_out_p->data; }
	if(ret_data){ memcpy(ret_data, entry_out_p->data, entry_out_p->data_size); }
	if(ret_sz){ *ret_sz = entry_out_p->data_size; }

	user->data_rd_id = pool->data_counter;

	if(pool->on_entry_out){
		pool->on_entry_out(pool->preferred_name, entry_out_p->data, entry_out_p->data_size, entry_out_p->key_flag);
	}
	return 0;
}


void MEDIAPOOL_rw_status(lpMEDIA_POOL const pool)
{
#define MEDIA_POOL_PRINT_WRLOCK_ATTR
#define MEDIA_POOL_PRINT_RDLOCK_ATTR
#define MEDIA_POOL_PRINT_NOLOCK_ATTR
	int i = 0;
	uint32_t status = 0;
	printf("\r\n");
	for(i = 0; i < pool->entry_available; ++i){
		status = pool->entries[i]->rwlock->getstatus(pool->entries[i]->rwlock);
		if(status < 0){
			// it's writing
			printf("W");
		}else if(0 == status){
			// idle
			printf(".");
		}else{
			// reading
			printf("R");
		}
	}
	printf("\r\n");
}

const char *MEDIAPOOL_preferred_name(lpMEDIA_POOL const pool)
{
	return pool->preferred_name;
}

const char *MEDIAPOOL_alternate_name(lpMEDIA_POOL const pool)
{
	return pool->alternate_name;
}


uint32_t MEDIAPOOL_in_speed(lpMEDIA_POOL const pool) // uint. us
{
	return pool->entry_in_speed;
}

bool MEDIAPOOL_is_empty(lpMEDIA_POOL_USER user)
{
	lpMEDIA_POOL pool = NULL;

	if (user == NULL) return true;
	pool = (lpMEDIA_POOL)user->pool;
	if (pool == NULL) return true;
	if (pool->dataflow_flag == false) return true;
	else {
		int const entry_rd = user->entry_rd;
		lpMEDIA_ENTRY const entry_out_p = pool->entries[entry_rd];
	
		if((0 == entry_out_p->data_size) && (user->data_rd_id == pool->data_counter)){
			MEDIABUF_TRACE("%s user read to EOB", pool->preferred_name);
			return true;
		}
	}
	return false;
}



