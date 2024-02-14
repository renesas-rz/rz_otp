#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <tee_client_api.h>
#include <otp_ta.h>

typedef struct {
    TEEC_Context ctx;
    TEEC_Session sess;
} SessionContext;

void close_session_context(SessionContext *sc) 
{
    TEEC_CloseSession(&sc->sess);
    TEEC_FinalizeContext(&sc->ctx);
}

void prepare_tee_session(SessionContext *sc) 
{
    TEEC_Result result = TEEC_SUCCESS; // Initialize result

	TEEC_UUID uuid = PTA_OTP_UUID;

    // Initialize the TEE context
    result = TEEC_InitializeContext(NULL, &sc->ctx);
    if (result != TEEC_SUCCESS) {
        fprintf(stderr, "TEEC_InitializeContext failed with code 0x%x\n", result);
        return ;
    }

    // Open a session to the TA
    result = TEEC_OpenSession(&sc->ctx, &sc->sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, NULL);
    if (result != TEEC_SUCCESS) {
        fprintf(stderr, "TEEC_OpenSession failed with code 0x%x\n", result);
        return;
    }
}

TEEC_Result read_otp_value(int32_t address, int32_t *value, SessionContext *sc) 
{

	TEEC_Result res = TEEC_SUCCESS; // Initialize result

	prepare_tee_session(sc);

	TEEC_Operation op_read;
	uint32_t origin;

	/* Clear the TEEC_Operation struct */
	memset(&op_read, 0, sizeof(op_read));

	op_read.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_OUTPUT, 0, 0);
	op_read.params[0].value.a = address;

	res = TEEC_InvokeCommand(&sc->sess,  PTA_OTP_CMD_READ , &op_read, &origin);

	// Check for error after the operation
    if (res != TEEC_SUCCESS) {
		fprintf(stderr, "TEEC_InvokeCommand(OTP_READ) failed 0x%x origin 0x%x\n", res, origin);
		return res;
	}

	// Retrieve the result from the operation structure
	*value =  op_read.params[1].value.a;
	printf("0x%X: 0x%08X\n",address, *value);

	// Close the session and finalize the context

    return res;
}

TEEC_Result write_otp_value(int32_t address, int32_t value, SessionContext *sc) 
{

	TEEC_Result res = TEEC_SUCCESS; // Initialize result

	prepare_tee_session(sc);

	TEEC_Operation op_write;
	uint32_t origin;

	memset(&op_write, 0, sizeof(op_write));

	op_write.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_INPUT, 0, 0);
	op_write.params[0].value.a= address;
	op_write.params[1].value.a = value;

	res = TEEC_InvokeCommand(&sc->sess, PTA_OTP_CMD_WRITE, &op_write, &origin);
	if (res != TEEC_SUCCESS) {
		fprintf(stderr, "TEEC_InvokeCommand(OTP_WRITE) failed 0x%x origin 0x%x\n", res, origin);
        return res;
	}
   // Print success message
    printf("Write successful at address 0x%X with value 0x%08X\n", address, value);
    // Close the session and finalize the context
    return res;
}

int main(int argc, char *argv[])
{
	 if (argc < 3 || argc > 5) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s read <address> [count]\n", argv[0]);
		fprintf(stderr, "    - Read a value(s) from OTP at offset 'address'\n");
        fprintf(stderr, "    - Optional argument 'count' specifies the number of values to read (default is 1)\n");
        fprintf(stderr, "  %s write <address> <value>\n", argv[0]);
		fprintf(stderr, "    - Write a value to OTP at offset 'address'\n");
        return 1;
    }

	// Parse command-line arguments
    bool is_write = (strcmp(argv[1], "write") == 0);
    bool is_read = (strcmp(argv[1], "read") == 0);

    if ((!is_write && !is_read) || (is_read && argc != 3 && argc != 4) || (is_write && argc != 4)) {
        fprintf(stderr, "Invalid operation or incorrect number of arguments\n");
        return 1;
    }

    int32_t address, value;
    if (sscanf(argv[2], "%x", &address) != 1) {
        fprintf(stderr, "Invalid hexadecimal address input\n");
        return 1;
    }

	SessionContext sc;
    TEEC_Result res;
    memset(&sc, 0, sizeof(sc));

    if (is_write) {
        if (sscanf(argv[3], "%x", &value) != 1) {
            fprintf(stderr, "Invalid hexadecimal value input\n");
            return 1;
        }
        // Perform the write operation
        res = write_otp_value(address, value, &sc);
        if (res != TEEC_SUCCESS) {
         // Handle write error
            fprintf(stderr, "Write OTP failed\n");
            goto err;
        }
		
    } else if (is_read) {
        // Optional argument for the number of register outputs to print
        int count = 1;  // Default count is 1
        if (argc == 4) {
            if (sscanf(argv[3], "%d", &count) != 1 || count <= 0) {
                fprintf(stderr, "Invalid count argument\n");
                return 1;
            }
        }

        // Perform the read operation for the specified count
        for (int i = 0; i < count; ++i) {
            int32_t value;
           res = read_otp_value(address + (i * sizeof(int32_t)), &value, &sc);
           if (res != TEEC_SUCCESS) {
                fprintf(stderr, "Read OTP failed \n");
                goto err;
            }
        }
    }

    err:
        close_session_context(&sc);
        return 0;
}
