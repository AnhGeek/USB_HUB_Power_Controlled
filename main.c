#include "ch552.h"
#include "UsbDef.h"
#include "UsbDescriptor.h"
#include <stdint.h>
#include <string.h>

#define DATA_STATE 0
#define STATUS_STATE 1


void delay_ms(uint16_t u16Delay);
void jump_to_bootloader();

void delay_ms(uint16_t u16Delay)
{
    
    while (u16Delay) {
        TF0 = 0;
        TH0 = 0xA2;
        TL0 = 0x40;
        TR0 = 1;
        while (!TF0) {
        }
        TR0 = 0;
        --u16Delay;
    }
}

SBIT(P3_0, 0xB0, 0);
SBIT(P1_5, 0x90, 5);
SBIT(P1_6, 0x90, 6);
SBIT(P1_7, 0x90, 7);
SBIT(UMS_SUSPEND, 0xDA, 2);


static PUINT8  pDescr;   
__xdata uint8_t u8Buff[64];
__xdata uint8_t u8Ep1Buff[64];
__xdata uint8_t u8Ep2Buff[64];

uint8_t u8UsbIndex;
uint8_t usb_port_state = 0xFF;

void send(uint8_t u8Data);
uint8_t update_bit(uint8_t byte, uint8_t bit_position, uint8_t value);
uint8_t toggle_bit(uint8_t byte, uint8_t bit_position);

void send(uint8_t u8Data)
{
    uint8_t i;
    
    for (i = 0; i < 8; ++i) {
        if (u8Data & 0x80) {
            //P3_0 = 1;
            delay_ms(4);
            //P3_0 = 0;
            delay_ms(1);
        } else {
            //P3_0 = 1;
            delay_ms(1);
            //P3_0 = 0;
            delay_ms(4);
        }
        u8Data <<= 1;
    }
}

#define BOOT_ADDR  0x3800

/* This function provided a way to access the internal bootloader */
void jump_to_bootloader()
{
    USB_INT_EN = 0;
    USB_CTRL = 0x06;
    
    delay_ms(100);
    P1 = 0xFF;
    
    EA = 0;/* Disable all interrupts */
    
    __asm
        LJMP BOOT_ADDR /* Jump to bootloader */
    __endasm;	
    while(1); 
}

uint8_t update_bit(uint8_t byte, uint8_t bit_position, uint8_t value) {
    if (bit_position > 7 || (value != 0 && value != 1)) {
        // Invalid input; return the original byte.
        return byte;
    }
    
    if (value == 1) {
        // Set the bit to 1.
        byte |= (1 << bit_position);
    } else {
        // Clear the bit to 0.
        byte &= ~(1 << bit_position);
    }
    
    return byte;
}

uint8_t toggle_bit(uint8_t byte, uint8_t bit_position) {
    if (bit_position > 7) {
        // Invalid input; return the original byte.
        return byte;
    }

    if (byte & (1 << bit_position)) {
        // Clear the bit to 0.
        byte &= ~(1 << bit_position);
    } else {
        // Set the bit to 1.
        byte |= (1 << bit_position);
    }
    
    return byte;
}

