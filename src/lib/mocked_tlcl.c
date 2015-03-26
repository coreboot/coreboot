/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <tpm_lite/tlcl.h>

#ifdef FOR_TEST
#include <stdio.h>
#define VBDEBUG(format, args...) printf(format, ## args)
#else
#include <console/console.h>
#define VBDEBUG(format, args...) printk(BIOS_DEBUG, format, ## args)
#endif

uint32_t tlcl_lib_init(void) {
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_startup(void) {
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_resume(void) {
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_self_test_full(void)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_continue_self_test(void)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_define_space(uint32_t index, uint32_t perm, uint32_t size)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_write(uint32_t index, const void* data, uint32_t length)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_read(uint32_t index, void* data, uint32_t length)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}


uint32_t tlcl_assert_physical_presence(void) {
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_physical_presence_cmd_enable(void) {
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_finalize_physical_presence(void) {
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_set_nv_locked(void) {
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_force_clear(void) {
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_set_enable(void) {
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_set_deactivated(uint8_t flag)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_get_permanent_flags(TPM_PERMANENT_FLAGS* pflags)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_get_flags(uint8_t* disable, uint8_t* deactivated,
                        uint8_t *nvlocked)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_set_global_lock(void)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}

uint32_t tlcl_extend(int pcr_num, const uint8_t* in_digest,
                     uint8_t* out_digest)
{
	VBDEBUG("MOCK_TPM: %s\n", __func__);
	return TPM_E_NO_DEVICE;
}
