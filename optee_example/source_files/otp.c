#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <io.h>
#include <initcall.h>
#include <mm/core_memprot.h>
#include <platform_config.h>
#include <arm.h>
#include <mm/core_mmu.h>
#include <kernel/tee_common_otp.h>
#include <otp.h>

#define OTP_DUMMY_READ	(0x400 >> 2)

static void otp_dummy_read(void);

static vaddr_t otp_base;

inline static uint32_t otp_io_read(uint32_t reg)
{
	return io_read32(otp_base + reg);
}

inline static void otp_io_write(uint32_t reg, uint32_t data)
{
	io_write32(otp_base + reg, data);
}

int32_t otp_open(void)
{
	int32_t ret_val = TEE_SUCCESS;
	if ( otp_io_read(RZ_OTP_PWR) & 1) {
		// already powered up
		DMSG("OTP already powered up.\n");
		ret_val = TEE_ERROR_BAD_STATE;
	} else {
		while (( otp_io_read(RZ_OTP_STR) & 1) || ( otp_io_read(RZ_OTP_FLAG) & 1) == 0) {
			;
		}
		 otp_io_write(RZ_OTP_PWR,  otp_io_read(RZ_OTP_PWR) | OTP_PWR | OTP_ACCL);
	}

	return ret_val;
}

void otp_dummy_read(void)
{
	// poll ready
	while (( otp_io_read(RZ_OTP_STR) & 1) == 0) {
		; /* polling */
	}

	 otp_io_write(RZ_OTP_ADRRD, OTP_DUMMY_READ);
	 otp_io_read(RZ_OTP_DATARD);
}

void otp_close(void)
{
	otp_dummy_read();

	// pwr down
	otp_io_write(RZ_OTP_PWR,  otp_io_read(RZ_OTP_PWR) & OTP_PWR & OTP_ACCL);

	// poll until ready state left
	while (( otp_io_read(RZ_OTP_STR) & 1)) {
		; /* wait */
	}

	return;
}

TEE_Result otp_read(uint32_t addr, uint32_t *val)
{
	TEE_Result ret_val = TEE_SUCCESS;

	if (!val)
		return TEE_ERROR_BAD_PARAMETERS;

	if ((0 != (addr & 0x03)) || ((addr >> 2) > 0x1FF)){
		return TEE_ERROR_BAD_PARAMETERS;
    }

	ret_val = otp_open();
	if(ret_val == TEE_SUCCESS){

		while ((otp_io_read(RZ_OTP_STR) & 1) == 0) {
			; //polling
		}

		otp_io_write(RZ_OTP_ADRRD, (addr >> 2));
		*val = otp_io_read(RZ_OTP_DATARD);
		otp_close();
	}
	return ret_val;
}

TEE_Result otp_write(uint32_t addr, uint32_t val)
{
	
	int32_t ret_val = TEE_SUCCESS;
	uint32_t otpval;

	if ((0 != (addr & 0x03)) || ((addr >> 2) > 0x1FF)){
		return TEE_ERROR_BAD_PARAMETERS;
    }

	ret_val = otp_open();
	if (ret_val == TEE_SUCCESS){

		// poll ready
		while ((otp_io_read(RZ_OTP_STR) & 1) == 0) {
			; /* wait */
		}

		otp_io_write(RZ_OTP_ADRWR, addr >> 2);
		otp_io_write(RZ_OTP_DATAWR, val);

		// start write
		otp_io_write(RZ_OTP_STAWR, 1);

		// write was performed when not ready
		if (otp_io_read(RZ_OTP_STR) & ERR_RDY_WR) {
			EMSG("Was not ready to write.\n");
			// clear the error
			otp_io_write(RZ_OTP_STR, otp_io_read(RZ_OTP_STR) & ERR_RDY_WR);
			ret_val = TEE_ERROR_BAD_STATE;
		} else {
			// poll until write finished
			while ((otp_io_read(RZ_OTP_STR) & 1) == 0 || (otp_io_read(RZ_OTP_STAWR) & 1)) {
				; /* wait */
			}

			if ((otp_io_read(RZ_OTP_STR) & ERR_WP) ||
				(otp_io_read(RZ_OTP_STR) & ERR_WR_1) ||
				(otp_io_read(RZ_OTP_STR) & ERR_WR_2)){
				EMSG("OTP write error - protected.\n");
				ret_val = TEE_ERROR_ACCESS_DENIED;
			} else {
				otp_io_write(RZ_OTP_ADRRD, addr >> 2);
				otpval = otp_io_read(RZ_OTP_DATARD);

				if(otpval != val) {
					EMSG("Read different value than written to OTP.\n");
					ret_val = TEE_ERROR_BAD_PARAMETERS;
				}
			}
		}
		otp_close();
	}
	return ret_val;
}

static TEE_Result otp_init(void)
{
	otp_base = core_mmu_get_va(RZG2L_OTP_BASE, MEM_AREA_IO_SEC, 0x4000);
	if (!otp_base)
		return TEE_ERROR_GENERIC;

	return TEE_SUCCESS;
}
service_init(otp_init);