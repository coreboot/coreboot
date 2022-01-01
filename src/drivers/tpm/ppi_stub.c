/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>

#include "tpm_ppi.h"

static void tpm_ppi_func0_cb(void *arg)
{
	/* Functions 1-8. */
	u8 buf[] = {0xff, 0x01};
	acpigen_write_return_byte_buffer(buf, sizeof(buf));
}

static void tpm_ppi_func1_cb(void *arg)
{
	if (CONFIG(TPM2))
		/* Interface version: 1.3 */
		acpigen_write_return_string("1.3");
	else
		/* Interface version: 1.2 */
		acpigen_write_return_string("1.2");
}

static void tpm_ppi_func2_cb(void *arg)
{
	/* Submit operations: drop on the floor and return success. */
	acpigen_write_return_byte(PPI2_RET_SUCCESS);
}

static void tpm_ppi_func3_cb(void *arg)
{
	/* Pending operation: none. */
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(2);
	acpigen_write_byte(0);
	acpigen_write_byte(0);
	acpigen_pop_len();
}

static void tpm_ppi_func4_cb(void *arg)
{
	/* Pre-OS transition method: reboot. */
	acpigen_write_return_byte(2);
}

static void tpm_ppi_func5_cb(void *arg)
{
	/* Operation response: no operation executed. */
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(3);
	acpigen_write_byte(0);
	acpigen_write_byte(0);
	acpigen_write_byte(0);
	acpigen_pop_len();
}

static void tpm_ppi_func6_cb(void *arg)
{
	/*
	 * Set preferred user language: deprecated and must return 3 AKA
	 * "not implemented".
	 */
	acpigen_write_return_byte(PPI6_RET_NOT_IMPLEMENTED);
}

static void tpm_ppi_func7_cb(void *arg)
{
	/* Submit operations: deny. */
	acpigen_write_return_byte(PPI7_RET_BLOCKED_BY_FIRMWARE);
}

static void tpm_ppi_func8_cb(void *arg)
{
	/* All actions are forbidden. */
	acpigen_write_return_byte(PPI8_RET_FIRMWARE_ONLY);
}

static void (*tpm_ppi_callbacks[])(void *) = {
	tpm_ppi_func0_cb,
	tpm_ppi_func1_cb,
	tpm_ppi_func2_cb,
	tpm_ppi_func3_cb,
	tpm_ppi_func4_cb,
	tpm_ppi_func5_cb,
	tpm_ppi_func6_cb,
	tpm_ppi_func7_cb,
	tpm_ppi_func8_cb,
};

static void tpm_mci_func0_cb(void *arg)
{
	/* Function 1. */
	acpigen_write_return_singleton_buffer(0x3);
}
static void tpm_mci_func1_cb(void *arg)
{
	/* Just return success. */
	acpigen_write_return_byte(0);
}

static void (*tpm_mci_callbacks[])(void *) = {
	tpm_mci_func0_cb,
	tpm_mci_func1_cb,
};

void tpm_ppi_acpi_fill_ssdt(const struct device *dev)
{
	/*
	 * _DSM method
	 */
	struct dsm_uuid ids[] = {
		/* Physical presence interface.
		 * This is used to submit commands like "Clear TPM" to
		 * be run at next reboot provided that user confirms
		 * them. Spec allows user to cancel all commands and/or
		 * configure BIOS to reject commands. So we pretend that
		 * user did just this: cancelled everything. If user
		 * really wants to clear TPM the only option now is to
		 * do it manually in payload.
		 */
		DSM_UUID(TPM_PPI_UUID, tpm_ppi_callbacks,
			ARRAY_SIZE(tpm_ppi_callbacks), NULL),
		/* Memory clearing on boot: just a dummy. */
		DSM_UUID(TPM_MCI_UUID, tpm_mci_callbacks,
			ARRAY_SIZE(tpm_mci_callbacks), NULL),
	};

	acpigen_write_dsm_uuid_arr(ids, ARRAY_SIZE(ids));
}
