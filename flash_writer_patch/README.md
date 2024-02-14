## Patch file to access OTP from falsh writer in RZ/G2L
This is the OTP path file to access OTP in flashwriter.
##Instructions:
This will describe how to apply patch file and build RZ/G2L flash writer
### Yocto SDK
You must install a Yocto SDK for RZG2L
<pre>
  $ source /opt/poky/3.1.5/environment-setup-aarch64-poky-linux
</pre>
### Prepare the source code
<pre>
  $ git clone https://github.com/renesas-rz/rzg2_flash_writer.git
  $ cd rzg2_flash_writer
  $ git checkout rz_g2l
  $ wget https://github.com/renesas-rz/rz_otp/blob/main/flash_writer_patch/0001-add-OTP-support.patch
  $ patch -p1 < 0001-Add-OTP-support.patch
</pre>
### Build the RZ/G2L flash writer
Yocto SDK:

<pre>
  make clean
  make BOARD=RZG2L_SMARC_PMIC
</pre>

Output image will be available in the following directory.

* /AArch64_output/Flash_Writer_SCIF_RZG2L_SMARC_PMIC_DDR4_2GB_1PCS.mot

### Run RZ/G2L flash writer
The instructions for setting up the board and writing the flash writer to the board are located in the **Board Start-up Guide** that comes in the BSP download package.

After the transfer has succeeded, the following log will be shown.
<pre>
Flash writer for RZ/G2 Series V1.06 Aug.10,2022
 Product Code : RZ/G2L
 >
</pre>
### Execute examples
<pre>
Flash writer for RZ/G2 Series V1.06 Aug.10,2022
 Product Code : RZ/G2L
 >h
        SPI Flash write command
 XCS            erase program to SPI Flash
 XLS2           write program to SPI Flash
 XLS3           write program to SPI Flash(Binary)

        eMMC write command
 EM_DCID        display register CID
 EM_DCSD        display register CSD
 EM_DECSD       display register EXT_CSD
 EM_SECSD       change register EXT_CSD byte
 EM_W           write program to eMMC
 EM_WB          write program to eMMC (Binary)
 EM_E           erase program to eMMC

 SUP            Scif speed UP (Change to speed up baud rate setting)
 SUD            Scif speed DOWN (Change to speed down baud rate setting)
 RESET          Perform RESET of the CPU
 H              help
 D  {sadr {eadr}}          memory dump  (DM sets dump size)
 DM {B|W|L|X}              set&disp dump mode
 M  [adr]                  set memory(BYTE)
 MW [adr]                  set memory(WORD)
 ML [adr]                  set memory(LONG)
 MX [adr]                  set memory(LONG LONG)
 F  [sadr] [eadr] [data]   fill memory
 FL [sadr] [eadr] [data]   fill memory(LONG)
 FX [sadr] [eadr] [data]   fill memory(LONG LONG)
         One Time PROM acccess command
 OTP_W          write OTP
 OTP_R          read OTP
 OTP_D          read OTP 64byte
  
>otp_w
===== Write data to OTP ============
===== Please Input Address ============
  Please Input : H'200
===== Please Input Data ============
  Please Input : H'aaaa5555
 Read data : H'AAAA5555
=======================================
>otp_r
===== Read data from OTP ============
===== Please Input Address ============
  Please Input : H'200
 Read data : H'AAAA5555
=======================================
>otp_d
===== Read data from OTP ============
===== Please Input Address ============
  Please Input : H'200
 Read data : H'AAAA5555
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
 Read data : H'00000000
</pre>

