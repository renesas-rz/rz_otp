#include <compiler.h>
#include <stdio.h>
#include <trace.h>
#include <kernel/pseudo_ta.h>
#include <mm/tee_pager.h>
#include <mm/tee_mm.h>
#include <string.h>
#include <string_ext.h>
#include <malloc.h>
#include <pta_otp.h>
#include <otp.h>

#define TA_NAME		"otp.ta"

/* Trusted Application Entry Points
 */
static TEE_Result create_ta(void)
{
	DMSG("create entry point for pseudo TA \"%s\"", TA_NAME);
	return TEE_SUCCESS;
}

static void destroy_ta(void)
{
	DMSG("destroy entry point for pseudo ta \"%s\"", TA_NAME);
}

static TEE_Result open_session(uint32_t param_types,
							   TEE_Param params[4],
							  void **session_context)
{
	DMSG("open entry point for pseudo ta \"%s\"", TA_NAME);
	return TEE_SUCCESS;
}

static void close_session(void *session_context)
{
	DMSG("close entry point for pseudo ta \"%s\"", TA_NAME);
}

static TEE_Result invoke_command(void *session_context,
								 uint32_t cmd,uint32_t param_types,
								 TEE_Param params[4])
{
	switch (cmd) {
	case  PTA_OTP_CMD_WRITE:
		if (param_types != TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
											TEE_PARAM_TYPE_VALUE_INPUT,
                                            TEE_PARAM_TYPE_NONE,
											TEE_PARAM_TYPE_NONE)) {
        	return TEE_ERROR_BAD_PARAMETERS;
        }
        return otp_write(params[0].value.a, params[1].value.a);
	
	case PTA_OTP_CMD_READ:
		if (param_types != TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
		 									TEE_PARAM_TYPE_VALUE_OUTPUT,
                                            TEE_PARAM_TYPE_NONE,
											TEE_PARAM_TYPE_NONE)) {
            return TEE_ERROR_BAD_PARAMETERS;
        }
        return otp_read(params[0].value.a, &params[1].value.a);

	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}

pseudo_ta_register(.uuid = PTA_OTP_UUID, .name = TA_NAME,
				   .flags = PTA_DEFAULT_FLAGS,
				   .create_entry_point = create_ta,
				   .destroy_entry_point = destroy_ta,
				   .open_session_entry_point = open_session,
				   .close_session_entry_point = close_session,
				   .invoke_command_entry_point = invoke_command);
