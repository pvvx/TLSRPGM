# tb-0x-kit-pgm
How to write TLSRPGM program to TB-03F-KIT or TB-04-KIT.

1. Connect the SWS pin to pin 2 of CH340.
2. Execute the [command](https://github.com/pvvx/TlsrComProg825x), correcting the com port name.

![SCH](https://github.com/pvvx/TlsrComProg825x/blob/main/Doc/img/TB-03F-KIT-PGM.gif)

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



