
#pragma once

_attribute_ram_code_ u8 analog_read(u8 addr);
_attribute_ram_code_ void analog_write(u8 addr, u8 v);
_attribute_ram_code_ void analog_read_blk(u8 addr, u8 *v, int len);
_attribute_ram_code_ void analog_write_blk(u8 addr, u8 *v, int len);

#if ((CHIP_TYPE == MCU_CORE_8266) || (CHIP_TYPE == MCU_CORE_8269))
static inline void usb_dp_pullup_enable(void) {
	analog_write(0x00, 0x88); // analog_read(0x00) & (~BIT(4)));
}
static inline void usb_dp_pullup_disable(void) {
	analog_write(0x00, 0x98); // (analog_read(0x00) | BIT(4)));
}
#else
static inline void usb_dp_pullup_enable(void) {
	analog_write(0x08, (analog_read(0x08) & 0x3f) | BIT(7));
}
static inline void usb_dp_pullup_disable(void) {
	analog_write(0x08, (analog_read(0x08) & 0x3f) | BIT(6));
}
#endif



