#ifndef PTA_OTP_H_
#define PTA_OTP_H_

#define PTA_OTP_UUID                                       \
    {                                                      \
        0xde8eeecb, 0xb87b, 0x4018,                        \
        {                                                  \
            0xa3, 0xeb, 0x65, 0xb7, 0x7e, 0x64, 0x14, 0xb5 \
        }                                                  \
    }

/*
 * Read OTP 
 * [in]     param[0].value.a   OTP address
 * [out]    param[1].value.a   OTP value
 */
#define PTA_OTP_CMD_READ 0

/*
 * Write OTP 
 * [in]     param[0].value.a   OTP address
 * [in]     param[1].value.a   OTP value
 */
#define PTA_OTP_CMD_WRITE 1

#endif /* PTA_OTP_H_ */