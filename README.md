# RZ OTP Access for RZ/G2L,V2L,G3S
This repositoy contains the instructions to access OTP (read/write) from flashwriter, uboot and Linux.  

## OTP Memory Map
The Base address of the OTP for both RZ/G2L and RZ/G3S is 0x11860000. OTP registers can only be accessed in 32-bit.

RZ/G2L: The Offset Address 

User Area (One Time Read) <b>0x0200-0x0400</b> and User Area <b>0x580-0x800</b>

RZ/G3S: The Offset Address 

User Area (One Time Read) <b>0x0200 - 0x027F</b> and User Area <b>0x0380 - 0x03FF </b>
## What is included:
### flash_writer_patch
* Patch for flash writer to access OTP
### optee_example
* Sample code to access OTP in Linux using OPTEE OS (secure access only)
### u-boot_fuse
* Patch files to access OTP in u-boot using fuse command (secure access only)
### u-boot (Security Disabled)
Instructions to access OTP in uboot using md command for RZ/G2L, RZ/V2L and RZ/G3S
  
Please refer to file [uboot_access_otp_non-secure.md](uboot_access_otp_non-secure.md)
### Linux (Security Disabled)
Instructions to access OTP in Linux using devmem2 command for RZ/G2L, RZ/V2L and RZ/G3S
  
Please refer to file [linux_access_otp_non-secure.md](linux_access_otp_non-secure.md)

### Note for Security Disabled
#### For RZ/G2L :
Slave Access Control Register 7 (SYS_SLVACCCTL7) sets the security level for determining whether to enable or disable access to each set of registers in the OTP.

This is hard coded in the array sys_acctl[] in file plat/renesas/rz/common/plat_security.c.

Please comment out the register SYS_SLVACCCTL7 in TF_A code which puts the OTP in "Trust Zone" and re-program TF_A.

<pre>
  static const struct {
	uint32_t reg;
	uint32_t msk;
	uint32_t val;
} sys_acctl[] = {
	/* Master Access Control Register */
	{SYS_MSTACCCTL0,  0x00BB00BBU, 0x00AAAA88U},
	{SYS_MSTACCCTL1,  0xBBBBBBBBU, 0xAAAAAAAAU},
	{SYS_MSTACCCTL2,  0x00BBBBBBU, 0x00AAAAAAU},
	{SYS_MSTACCCTL3,  0x00BBBBBBU, 0x00AAAAAAU},
	{SYS_MSTACCCTL4,  0x0B0B00BBU, 0x0A0A00AAU},
	{SYS_MSTACCCTL5,  0x00000000U, 0x00000000U},
	/* Slave Access Control Register */
	{SYS_SLVACCCTL0,  0x0000000FU, 0x00000008U},
	{SYS_SLVACCCTL1,  0x3FFF3CFFU, 0x0800C0AAU},
	{SYS_SLVACCCTL2,  0x00FFCFFFU, 0x00000002U},
	{SYS_SLVACCCTL3,  0x3FFF3FFFU, 0x00000000U},
	{SYS_SLVACCCTL4,  0xFFFF0FFFU, 0x00000000U},
	{SYS_SLVACCCTL5,  0x00000033U, 0x00000000U},
	{SYS_SLVACCCTL6,  0x0000000FU, 0x00000000U},
//	{SYS_SLVACCCTL7,  0x0000000CU, 0x00000008U},
	{SYS_SLVACCCTL8,  0x0000000FU, 0x00000000U},
	{SYS_SLVACCCTL9,  0x00000000U, 0x00000000U},
	{SYS_SLVACCCTL10, 0x00000003U, 0x00000000U},
	{SYS_SLVACCCTL11, 0x00000000U, 0x00000000U},
	{SYS_SLVACCCTL12, 0x00000003U, 0x00000000U},
	{SYS_SLVACCCTL13, 0x00000003U, 0x00000000U},
	{SYS_SLVACCCTL14, 0x00000003U, 0x00000000U},
	{SYS_SLVACCCTL15, 0x00000000U, 0x00000000U},
};
</pre>

#### For RZ/G3S :
Slave Access Control Register 11 (SYS_SLVACCCTL11) sets the security level for determining whether to enable or disable access to each set of registers in the OTP.

This is hard coded in the array sys_acctl[] in file /plat/renesas/rz/soc/g3s/plat_security.c

Please comment out the register SYS_SLVACCCTL11 in TF_A code which puts the OTP in "Trust Zone" and re-program TF_A.

<pre>
  static const struct {
	uint32_t reg;
	uint32_t msk;
	uint32_t val;
} sys_acctl[] = {
	/* Master Access Control Register */
	{SYS_MSTACCCTL0,  0x00BB00BBU, 0x00AAAA88U},
	{SYS_MSTACCCTL1,  0xBBBBBBBBU, 0xAAAAAAAAU},
	{SYS_MSTACCCTL2,  0x00BBBBBBU, 0x00AAAAAAU},
	{SYS_MSTACCCTL3,  0x00BBBBBBU, 0x00AAAAAAU},
	{SYS_MSTACCCTL4,  0x0B0B00BBU, 0x0A0A00AAU},
	{SYS_MSTACCCTL5,  0x00000000U, 0x00000000U},
	/* Slave Access Control Register */
	{SYS_SLVACCCTL0,  0x0000000FU, 0x00000008U},
	{SYS_SLVACCCTL1,  0x3FFF3CFFU, 0x0800C0AAU},
	{SYS_SLVACCCTL2,  0x00FFCFFFU, 0x00000002U},
	{SYS_SLVACCCTL3,  0x3FFF3FFFU, 0x00000000U},
	{SYS_SLVACCCTL4,  0xFFFF0FFFU, 0x00000000U},
	{SYS_SLVACCCTL5,  0x00000033U, 0x00000000U},
	{SYS_SLVACCCTL6,  0x0000000FU, 0x00000000U},
	{SYS_SLVACCCTL7,  0x0000000CU, 0x00000008U},
	{SYS_SLVACCCTL8,  0x0000000FU, 0x00000000U},
	{SYS_SLVACCCTL9,  0x00000000U, 0x00000000U},
	{SYS_SLVACCCTL10, 0x00000003U, 0x00000000U},
//	{SYS_SLVACCCTL11, 0x00000000U, 0x00000000U},
	{SYS_SLVACCCTL12, 0x00000003U, 0x00000000U},
	{SYS_SLVACCCTL13, 0x00000003U, 0x00000000U},
	{SYS_SLVACCCTL14, 0x00000003U, 0x00000000U},
	{SYS_SLVACCCTL15, 0x00000000U, 0x00000000U},
};
</pre>
