
#ifndef MEDIA_MALLOC_H_
#define MEDIA_MALLOC_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef void* mspace;

extern mspace create_mspace(size_t capacity, int locked);
extern mspace create_mspace_with_base(void* base, size_t capacity, int locked);
extern size_t destroy_mspace(mspace msp);

/*
  mspace versions of routines are near-clones of the global
  versions. This is not so nice but better than the alternatives.
*/


extern void* mspace_malloc(mspace msp, size_t bytes);
extern void mspace_free(mspace msp, void* mem);

extern void* mspace_calloc(mspace msp, size_t n_elements, size_t elem_size);
extern void* mspace_realloc(mspace msp, void* oldmem, size_t bytes);
extern void* mspace_memalign(mspace msp, size_t alignment, size_t bytes);

extern void** mspace_independent_calloc(mspace msp, size_t n_elements,
                                 size_t elem_size, void* chunks[]);

extern void** mspace_independent_comalloc(mspace msp, size_t n_elements,
                                   size_t sizes[], void* chunks[]);

extern int mspace_trim(mspace msp, size_t pad);
extern void mspace_malloc_stats(mspace msp);

extern size_t mspace_footprint(mspace msp);

extern size_t mspace_max_footprint(mspace msp);

extern int mspace_mallopt(int param_number, int value);

#ifdef __cplusplus
};
#endif
#endif //MEDIA_MALLOC_H_

