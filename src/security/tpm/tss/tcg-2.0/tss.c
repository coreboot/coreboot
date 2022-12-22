/* SPDX-License-Identifier: BSD-3-Clause */

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
	static uint8_t cr_buffer[TPM_BUFFER_SIZE];

	obuf_init(&ob, cr_buffer, sizeof(cr_buffer));

	if (tpm_marshal_command(command, command_body, &ob) < 0) {
		printk(BIOS_ERR, "command %#x\n", command);
		return NULL;
	}

	sendb = obuf_contents(&ob, &out_size);

	in_size = sizeof(cr_buffer);
	if (tis_sendrecv(sendb, out_size, cr_buffer, &in_size)) {
		printk(BIOS_ERR, "tpm transaction failed\n");
		return NULL;
	}

	ibuf_init(&ib, cr_buffer, in_size);

	return tpm_unmarshal_response(command, &ib);
}

static uint32_t tlcl_send_startup(TPM_SU type)
{
	struct tpm2_startup startup;
	struct tpm2_response *response;

	startup.startup_type = type;
	response = tpm_process_command(TPM2_Startup, &startup);

	/* IO error, tpm2_response pointer is empty. */
	if (!response) {
		printk(BIOS_ERR, "%s: TPM communication error\n", __func__);
		return TPM_E_IOERROR;
	}

	printk(BIOS_INFO, "%s: Startup return code is %x\n",
	       __func__, response->hdr.tpm_code);

	switch (response->hdr.tpm_code) {
	case TPM_RC_INITIALIZE:
		/* TPM already initialized. */
		return TPM_E_INVALID_POSTINIT;
	case TPM2_RC_SUCCESS:
		return TPM_SUCCESS;
	}

	/* Collapse any other errors into TPM_E_IOERROR. */
	return TPM_E_IOERROR;
}

uint32_t tlcl_resume(void)
{
	return tlcl_send_startup(TPM_SU_STATE);
}

static uint32_t tlcl_send_shutdown(TPM_SU type)
{
	struct tpm2_shutdown shutdown;
	struct tpm2_response *response;

	shutdown.shutdown_type = type;
	response = tpm_process_command(TPM2_Shutdown, &shutdown);

	/* IO error, tpm2_response pointer is empty. */
	if (!response) {
		printk(BIOS_ERR, "%s: TPM communication error\n", __func__);
		return TPM_E_IOERROR;
	}

	printk(BIOS_INFO, "%s: Shutdown return code is %x\n",
	       __func__, response->hdr.tpm_code);

	if (response->hdr.tpm_code == TPM2_RC_SUCCESS)
		return TPM_SUCCESS;

	/* Collapse any other errors into TPM_E_IOERROR. */
	return TPM_E_IOERROR;
}

uint32_t tlcl_save_state(void)
{
	return tlcl_send_shutdown(TPM_SU_STATE);
}

uint32_t tlcl_assert_physical_presence(void)
{
	/*
	 * Nothing to do on TPM2 for this, use platform hierarchy availability
	 * instead.
	 */
	return TPM_SUCCESS;
}

static TPM_ALG_ID tpmalg_from_vb2_hash(enum vb2_hash_algorithm hash_type)
{
	switch (hash_type) {
	case VB2_HASH_SHA1:
		return TPM_ALG_SHA1;
	case VB2_HASH_SHA256:
		return TPM_ALG_SHA256;
	case VB2_HASH_SHA384:
		return TPM_ALG_SHA384;
	case VB2_HASH_SHA512:
		return TPM_ALG_SHA512;

	default:
		return TPM_ALG_ERROR;
	}
}

uint32_t tlcl_extend(int pcr_num, const uint8_t *digest_data,
		     enum vb2_hash_algorithm digest_type)
{
	struct tpm2_pcr_extend_cmd pcr_ext_cmd;
	struct tpm2_response *response;
	TPM_ALG_ID alg;

	alg = tpmalg_from_vb2_hash(digest_type);
	if (alg == TPM_ALG_ERROR)
		return TPM_E_HASH_ERROR;

