/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <assert.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/cr.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/reg_access.h>

static uint16_t get_gpe0_address(uint32_t reg_address)
{
	uint32_t gpe0_base_address;

	/* Get the GPE0 base address */
	gpe0_base_address = pci_read_config32(LPC_BDF, R_QNC_LPC_GPE0BLK);
	ASSERT(gpe0_base_address >= 0x80000000);
	gpe0_base_address &= B_QNC_LPC_GPE0BLK_MASK;

	/* Return the GPE0 register address */
	return (uint16_t)(gpe0_base_address + reg_address);
}

static uint32_t *get_gpio_address(uint32_t reg_address)
{
	uint32_t gpio_base_address;

	/* Get the GPIO base address */
	gpio_base_address = pci_read_config32(I2CGPIO_BDF, PCI_BASE_ADDRESS_1);
	gpio_base_address &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	ASSERT(gpio_base_address != 0x00000000);

	/* Return the GPIO register address */
	return (uint32_t *)(gpio_base_address + reg_address);
}

void *get_i2c_address(void)
{
	uint32_t gpio_base_address;

	/* Get the GPIO base address */
	gpio_base_address = pci_read_config32(I2CGPIO_BDF, PCI_BASE_ADDRESS_0);
	gpio_base_address &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	ASSERT(gpio_base_address != 0x00000000);

	/* Return the GPIO register address */
	return (void *)gpio_base_address;
}

static uint16_t get_legacy_gpio_address(uint32_t reg_address)
{
	uint32_t gpio_base_address;

	/* Get the GPIO base address */
	gpio_base_address = pci_read_config32(LPC_BDF, R_QNC_LPC_GBA_BASE);
	ASSERT(gpio_base_address >= 0x80000000);
	gpio_base_address &= B_QNC_LPC_GPA_BASE_MASK;

	/* Return the GPIO register address */
	return (uint16_t)(gpio_base_address + reg_address);
}

static uint32_t mtrr_index_to_host_bridge_register_offset(unsigned long index)
{
	uint32_t offset;

	/* Convert from MTRR index to host bridge offset (Datasheet 12.7.2) */
	if (index == MTRR_CAP_MSR)
		offset = QUARK_NC_HOST_BRIDGE_IA32_MTRR_CAP;
	else if (index == MTRR_DEF_TYPE_MSR)
		offset = QUARK_NC_HOST_BRIDGE_IA32_MTRR_DEF_TYPE;
	else if (index == MTRR_FIX_64K_00000)
		offset = QUARK_NC_HOST_BRIDGE_MTRR_FIX64K_00000;
	else if ((index >= MTRR_FIX_16K_80000) && (index <= MTRR_FIX_16K_A0000))
		offset = ((index - MTRR_FIX_16K_80000) << 1)
			+ QUARK_NC_HOST_BRIDGE_MTRR_FIX16K_80000;
	else if ((index >= MTRR_FIX_4K_C0000) && (index <= MTRR_FIX_4K_F8000))
		offset = ((index - MTRR_FIX_4K_C0000) << 1)
			+ QUARK_NC_HOST_BRIDGE_IA32_MTRR_PHYSBASE0;
	else if ((index >= MTRR_PHYS_BASE(0)) && (index <= MTRR_PHYS_MASK(7)))
		offset = (index - MTRR_PHYS_BASE(0))
			+ QUARK_NC_HOST_BRIDGE_IA32_MTRR_PHYSBASE0;
	else {
		printk(BIOS_SPEW, "index: 0x%08lx\n", index);
		die("Invalid MTRR index specified!\n");
	}
	return offset;
}

void mcr_write(uint8_t opcode, uint8_t port, uint32_t reg_address)
{
	pci_write_config32(MC_BDF, QNC_ACCESS_PORT_MCR,
		(opcode << QNC_MCR_OP_OFFSET)
		| ((uint32_t)port << QNC_MCR_PORT_OFFSET)
		| ((reg_address & QNC_MCR_MASK) << QNC_MCR_REG_OFFSET)
		| QNC_MCR_BYTE_ENABLES);
}

