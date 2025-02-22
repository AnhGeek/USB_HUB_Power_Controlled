# USB_HUB_Power_Controlled
This project aim to create a USB HUB, which can help you control an USB port via a command sent from user.

HID interface: no need to install any driver to make it works.
Python control: you can use HID lib to control the board.
Simple HID program: Attached a simple HID program to help you test connection.

## 🚀 Features
 - **Tested with three devices**: my mouse receiver, USB UART PLX2302, Pickit 2
 ![tested](https://github.com/AnhGeek/USB_HUB_Power_Controlled/blob/main/img/tested.PNG)
 - **[How to write data to an ENDPOINT](https://github.com/AnhGeek/USB_Relay_HID/tree/main#-how-to-use)**: instruction how to write a new value to an ENDPOINT. You can control USB port with a data written to ENDPOINT0
 - **Turn on USB by writing data to ENDPOINT0**:
   - USB1: ENDPOINT0=0x01, send this command again to turn off
   - USB2: ENDPOINT0=0x02, send this command again to turn off
   - USB3: ENDPOINT0=0x03, send this command again to turn off
   - Reset command: ENDPOINT0=0xFE
   - Enter Bootloader from app: ENDPOINT0=0xFD
 - **Turn off all**: ENDPOINT0=0x00
 - Note: you can set default status of all USB port via the variable **usb_port_state**.

## 💻 MCU Used
- CH551G: datasheet [Eng ver](https://akizukidenshi.com/goodsaffix/CH552.pdf)
- Programmer: CH551G doesn't need a programmer hardware, you can download program via USB interface. It only needs a [Download Tool](https://www.wch-ic.com/downloads/WCHISPTool_Setup_exe.html) to flash the software.

## 📄 Files
- **PCB folder**: contains the PCB design.
     - Schematic: [micro USB](https://github.com/AnhGeek/USB_HUB_Power_Controlled/blob/main/PCB/TypeA/Schematic_Usb-hub-power-control_2024-12-14.pdf), [type C](https://github.com/AnhGeek/USB_HUB_Power_Controlled/blob/main/PCB/TypeC/Schematic_Usb-hub-power-control-USB-C_2024-12-15.pdf)
       ![sche](https://github.com/AnhGeek/USB_HUB_Power_Controlled/blob/main/img/Schematic_Usb-hub-power-control_2024-12-14.png)
     - PCB: very small, I designed it to match the dimensions of the Raspberry Pi Zero.
       ![PCB](https://github.com/AnhGeek/USB_HUB_Power_Controlled/blob/main/img/PCB_PCB_Usb-hub-power-control_2024-12-14.png)
     - 3D picture:
       ![3d-image](https://github.com/AnhGeek/USB_HUB_Power_Controlled/blob/main/img/3D_Usb-hub-power-control_2024-12-14.png)
     - **New USB-C version**: I haven't tested this version, you might be the first victim of my design, consider to use it!!!!
       ![typeC](https://github.com/AnhGeek/USB_HUB_Power_Controlled/blob/main/img/USB-C.PNG)
- **[SimpleHIDWrite3](https://github.com/AnhGeek/USB_Relay_HID/tree/main/SimpleHIDWrite3)**: an simple HID program to write directly data to an endpoint
- **Makefile**: run this makefile via **"make all** to rebuild entirely project

## :anger: Disclaimer

This design is provided free of charge and is intended for educational or personal use only. The creator of this design assumes no responsibility or liability for any errors, issues, damages, or problems arising from its use, implementation, or modification.

By using this design, you acknowledge and agree that you do so at your own risk and discretion. It is your responsibility to verify the suitability, safety, and compliance of this design for your specific needs and requirements.

If you are uncertain about any aspect of this design, please consult a professional before proceeding.

Note: **I have tried a new hub from Shopee, but it can't use with morden USB flash**. It might be the limitation of FE1.1s. It works well with others like: keyboard, mouse, usb uart, MCU programmer..., which it not required high USB bandwith.

## 📄 License

This project is licensed under the [MIT License](LICENSE).
