/*
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <commonlib/endian.h>
#include <console/console.h>
#include <stdlib.h>
#include <string.h>

#include "tpm2_marshaling.h"

static uint16_t tpm_tag;  /* Depends on the command type. */

/*
 * Each unmarshaling function receives a pointer to the buffer pointer and a
 * pointer to the size of data still in the buffer. The function extracts data
 * from the buffer and adjusts both buffer pointer and remaining data size.
 *
 * Should there be not enough data in the buffer to unmarshal the required
 * object, the remaining data size is set to -1 to indicate the error. The
 * remaining data size is expected to be set to zero once the last data item
 * has been extracted from the buffer.
 */
static uint16_t unmarshal_u16(void **buffer, int *buffer_space)
{
	uint16_t value;

	if (*buffer_space < sizeof(value)) {
		*buffer_space = -1; /* Indicate a failure. */
		return 0;
	}

	value = read_be16(*buffer);
	*buffer = (void *) ((uintptr_t) (*buffer) + sizeof(value));
	*buffer_space -= sizeof(value);

	return value;
}

static uint16_t unmarshal_u32(void **buffer, int *buffer_space)
{
	uint32_t value;

	if (*buffer_space < sizeof(value)) {
		*buffer_space = -1; /* Indicate a failure. */
		return 0;
	}

	value = read_be32(*buffer);
	*buffer = (void *) ((uintptr_t) (*buffer) + sizeof(value));
	*buffer_space -= sizeof(value);

	return value;
}

static uint8_t unmarshal_u8(void **buffer, int *buffer_space)
{
	uint8_t value;

	if (*buffer_space < sizeof(value)) {
		*buffer_space = -1; /* Indicate a failure. */
		return 0;
	}

	value = ((uint8_t *)(*buffer))[0];
	*buffer = (void *) ((uintptr_t) (*buffer) + sizeof(value));
	*buffer_space -= sizeof(value);

	return value;
}

#define unmarshal_TPM_CAP(a, b) unmarshal_u32(a, b)
#define unmarshal_TPM_CC(a, b) unmarshal_u32(a, b)
#define unmarshal_TPM_PT(a, b) unmarshal_u32(a, b)
#define unmarshal_TPM_HANDLE(a, b) unmarshal_u32(a, b)

/*
 * Each marshaling function receives a pointer to the buffer to marshal into,
 * a pointer to the data item to be marshaled, and a pointer to the remaining
 * room in the buffer.
 */

 /*
  * Marshaling an arbitrary blob requires its size in addition to common
  * parameter set.
  */
static void marshal_blob(void **buffer, void *blob,
			 size_t blob_size, int *buffer_space)
{
	if (*buffer_space < blob_size) {
		*buffer_space = -1;
		return;
	}

	memcpy(*buffer, blob, blob_size);
	buffer_space -= blob_size;
	*buffer = (void *)((uintptr_t)(*buffer) + blob_size);
}

static void marshal_u8(void **buffer, uint8_t value, int *buffer_space)
{
	uint8_t *bp = *buffer;

	if (*buffer_space < sizeof(value)) {
		*buffer_space = -1;
		return;
	}

	*bp++ = value;
	*buffer = bp;
	*buffer_space -= sizeof(value);
}

static void marshal_u16(void **buffer, uint16_t value, int *buffer_space)
{
	if (*buffer_space < sizeof(value)) {
		*buffer_space = -1;
		return;
	}
	write_be16(*buffer, value);
	*buffer = (void *)((uintptr_t)(*buffer) + sizeof(value));
	*buffer_space -= sizeof(value);
}

static void marshal_u32(void **buffer, uint32_t value, int *buffer_space)
{
	if (*buffer_space < sizeof(value)) {
		*buffer_space = -1;
		return;
	}

	write_be32(*buffer, value);
	*buffer = (void *)((uintptr_t)(*buffer) + sizeof(value));
	*buffer_space -= sizeof(value);
}

#define marshal_TPM_HANDLE(a, b, c) marshal_u32(a, b, c)
#define marshal_TPMI_RH_NV_AUTH(a, b, c) marshal_TPM_HANDLE(a, b, c)
#define marshal_TPMI_RH_NV_INDEX(a, b, c) marshal_TPM_HANDLE(a, b, c)
#define marshal_TPMI_SH_AUTH_SESSION(a, b, c) marshal_TPM_HANDLE(a, b, c)
#define marshal_TPMI_ALG_HASH(a, b, c) marshal_u16(a, b, c)

