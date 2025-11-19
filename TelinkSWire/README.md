# SWM and SWS

SWM (Single Wire Master) and SWS (Single Wire Slave) represent the master and slave device of the single wire communication system developed by Telink. The maximum data rate can be up to 2Mbps.

### Communication protocol

The single wire communication protocol developed by Telink is as following:

1) Pull-up resistor is added to the single wire;

2) The voltage of Swire bus is the same as VCC when it is idle;

3) Refer to the following sequence diagram:

Figure 7-1 Sequence diagram for Swire bus communication protocol

![figure7-1](https://raw.githubusercontent.com/pvvx/TLSRPGM/refs/heads/main/TelinkSWire/figure7-1.gif)

One bit contains some units of low level and high level. Sending successively one unit of low level and four units of high level by master represents one bit of 0; while sending successively four units of low level and one unit of high level represents one bit of 1.

Each received byte contains nine bits and some units of low level (end). The foremost bit “cmd” is a flag bit to distinguish between command and data; the “cmd” bit is 0 except for START and END byte.

Regarding to the whole protocol:

1) Sending START byte from master to slave starts communication. The bit “cmd” of START byte is 1; data=0x5a.

2) Send the 16-bit address containing address high and address low from master to slave.

3) Send RW_ID byte from master to slave. Bit7 of RW_ID indicates reading operation (1) or writing operation (0); bit6~bit0 indicate ID of slave.

4) For writing operation, master sends data to slave, and the data is written into the memory block of slave. Operation address will add 1 based on current address for each written data; End byte sent from master to slave after all data are written indicates the end of communication. The cmd flag is 1 for END byte; data= 0xff.

5) For reading operation, master sends one unit of low level to slave; bus is released. Because of the pull-up resistor, the bus will be pulled to high state. When the period of high level is longer than that of low level, slave starts to send 8-bit data and one unit of low level successively. The bus is released until another unit of low level is sent by master. The reading pointer of slave will add 1 for each read byte. The reading operation comes to an end after the END signal is sent by


### Register configuration 

Table 7-1 Register configuration related to Swire communication

![Table7-1](https://raw.githubusercontent.com/pvvx/TLSRPGM/refs/heads/main/TelinkSWire/Table7-1.gif)

1) 0xb0 indicates sending 8-bit data;

2) Swire_usb_en indicates whether to use Pin dp of USB as Pin Swire;

3) Cmd indicates that cmd bit is sent. 1: the cmd bit is command; 0: the cmd bit is data.

4) Rd indicates that reading process starts;

5) Wr indicates that writing process starts;

6) Clk_div indicates the number of system clock for each bit of Swire;

7) Swire_id indicates the ID information in debug process.