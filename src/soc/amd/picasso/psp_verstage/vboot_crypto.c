/* SPDX-License-Identifier: GPL-2.0-only */

#include <2crypto.h>
#include <2return_codes.h>
#include <bl_uapp/bl_syscall_public.h>
#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include "psp_verstage.h"
#include <stddef.h>
#include <string.h>
#include <vb2_api.h>

static struct SHA_GENERIC_DATA_T sha_op;
static uint32_t sha_op_size_remaining;
static uint8_t __attribute__((aligned(32))) sha_hash[64];

vb2_error_t vb2ex_hwcrypto_digest_init(enum vb2_hash_algorithm hash_alg, uint32_t data_size)
{
	printk(BIOS_DEBUG, "Calculating hash of %d bytes\n", data_size);

	sha_op_size_remaining = data_size;

	if (hash_alg == VB2_HASH_SHA256) {
		sha_op.SHAType = SHA_TYPE_256;
		sha_op.DigestLen = 32;
	} else if (hash_alg == VB2_HASH_SHA512) {
		sha_op.SHAType = SHA_TYPE_512;
		sha_op.DigestLen = 64;
	} else {
		printk(BIOS_INFO, "Unsupported hash_alg %d!\n", hash_alg);
		return VB2_ERROR_EX_HWCRYPTO_UNSUPPORTED;
	}

	/* Set init flag for first operation */
	sha_op.Init = 1;

	/* Clear eom flag until last operation */
	sha_op.Eom = 0;

	/* Need documentation on this b:157610147 */
	sha_op.DataMemType = 2;

	sha_op.Digest = sha_hash;

	sha_op.IntermediateDigest = NULL;

	sha_op.IntermediateMsgLen = 0;

	return VB2_SUCCESS;
}

vb2_error_t vb2ex_hwcrypto_digest_extend(const uint8_t *buf, uint32_t size)
{
	uint32_t retval;
	sha_op.Data = (uint8_t *) buf;

	if (!sha_op_size_remaining) {
		printk(BIOS_ERR, "ERROR: got more data than expected.\n");
		return VB2_ERROR_UNKNOWN;
	}

	while (size) {
		sha_op.DataLen = size;

		sha_op_size_remaining -= sha_op.DataLen;

		/* Set eom flag for final operation */
		if (sha_op_size_remaining == 0)
			sha_op.Eom = 1;

		retval = svc_crypto_sha(&sha_op, SHA_GENERIC);
		if (retval) {
			printk(BIOS_ERR, "ERROR: HW crypto failed - errorcode: %#x\n",
					retval);
			return VB2_ERROR_UNKNOWN;
		}

		/* Clear init flag after first operation */
		if (sha_op.Init == 1)
			sha_op.Init = 0;

		size -= sha_op.DataLen;
	}

	return VB2_SUCCESS;
}

/* Copy the hash back to verstage */
vb2_error_t vb2ex_hwcrypto_digest_finalize(uint8_t *digest, uint32_t digest_size)
{
	if (sha_op.Eom == 0) {
		printk(BIOS_ERR, "ERROR: Got less data than expected.\n");
		return VB2_ERROR_UNKNOWN;
	}

	if (digest_size != sha_op.DigestLen) {
		printk(BIOS_ERR, "ERROR: Digest size does not match expected length.\n");
		return VB2_ERROR_UNKNOWN;
	}

	memcpy(digest, sha_hash, digest_size);

	return VB2_SUCCESS;
}

vb2_error_t vb2ex_hwcrypto_rsa_verify_digest(const struct vb2_public_key *key,
					     const uint8_t *sig, const uint8_t *digest)
{
	RSAPKCS_VERIFY_PARAMS RSAParams;
	uint32_t retval;
	uint32_t exp = 65537;
	uint32_t sig_size;
	size_t digest_size;

	/* PSP only supports 2K and 4K RSA */
	if (key->sig_alg != VB2_SIG_RSA2048 &&
	    key->sig_alg != VB2_SIG_RSA2048_EXP3 &&
	    key->sig_alg != VB2_SIG_RSA4096) {
		return VB2_ERROR_EX_HWCRYPTO_UNSUPPORTED;
	}

	/* PSP only supports SHA256, SHA384 and SHA512*/
	if (key->hash_alg != VB2_HASH_SHA256 &&
	    key->hash_alg != VB2_HASH_SHA384 &&
	    key->hash_alg != VB2_HASH_SHA512) {
		return VB2_ERROR_EX_HWCRYPTO_UNSUPPORTED;
	}

	if (key->sig_alg == VB2_SIG_RSA2048_EXP3)
		exp = 3;
	sig_size = vb2_rsa_sig_size(key->sig_alg);
	digest_size = vb2_digest_size(key->hash_alg);

	RSAParams.pHash = (char *)digest;
	RSAParams.HashLen = digest_size;
	RSAParams.pModulus = (char *)key->n;
	RSAParams.ModulusSize = sig_size;
	RSAParams.pExponent = (char *)&exp;
	RSAParams.ExpSize = sizeof(exp);
	RSAParams.pSig = (char *)sig;

	retval = svc_rsa_pkcs_verify(&RSAParams);
	if (retval) {
		printk(BIOS_ERR, "ERROR: HW crypto failed - errorcode: %#x\n",
				retval);
		return VB2_ERROR_RSA_VERIFY_DIGEST;
	}

	return VB2_SUCCESS;
}