static void marshal_startup(void **buffer,
			   struct tpm2_startup *cmd_body,
			   int *buffer_space)
{
	marshal_u16(buffer, cmd_body->startup_type, buffer_space);
}

static void marshal_get_capability(void **buffer,
				   struct tpm2_get_capability *cmd_body,
				   int *buffer_space)
{
	marshal_u32(buffer, cmd_body->capability, buffer_space);
	marshal_u32(buffer, cmd_body->property, buffer_space);
	marshal_u32(buffer, cmd_body->propertyCount, buffer_space);
}

static void marshal_TPM2B(void **buffer,
			  TPM2B *data,
			  int *buffer_space)
{
	size_t total_size = data->size + sizeof(data->size);

	if (total_size > *buffer_space) {
		*buffer_space = -1;
		return;
	}
	marshal_u16(buffer, data->size, buffer_space);
	memcpy(*buffer, data->buffer, data->size);
	*buffer = ((uint8_t *)(*buffer)) + data->size;
	*buffer_space -= data->size;
}

static void marshal_TPMA_NV(void **buffer,
			    TPMA_NV *nv,
			    int *buffer_space)
{
	marshal_u32(buffer, *((uint32_t *)nv), buffer_space);
}

static void marshal_TPMS_NV_PUBLIC(void **buffer,
				   TPMS_NV_PUBLIC *nvpub,
				   int *buffer_space)
{
	marshal_TPM_HANDLE(buffer, nvpub->nvIndex, buffer_space);
	marshal_TPMI_ALG_HASH(buffer, nvpub->nameAlg, buffer_space);
	marshal_TPMA_NV(buffer, &nvpub->attributes, buffer_space);
	marshal_TPM2B(buffer, &nvpub->authPolicy.b, buffer_space);
	marshal_u16(buffer, nvpub->dataSize, buffer_space);
}

static void marshal_session_header(void **buffer,
				   struct tpm2_session_header *session_header,
				   int *buffer_space)
{
	int base_size;
	void *size_location = *buffer;

	/* Skip room for the session header size. */
	*buffer_space -= sizeof(uint32_t);
	*buffer = (void *)(((uintptr_t) *buffer) + sizeof(uint32_t));

	base_size = *buffer_space;

	marshal_u32(buffer, session_header->session_handle, buffer_space);
	marshal_u16(buffer, session_header->nonce_size, buffer_space);
	marshal_blob(buffer, session_header->nonce,
		     session_header->nonce_size, buffer_space);
	marshal_u8(buffer, session_header->session_attrs, buffer_space);
	marshal_u16(buffer, session_header->auth_size, buffer_space);
	marshal_blob(buffer, session_header->auth,
		     session_header->auth_size, buffer_space);

	if (*buffer_space < 0)
		return;  /* The structure did not fit. */

	/* Paste in the session size. */
	marshal_u32(&size_location, base_size - *buffer_space, &base_size);
}

static void marshal_nv_define_space(void **buffer,
				    struct tpm2_nv_define_space_cmd *nvd_in,
				    int *buffer_space)
{
	void *size_location;
	int base_size;
	struct tpm2_session_header session_header;

	marshal_TPM_HANDLE(buffer, TPM_RH_PLATFORM, buffer_space);
	memset(&session_header, 0, sizeof(session_header));
	session_header.session_handle = TPM_RS_PW;
	marshal_session_header(buffer, &session_header, buffer_space);
	tpm_tag = TPM_ST_SESSIONS;

	marshal_TPM2B(buffer, &nvd_in->auth.b, buffer_space);

	/* This is where the TPMS_NV_PUBLIC size will be stored. */
	size_location = *buffer;

	/* Allocate room for the size. */
	*buffer = ((uint8_t *)(*buffer)) + sizeof(uint16_t);
	*buffer_space -= sizeof(uint16_t);
	base_size = *buffer_space;

	marshal_TPMS_NV_PUBLIC(buffer, &nvd_in->publicInfo, buffer_space);
	base_size = base_size - *buffer_space;
	marshal_u16(&size_location, (uint16_t)base_size, &base_size);
}

