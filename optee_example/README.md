## Sample example to access OTP from Linux userspace
This repository includes the source files required to access OTP from Linux using OPTEE-OS. 

You can find more information about OPTEE-OS on [OPTEE-OS-Note](optee_os_note.md)
## Instructions to build the OTP Pseudo Trusted application (PTA) in rzg_optee_os

### Build OPTEE-OS with RZ BSP :
Starting rzg_vlp_v3.0.6, OPTEE OS is moved from meta-renesas to meta-rz-features in RZ MPU Security Package for RZ/G v1.4.3. 
Get the security package from Renesas Official Site https://www.renesas.com/us/en/software-tool/rz-mpu-security-package#overview
<pre>
$ tar zxvf ~/rzg_vlp_security.tar.gz -C ~/ # decompress security package
$ cd ~/rzg_vlp_&lt;package_version&gt
$ tar zxvf ~/rzg_vlp_security/meta-rz-features.tar.gz
$ cd ~/rzg_vlp_&lt;package_version&gt/build
$ bitbake-layers add-layer ../meta-rz-features/meta-rz-security # add security layer
$ echo ENABLE_SPD_OPTEE = \"1\" >> conf/local.conf # enable OPTEE
$ echo TRUSTED_BOARD_BOOT = \"1\" >> conf/local.conf # enable secure boot
</pre>
This setting enables OPTEE and secure boot and build OPTEE OS

### Prepare source code to build OTP PTA with yocto for RZ/G2L :
Once BSP is built with OPTEE-OS, you can find the optee-os source files in location **"rzg_vlp_&lt;package_version&gt;/build/tmp/work/smarc_rzg2l-poky-linux/optee-os/3.19.0+gitAUTOINC+c6e430d7b8-r0/git/"**

The folder structure for G2L is:
<pre>
optee-os
├── core
    ├── arch
        ├── arm
            ├── plat-rz
                ├── g2l
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

### Prepare source code to build OTP PTA with yocto for RZ/G3S :
Once BSP is built with OPTEE-OS, you can find the optee-os source files in location **"rzg_vlp_&lt;package_version&gt;/build/tmp/work/smarc_rzg3s-poky-linux/optee-os/3.19.0+gitAUTOINC+ff9f077ed9-r0/git/"**

The folder structure for G3S is:
<pre>
optee-os
├── core
    ├── arch
        ├── arm
            ├── plat-rz
                ├── g3s
                    ├── drivers
                    └── pta

</pre>

As seen in the folder structure, any source codes for the application are included inside "drivers" and "pta".
For OTP PTA application, all the drivers code for OTP access (read/write) are included in the "drivers" folder and
all the code for Pseudo TA  are included in "pta" folder. 

Currently, for G3S, there is no "pta" folder, so create directory "pta"

Please copy files from source_files in the respective folder. The folder structure looks like below after adding files.
Please update the Makefiles that needs to be updated once source files are copied.
<pre>
g3s
├── drivers
│   ├── otp.c
│   ├── otp.h
│   └── sub.mk            
└── pta
│   ├── include
│   │   └── pta_otp.h
│   ├── pta_otp.c
│	└── sub.mk
└── sub.mk
</pre>

Please make following change in Makefiles:

for "g3s/drivers/sub.mk", please add below line
<pre>
  srcs-y += otp.c
</pre>
To include "pta" folder in the build, for "g3s/sub.mk"
<pre>
  global-incdirs-y += .
  subdirs-y += drivers pta
</pre>
for "g3s/pta/sub.mk", please add below line
<pre>
  srcs-y += pta_otp.c
</pre>

### How to build and deploy OPTEE-OS in yocto for RZ/G2L,G3S :

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

