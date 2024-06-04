## uboot_fuse
This repository includes the source files and instructios to build fuse command for RZ/G2L and RZ/G3S to access OTP in u-boot.

### u-boot access to OPTEE and fuse command
The 'fuse' command u-boot is commonly used to read and program fuses for various purposes such as board configuration,
security seeting and device identification. In our case, the 'fuse' command is used to read and write to one-time 
programmable (OTP) registers.

In u-boot, there is an API for fuse command (fuse_read, fuse_prog, fuse_sense, fuse_override) for interacting with fuses on a device. This API allows software to read,
write and configure fuse settings as needed for the specific application or use case. 

u-boot runs in non-secure world, which means to call OP-TEE services, it needs to change the exception level of the CPU.
To do this, u-boot has a [ARM SiP services](https://review.trustedfirmware.org/plugins/gitiles/TF-A/trusted-firmware-a/+/74de040f15ffb4110e8390a32fa41ba4217b14c9/docs/arm-sip-service.rst).

Execution State Switching service provides a mechanism for a non-secure lower Exception Level to request to switch its 
execution state. When doing  a Exception Level change, you need to put in and ID of 0x82000000 - 0x8200ffff. 
In our case, the following ID are used to access OTP register.
<pre>
#define RZ_SIP_SVC_READ_OTP         U(0x82000012)
#define RZ_SIP_SVC_WRITE_OTP        U(0x82000013)
</pre>
The fuse API then use SiP in order to pass the data and transition the Exception Level so OPTEE can run.

### TF-A
There is already Sample Code for SiP handling to get the Device ID and Chip ID in the trusted firmware , so we are extending
the same sample code to get OTP read/write data.

## Prepare Source Code for RZ/G2L
### TF-A (rzg_trusted-firmware-a):
Please make the following changes in TF_A
<pre>
$ git clone https://github.com/renesas-rz/rzg_trusted-firmware-a.git
$ git checkout v2.7/rz
</pre>
#### File: rzg_trusted-firmware-a/plat/renesas/common/common.mk
<pre>
PLAT_BL_COMMON_SOURCES	:=	drivers/renesas/common/iic_dvfs/iic_dvfs.c \
<b>+	drivers/delay_timer/delay_timer.c 	   \</b>
 			plat/renesas/common/rcar_common.c
</pre>
#### File: rzg_trusted-firmware-a/plat/renesas/rz/common/bl31_plat_setup.c
<pre>
#include &lt;common/bl_common.h&gt;
<b>+ #include &lt;drivers/generic_delay_timer.h&gt;</b>
#include &lt;lib/xlat_tables/xlat_tables_compat.h&gt; 
  
plat_security_setup();
<b>+ // generic timer setup</b>
<b>+ generic_delay_timer_init();</b>
</pre>
#### File: rzg_trusted-firmware-a/plat/renesas/rz/common/include/rz_sip_svc.h
<pre>
<b>+ /* Function ID to read from OTP */</b>
<b>+ #define RZ_SIP_SVC_READ_OTP     	U(0x82000012) </b>
<b>+ #define RZ_SIP_SVC_WRITE_OTP     	U(0x82000013) </b>
</pre>
#### File: rzg_trusted-firmware-a/plat/renesas/rz/common/rz_common.mk
<pre>
  BL31_SOURCES	+=	lib/cpus/aarch64/cortex_a55.S	\
 					${GICV3_SOURCES} \
<b>+			drivers/delay_timer/delay_timer.c	\</b>
<b>+			drivers/delay_timer/generic_delay_timer.c	\</b>
 					drivers/arm/tzc/tzc400.c \
					
</pre>

#### File: rzg_trusted-firmware-a/plat/renesas/rz/common/rz_plat_sip_handler.c

Please update the file with the codes from source_files [rz_plat_sip_handler.c](source_files/rz_plat_sip_handler.c)

### u-boot (renesas-u-boot-cip):

Please make the following changes in uboot
<pre>
  git clone https://github.com/renesas-rz/renesas-u-boot-cip.git
  git checkout v2021.10/rz
</pre>
#### File: renesas-u-boot-cip/arch/arm/mach-rmobile/sip.c
Download sip.c file inside directory "renesas-u-boot-cip/arch/arm/mach-rmobile/"
<pre>
  wget https://raw.githubusercontent.com/renesas-rz/rz_otp/main/uboot_fuse/source_files/sip.c
</pre> 
#### File: renesas-u-boot-cip/arch/arm/mach-rmobile/Makefile
<pre>
  <b>+ obj-y += sip.o</b>
</pre>
#### File: renesas-u-boot-cip/arch/arm/mach-rmobile/include/mach/sys_proto.h
<pre>
<b> +unsigned long call_rzg2l_sip(unsigned long id, unsigned long reg0,</b>
<b> +			     unsigned long reg1, unsigned long reg2,</b>
<b> +			     unsigned long reg3);</b>
<b> +unsigned long call_rzg2l_sip_ret2(unsigned long id, unsigned long reg0,</b>
<b> +				  unsigned long *reg1, unsigned long reg2,</b>
<b> +				  unsigned long reg3);</b>
</pre>
#### File: renesas-u-boot-cip/configs/smarc-rzg2l_defconfig
<pre>
<b>+ CONFIG_CMD_FUSE=y</b>
<b>+ CONFIG_MISC=y</b>
</pre>
#### File: renesas-u-boot-cip/drivers/misc/Kconfig
<pre>
<b>+ config RZG2L_FUSE</b>
<b>+	bool "Enable RZ/G2L fuse wrapper providing the fuse API"</b>
<b>+	depends on MISC</b>
<b>+	default y if CMD_FUSE</b>
<b>+	help</b>
<b>+	  If you say Y here, you will get support for the fuse API (OTP)</b>
<b>+	  for RZ/G2L architecture.</b>
<b>+	  This API is needed for CMD_FUSE.</b>
</pre>
#### File: renesas-u-boot-cip/drivers/misc/rzg2l_fuse.c
Download rzg2l_fuse.c file inside directory "renesas-u-boot-cip/drivers/misc/"
<pre>
  wget https://raw.githubusercontent.com/renesas-rz/rz_otp/main/uboot_fuse/source_files/rzg2l_fuse.c
</pre>
#### File: renesas-u-boot-cip/drivers/misc/Makefile
<pre>
<b> + obj-$(CONFIG_RZG2L_FUSE) += rzg2l_fuse.o</b>
</pre>

## Prepare Source Code for RZ/G3S

### TF-A (rzg_trusted-firmware-a):
Please make the following changes in TF_A based on rzg3s_bsp_v1.0.0
<pre>
$ git clone https://github.com/renesas-rz/rzg_trusted-firmware-a.git
$ git checkout 2.7.0/g3s_1.0.0_rc2
</pre>
#### File: rzg_trusted-firmware-a/plat/renesas/common/common.mk
<pre>
PLAT_BL_COMMON_SOURCES	:=	drivers/renesas/common/iic_dvfs/iic_dvfs.c \
<b>+	drivers/delay_timer/delay_timer.c 	   \</b>
 			plat/renesas/common/rcar_common.c
</pre>
#### File: rzg_trusted-firmware-a/plat/renesas/rz/soc/g3s/bl31_plat_setup.c
<pre>
#include &lt;common/bl_common.h&gt;
<b>+ #include &lt;drivers/generic_delay_timer.h&gt;</b>
#include &lt;lib/xlat_tables/xlat_tables_compat.h&gt; 
	
<b>+ // generic timer setup</b>
<b>+ generic_delay_timer_init();</b>
</pre>
#### File: rzg_trusted-firmware-a/plat/renesas/rz/common/include/rz_sip_svc.h
<pre>
<b>+ /* Function ID to read from OTP */</b>
<b>+ #define RZ_SIP_SVC_READ_OTP     	U(0x82000022) </b>
<b>+ #define RZ_SIP_SVC_WRITE_OTP     	U(0x82000023) </b>
</pre>
#### File: rzg_trusted-firmware-a/plat/renesas/rz/soc/g3s/platform.mk
<pre>
  BL31_SOURCES	+=	plat/renesas/rz/soc/g3s/bl31_plat_setup.c	\
			plat/renesas/rz/soc/g3s/plat_pm.c               \
<b>+			drivers/delay_timer/delay_timer.c		\
<b>+			drivers/delay_timer/generic_delay_timer.c								
</pre>

#### File: rzg_trusted-firmware-a/plat/renesas/rz/common/rz_plat_sip_handler.c

Please update the file with the codes from source_files [rz_plat_sip_handler.c](source_files/rz_plat_sip_handler_g3s.c)

### u-boot (renesas-u-boot-cip):

Please make the following changes in uboot
<pre>
  git clone https://github.com/renesas-rz/renesas-u-boot-cip.git
  git checkout 3cc7decb2851a1f3e0e4d4581e3113bfdd791acc
</pre>
#### File: renesas-u-boot-cip/arch/arm/mach-rmobile/sip.c
Download sip.c file inside directory "renesas-u-boot-cip/arch/arm/mach-rmobile/"
<pre>
  wget https://raw.githubusercontent.com/renesas-rz/rz_otp/main/uboot_fuse/source_files/sip.c
</pre> 
#### File: renesas-u-boot-cip/arch/arm/mach-rmobile/Makefile
<pre>
  <b>+ obj-y += sip.o</b>
</pre>
#### File: renesas-u-boot-cip/arch/arm/mach-rmobile/include/mach/sys_proto.h
<pre>
<b> +unsigned long call_rzg2l_sip(unsigned long id, unsigned long reg0,</b>
<b> +			     unsigned long reg1, unsigned long reg2,</b>
<b> +			     unsigned long reg3);</b>
<b> +unsigned long call_rzg2l_sip_ret2(unsigned long id, unsigned long reg0,</b>
<b> +				  unsigned long *reg1, unsigned long reg2,</b>
<b> +				  unsigned long reg3);</b>
</pre>
#### File: renesas-u-boot-cip/configs/smarc-rzg3s_defconfig
<pre>
<b>+ CONFIG_CMD_FUSE=y</b>
<b>+ CONFIG_MISC=y</b>
</pre>
#### File: renesas-u-boot-cip/drivers/misc/Kconfig
<pre>
<b>+ config RZG3S_FUSE</b>
<b>+	bool "Enable RZ/G3S fuse wrapper providing the fuse API"</b>
<b>+	depends on MISC</b>
<b>+	default y if CMD_FUSE</b>
<b>+	help</b>
<b>+	  If you say Y here, you will get support for the fuse API (OTP)</b>
<b>+	  for RZ/G3S architecture.</b>
<b>+	  This API is needed for CMD_FUSE.</b>
</pre>
#### File: renesas-u-boot-cip/drivers/misc/rzg2l_fuse.c
Download rzg2l_fuse.c file inside directory "renesas-u-boot-cip/drivers/misc/"
<pre>
wget https://raw.githubusercontent.com/renesas-rz/rz_otp/main/uboot_fuse/source_files/rzg2l_fuse.
change OTP_READ and WRITE ID:
#define RZG_SIP_OTP_READ             0x82000022
#define RZG_SIP_OTP_WRITE            0x82000023
</pre>

#### File: renesas-u-boot-cip/drivers/misc/Makefile
<pre>
<b> + obj-$(CONFIG_RZG3S_FUSE) += rzg2l_fuse.o</b>
</pre>

### How to Build
Please follow instruction from [rzg2_bsp_scripts](https://github.com/renesas-rz/rzg2_bsp_scripts/tree/master/build_scripts)
to build trusted firmware and u-boot.
### How to Run:
Once build is finished, please program the board with the updated firmware.
### Sample Run:
<pre>
=> fuse
fuse - Fuse sub-system

Usage:
fuse read &lt;bank&gt &lt;word&gt [&lt;cnt&gt] - read 1 or 'cnt' fuse words,
    starting at 'word'
fuse sense &lt;bank&gt &lt;word&gt [&lt;cnt&gt] - sense 1 or 'cnt' fuse words,
    starting at 'word'
fuse prog [-y] &lt;bank&gt &lt;word&gt &lt;hexval&gt [&lt;hexval&gt...] - program 1 or
    several fuse words, starting at 'word' (PERMANENT)
fuse override &lt;bank&gt &lt;word&gt &lt;hexval&gt [&lt;hexval&gt...] - override 1 or
    several fuse words, starting at 'word'
=> fuse read 0 0x200
Reading bank 0:

Word 0x00000200: aaaa5555
=> fuse prog 0 0x404 0xaaaa5558
Programming bank 0 word 0x00000404 to 0xaaaa5558...
Warning: Programming fuses is an irreversible operation!
         This may brick your system.
         Use this command only if you are sure of what you are doing!

Really perform this fuse programming? &lt;y/N&gt
y
=> fuse read 0 0x404 
Reading bank 0:

Word 0x00000404: aaaa5558
</pre>
