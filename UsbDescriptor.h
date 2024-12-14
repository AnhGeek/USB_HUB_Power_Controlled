#include <stdint.h>

#define USB_DESCR_TYP_DEVICE    0x01
#define USB_DESCR_TYP_CONFIG    0x02
#define USB_DESCR_TYP_STRING    0x03
#define USB_DESCR_TYP_INTERF    0x04
#define USB_DESCR_TYP_ENDP      0x05
#define USB_DESCR_TYP_QUALIF    0x06
#define USB_DESCR_TYP_SPEED     0x07
#define USB_DESCR_TYP_OTG       0x09
#define USB_DESCR_TYP_BOS       0x0f
#define USB_DESCR_TYP_HID       0x21
#define USB_DESCR_TYP_REPORT    0x22
#define USB_DESCR_TYP_PHYSIC    0x23
#define USB_DESCR_TYP_CS_INTF   0x24
#define USB_DESCR_TYP_CS_ENDP   0x25
#define USB_DESCR_TYP_HUB       0x29


typedef unsigned char *PUINT8;

typedef struct
{
	uint8_t* descr;
	uint8_t size;
} DescriptorData;

/* device descriptor */
extern const DescriptorData DevDesc;

/* configure descriptor */
extern const DescriptorData CfgDesc;

/* device qualifier descriptor */
extern const DescriptorData DeviceQualifierCfg;

/* custome HID report descriptor */
extern const DescriptorData CustRepDesc;

/* string descriptors */
extern const DescriptorData StringDescriptors[4];