uint32_t mdr_read(void)
{
	return pci_read_config32(MC_BDF, QNC_ACCESS_PORT_MDR);
}

void mdr_write(uint32_t value)
{
	pci_write_config32(MC_BDF, QNC_ACCESS_PORT_MDR, value);
}

void mea_write(uint32_t reg_address)
{
	pci_write_config32(MC_BDF, QNC_ACCESS_PORT_MEA, reg_address
		& QNC_MEA_MASK);
}

uint32_t port_reg_read(uint8_t port, uint32_t offset)
{
	/* Read the port register */
	mea_write(offset);
	mcr_write(QUARK_OPCODE_READ, port, offset);
	return mdr_read();
}

void port_reg_write(uint8_t port, uint32_t offset, uint32_t value)
{
	/* Write the port register */
	mea_write(offset);
	mdr_write(value);
	mcr_write(QUARK_OPCODE_WRITE, port, offset);
}

static CRx_TYPE reg_cpu_cr_read(uint32_t reg_address)
{
	/* Read the CPU CRx register */
	switch (reg_address) {
	case 0:
		return read_cr0();

	case 4:
		return read_cr4();
	}
	die("ERROR - Unsupported CPU register!\n");
}

static void reg_cpu_cr_write(uint32_t reg_address, CRx_TYPE value)
{
	/* Write the CPU CRx register */
	switch (reg_address) {
	default:
		die("ERROR - Unsupported CPU register!\n");

	case 0:
		write_cr0(value);
		break;

	case 4:
		write_cr4(value);
		break;
	}
}

static uint32_t reg_gpe0_read(uint32_t reg_address)
{
	/* Read the GPE0 register */
	return inl(get_gpe0_address(reg_address));
}

static void reg_gpe0_write(uint32_t reg_address, uint32_t value)
{
	/* Write the GPE0 register */
	outl(value, get_gpe0_address(reg_address));
}

static uint32_t reg_gpio_read(uint32_t reg_address)
{
	/* Read the GPIO register */
	return *get_gpio_address(reg_address);
}

static void reg_gpio_write(uint32_t reg_address, uint32_t value)
{
	/* Write the GPIO register */
	*get_gpio_address(reg_address) = value;
}

uint32_t reg_host_bridge_unit_read(uint32_t reg_address)
{
	/* Read the host bridge register */
	mea_write(reg_address);
	mcr_write(QUARK_OPCODE_READ, QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
		reg_address);
	return mdr_read();
}

static void reg_host_bridge_unit_write(uint32_t reg_address, uint32_t value)
{
	/* Write the host bridge register */
	mea_write(reg_address);
	mdr_write(value);
	mcr_write(QUARK_OPCODE_WRITE, QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
		reg_address);
}

uint32_t reg_legacy_gpio_read(uint32_t reg_address)
{
	/* Read the legacy GPIO register */
	return inl(get_legacy_gpio_address(reg_address));
}

void reg_legacy_gpio_write(uint32_t reg_address, uint32_t value)
{
	/* Write the legacy GPIO register */
	outl(value, get_legacy_gpio_address(reg_address));
}

static uint32_t reg_pcie_afe_read(uint32_t reg_address)
{
	/* Read the PCIE AFE register */
	mea_write(reg_address);
	mcr_write(QUARK_OPCODE_IO_READ, QUARK_SC_PCIE_AFE_SB_PORT_ID,
		reg_address);
	return mdr_read();
}

static void reg_pcie_afe_write(uint32_t reg_address, uint32_t value)
{
	/* Write the PCIE AFE register */
	mea_write(reg_address);
	mdr_write(value);
	mcr_write(QUARK_OPCODE_IO_WRITE, QUARK_SC_PCIE_AFE_SB_PORT_ID,
		reg_address);
}