	pcr_ext_cmd.pcrHandle = HR_PCR + pcr_num;
	pcr_ext_cmd.digests.count = 1;
	pcr_ext_cmd.digests.digests[0].hashAlg = alg;
	/* Always copying to sha512 as it's the largest one */
	memcpy(pcr_ext_cmd.digests.digests[0].digest.sha512, digest_data,
	       vb2_digest_size(digest_type));

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

uint32_t tlcl_clear_control(bool disable)
{
	struct tpm2_response *response;
	struct tpm2_clear_control_cmd cc = {
		.disable = 0,
	};

	response = tpm_process_command(TPM2_ClearControl, &cc);
	printk(BIOS_INFO, "%s: response is %x\n",
		__func__, response ? response->hdr.tpm_code : -1);

	if (!response || response->hdr.tpm_code)
		return TPM_E_IOERROR;

	return TPM_SUCCESS;
}

static uint8_t tlcl_init_done;

/* This function is called directly by vboot, uses vboot return types. */
uint32_t tlcl_lib_init(void)
{
	if (tlcl_init_done)
		return VB2_SUCCESS;

	if (tis_init()) {
		printk(BIOS_ERR, "%s: tis_init returned error\n", __func__);
		return VB2_ERROR_UNKNOWN;
	}

	if (tis_open()) {
		printk(BIOS_ERR, "%s: tis_open returned error\n", __func__);
		return VB2_ERROR_UNKNOWN;
	}

	tlcl_init_done = 1;

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

	case TPM_RC_NV_RANGE:
		return TPM_E_RANGE;

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
	/* TPM Will reject attempts to write at non-defined index. */
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

uint32_t tlcl_set_bits(uint32_t index, uint64_t bits)
{
	struct tpm2_nv_setbits_cmd nvsb_cmd;
	struct tpm2_response *response;

	/* Prepare the command structure */
	memset(&nvsb_cmd, 0, sizeof(nvsb_cmd));

	nvsb_cmd.nvIndex = HR_NV_INDEX + index;
	nvsb_cmd.bits = bits;

	response = tpm_process_command(TPM2_NV_SetBits, &nvsb_cmd);

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

	/* Map TPM2 return codes into common vboot representation. */
	switch (response->hdr.tpm_code) {
	case TPM2_RC_SUCCESS:
		return TPM_SUCCESS;
	case TPM2_RC_NV_DEFINED:
		return TPM_E_NV_DEFINED;
	default:
		return TPM_E_INTERNAL_INCONSISTENCY;
	}
}

uint16_t tlcl_get_hash_size_from_algo(TPMI_ALG_HASH hash_algo)
{
	uint16_t value;

	switch (hash_algo) {
	case TPM_ALG_ERROR:
		value = 1;
		break;
	case TPM_ALG_SHA1:
		value = SHA1_DIGEST_SIZE;
		break;
	case TPM_ALG_SHA256:
		value = SHA256_DIGEST_SIZE;
		break;
	case TPM_ALG_SHA384:
		value = SHA384_DIGEST_SIZE;
		break;
	case TPM_ALG_SHA512:
		value = SHA512_DIGEST_SIZE;
		break;
	case TPM_ALG_SM3_256:
		value = SM3_256_DIGEST_SIZE;
		break;
	default:
		printk(BIOS_SPEW, "%s: unknown hash algorithm %d\n", __func__,
		hash_algo);
		value = 0;
	};

	return value;
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

uint32_t tlcl_get_capability(TPM_CAP capability, uint32_t property,
		uint32_t property_count,
		TPMS_CAPABILITY_DATA *capability_data)
{
	struct tpm2_get_capability cmd;
	struct tpm2_response *response;

	cmd.capability = capability;
	cmd.property = property;
	cmd.propertyCount = property_count;

	if (property_count > 1) {
		printk(BIOS_ERR, "%s: property_count more than one not "
		       "supported yet\n", __func__);
		return TPM_E_IOERROR;
	}

	response = tpm_process_command(TPM2_GetCapability, &cmd);

	if (!response) {
		printk(BIOS_ERR, "%s: Command Failed\n", __func__);
		return TPM_E_IOERROR;
	}

	memcpy(capability_data, &response->gc.cd, sizeof(TPMS_CAPABILITY_DATA));
	return TPM_SUCCESS;
}
