Overview
========
The SDCARD Polling project is a demonstration program that uses the SDK software. It reads/writes
/erases the SD card continusly. The purpose of this example is to show how to use SDCARD driver and
show how to use polling based transfer API in SDHC driver in SDK software to access SD card.
Note: If DATA3 is used as the card detect PIN, please make sure DATA3 is pull down, no matter internal or external, at the same time, make sure the card can pull DATA3 up, then host can detect card through DATA3.And SDHC do not support detect card through CD by host, card can be detected through DATA3 or GPIO.No matter detect card through host or gpio, make sure the pinmux configuration is correct.

Toolchain supported
===================
- IAR embedded Workbench  8.40.2
- Keil MDK  5.29
- GCC ARM Embedded  8.3.1
- MCUXpresso  11.1.0

Hardware requirements
=====================
- Mini/micro USB cable
- TWR-K64F120M board
- Personal Computer

Board settings
==============
The SDCARD Polling example is configured to use SDHC0 with PTE0, PTE1, PTE2, PTE3, PTE4, PTE5 pins
and use PTB20 pin as card detection pin. It doesn't detetet write protect switch state so don't
open the write protect switch on the SD card socket. It also uses UART1 with PTC4 and PTC3 pins.
To make OpenSDA serial device work, the jumpers should be set as following:
- J10: 2-3 connected
- J15: 2-3 connected

Prepare the Demo
================
1. Connect a USB cable between the PC host and the OpenSDA USB port (J2) on the board.
2. Open a serial terminal on PC for OpenSDA serial device with these settings:
   - 115200 baud rate
   - 8 data bits
   - No parity
   - One stop bit
   - No flow control
3. Download the program to the target board.
4. Either press the reset button on your board or launch the debugger in your IDE to begin running
   the example.

Running the demo
================
When the example runs successfully, you can see the similar information from the terminal as below.

SDCARD polling example.

Please insert a card into board.

Card inserted.

Card size xxxx bytes

Working condition:

  Voltage : xxxxV

  Timing mode: High Speed

  Freq : xxxx HZ

Read/Write/Erase the card continuously until encounter error......

Write/read one data block......
Compare the read/write content......
The read/write content is consistent.
Write/read multiple data blocks......
Compare the read/write content......
The read/write content is consistent.
Erase multiple data blocks......

Input 'q' to quit read/write/erase process.                
Input other char to read/write/erase data blocks again.


Customization options
=====================
1. If TWR-SER board was connected to the tower system with twrk64f120m, we can switch the serial
   output to TWR-SER as long as following settings are applied:
   - On twrk64f120m board
	   J10: 1-2 connected
	   J15: 1-2 connected
   - Connect the UART cable between PC and TWR-SER (J?)
   - Open a serial terminal on PC for TWR-SER serial device instead of OpenSDA with same settings

2. <FIXME>The USE_STDIO_FUNCTIONS definition determines whether the example uses standard I/O
   functions, such as printf. If it is not defined, then the example accesses the UART driver directly.

