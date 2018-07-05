/*
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/early_variables.h>
#include <console/console.h>
#include <endian.h>
#include <string.h>
#include <vb2_api.h>
#include <security/tpm/tis.h>
#include <security/tpm/tss.h>

#include "tss_structures.h"
#include "tss_marshaling.h"

/*
 * This file provides interface between firmware and TPM2 device. The TPM1.2
 * API was copied as is and relevant functions modified to comply with the
 * TPM2 specification.
 */

void *tpm_process_command(TPM_CC command, void *command_body)
{
	struct obuf ob;
	struct ibuf ib;
	size_t out_size;
	size_t in_size;
	const uint8_t *sendb;
	/* Command/response buffer. */
	static uint8_t cr_buffer[TPM_BUFFER_SIZE] CAR_GLOBAL;

	uint8_t *cr_buffer_ptr = car_get_var_ptr(cr_buffer);

	obuf_init(&ob, cr_buffer_ptr, sizeof(cr_buffer));

	if (tpm_marshal_command(command, command_body, &ob) < 0) {
		printk(BIOS_ERR, "command %#x\n", command);
		return NULL;
	}

	sendb = obuf_contents(&ob, &out_size);

	in_size = sizeof(cr_buffer);
	if (tis_sendrecv(sendb, out_size, cr_buffer_ptr, &in_size)) {
		printk(BIOS_ERR, "tpm transaction failed\n");
		return NULL;
	}

	ibuf_init(&ib, cr_buffer_ptr, in_size);

	return tpm_unmarshal_response(command, &ib);
}

static uint32_t tlcl_send_startup(TPM_SU type)
{
	struct tpm2_startup startup;
	struct tpm2_response *response;

	startup.startup_type = type;
	response = tpm_process_command(TPM2_Startup, &startup);

	if (response && response->hdr.tpm_code &&
	    (response->hdr.tpm_code != TPM_RC_INITIALIZE)) {
		printk(BIOS_INFO, "%s: Startup return code is %x\n",
		       __func__, response->hdr.tpm_code);
		return TPM_E_IOERROR;
	}
	return TPM_SUCCESS;

}

uint32_t tlcl_resume(void)
{
	return tlcl_send_startup(TPM_SU_STATE);
}

uint32_t tlcl_assert_physical_presence(void)
{
	/*
	 * Nothing to do on TPM2 for this, use platform hierarchy availability
	 * instead.
	 */
	return TPM_SUCCESS;
}

/*
 * The caller will provide the digest in a 32 byte buffer, let's consider it a
 * sha256 digest.
 */
uint32_t tlcl_extend(int pcr_num, const uint8_t *in_digest,
		     uint8_t *out_digest)
{
	struct tpm2_pcr_extend_cmd pcr_ext_cmd;
	struct tpm2_response *response;

	pcr_ext_cmd.pcrHandle = HR_PCR + pcr_num;
	pcr_ext_cmd.digests.count = 1;
	pcr_ext_cmd.digests.digests[0].hashAlg = TPM_ALG_SHA256;
	memcpy(pcr_ext_cmd.digests.digests[0].digest.sha256, in_digest,
	       sizeof(pcr_ext_cmd.digests.digests[0].digest.sha256));

	response = tpm_process_command(TPM2_PCR_Extend, &pcr_ext_cmd);

	printk(BIOS_INFO, "%s: response is %x\n",
	       __func__, response ? response->hdr.tpm_code : -1);
	if (!response || response->hdr.tpm_code)
		return TPM_E_IOERROR;

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

static uint8_t tlcl_init_done CAR_GLOBAL;

/* This function is called directly by vboot, uses vboot return types. */
uint32_t tlcl_lib_init(void)
{
	uint8_t done = car_get_var(tlcl_init_done);
	if (done)
		return VB2_SUCCESS;

	if (tis_init())
		return VB2_ERROR_UNKNOWN;
	if (tis_open())
		return VB2_ERROR_UNKNOWN;

	car_set_var(tlcl_init_done, 1);

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

		/* Uninitialized, returned if the space hasn't been written. */
	case TPM_RC_NV_UNINITIALIZED:
		/*
		 * Bad index, cr50 specific value, returned if the space
		 * hasn't been defined.
		 */
	case TPM_RC_CR50_NV_UNDEFINED:
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
	return tlcl_send_startup(TPM_SU_CLEAR);
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

uint32_t tlcl_define_space(uint32_t space_index, size_t space_size,
			   const TPMA_NV nv_attributes,
			   const uint8_t *nv_policy, size_t nv_policy_size)
{
	struct tpm2_nv_define_space_cmd nvds_cmd;
	struct tpm2_response *response;

	/* Prepare the define space command structure. */
	memset(&nvds_cmd, 0, sizeof(nvds_cmd));

	nvds_cmd.publicInfo.dataSize = space_size;
	nvds_cmd.publicInfo.nvIndex = HR_NV_INDEX + space_index;
	nvds_cmd.publicInfo.nameAlg = TPM_ALG_SHA256;
	nvds_cmd.publicInfo.attributes = nv_attributes;

	/*
	 * Use policy digest based on default pcr0 value. This makes
	 * sure that the space can not be deleted as soon as PCR0
	 * value has been extended from default.
	 */
	if (nv_policy && nv_policy_size) {
		nvds_cmd.publicInfo.authPolicy.t.buffer = nv_policy;
		nvds_cmd.publicInfo.authPolicy.t.size = nv_policy_size;
	}

	response = tpm_process_command(TPM2_NV_DefineSpace, &nvds_cmd);
	printk(BIOS_INFO, "%s: response is %x\n", __func__,
	       response ? response->hdr.tpm_code : -1);

	if (!response)
		return TPM_E_NO_DEVICE;

	/* Map TPM2 retrun codes into common vboot represenation. */
	switch (response->hdr.tpm_code) {
	case TPM2_RC_SUCCESS:
		return TPM_SUCCESS;
	case TPM2_RC_NV_DEFINED:
		return TPM_E_NV_DEFINED;
	default:
		return TPM_E_INTERNAL_INCONSISTENCY;
	}
}

uint32_t tlcl_disable_platform_hierarchy(void)
{
	struct tpm2_response *response;
	struct tpm2_hierarchy_control_cmd hc = {
		.enable = TPM_RH_PLATFORM,
		.state = 0,
	};

	response = tpm_process_command(TPM2_Hierarchy_Control, &hc);

	if (!response || response->hdr.tpm_code)
		return TPM_E_INTERNAL_INCONSISTENCY;

	return TPM_SUCCESS;
}
