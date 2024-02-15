## Instructions to build the OTP Pseudo Trusted application (PTA) in rzg_optee_os

### Build OPTEE-OS with RZ/G2 BSP :
To enable OPTEE-OS build with RZ/G2 BSP, set the line in

 **"~rzg_vlp_\<package_version\>/meta-renesas/meta-rz-common/include/rzg2l-security-config.inc"** as follows.
  <pre>
    # Enable build and load of OP-TEE
    ENABLE_SPD_OPTEE            ?= '1'
  </pre>
This setting enables OPTEE and build OPTEE OS.

### Prepare source code to build OTP PTA with yocto :
Once BSP is built with OPTEE-OS, you can find the optee-os source files in location **"rzg_vlp_&lt;package_version&gt/build/tmp/work/smarc_rzg2l-poky-linux/optee-os/3.19.0+gitAUTOINC+c6e430d7b8-r0/git/"**

<pre>
The folder structure for G2L is:
├── core
    ├── arch
		├── arm
			├── plat-rz
				└── g2l
					├── drivers
					└── pta

</pre>

As seen in the folder structure, any source codes for the application are included inside "drivers" and "pta".
For OTP PTA application, all the drivers code for OTP access (read/write) are included in the "drivers" folder and
all the code for Pseudo TA  are included in "pta" folder. 

Please copy files from source_files in the respective folder. The folder structure looks like below after adding files.
There are two Makefiles that needs to be updated once source files are copied.
<pre>
g2l
├── drivers
│   ├── otp.c
│   ├── otp.h
│   └── sub.mk            
└── pta
    ├── include
    │   └── pta_otp.h
    ├── pta_otp.c
	  └── sub.mk
</pre>

Please make following change in Makefiles:

for "g2l/drivers/sub.mk", please add below line
<pre>
  srcs-y += otp.c
</pre>
for "g2l/pta/sub.mk", please add below line
<pre>
  srcs-y += pta_otp.c
</pre>

### How to build and deploy OPTEE-OS in yocto :

To build OPTEE-OS :

<pre>
  $ ~/rzg_vlp_&lt;package_version&gt
  $ source poky/oe-init-build-env 
  $ bitbake optee-os -fc compile
  $ bitbake optee-os  -fc install
  $ bitbake optee-os -fc populate_sysroot
</pre>

To build firmware with updated OPTEE-OS :

<pre>
  $ bitbake firmware-pack -fc compile
  $ bitbake firmware-pack  -fc deploy
</pre>

After successful build, reprogram the board with the updated firmware. 

Once board is programmed, test your program running client TA from

https://github.com/renesas-rz/rz_otp/tree/main/rzg_optee-ta_otp
