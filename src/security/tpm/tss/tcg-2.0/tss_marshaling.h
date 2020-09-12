/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef TCG2_TSS_MARSHALING_H_
#define TCG2_TSS_MARSHALING_H_

#include <commonlib/iobuf.h>
#include "tss_structures.h"

/* The below functions are used to serialize/deserialize TPM2 commands. */

/**
 * tpm_marshal_command
 *
 * Given a structure containing a TPM2 command, serialize the structure for
 * sending it to the TPM.
 *
 * @command: code of the TPM2 command to marshal
 * @tpm_command_body: a pointer to the command specific structure
 * @ob: output buffer where command is marshaled to
 *
 * Returns 0 on success or -1 on error.
 *
 */
int tpm_marshal_command(TPM_CC command, const void *tpm_command_body,
			struct obuf *ob);

/**
 * tpm_unmarshal_response
 *
 * Given a buffer received from the TPM in response to a certain command,
 * deserialize the buffer into the expeced response structure.
 *
 * struct tpm2_response is a union of all possible responses.
 *
 * @command: code of the TPM2 command for which a response is unmarshaled
 * @ib: input buffer containing the serialized response.
 *
 * Returns a pointer to the deserialized response or NULL in case of
 * unmarshaling problems.
 */
struct tpm2_response *tpm_unmarshal_response(TPM_CC command, struct ibuf *ib);

#endif // TCG2_TSS_MARSHALING_H_
