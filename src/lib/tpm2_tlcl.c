/*
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <console/console.h>
#include <endian.h>
#include <lib/tpm2_tlcl_structures.h>
#include <string.h>
#include <tpm.h>
#include <vb2_api.h>

#include "tpm2_marshaling.h"

/*
 * This file provides interface between firmware and TPM2 device. The TPM1.2
 * API was copied as is and relevant functions modified to comply with the
 * TPM2 specification.
 */

static void *tpm_process_command(TPM_CC command, void *command_body)
{
	size_t out_size, in_size;
	/* Command/response buffer. */
	static uint8_t cr_buffer[TPM_BUFFER_SIZE];

	out_size = tpm_marshal_command(command, command_body,
				       cr_buffer, sizeof(cr_buffer));
	if (out_size < 0) {
		printk(BIOS_ERR, "command %#x, cr size %zd\n",
		       command, out_size);
		return NULL;
	}

	in_size = sizeof(cr_buffer);
	if (tis_sendrecv(cr_buffer, out_size,
			 cr_buffer, &in_size)) {
		printk(BIOS_ERR, "tpm transaction failed\n");
		return NULL;
	}

	return tpm_unmarshal_response(command, cr_buffer, in_size);
}


uint32_t tlcl_get_permanent_flags(TPM_PERMANENT_FLAGS *pflags)
{
	printk(BIOS_INFO, "%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return TPM_SUCCESS;
}

uint32_t tlcl_resume(void)
{
	printk(BIOS_INFO, "%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return TPM_SUCCESS;
}

uint32_t tlcl_assert_physical_presence(void)
{
	/*
	 * Nothing to do on TPM2 for this, use platform hierarchy availability
	 * instead.
	 */
	return TPM_SUCCESS;
}

uint32_t tlcl_extend(int pcr_num, const uint8_t *in_digest,
		     uint8_t *out_digest)
{
	printk(BIOS_INFO, "%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return TPM_SUCCESS;
}

uint32_t tlcl_finalize_physical_presence(void)
{
	/* Nothing needs to be done with tpm2. */
	printk(BIOS_INFO, "%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return TPM_SUCCESS;
}

uint32_t tlcl_force_clear(void)
{
	struct tpm2_response *response;

	response = tpm_process_command(TPM2_Clear, NULL);
	printk(BIOS_INFO, "%s: response is %x\n",
	       __func__, response ? response->hdr.tpm_code : -1);

	if (!response || response->hdr.tpm_code)
		return TPM_E_IOERROR;

	return TPM_SUCCESS;
}

uint32_t tlcl_get_flags(uint8_t *disable, uint8_t *deactivated,
			uint8_t *nvlocked)
{
	/*
	 * TPM2 does not map directly into these flags TPM1.2 based firmware
	 * expects to be able to retrieve.
	 *
	 * In any case, if any of these conditions are present, the following
	 * firmware flow would be interrupted and will have a chance to report
	 * an error. Let's just hardcode an "All OK" response for now.
	 */

	if (disable)
		*disable = 0;

	if (nvlocked)
		*nvlocked = 1;

	if (deactivated)
		*deactivated = 0;

	return TPM_SUCCESS;
}

uint32_t tlcl_lib_init(void)
{
	/*
	 * This function is called directly by vboot, uses vboot return
	 * types.
	 */
	if (tis_init())
		return VB2_ERROR_UNKNOWN;
	if (tis_open())
		return VB2_ERROR_UNKNOWN;
	return VB2_SUCCESS;
}

uint32_t tlcl_physical_presence_cmd_enable(void)
{
	printk(BIOS_INFO, "%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return TPM_SUCCESS;
}

uint32_t tlcl_read(uint32_t index, void *data, uint32_t length)
{
	struct tpm2_nv_read_cmd nv_readc;
	struct tpm2_response *response;

	memset(&nv_readc, 0, sizeof(nv_readc));

	nv_readc.nvIndex = HR_NV_INDEX + index;
	nv_readc.size = length;

	response = tpm_process_command(TPM2_NV_Read, &nv_readc);

	/* Need to map tpm error codes into internal values. */
	if (!response)
		return TPM_E_READ_FAILURE;

	printk(BIOS_INFO, "%s:%d index %#x return code %x\n",
	       __FILE__, __LINE__, index, response->hdr.tpm_code);
	switch (response->hdr.tpm_code) {
	case 0:
		break;

	case 0x28b:
		return TPM_E_BADINDEX;

	default:
		return TPM_E_READ_FAILURE;
	}

	if (length > response->nvr.buffer.t.size)
		return TPM_E_RESPONSE_TOO_LARGE;

	if (length < response->nvr.buffer.t.size)
		return TPM_E_READ_EMPTY;

	memcpy(data, response->nvr.buffer.t.buffer, length);

	return TPM_SUCCESS;
}

uint32_t tlcl_self_test_full(void)
{
	struct tpm2_self_test st;
	struct tpm2_response *response;

	st.yes_no = 1;

	response = tpm_process_command(TPM2_SelfTest, &st);
	printk(BIOS_INFO, "%s: response is %x\n",
	       __func__, response ? response->hdr.tpm_code : -1);
	return TPM_SUCCESS;
}

uint32_t tlcl_set_deactivated(uint8_t flag)
{
	printk(BIOS_INFO, "%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return TPM_SUCCESS;
}

uint32_t tlcl_set_enable(void)
{
	printk(BIOS_INFO, "%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return TPM_SUCCESS;
}

uint32_t tlcl_lock_nv_write(uint32_t index)
{
	struct tpm2_response *response;
	/* TPM Wll reject attempts to write at non-defined index. */
	struct tpm2_nv_write_lock_cmd nv_wl = {
		.nvIndex = HR_NV_INDEX + index,
	};

	response = tpm_process_command(TPM2_NV_WriteLock, &nv_wl);

	printk(BIOS_INFO, "%s: response is %x\n",
	       __func__, response ? response->hdr.tpm_code : -1);

	if (!response || response->hdr.tpm_code)
		return TPM_E_IOERROR;

	return TPM_SUCCESS;
}

uint32_t tlcl_startup(void)
{
	struct tpm2_startup startup;
	struct tpm2_response *response;

	startup.startup_type = TPM_SU_CLEAR;
	response = tpm_process_command(TPM2_Startup, &startup);
	if (response && response->hdr.tpm_code &&
	    (response->hdr.tpm_code != TPM_RC_INITIALIZE)) {
		printk(BIOS_INFO, "startup return code is %x\n",
		       response->hdr.tpm_code);
		return TPM_E_IOERROR;
	}
	return TPM_SUCCESS;
}

uint32_t tlcl_write(uint32_t index, const void *data, uint32_t length)
{
	struct tpm2_nv_write_cmd nv_writec;
	struct tpm2_response *response;

	memset(&nv_writec, 0, sizeof(nv_writec));

	nv_writec.nvIndex = HR_NV_INDEX + index;
	nv_writec.data.t.size = length;
	nv_writec.data.t.buffer = data;

	response = tpm_process_command(TPM2_NV_Write, &nv_writec);

	printk(BIOS_INFO, "%s: response is %x\n",
	       __func__, response ? response->hdr.tpm_code : -1);

	/* Need to map tpm error codes into internal values. */
	if (!response || response->hdr.tpm_code)
		return TPM_E_WRITE_FAILURE;

	return TPM_SUCCESS;
}

uint32_t tlcl_define_space(uint32_t space_index, size_t space_size)
{
	struct tpm2_nv_define_space_cmd nvds_cmd;
	struct tpm2_response *response;
	/*
	 * This policy digest was obtained using TPM2_PolicyPCR selecting only
	 * PCR_0 with a value of all zeros.
	 */
	static const uint8_t pcr0_unchanged_policy[] = {
		0x09, 0x93, 0x3C, 0xCE, 0xEB, 0xB4, 0x41, 0x11,
		0x18, 0x81, 0x1D, 0xD4, 0x47, 0x78, 0x80, 0x08,
		0x88, 0x86, 0x62, 0x2D, 0xD7, 0x79, 0x94, 0x46,
		0x62, 0x26, 0x68, 0x8E, 0xEE, 0xE6, 0x6A, 0xA1
	};

	/* Prepare the define space command structure. */
	memset(&nvds_cmd, 0, sizeof(nvds_cmd));

	nvds_cmd.publicInfo.dataSize = space_size;
	nvds_cmd.publicInfo.nvIndex = HR_NV_INDEX + space_index;
	nvds_cmd.publicInfo.nameAlg = TPM_ALG_SHA256;

	/* Attributes common for all NV ram spaces used by firmware. */
	nvds_cmd.publicInfo.attributes.TPMA_NV_PPWRITE = 1;
	nvds_cmd.publicInfo.attributes.TPMA_NV_AUTHREAD = 1;
	nvds_cmd.publicInfo.attributes.TPMA_NV_PPREAD = 1;
	nvds_cmd.publicInfo.attributes.TPMA_NV_PLATFORMCREATE = 1;
	nvds_cmd.publicInfo.attributes.TPMA_NV_WRITE_STCLEAR = 1;
	nvds_cmd.publicInfo.attributes.TPMA_NV_POLICY_DELETE = 1;

	/*
	 * Use policy digest based on default pcr0 value. This makes sure that
	 * the space can not be deleted as soon as PCR0 value has been
	 * extended from default.
	 */
	nvds_cmd.publicInfo.authPolicy.t.buffer = pcr0_unchanged_policy;
	nvds_cmd.publicInfo.authPolicy.t.size = sizeof(pcr0_unchanged_policy);

	response = tpm_process_command(TPM2_NV_DefineSpace, &nvds_cmd);
	printk(BIOS_INFO, "%s: response is %x\n",
	       __func__, response ? response->hdr.tpm_code : -1);

	if (!response)
		return TPM_E_NO_DEVICE;

	return response->hdr.tpm_code ? TPM_E_INTERNAL_INCONSISTENCY :
		TPM_SUCCESS;
}
