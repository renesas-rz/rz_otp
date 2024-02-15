#ifndef OTP_H_
#define OTP_H_

#include <tee_api_types.h>

#define RZG2L_OTP_BASE      (0x11860000)
#define RZ_OTP_PWR          0x0000
#define RZ_OTP_STR 		    0x0004
#define RZ_OTP_STAWR 		0x0008
#define RZ_OTP_ADRWR 		0x000c
#define RZ_OTP_DATAWR 		0x0010
#define RZ_OTP_ADRRD 		0x0014
#define RZ_OTP_DATARD 		0x0018
#define RZ_OTP_FLAG 		0x001c

#define OTP_PWR 		BIT(0)
#define ERR_WR_1 		BIT(1)
#define ERR_WR_2 		BIT(2)
#define ERR_WP	 		BIT(3)
#define OTP_ACCL 		BIT(4)
#define ERR_RDY_WR 		BIT(8)

/* Open the OTP */
extern int32_t otp_open(void);
/* Close the OTP */
extern void otp_close(void);
/* Read the value from the OTP */
TEE_Result otp_read (uint32_t addr, uint32_t *val);
/* Write the value to OTP */
TEE_Result otp_write(uint32_t addr, uint32_t val);
#endif /* OTP_H_ */