### Sample Trusted Applcation (TA) to access OTP in Linux using OPTEE-OS 

### How to build :
This section describes how to build OTP access(read/write) TA
#### Prepare the compiler
Get Yocto SDK for RZ/G2L,G3S with OPTEE-OS built
<pre>
  source /opt/poky/3.1.21/environment-setup-aarch64-poky-linux
</pre>
#### Prepare the source code
<pre>
  $ git clone https://github.com/renesas-rz/rz_otp.git
  $ cd rz_otp
  $ cd rzg_optee-ta_otp
  $ make clean
  $ make
</pre>
The executable file will be available in the following directory:
* Client Application: out/ca
  
### How to execute :
copy the executale file to the evaluation board
##### Client Application
<pre>
  - Executable file:
      rzg_otp
  - Location on the board:
      /usr/bin/
</pre>
##### Run the application
<pre>
 $ rzg_otp
  Usage:
    rzg_otp read &lt;address&gt [count]
      - Read a value(s) from OTP at offset 'address'
      - Optional argument 'count' specifies the number of values to read (default is 1)
    rzg_otp write &lt;address&gt &lt;value&gt
      - Write a value to OTP at offset 'address'
  $ rzg_otp read 0x200
    0x200: 0xAAAA5555
  $ rzg_otp read 0x200 5  
    0x200: 0xAAAA5555
    0x204: 0xAAAA5510
    0x208: 0xAAAA5511
    0x20C: 0xAAAA5512
    0x210: 0xAAAA5513
  $ rzg_otp write 0x224 aaaa5517
    Write successful at address 0x224 with value 0xAAAA5517
  $ rzg_otp read 0x224
    0x224: 0xAAAA5517
</pre>