uint32_t reg_rmu_temp_read(uint32_t reg_address)
{
	/* Read the RMU temperature register */
	mea_write(reg_address);
	mcr_write(QUARK_OPCODE_READ, QUARK_NC_RMU_SB_PORT_ID, reg_address);
	return mdr_read();
}

static void reg_rmu_temp_write(uint32_t reg_address, uint32_t value)
{
	/* Write the RMU temperature register */
	mea_write(reg_address);
	mdr_write(value);
	mcr_write(QUARK_OPCODE_WRITE, QUARK_NC_RMU_SB_PORT_ID, reg_address);
}

static uint32_t reg_soc_unit_read(uint32_t reg_address)
{
	/* Read the temperature sensor register */
	mea_write(reg_address);
	mcr_write(QUARK_ALT_OPCODE_READ, QUARK_SCSS_SOC_UNIT_SB_PORT_ID,
		reg_address);
	return mdr_read();
}

static void reg_soc_unit_write(uint32_t reg_address, uint32_t value)
{
	/* Write the temperature sensor register */
	mea_write(reg_address);
	mdr_write(value);
	mcr_write(QUARK_ALT_OPCODE_WRITE, QUARK_SCSS_SOC_UNIT_SB_PORT_ID,
		reg_address);
}

static uint32_t reg_usb_read(uint32_t reg_address)
{
	/* Read the USB register */
	mea_write(reg_address);
	mcr_write(QUARK_ALT_OPCODE_READ, QUARK_SC_USB_AFE_SB_PORT_ID,
		reg_address);
	return mdr_read();
}

static void reg_usb_write(uint32_t reg_address, uint32_t value)
{
	/* Write the USB register */
	mea_write(reg_address);
	mdr_write(value);
	mcr_write(QUARK_ALT_OPCODE_WRITE, QUARK_SC_USB_AFE_SB_PORT_ID,
		reg_address);
}

static uint64_t reg_read(struct reg_script_context *ctx)
{
	const struct reg_script *step = ctx->step;
	uint64_t value = 0;

	switch (step->id) {
	default:
		printk(BIOS_ERR,
			"ERROR - Unknown register set (0x%08x)!\n",
			step->id);
		ctx->display_features = REG_SCRIPT_DISPLAY_NOTHING;
		return 0;

	case CPU_CR:
		ctx->display_prefix = "CPU CR";
		value = reg_cpu_cr_read(step->reg);
		break;

	case GPE0_REGS:
		ctx->display_prefix = "GPE0";
		value = reg_gpe0_read(step->reg);
		break;

	case GPIO_REGS:
		ctx->display_prefix = "GPIO";
		value = reg_gpio_read(step->reg);
		break;

	case HOST_BRIDGE:
		ctx->display_prefix = "Host Bridge";
		value = reg_host_bridge_unit_read(step->reg);
		break;

	case LEG_GPIO_REGS:
		ctx->display_prefix = "Legacy GPIO";
		value = reg_legacy_gpio_read(step->reg);
		break;

	case PCIE_AFE_REGS:
		ctx->display_prefix = "PCIe AFE";
		value = reg_pcie_afe_read(step->reg);
		break;

	case RMU_TEMP_REGS:
		ctx->display_prefix = "RMU TEMP";
		value = reg_rmu_temp_read(step->reg);
		break;

	case SOC_UNIT_REGS:
		ctx->display_prefix = "SOC Unit";
		value = reg_soc_unit_read(step->reg);
		break;

	case USB_PHY_REGS:
		ctx->display_prefix = "USB PHY";
		value = reg_usb_read(step->reg);
		break;
	}
	return value;
}

