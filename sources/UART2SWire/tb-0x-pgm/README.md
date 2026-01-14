# tb-0x-kit-pgm
How to write TLSRPGM [FW](https://github.com/pvvx/TLSRPGM/raw/main/sources/UART2SWire/uart2swire.bin) to TB-03F-KIT or TB-04-KIT.

 `python TlsrComProg.py -p COM6 -t500 we 0 uart2swire.bin`

 `python3 TlsrComProg.py -p /dev/ttyUSB0 -t500 we 0 uart2swire.bin`


1. Connect the SWS pin to pin 2 of CH340.
2. Execute the command, correcting the com port name.

![TB-03F-KIT-PGM.gif](https://github.com/pvvx/TLSRPGM/blob/main/sources/UART2SWire/tb-0x-pgm/TB-03F-Kit-pgm.jpg)

![TB-04-kit-pgm.jpg](https://github.com/pvvx/TLSRPGM/blob/main/sources/UART2SWire/tb-0x-pgm/TB-04-kit-pgm.jpg)

3. After programming the module, the SWS pin to pin 2 of CH340 connection is not required.

Sampling log:
```
>TlsrComProg.py -p COM7 -t5000 we 0 ../urat2swire.bin
================================================
TLSR825x Floader version 10.11.20
------------------------------------------------
Open COM7, 230400 baud...
Reset module (RTS low)...
Activate (5000 ms)...
Warning: Wrong RX-TX connection?
Connection...
Load <floader.bin> to 0x40000...
Bin bytes writen: 1896
CPU go Start...
------------------------------------------------
ChipID: 0x5562 (TLSR8253), Floader ver: 1.0
Flash JEDEC ID: c86013, Size: 512 kbytes
------------------------------------------------
Inputfile: ../uart2swire.bin
Write Flash data 0x00000000 to 0x00000dfc...
------------------------------------------------
(1) Warning
```


[TlsrComProg825x](https://github.com/pvvx/TlsrComProg825x)