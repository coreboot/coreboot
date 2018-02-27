/* Copyright (c) 2015 The Chromium OS Authors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *    * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Functions for querying, manipulating and locking rollback indices
 * stored in the TPM NVRAM.
 */

#include <stdlib.h>
#include <security/tpm/tspi.h>
#include <vb2_api.h>

#include "antirollback.h"

int vb2ex_tpm_clear_owner(struct vb2_context *ctx)
{
	return VB2_SUCCESS;
}

uint32_t vboot_extend_pcr(struct vb2_context *ctx, int pcr,
			enum vb2_pcr_digest which_digest)
{
	return VB2_SUCCESS;
}

uint32_t antirollback_read_space_firmware(struct vb2_context *ctx)
{
	vb2api_secdata_create(ctx);
	return VB2_SUCCESS;
}

uint32_t antirollback_write_space_firmware(struct vb2_context *ctx)
{
	return VB2_SUCCESS;
}

uint32_t antirollback_lock_space_firmware()
{
	return VB2_SUCCESS;
}

uint32_t antirollback_lock_space_rec_hash(void)
{
	return VB2_SUCCESS;
}

uint32_t antirollback_read_space_rec_hash(uint8_t *data, uint32_t size)
{
	return VB2_SUCCESS;
}

uint32_t antirollback_write_space_rec_hash(const uint8_t *data, uint32_t size)
{
	return VB2_SUCCESS;
}
