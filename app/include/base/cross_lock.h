#ifndef __CROSS_LOCK_HEAD_FILE__
#define __CROSS_LOCK_HEAD_FILE__

#ifdef __cplusplus
extern "C" {
#endif

typedef void * HMUTEX;

HMUTEX mutex_create();
void mutex_destroy(HMUTEX mutex);
int mutex_lock(HMUTEX mutex);
int mutex_trylock(HMUTEX mutex);
int mutex_unlock(HMUTEX mutex);


typedef void * HRWLOCK;

HRWLOCK rwlock_create();
void rwlock_destroy(HRWLOCK rwlock);
int rwlock_rdlock(HRWLOCK rwlock);
int rwlock_wrlock(HRWLOCK rwlock);
int rwlock_unlock(HRWLOCK rwlock);


#ifdef __cplusplus
}
#endif


#endif
