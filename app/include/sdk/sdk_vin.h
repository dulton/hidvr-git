
#include "sdk_def.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef SDK_VIN_H_
#define SDK_VIN_H_

typedef enum SDK_VIN_HW_SPEC
{
	SDK_VIN_HW_SPEC_INVALID = 0,
	SDK_VIN_HW_SPEC_HI35XX_4D1X2_CH0_CH2,
	SDK_VIN_HW_SPEC_HI35XX_4D1X2_CH2_CH0,
	//
}SDK_VIN_HW_SPEC_t;

typedef struct SDK_VIN_API
{
	SDK_VIN_HW_SPEC_t (*hw_spec)();
	int (*capture)(int vin, FILE* bitmap_stream);
}SDK_VIN_API_t;

// could be used after 'SDK_init_vin' success to call
__SDK_API SDK_VIN_API_t* sdk_vin;

__SDK_API int SDK_init_vin(SDK_VIN_HW_SPEC_t hw_spec);
__SDK_API int SDK_destroy_vin();


#endif //SDK_VIN_H_