static void marshal_nv_write(void **buffer,
			     struct tpm2_nv_write_cmd *command_body,
			     int *buffer_space)
{
	struct tpm2_session_header session_header;

	marshal_TPM_HANDLE(buffer, TPM_RH_PLATFORM, buffer_space);
	marshal_TPM_HANDLE(buffer, command_body->nvIndex, buffer_space);
	memset(&session_header, 0, sizeof(session_header));
	session_header.session_handle = TPM_RS_PW;
	marshal_session_header(buffer, &session_header, buffer_space);
	tpm_tag = TPM_ST_SESSIONS;

	marshal_TPM2B(buffer, &command_body->data.b, buffer_space);
	marshal_u16(buffer, command_body->offset, buffer_space);
}

static void marshal_nv_read(void **buffer,
			    struct tpm2_nv_read_cmd *command_body,
			    int *buffer_space)
{
	struct tpm2_session_header session_header;

	marshal_TPM_HANDLE(buffer, TPM_RH_PLATFORM, buffer_space);
	marshal_TPM_HANDLE(buffer, command_body->nvIndex, buffer_space);
	memset(&session_header, 0, sizeof(session_header));
	session_header.session_handle = TPM_RS_PW;
	marshal_session_header(buffer, &session_header, buffer_space);
	tpm_tag = TPM_ST_SESSIONS;
	marshal_u16(buffer, command_body->size, buffer_space);
	marshal_u16(buffer, command_body->offset, buffer_space);
}

static void marshal_selftest(void **buffer,
			     struct tpm2_self_test *command_body,
			     int *buffer_space)
{
	marshal_u8(buffer, command_body->yes_no, buffer_space);
}

int tpm_marshal_command(TPM_CC command, void *tpm_command_body,
			void *buffer, int buffer_size)
{
	void *cmd_body = (uint8_t *)buffer + sizeof(struct tpm_header);
	int max_body_size = buffer_size - sizeof(struct tpm_header);
	int body_size = max_body_size;

	/* Will be modified when marshaling some commands. */
	tpm_tag = TPM_ST_NO_SESSIONS;

	switch (command) {
	case TPM2_Startup:
		marshal_startup(&cmd_body, tpm_command_body, &body_size);
		break;

	case TPM2_GetCapability:
		marshal_get_capability(&cmd_body, tpm_command_body,
				       &body_size);
		break;

	case TPM2_NV_Read:
		marshal_nv_read(&cmd_body, tpm_command_body, &body_size);
		break;

	case TPM2_NV_DefineSpace:
		marshal_nv_define_space(&cmd_body,
					tpm_command_body, &body_size);
		break;

	case TPM2_NV_Write:
		marshal_nv_write(&cmd_body, tpm_command_body, &body_size);
		break;

	case TPM2_SelfTest:
		marshal_selftest(&cmd_body, tpm_command_body, &body_size);
		break;

	default:
		body_size = -1;
		printk(BIOS_INFO, "%s:%d:Request to marshal unsupported command %#x\n",
		       __FILE__, __LINE__, command);
	}

	if (body_size > 0) {

		/* See how much room was taken by marshaling. */
		body_size = max_body_size - body_size;

		body_size += sizeof(struct tpm_header);

		marshal_u16(&buffer, tpm_tag, &max_body_size);
		marshal_u32(&buffer, body_size, &max_body_size);
		marshal_u32(&buffer, command, &max_body_size);
	}

	return body_size;
}

static void unmarshal_get_capability(void **buffer, int *size,
				     struct get_cap_response *gcr)
{
	int i;

	gcr->more_data = unmarshal_u8(buffer, size);

	gcr->cd.capability = unmarshal_TPM_CAP(buffer, size);
	if (*size < 0)
		return;

	switch (gcr->cd.capability) {
	case TPM_CAP_TPM_PROPERTIES:
		gcr->cd.data.tpmProperties.count =
			unmarshal_u32(buffer, size);
		if (*size < 0)
			return;
		if (gcr->cd.data.tpmProperties.count > ARRAY_SIZE
		    (gcr->cd.data.tpmProperties.tpmProperty)) {
			printk(BIOS_INFO, "%s:%s:%d - %d - too many properties\n",
			       __FILE__, __func__, __LINE__,
			       gcr->cd.data.tpmProperties.count);
			*size = -1;
			return;
		}
		for (i = 0; i < gcr->cd.data.tpmProperties.count; i++) {
			TPMS_TAGGED_PROPERTY *pp;

			pp = gcr->cd.data.tpmProperties.tpmProperty + i;
			pp->property = unmarshal_TPM_PT(buffer, size);
			pp->value = unmarshal_u32(buffer, size);
		}
		break;
	default:
		printk(BIOS_ERR,
		       "%s:%d - unable to unmarshal capability response",
		       __func__, __LINE__);
		printk(BIOS_ERR, " for %d\n", gcr->cd.capability);
		*size = -1;
		break;
	}
}

