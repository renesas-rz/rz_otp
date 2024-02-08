## Read/Write OTP in uboot using md command
Example of OTP read and write in uboot using md command.
### OTP read :
```
# confirm PWR bit is 0 in OTP power control register (OTPPWR)
=> md.l 0x11860000 1
11860000: 00000000                             ....

# confirm CMD_RDY bit is 0 in OTP access status register (OTPSTR)
=> md.l 0x11860004 1
11860004: 00000000                             ....

# confirm OTPFLAG bit is 1 in OTP power down flag register (OTPFLAG)
=> md.l 0x1186001C 1
1186001c: 00000001                             ....

# set the PWR and ACCL bits to 1 at the same time in the OTP power control register (OTPPWR)
=> mw.l 0x11860000 0x11

=> md.l 0x11860000 1
11860000: 00000011                             ....

# wait for the CMD_RDY bit to change to 1 in the OTP access status register (OTPSTR)
=> md.l 0x11860004 1   // Wait for the CMD_RDY bit to change to 1 in the OTP access status register(OTPSTR)
11860004: 00000001

#  set the read address in the OTP read address register (OTPADRRD).(ADRRD = (ADRRD = OTP internal memory address >> 2) (200H >> 2 =80H)        
=> mw.l 0x11860014 0x80

# read the data in the OTP read data register (OTPDATARD)                                             
=> md.l 0x11860018 1
11860018: aaaa5555                             UU..

```
### OTP write :
```
# confirm PWR bit is 0 in OTP power control register (OTPPWR)
=> md.l 0x11860000 1
11860000: 00000000                             ....

# confirm CMD_RDY bit is 0 in OTP access status register (OTPSTR)
=> md.l 0x11860004 1
11860004: 00000000                             ....

# confirm OTPFLAG bit is 1 in OTP power down flag register (OTPFLAG)
=> md.l 0x1186001C 1
1186001c: 00000001                             ....

# set the PWR and ACCL bits to 1 at the same time in the OTP power control register (OTPPWR)
=> mw.l 0x11860000 0x11

=> md.l 0x11860000 1
11860000: 00000011                             ....

# wait for the CMD_RDY bit to change to 1 in the OTP access status register (OTPSTR)
=> md.l 0x11860004 1                                                                      
11860004: 00000001

# set the write address in the OTP write address register (OTPADRWR). (ADRRD = OTP internal memory address >> 2) (280H >> 2 =a0H)
=> mw.l 1186000c 0xa0

# set the write data in the OTP write data register (OTPDATAWR)
=> mw.l 11860010 0xaaaa5558

# set the STAWR bit to 1 in the OTP write start register (OTPSTAWR)
=> mw.l 11860008 0x1

```
