#ifndef _USB_CONFIG_H_
#define _USB_CONFIG_H_

#define VID     0x413C
#define PID     0x2107

#define DEFAULT_ENDP0_SIZE      64       /* default maximum packet size for endpoint 0 */

#define MAX_PACKET_SIZE         64       /* maximum packet size */

#define LOW_SPEED_DEVICE        0
#define FULL_SPEED_DEVICE       1
	
#define USB_DEVICE_TYPE         FULL_SPEED_DEVICE

#endif