static void reg_write(struct reg_script_context *ctx)
{
	const struct reg_script *step = ctx->step;

	switch (step->id) {
	default:
		printk(BIOS_ERR,
			"ERROR - Unknown register set (0x%08x)!\n",
			step->id);
		ctx->display_features = REG_SCRIPT_DISPLAY_NOTHING;
		return;

	case CPU_CR:
		ctx->display_prefix = "CPU CR";
		reg_cpu_cr_write(step->reg, step->value);
		break;

	case GPE0_REGS:
		ctx->display_prefix = "GPE0";
		reg_gpe0_write(step->reg, (uint32_t)step->value);
		break;

	case GPIO_REGS:
		ctx->display_prefix = "GPIO";
		reg_gpio_write(step->reg, (uint32_t)step->value);
		break;

	case HOST_BRIDGE:
		ctx->display_prefix = "Host Bridge";
		reg_host_bridge_unit_write(step->reg, (uint32_t)step->value);
		break;

	case LEG_GPIO_REGS:
		ctx->display_prefix = "Legacy GPIO";
		reg_legacy_gpio_write(step->reg, (uint32_t)step->value);
		break;

	case PCIE_AFE_REGS:
		ctx->display_prefix = "PCIe AFE";
		reg_pcie_afe_write(step->reg, (uint32_t)step->value);
		break;

	case PCIE_RESET:
		if (ctx->display_features) {
			ctx->display_prefix = "PCIe reset";
			ctx->display_features &= ~REG_SCRIPT_DISPLAY_REGISTER;
		}
		mainboard_gpio_pcie_reset(step->value);
		break;

	case RMU_TEMP_REGS:
		ctx->display_prefix = "RMU TEMP";
		reg_rmu_temp_write(step->reg, (uint32_t)step->value);
		break;

	case SOC_UNIT_REGS:
		ctx->display_prefix = "SOC Unit";
		reg_soc_unit_write(step->reg, (uint32_t)step->value);
		break;

	case MICROSECOND_DELAY:
		/* The actual delay is >= the requested delay */
		if (ctx->display_features) {
			/* Higher baud-rates will reduce the impact of
			 * displaying this message
			 */
			printk(BIOS_INFO, "Delay %lld uSec\n", step->value);
			ctx->display_features = REG_SCRIPT_DISPLAY_NOTHING;
		}
		udelay(step->value);
		break;

	case USB_PHY_REGS:
		ctx->display_prefix = "USB PHY";
		reg_usb_write(step->reg, (uint32_t)step->value);
		break;
	}
}

msr_t soc_msr_read(unsigned int index)
{
	uint32_t offset;
	union {
		uint64_t u64;
		msr_t msr;
	} value;

	/* Read the low 32-bits of the register */
	offset = mtrr_index_to_host_bridge_register_offset(index);
	value.u64 = port_reg_read(QUARK_NC_HOST_BRIDGE_SB_PORT_ID, offset);

	/* For 64-bit registers, read the upper 32-bits */
	if ((offset >=  QUARK_NC_HOST_BRIDGE_MTRR_FIX64K_00000)
		&& (offset <= QUARK_NC_HOST_BRIDGE_MTRR_FIX4K_F8000)) {
		offset += 1;
		value.u64 |= port_reg_read(QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
					   offset);
	}
	return value.msr;
}

void soc_msr_write(unsigned int index, msr_t msr)
{
	uint32_t offset;
	union {
		uint32_t u32[2];
		msr_t msr;
	} value;

	/* Write the low 32-bits of the register */
	value.msr = msr;
	offset = mtrr_index_to_host_bridge_register_offset(index);
	port_reg_write(QUARK_NC_HOST_BRIDGE_SB_PORT_ID, offset, value.u32[0]);

	/* For 64-bit registers, write the upper 32-bits */
	if ((offset >=  QUARK_NC_HOST_BRIDGE_MTRR_FIX64K_00000)
		&& (offset <= QUARK_NC_HOST_BRIDGE_MTRR_FIX4K_F8000)) {
		offset += 1;
		port_reg_write(QUARK_NC_HOST_BRIDGE_SB_PORT_ID, offset,
				value.u32[1]);
	}
}

const struct reg_script_bus_entry soc_reg_script_bus_table = {
	SOC_TYPE, reg_read, reg_write
};

REG_SCRIPT_BUS_ENTRY(soc_reg_script_bus_table);
