# USB_HUB_Power_Controlled
This project aim to create a USB HUB, which can help you control an USB port via a command sent from user.

HID interface: no need to install any driver to make it works.
Python control: you can use HID lib to control the board.
Simple HID program: Attached a simple HID program to help you test connection.

## 🚀 Features
 - **[How to write data to an ENDPOINT](https://github.com/AnhGeek/USB_Relay_HID/tree/main#-how-to-use)**: instruction how to write a new value to an ENDPOINT. You can control USB port with a data written to ENDPOINT0

## 💻 MCU Used
- CH551G: datasheet [Eng ver](https://akizukidenshi.com/goodsaffix/CH552.pdf)
- Programmer: CH551G doesn't need a programmer hardware, you can download program via USB interface. It only needs a [Download Tool](https://www.wch-ic.com/downloads/WCHISPTool_Setup_exe.html) to flash the software.

## 📄 Files
- **PCB**: contains the PCB design.
     - Schematic: [link](https://github.com/AnhGeek/USB_HUB_Power_Controlled/blob/main/PCB/Schematic_Usb-hub-power-control_2024-12-14.pdf)
- **[SimpleHIDWrite3](https://github.com/AnhGeek/USB_Relay_HID/tree/main/SimpleHIDWrite3)**: an simple HID program to write directly and data to endpoint
- **Makefile**: run this makefile via **"make all** to rebuild entirely project

## 📄 License

This project is licensed under the [MIT License](LICENSE).
