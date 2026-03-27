#include "common.h"
#include "utils.h"

_attribute_ram_code_
void sleep_us (u32 us) {
	u32 t = clock_time();
	while(!clock_time_exceed(t, us));
}
