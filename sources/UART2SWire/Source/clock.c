#include "common.h"

static inline unsigned long clock_time(void) {
	return reg_system_tick;
}

static inline unsigned int clock_time_exceed(unsigned int ref, unsigned int us) {
	return ((unsigned int)(clock_time() - ref) > us * 16);
}

_attribute_ram_code_ void sleep_us (u32 us)
{
	u32 t = clock_time();
	while(!clock_time_exceed(t, us)){
	}
}
