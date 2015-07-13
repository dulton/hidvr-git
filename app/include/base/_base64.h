#ifndef __BASE64___H__
#define __BASE64___H__

#ifdef __cplusplus
extern "C" {
#endif

int BASE64_encode(const void *lpSoure, int cbSource, void *lpDest, int cbDest); 
int BASE64_decode(const void *lpSoure, int cbSource, void *lpDest, int cbDest); 
 
#ifdef __cplusplus
}
#endif

#endif
