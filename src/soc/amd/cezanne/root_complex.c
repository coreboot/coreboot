/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/alib.h>
#include <amdblocks/data_fabric.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/root_complex.h>
#include <arch/ioapic.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/iomap.h>
#include <static.h>
#include <stdint.h>
#include "chip.h"

#define DPTC_TOTAL_UPDATE_PARAMS	4

struct dptc_input {
	uint16_t size;
	struct alib_dptc_param params[DPTC_TOTAL_UPDATE_PARAMS];
} __packed;

#define DPTC_INPUTS(_thermctllmit, _sustained, _fast, _slow)			\
	{									\
		.size = sizeof(struct dptc_input),				\
		.params = {							\
			{							\
				.id = ALIB_DPTC_THERMAL_CONTROL_LIMIT_ID,	\
				.value = _thermctllmit,				\
			},							\
			{							\
				.id = ALIB_DPTC_SUSTAINED_POWER_LIMIT_ID,	\
				.value = _sustained,				\
			},							\
			{							\
				.id = ALIB_DPTC_FAST_PPT_LIMIT_ID,		\
				.value = _fast,					\
			},							\
			{							\
				.id = ALIB_DPTC_SLOW_PPT_LIMIT_ID,		\
				.value = _slow,					\
			},							\
		},								\
	}

static void acipgen_dptci(void)
{
	const struct soc_amd_cezanne_config *config = config_of_soc();

	/* Normal mode DPTC values. */
	struct dptc_input default_input = DPTC_INPUTS(config->thermctl_limit_degreeC,
		config->sustained_power_limit_mW,
		config->fast_ppt_limit_mW,
		config->slow_ppt_limit_mW);
	acpigen_write_alib_dptc_default((uint8_t *)&default_input, sizeof(default_input));
}

static void root_complex_fill_ssdt(const struct device *device)
{
	if (CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC))
		acipgen_dptci();

	/* Add ACPI device, opregion to host bridge needed for ACP driver.
	 *
	 * This is used by an ACPI method in the ACP's ACPI code to access different mailbox
	 * interfaces in th hardware. Some ACP drivers will use that to both notify the PSP
	 * that the DSP firmware has been loaded, so that the PSP can validate the firmware
	 * and set the qualifier bit to enable running it, and to configure the ACP's clock
	 * source.
	 *
	 * As this SMN access is not arbitrated and there may be other drivers or parts of
	 * the firmware attempting to use the SMN access register pair, there is a risk of
	 * conflict / incorrect data, but given the frequency and duration of accesses, the
	 * risk is deemed to be quite low.
	 *
	 *	Scope (\_SB.PCI0)
	 *	{
	 *		Device (GNB)
	 *		{
	 *			Name (_ADR, 0x0000000000000000)
	 *			Method (_STA, 0, NotSerialized)
	 *			{
	 *				Return (0x0F)
	 *			}
	 *		}
	 *	}
	 *	Scope (\_SB.PCI0.GNB)
	 *	{
	 *		OperationRegion(SMN, SystemMemory , 0xF80000B8, 0x8)
	 *		Field(SMN, AnyAcc, NoLock, Preserve) {
	 *			SMNA,   32,
	 *			SMND,   32,
	 *		}
	 *	}
	 */
	acpi_device_write_pci_dev(device);
	acpigen_write_scope(acpi_device_path(device));
	struct opregion opreg = OPREGION("SMN", SYSTEMMEMORY,
					 CONFIG_ECAM_MMCONF_BASE_ADDRESS + 0xb8, 0x8);
	acpigen_write_opregion(&opreg);
	static const struct fieldlist list[] = {
		FIELDLIST_NAMESTR("SMNA", 32),
		FIELDLIST_NAMESTR("SMND", 32),
	};
	acpigen_write_field(opreg.name, list, ARRAY_SIZE(list),
			FIELD_ANYACC | FIELD_NOLOCK | FIELD_PRESERVE);
	acpigen_write_scope_end();
}

static const char *gnb_acpi_name(const struct device *dev)
{
	return "GNB";
}

struct device_operations cezanne_root_complex_operations = {
	/* The root complex has no PCI BARs implemented, so there's no need to call
	   pci_dev_read_resources for it */
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_name		= gnb_acpi_name,
	.acpi_fill_ssdt		= root_complex_fill_ssdt,
};

static const struct domain_iohc_info iohc_info[] = {
	[0] = {
		.fabric_id = IOMS0_FABRIC_ID,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13B1,
	},
};

const struct domain_iohc_info *get_iohc_info(size_t *count)
{
	*count = ARRAY_SIZE(iohc_info);
	return iohc_info;
}

static const struct non_pci_mmio_reg non_pci_mmio[] = {
	{ 0x2d8, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x2e0, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x2e8, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	/* The hardware has a 256 byte alignment requirement for the IOAPIC MMIO base, but we
	   tell the FSP to configure a 4k-aligned base address and this is reported as 4 KiB
	   resource. */
	{ 0x2f0, 0xffffffffff00ull,   4 * KiB, IOMMU_IOAPIC_IDX },
	{ 0x2f8, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x300, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x308, 0xfffffffff000ull,   4 * KiB, NON_PCI_RES_IDX_AUTO },
	{ 0x318, 0xfffffff80000ull, 512 * KiB, NON_PCI_RES_IDX_AUTO },
};

const struct non_pci_mmio_reg *get_iohc_non_pci_mmio_regs(size_t *count)
{
	*count = ARRAY_SIZE(non_pci_mmio);
	return non_pci_mmio;
}