void main(void)
{
    //uint8_t i;
    uint8_t tmp, len = 0;
    uint8_t u8ControlState = DATA_STATE;
    uint8_t jump_to_boot = 0x00;
    uint16_t timer_loop = 0;
    
    /* clock */
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG = 0b10000110; //, enable internal OSC, MASK_SYS_CK_SEL = 110 - 24MHz
    GLOBAL_CFG |= (1 << 0);
    SAFE_MOD = 0x00;
    
    /* P3.0 */
    /* Push-pull */
    P3_MOD_OC &= ~(1 << 0);
    P3_DIR_PU |= (1 << 0);

    /* P1.5 */
    /* Push-pull */
    P1_MOD_OC &= ~(1 << 5);
    P1_DIR_PU |= (1 << 5);
    /* P1.6 */
    /* Push-pull */
    P1_MOD_OC &= ~(1 << 6);
    P1_DIR_PU |= (1 << 6);
    /* P1.7 */
    /* Push-pull */
    P1_MOD_OC &= ~(1 << 7);
    P1_DIR_PU |= (1 << 7);

    /* Don't turn any UBS devices when being not recognized */
    P1 = 0xFF;
    /* Update P1 to USB port state */
    //usb_port_state = 0b00011111; // all USBs on
    usb_port_state = 0xFF; // all USBs off

    /* Internal pin debug */
    P3 &= ~(1 << 0);
    
    T2MOD |= (1 << 7);
    T2MOD |= (1 << 4);
    TMOD = 0x01;
    
#if USB_DEVICE_TYPE == LOW_SPEED_DEVICE
    USB_CTRL = 0b01101001; // Low speed
    UDEV_CTRL |= (1 << 2) | (1 << 0);
#elif USB_DEVICE_TYPE == FULL_SPEED_DEVICE
    USB_CTRL = (1 << 5) | (1 << 3) | (1 << 0);
    UDEV_CTRL |= (1 << 0); 
#else
    #error "usb device type error"
#endif
    u8UsbIndex = 0;
    while (1) 
    {
        if (UIF_BUS_RST) 
        {
            UEP0_DMA = (uint16_t)u8Buff;
            UEP0_CTRL = 0x02;
            UIF_BUS_RST = 0;
            USB_DEV_AD = 0;
        }
        if(U_SIE_FREE)
        {
            P1 = usb_port_state;
            if(jump_to_boot == 0x01)
            {
                jump_to_bootloader();
            }
        }
        /* Only turn on/off when CH551G USB is free */
        if(!UMS_SUSPEND){
            if (timer_loop == 60000)
            {
                if(P3 & 0x01) P3_0 = 0;
                else P3_0 = 1;
                timer_loop = 0;
                WDOG_COUNT = 0;
            } else timer_loop++;
        }
        
        if (UIF_TRANSFER) {
            /* Program will jump in to this branch after a transfer being transmited successfully
               You must prepare data within the previous transaction. */
            if ((USB_INT_ST & 0x0F) == 0x00) {
                /* reset buffer */
                UEP0_T_LEN = 0;
                // New request to EP0?
                switch((USB_INT_ST & 0x30)){
                    case 0x30:
                        //Setup transfer
                        P3_0 = 1;
                        P3_0 = 0;
                        if (u8Buff[0] & 0x80) {
                            /* Check bmRequestType = device to host */
                            switch (u8Buff[1]) {
                                case 0x06:
                                    /* bRequest == Get descriptor */
                                    switch (u8Buff[3]) {
                                        //Check descriptor types
                                        case USB_DESCR_TYP_DEVICE:
                                            /* device descriptor */
                                            u8ControlState = DATA_STATE;
                                            if (u8Buff[6] >= 0x12) {
                                                //copy data to enp0 buffer
                                                memcpy(u8Buff, DevDesc.descr, DevDesc.size);
                                                UEP0_T_LEN = DevDesc.size;
                                                UEP0_CTRL = 0x80 | 0x40;
                                            }
                                            break;
                                        case USB_DESCR_TYP_CONFIG:
                                            /* config descriptor */
                                            u8ControlState = DATA_STATE;
                                            if (u8Buff[6] >= 0x12) {
                                                //copy data to enp0 buffer
                                                memcpy(u8Buff, CfgDesc.descr, CfgDesc.size);
                                                UEP0_T_LEN = CfgDesc.size;
                                                UEP0_CTRL = 0x80 | 0x40;
                                            }
                                            else 
                                            {
                                                len = u8Buff[6];
                                                memcpy(u8Buff, CfgDesc.descr, len);
                                                UEP0_T_LEN = len;
                                                UEP0_CTRL = 0x80 | 0x40;
                                            }
                                            break;
                                        case USB_DESCR_TYP_STRING:
                                            u8ControlState = DATA_STATE;
                                            tmp = u8Buff[2];
                                            //copy data to enp0 buffer
                                            memcpy(u8Buff, StringDescriptors[tmp].descr, StringDescriptors[tmp].size);
                                            UEP0_T_LEN = StringDescriptors[tmp].size;
                                            UEP0_CTRL = 0x80 | 0x40;
                                            break;
                                        case USB_DESCR_TYP_QUALIF:
                                            u8ControlState = DATA_STATE;
                                            if (u8Buff[6] == 0x0a) {
                                                //copy data to enp0 buffer
                                                memcpy(u8Buff, DeviceQualifierCfg.descr, DeviceQualifierCfg.size);
                                                UEP0_T_LEN = DeviceQualifierCfg.size;
                                                UEP0_CTRL = 0x80 | 0x40;
                                            }
                                            break;
                                        case USB_DESCR_TYP_REPORT:
                                            u8ControlState = DATA_STATE;
                                            //copy data to enp0 buffer
                                            memcpy(u8Buff, CustRepDesc.descr, CustRepDesc.size);
                                            UEP0_T_LEN = CustRepDesc.size;
                                            UEP0_CTRL = 0x80 | 0x40;
                                            break; 
                                    }
                                    break;
                                default:
                                    //Default
                                    break;
                            }
                        } 
                        else 
                        {
                            /* Check bmRequestType = host to device */
                            switch (u8Buff[1]) {
                                case 0x05:
                                    /* bRequest == SET_ADDRESS */
                                    u8UsbIndex = u8Buff[2];
                                    UEP0_T_LEN = 0;
                                    UEP0_CTRL = 0x80 | 0x40;
                                    break;
                                case 0x09:
                                    /* bRequest == SET_CONFIG */
                                    UEP1_CTRL = (1 << 4) | (1 << 1);
                                    UEP4_1_MOD |= (1 << 7);
                                    UEP4_1_MOD &= ~(1 << 6);
                                    UEP1_DMA = (uint16_t)u8Ep1Buff;
                                    
                                    UEP2_CTRL = (1 << 4) | (1 << 3) | (1 << 1);
                                    UEP2_3_MOD |= (1 << 2);
                                    UEP2_3_MOD &= ~(1 << 3);
                                    UEP2_DMA = (uint16_t)u8Ep2Buff;
                                    u8ControlState = STATUS_STATE;
                                    /* set config */
                                    UEP0_T_LEN = 0;
                                    UEP0_CTRL = 0x40;
                                    break;
                                default:
                                    //Default
                                    break;
                            }
                        }
                        break;	
                    case 0x20:
                        /* EP0 in */
                        if (u8ControlState == DATA_STATE) {
                            u8ControlState = STATUS_STATE;
                            UEP0_CTRL = 0x80 | 0x40 | 0x02;
                        } else {
                            /* EP0 in */
                            if ((u8UsbIndex != 0x00) && (USB_DEV_AD != u8UsbIndex )) 
                            {
                                USB_DEV_AD = u8UsbIndex;
                            }
                            UEP0_CTRL = 0x40 | 0x02;
                        }
                        break;
                        
                    case 0x00:
                        /* EP0 out */
                        break;
                        
                    default:
                        //SOF packets
                        break;
                }
            } else if ((USB_INT_ST & 0x0F) == 0x01) 
            {
                /* ep1 */
                if (u8Ep1Buff[0] == 0x00) 
                {
                    usb_port_state = update_bit(usb_port_state, 5, 1);
                    usb_port_state = update_bit(usb_port_state, 6, 1);
                    usb_port_state = update_bit(usb_port_state, 7, 1);
                } else if (u8Ep1Buff[0] == 0x01) 
                {
                    usb_port_state = toggle_bit(usb_port_state, 7);
                } else if (u8Ep1Buff[0] == 0x02) 
                {
                    usb_port_state = toggle_bit(usb_port_state, 6);
                } else if (u8Ep1Buff[0] == 0x03) 
                {
                    usb_port_state = toggle_bit(usb_port_state, 5);
                }  else if (u8Ep1Buff[0] == 0xFD) 
                {
                    jump_to_boot = 0x01;
                } 
                  else if (u8Ep1Buff[0] == 0xFE) 
                {
                    P1 = 0xFF;
                    delay_ms(200);
                    SAFE_MOD = 0x55;
                    SAFE_MOD = 0xAA;
                    GLOBAL_CFG = bSW_RESET;
                } 
                else if (u8Ep1Buff[0] == 0xFF) 
                {
                    u8Ep2Buff[0] = 0x01;
                    u8Ep2Buff[1] = 0xFF;
                    u8Ep2Buff[2] = 0xFF;
                    u8Ep2Buff[3] = 0xFF;
                    UEP2_T_LEN = 0x40;
                    tmp = UEP2_CTRL;
                    tmp &= ~(1 << 1);
                    tmp &= ~(1 << 0);
                    UEP2_CTRL = tmp;
                }
            } else if ((USB_INT_ST & 0x0F) == 0x02) 
            {
                /* ep2 */
                tmp = UEP2_CTRL;
                tmp |= (1 << 1);
                tmp &= ~(1 << 0);
                UEP2_CTRL = tmp;
            }
            UIF_TRANSFER = 0;
        }
    }
}