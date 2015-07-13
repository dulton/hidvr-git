
#include "media_entry.h"
#include "mediabuf_debug.h"

void MEDIAENTRY_clear(lpMEDIA_ENTRY const entry, int default_next)
{
	entry->magic = MEDIA_ENTRY_MAGIC;
	entry->key_flag = false;
	entry->data_size = 0;
	if(entry->data){
		free(entry->data);
		entry->data = NULL;
	}
	entry->next = default_next; // no next
}

lpMEDIA_ENTRY MEDIAENTRY_create()
{
	lpMEDIA_ENTRY const entry = calloc(sizeof(stMEDIA_ENTRY), 1);
	///////////////////////////
	MEDIAENTRY_clear(entry, 0);
	entry->rwlock = RWLOCK_create();
	///////////////////////////
	return entry;
}

void MEDIAENTRY_destroy(lpMEDIA_ENTRY const entry)
{
	if(entry){
		// destroy the lock
		RWLOCK_release(entry->rwlock);
		// destroy the whole entry
		free(entry);
	}
}

