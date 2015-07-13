
#include "rwlock.h"

typedef struct _RW_LOCK_PRIV {
	pthread_rwlock_t lock;
	int32_t status; // for debug print
}stRW_LOCK_PRIV, *lpRW_LOCK_PRIV;

static int rwlock_rdlock(lpRW_LOCK thiz)
{
	lpRW_LOCK_PRIV priv = (lpRW_LOCK_PRIV)(thiz + 1);
	if(0 == pthread_rwlock_rdlock(&priv->lock)){
		++priv->status;
		return 0;
	}
	return -1;
}

static int rwlock_tryrdlock(lpRW_LOCK thiz)
{
	lpRW_LOCK_PRIV priv = (lpRW_LOCK_PRIV)(thiz + 1);
	if(0 == pthread_rwlock_tryrdlock(&priv->lock)){
		++priv->status;
		return 0;
	}
	return -1;
}

static int rwlock_wrlock(lpRW_LOCK thiz)
{
	lpRW_LOCK_PRIV priv = (lpRW_LOCK_PRIV)(thiz + 1);
	if(0 == pthread_rwlock_wrlock(&priv->lock)){
		priv->status = -1;
		return 0;
	}
	return -1;
}

static int rwlock_trywrlock(lpRW_LOCK thiz)
{
	lpRW_LOCK_PRIV priv = (lpRW_LOCK_PRIV)(thiz + 1);
	if(0 == pthread_rwlock_trywrlock(&priv->lock)){
		priv->status = -1;
		return 0;
	}
	return -1;
}

static int rwlock_unlock(lpRW_LOCK thiz)
{
	lpRW_LOCK_PRIV priv = (lpRW_LOCK_PRIV)(thiz + 1);
	if(0 == pthread_rwlock_unlock(&priv->lock)){
		if(priv->status < 0){
			priv->status = 0;
		}else{
			--priv->status;
		}
		return 0;
	}
	return -1;
}

static int32_t rwlock_getstatus(lpRW_LOCK thiz)
{
	lpRW_LOCK_PRIV priv = (lpRW_LOCK_PRIV)(thiz + 1);
	return priv->status;
}

static void rwlock_install(lpRW_LOCK thiz)
{
	thiz->rdlock = rwlock_rdlock;
	thiz->tryrdlock = rwlock_tryrdlock;
	thiz->wrlock = rwlock_wrlock;
	thiz->trywrlock = rwlock_trywrlock;
	thiz->unlock = rwlock_unlock;
	thiz->getstatus = rwlock_getstatus;
}

lpRW_LOCK RWLOCK_create()
{
	int ret = 0;
	lpRW_LOCK rw_lock = NULL;
	lpRW_LOCK_PRIV priv = NULL;
	// alloc private area
	rw_lock = calloc(sizeof(stRW_LOCK) + sizeof(stRW_LOCK_PRIV), 1);
	assert(rw_lock);
	priv = (lpRW_LOCK_PRIV)(rw_lock + 1);
	// init interface
	rwlock_install(rw_lock);
	// init lock
	priv->status = 0;
	ret = pthread_rwlock_init(&priv->lock, NULL);
	assert(0 == ret);
	return rw_lock;
}

void RWLOCK_release(lpRW_LOCK rwlock)
{
	if(rwlock){
		void* const mem_base = rwlock;
		lpRW_LOCK_PRIV priv = mem_base + sizeof(stRW_LOCK);
		pthread_rwlock_destroy(&priv->lock);
		free(rwlock);
	}
}

