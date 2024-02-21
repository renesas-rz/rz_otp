#include <errno.h>
#include <drivers/delay_timer.h>
 
#define RZ_SYS_BASE_DEVID	(RZG2L_SYSC_BASE + 0x0A04)
#define RZ_OTP_BASE_DEVID	(RZG2L_OTP_BASE + 0x1178)
#define RZ_OTP_BASE_CHIPID	(RZG2L_OTP_BASE + 0x1140)
#define RZ_OTP_PWR 			RZG2L_OTP_BASE
#define RZ_OTP_STR 			(RZG2L_OTP_BASE + 0x0004)
#define RZ_OTP_STAWR 		(RZG2L_OTP_BASE + 0x0008)
#define RZ_OTP_ADRWR 		(RZG2L_OTP_BASE + 0x000c)
#define RZ_OTP_DATAWR 		(RZG2L_OTP_BASE + 0x0010)
#define RZ_OTP_ADRRD 		(RZG2L_OTP_BASE + 0x0014)
#define RZ_OTP_DATARD 		(RZG2L_OTP_BASE + 0x0018)
#define RZ_OTP_FLAG 		(RZG2L_OTP_BASE + 0x001c)

#define OTP_PWR 		BIT(0)
#define ERR_WR_1 		BIT(1)
#define ERR_WR_2 		BIT(2)
#define ERR_WP	 		BIT(3)
#define OTP_ACCL 		BIT(4)
#define ERR_RDY_WR 		BIT(8)
#define OTP_DUMMY_READ	(0x400 >> 2)

int32_t otp_open(void)
{
	int i = 0;

	if (mmio_read_32(RZ_OTP_PWR) & 1) {
		// already powered up
		INFO("OTP already powered up.\n");
	} else {
		while ((mmio_read_32(RZ_OTP_STR) & 1) || (mmio_read_32(RZ_OTP_FLAG) & 1) == 0) {
			i++;
			if (i > 1000) {
				ERROR("Timeout powering up OTP.\n");
				return -EINVAL;
			}
			mdelay(1);
		}
		mmio_write_32(RZ_OTP_PWR, mmio_read_32(RZ_OTP_PWR) | OTP_PWR | OTP_ACCL);
	}

	return 0;
}

static void otp_dummy_read(void)
{
	int i = 0;

	// poll ready
	while ((mmio_read_32(RZ_OTP_STR) & 1) == 0) {
		i++;
		if (i > 1000) {
			// just warn and no return code, bcs seems like dummy read is not really
			// needed for our purposes
			WARN("Timeout polling ready for dummy read.\n");
			break;
		}
		mdelay(1);
	}

	mmio_write_32(RZ_OTP_ADRRD, OTP_DUMMY_READ + RZG2L_OTP_BASE);
	mmio_read_32(RZ_OTP_DATARD);
}

int32_t otp_close(void)
{
	int i = 0;

	otp_dummy_read();

	// pwr down
	mmio_write_32(RZ_OTP_PWR, mmio_read_32(RZ_OTP_PWR) & OTP_PWR & OTP_ACCL);

	// poll until ready state left
	while ((mmio_read_32(RZ_OTP_STR) & 1)) {
		i++;
		if (i > 1000) {
			ERROR("Timeout polling for leaving ready state.\n");
			return -EINVAL;
		}
		mdelay(1);
	}

	return 0;
}

static uintptr_t rz_otp_fuse_read(void *handle, uint32_t word, uint32_t *val)
{
	int i = 0;
	int ret;

	*val = 0;

	ret = otp_open();
	if (ret < 0)
		return ret;

	// poll ready
	while ((mmio_read_32(RZ_OTP_STR) & 1) == 0) {
		i++;
		if (i > 1000) {
			ERROR("Timeout polling ready for OTP read.\n");
			return -EINVAL;
		}
		mdelay(1);
	}

	mmio_write_32(RZ_OTP_ADRRD, RZG2L_OTP_BASE + (word >> 2));
	*val = mmio_read_32(RZ_OTP_DATARD);

	ret = otp_close();
	return ret;
}

static uintptr_t rz_otp_fuse_write(void *handle, uint32_t word, uint32_t val)
{
	int i = 0;
	int ret, otpval;

	ret = otp_open();
	if (ret < 0)
		return ret;

	// poll ready
	while ((mmio_read_32(RZ_OTP_STR) & 1) == 0) {
		i++;
		if (i > 1000) {
			ERROR("Timeout polling ready for OTP write.\n");
			return -EINVAL;
		}
		mdelay(1);
	}

	mmio_write_32(RZ_OTP_ADRWR, RZG2L_OTP_BASE + (word >> 2));
	mmio_write_32(RZ_OTP_DATAWR, val);
	// start write
	mmio_write_32(RZ_OTP_STAWR, 1);

	// write was performed when not ready
	if (mmio_read_32(RZ_OTP_STR) & ERR_RDY_WR) {
		ERROR("Was not ready to write.\n");
		// clear the error
		mmio_write_32(RZ_OTP_STR, mmio_read_32(RZ_OTP_STR) & ERR_RDY_WR);
		return -EINVAL;
	} else {
		// poll until write finished
		i = 0;
		while ((mmio_read_32(RZ_OTP_STR) & 1) == 0 || (mmio_read_32(RZ_OTP_STAWR) & 1)) {
			i++;
			if (i > 1000) {
				ERROR("Timeout polling OTP write finished.\n");
				return -EINVAL;
			}
			mdelay(1);
		}

		if ((mmio_read_32(RZ_OTP_STR) & ERR_WP) ||
		    (mmio_read_32(RZ_OTP_STR) & ERR_WR_1) ||
		    (mmio_read_32(RZ_OTP_STR) & ERR_WR_2))
		{
			ERROR("OTP write error - protected.\n");
			return -EINVAL;
		} else {
			mmio_write_32(RZ_OTP_ADRRD, RZG2L_OTP_BASE + (word >> 2));
			otpval = mmio_read_32(RZ_OTP_DATARD);

			if(otpval != val) {
				ERROR("Read different value than written to OTP.\n");
				return -EINVAL;
			}
		}
	}

	ret = otp_close();
	return ret;
}
uintptr_t rz_plat_sip_handler(uint32_t smc_fid,
			      u_register_t x1,
			      u_register_t x2,
			      u_register_t x3,
			      u_register_t x4,
			      void *cookie,
			      void *handle,
			      u_register_t flags)
{
	int ret;
	uint32_t fuse;

	switch (smc_fid) {
		case RZ_SIP_SVC_GET_DEVID:
			return rz_otp_handler_devid(handle, x1);
		case RZ_SIP_SVC_GET_CHIPID:
			return rz_otp_handler_chipid(handle, x1, flags);
		case RZ_SIP_SVC_READ_OTP:
			ret = rz_otp_fuse_read(handle, x1, &fuse);
			SMC_RET2(handle, ret, fuse);
		case RZ_SIP_SVC_WRITE_OTP:
			ret = rz_otp_fuse_write(handle, x1, x2);
			SMC_RET1(handle, ret);
		default:
			WARN("%s: Unimplemented RZ SiP Service Call: 0x%x \n", __func__, smc_fid);
			SMC_RET1(handle, SMC_UNK);
	}
}