static void unmarshal_TPM2B_MAX_NV_BUFFER(void **buffer,
					  int *size,
					  TPM2B_MAX_NV_BUFFER *nv_buffer)
{
	nv_buffer->t.size = unmarshal_u16(buffer, size);
	if (nv_buffer->t.size > *size) {
		printk(BIOS_ERR, "%s:%d - "
		       "size mismatch: expected %d, remaining %d\n",
		       __func__, __LINE__, nv_buffer->t.size, *size);
		return;
	}

	nv_buffer->t.buffer = *buffer;

	*buffer = ((uint8_t *)(*buffer)) + nv_buffer->t.size;
	*size -= nv_buffer->t.size;
}

static void unmarshal_nv_read(void **buffer, int *size,
			      struct nv_read_response *nvr)
{
	/* Total size of the parameter field. */
	nvr->params_size = unmarshal_u32(buffer, size);
	unmarshal_TPM2B_MAX_NV_BUFFER(buffer, size, &nvr->buffer);

	if (nvr->params_size !=
	    (nvr->buffer.t.size + sizeof(nvr->buffer.t.size))) {
		printk(BIOS_ERR,
		       "%s:%d - parameter/buffer %d/%d size mismatch",
		       __func__, __LINE__, nvr->params_size,
		       nvr->buffer.t.size);
		return;
	}

	if (*size < 0)
		return;
	/*
	 * Let's ignore the authorisation section. It should be 5 bytes total,
	 * just confirm that this is the case and report any discrepancy.
	 */
	if (*size != 5)
		printk(BIOS_ERR,
		       "%s:%d - unexpected authorisation seciton size %d\n",
		       __func__, __LINE__, *size);

	*buffer = ((uint8_t *)(*buffer)) + *size;
	*size = 0;
}

struct tpm2_response *tpm_unmarshal_response(TPM_CC command,
					     void *response_body,
					     int cr_size)
{
	static struct tpm2_response tpm2_resp;

	if (cr_size < sizeof(struct tpm_header))
		return NULL;

	tpm2_resp.hdr.tpm_tag = unmarshal_u16(&response_body, &cr_size);
	tpm2_resp.hdr.tpm_size = unmarshal_u32(&response_body, &cr_size);
	tpm2_resp.hdr.tpm_code = unmarshal_TPM_CC(&response_body, &cr_size);

	if (!cr_size) {
		if (tpm2_resp.hdr.tpm_size != sizeof(tpm2_resp.hdr))
			printk(BIOS_ERR,
			       "%s: size mismatch in response to command %#x\n",
			       __func__, command);
		return &tpm2_resp;
	}

	switch (command) {
	case TPM2_Startup:
		break;

	case TPM2_GetCapability:
		unmarshal_get_capability(&response_body, &cr_size,
					 &tpm2_resp.gc);
		break;

	case TPM2_NV_Read:
		unmarshal_nv_read(&response_body, &cr_size,
				  &tpm2_resp.nvr);
		break;

	case TPM2_NV_DefineSpace:
	case TPM2_NV_Write:
		/* Session data included in response can be safely ignored. */
		cr_size = 0;
		break;

	default:
		{
			int i;

			printk(BIOS_INFO, "%s:%d:"
			       "Request to unmarshal unexpected command %#x,"
			       " code %#x",
			       __func__, __LINE__, command,
			       tpm2_resp.hdr.tpm_code);

			for (i = 0; i < cr_size; i++) {
				if (!(i % 16))
					printk(BIOS_INFO, "\n");
				printk(BIOS_INFO, "%2.2x ",
				       ((uint8_t *)response_body)[i]);
			}
		}
		printk(BIOS_INFO, "\n");
		return NULL;
	}

	if (cr_size) {
		printk(BIOS_INFO,
		       "%s:%d got %d bytes back in response to %#x,"
		       " failed to parse (%d)\n",
		       __func__, __LINE__, tpm2_resp.hdr.tpm_size,
		       command, cr_size);
		return NULL;
	}

	/* The entire message have been parsed. */
	return &tpm2_resp;
}
