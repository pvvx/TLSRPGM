/*********************************************************************************************
 * During OTA upgrade, the upgraded device will check the rules of the following three fields.
 * Refer to ZCL OTA specification for details.
 */

#define DEVICE_TYPE							1

#define APP_RELEASE							0x01 // app release 9.9
#define APP_BUILD							0x01 // app build 9.9
#define STACK_RELEASE						0x30 // stack release 3.0
#define STACK_BUILD							0x01 // stack build 01


/* Chip IDs */
#define CHIP_TYPE_8267						0x00
#define CHIP_TYPE_8269						0x01
#define CHIP_TYPE_8258						0x02
#define CHIP_TYPE_8258_1M					0x03
#define CHIP_TYPE_8278						0x04
#define CHIP_TYPE_B91						0x05

#define MANUFACTURER_CODE_TELINK           	0x1141 // Telink ID

#define CHIP_TYPE CHIP_TYPE_8258

#define	IMAGE_TYPE			((CHIP_TYPE << 8) | DEVICE_TYPE)
#define	FILE_VERSION		((APP_RELEASE << 24) | (APP_BUILD << 16) | (STACK_RELEASE << 8) | STACK_BUILD)
