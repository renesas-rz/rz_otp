## Read/Write OTP in Linux using devmem2 command
Example of OTP read and write in Linux using devmem2 command.
### OTP read :

```
# confirm PWR bit is 0 in OTP power control register(OTPPWR)
root@smarc-rzg2l:~# devmem2 0x11860000
/dev/mem opened.
Memory mapped at address 0xffff90c0a000.
Read at address  0x11860000 (0xffff90c0a000): 0x00000000

# confirm CMD_RDY bit is 0 in OTP access status register (OTPSTR)
root@smarc-rzg2l:~# devmem2 0x11860004
/dev/mem opened.
Memory mapped at address 0xffff9af76000.
Read at address  0x11860004 (0xffff9af76004): 0x00000000

# confirm OTPFLAG bit is 1 in OTP power down flag register (OTPFLAG)
root@smarc-rzg2l:~# devmem2 0x1186001c  
/dev/mem opened.
Memory mapped at address 0xffff8086f000.
Read at address  0x1186001C (0xffff8086f01c): 0x00000001

# set the PWR and ACCL bits to 1 at the same time in the OTP power control register (OTPPWR).
root@smarc-rzg2l:~# devmem2 0x11860000 w 0x11  
/dev/mem opened.
Memory mapped at address 0xffff9fe40000.
Read at address  0x11860000 (0xffff9fe40000): 0x00000000
Write at address 0x11860000 (0xffff9fe40000): 0x00000011, readback 0x00000011

root@smarc-rzg2l:~# devmem2 0x11860000
/dev/mem opened.
Memory mapped at address 0xffff9da76000.
Read at address  0x11860000 (0xffff9da76000): 0x00000011

# wait for the CMD_RDY bit to change to 1 in the OTP access status register (OTPSTR)
root@smarc-rzg2l:~# devmem2 0x11860004  
/dev/mem opened.
Memory mapped at address 0xffffbb585000.
Read at address  0x11860004 (0xffffbb585004): 0x00000001

# set the read address in the OTP read address register (OTPADRRD). (ADRRD = OTP internal memory address >> 2) (200H >> 2 =80H)
root@smarc-rzg2l:~# devmem2 0x11860014 w 0x80 
/dev/mem opened.
Memory mapped at address 0xffffa382f000.
Read at address  0x11860014 (0xffffa382f014): 0x00000000
Write at address 0x11860014 (0xffffa382f014): 0x00000080, readback 0x00000080

# read the data in the OTP read data register (OTPDATARD)
root@smarc-rzg2l:~# devmem2 0x11860018  
/dev/mem opened.
Memory mapped at address 0xffff94d29000.
Read at address  0x11860018 (0xffff94d29018): 0xAAAA5555

```
### OTP write :
```
# confirm PWR bit is 0 in OTP power control register (OTPPWR)
root@smarc-rzg2l:~# devmem2 0x11860000
/dev/mem opened.
Memory mapped at address 0xffff90c0a000.
Read at address  0x11860000 (0xffff90c0a000): 0x00000000

# confirm CMD_RDY bit is 0 in OTP access status register (OTPSTR)
root@smarc-rzg2l:~# devmem2 0x11860004
/dev/mem opened.
Memory mapped at address 0xffff9af76000.
Read at address  0x11860004 (0xffff9af76004): 0x00000000

# confirm OTPFLAG bit is 1 in OTP power down flag register (OTPFLAG)
root@smarc-rzg2l:~# devmem2 0x1186001c  
/dev/mem opened.
Memory mapped at address 0xffff8086f000.
Read at address  0x1186001C (0xffff8086f01c): 0x00000001

# set the PWR and ACCL bits to 1 at the same time in the OTP power control register (OTPPWR)
root@smarc-rzg2l:~# devmem2 0x11860000 w 0x11
/dev/mem opened.
Memory mapped at address 0xffff9fe40000.
Read at address  0x11860000 (0xffff9fe40000): 0x00000000
Write at address 0x11860000 (0xffff9fe40000): 0x00000011, readback 0x00000011

root@smarc-rzg2l:~# devmem2 0x11860000
/dev/mem opened.
Memory mapped at address 0xffff9da76000.
Read at address  0x11860000 (0xffff9da76000): 0x00000011

# wait for the CMD_RDY bit to change to 1 in the OTP access status register (OTPSTR)
root@smarc-rzg2l:~# devmem2 0x11860004  
/dev/mem opened.
Memory mapped at address 0xffffbb585000.
Read at address  0x11860004 (0xffffbb585004): 0x00000001

# set the write address in the OTP write address register (OTPADRWR). (ADRRD = OTP internal memory address >> 2) (02C0H>> 2 = b0H)
root@smarc-rzg2l:~# devmem2 0x1186000c w 0xb0   
/dev/mem opened.
Memory mapped at address 0xffffb1b05000.
Read at address  0x1186000C (0xffffb1b0500c): 0x00000000
Write at address 0x1186000C (0xffffb1b0500c): 0x000000B0, readback 0x000000B0

# set the write data in the OTP write data register (OTPDATAWR)
root@smarc-rzg2l:~# devmem2 0x11860010 w 0xaaaa5559  
/dev/mem opened.
Memory mapped at address 0xffffa31c6000.
Read at address  0x11860010 (0xffffa31c6010): 0x00000000
Write at address 0x11860010 (0xffffa31c6010): 0xAAAA5559, readback 0xAAAA5559

# set the STAWR bit to 1 in the OTP write start register (OTPSTAWR)
root@smarc-rzg2l:~# devmem2 0x11860008 w 0x1  
/dev/mem opened.
Memory mapped at address 0xffffa7b39000.
Read at address  0x11860008 (0xffffa7b39008): 0x00000000
Write at address 0x11860008 (0xffffa7b39008): 0x00000001, readback 0x00000001

```
