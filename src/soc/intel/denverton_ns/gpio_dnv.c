/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <string.h>
#include <device/mmio.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>

#include <soc/iomap.h>
#include <soc/pcr.h>
#include <soc/soc_util.h>
#include <soc/gpio_dnv.h>

//         Community               PadOwnOffset                HostOwnOffset
//         GpiIsOffset
//         GpiIeOffset             GpiGpeStsOffset             GpiGpeEnOffset
//         SmiStsOffset
//         SmiEnOffset             NmiStsOffset                NmiEnOffset
//         PadCfgLockOffset
//         PadCfgLockTxOffset      PadCfgOffset                PadPerGroup
static const struct GPIO_GROUP_INFO mGpioGroupInfo[] = {
	{PID_GPIOCOM0, R_PCH_PCR_GPIO_NC_PAD_OWN, R_PCH_PCR_GPIO_NC_HOSTSW_OWN,
	 R_PCH_PCR_GPIO_NC_GPI_IS, R_PCH_PCR_GPIO_NC_GPI_IE,
	 R_PCH_PCR_GPIO_NC_GPI_GPE_STS, R_PCH_PCR_GPIO_NC_GPI_GPE_EN,
	 R_PCH_PCR_GPIO_NC_SMI_STS, R_PCH_PCR_GPIO_NC_SMI_EN,
	 R_PCH_PCR_GPIO_NC_NMI_STS, R_PCH_PCR_GPIO_NC_NMI_EN,
	 R_PCH_PCR_GPIO_NC_PADCFGLOCK, R_PCH_PCR_GPIO_NC_PADCFGLOCKTX,
	 R_PCH_PCR_GPIO_NC_PADCFG_OFFSET,
	 V_PCH_GPIO_NC_PAD_MAX}, // DNV NORTH_ALL
	{PID_GPIOCOM1, R_PCH_PCR_GPIO_SC_DFX_PAD_OWN,
	 R_PCH_PCR_GPIO_SC_DFX_HOSTSW_OWN, R_PCH_PCR_GPIO_SC_DFX_GPI_IS,
	 R_PCH_PCR_GPIO_SC_DFX_GPI_IE, R_PCH_PCR_GPIO_SC_DFX_GPI_GPE_STS,
	 R_PCH_PCR_GPIO_SC_DFX_GPI_GPE_EN, NO_REGISTER_FOR_PROPERTY,
	 NO_REGISTER_FOR_PROPERTY, NO_REGISTER_FOR_PROPERTY,
	 NO_REGISTER_FOR_PROPERTY, R_PCH_PCR_GPIO_SC_DFX_PADCFGLOCK,
	 R_PCH_PCR_GPIO_SC_DFX_PADCFGLOCKTX,
	 R_PCH_PCR_GPIO_SC_DFX_PADCFG_OFFSET,
	 V_PCH_GPIO_SC_DFX_PAD_MAX}, // DNV SOUTH_DFX
	{PID_GPIOCOM1, R_PCH_PCR_GPIO_SC0_PAD_OWN,
	 R_PCH_PCR_GPIO_SC0_HOSTSW_OWN, R_PCH_PCR_GPIO_SC0_GPI_IS,
	 R_PCH_PCR_GPIO_SC0_GPI_IE, R_PCH_PCR_GPIO_SC0_GPI_GPE_STS,
	 R_PCH_PCR_GPIO_SC0_GPI_GPE_EN, R_PCH_PCR_GPIO_SC0_SMI_STS,
	 R_PCH_PCR_GPIO_SC0_SMI_EN, R_PCH_PCR_GPIO_SC0_NMI_STS,
	 R_PCH_PCR_GPIO_SC0_NMI_EN, R_PCH_PCR_GPIO_SC0_PADCFGLOCK,
	 R_PCH_PCR_GPIO_SC0_PADCFGLOCKTX, R_PCH_PCR_GPIO_SC0_PADCFG_OFFSET,
	 V_PCH_GPIO_SC0_PAD_MAX}, // DNV South Community 0
	{PID_GPIOCOM1, R_PCH_PCR_GPIO_SC1_PAD_OWN,
	 R_PCH_PCR_GPIO_SC1_HOSTSW_OWN, R_PCH_PCR_GPIO_SC1_GPI_IS,
	 R_PCH_PCR_GPIO_SC1_GPI_IE, R_PCH_PCR_GPIO_SC1_GPI_GPE_STS,
	 R_PCH_PCR_GPIO_SC1_GPI_GPE_EN, R_PCH_PCR_GPIO_SC1_SMI_STS,
	 R_PCH_PCR_GPIO_SC1_SMI_EN, R_PCH_PCR_GPIO_SC1_NMI_STS,
	 R_PCH_PCR_GPIO_SC1_NMI_EN, R_PCH_PCR_GPIO_SC1_PADCFGLOCK,
	 R_PCH_PCR_GPIO_SC1_PADCFGLOCKTX, R_PCH_PCR_GPIO_SC1_PADCFG_OFFSET,
	 V_PCH_GPIO_SC1_PAD_MAX}, // DNV South Community 1
};

/* Retrieve address and length of GPIO info table */
static struct GPIO_GROUP_INFO *
GpioGetGroupInfoTable(uint32_t *GpioGroupInfoTableLength)
{
	*GpioGroupInfoTableLength = ARRAY_SIZE(mGpioGroupInfo);
	return (struct GPIO_GROUP_INFO *)mGpioGroupInfo;
}

/* Get Gpio Pad Ownership */
static void GpioGetPadOwnership(GPIO_PAD GpioPad, GPIO_PAD_OWN *PadOwnVal)
{
	uint32_t Mask;
	uint32_t RegOffset;
	uint32_t GroupIndex;
	uint32_t PadNumber;
	struct GPIO_GROUP_INFO *GpioGroupInfo;
	uint32_t GpioGroupInfoLength;
	uint32_t PadOwnRegValue;

	GroupIndex = GPIO_GET_GROUP_INDEX_FROM_PAD(GpioPad);
	PadNumber = GPIO_GET_PAD_NUMBER(GpioPad);

	GpioGroupInfo = GpioGetGroupInfoTable(&GpioGroupInfoLength);

	//
	// Check if group argument exceeds GPIO GROUP INFO array
	//
	if ((uint32_t)GroupIndex >= GpioGroupInfoLength) {
		printk(BIOS_ERR, "GPIO ERROR: Group argument (%d) exceeds GPIO "
				 "group range\n",
		       GroupIndex);
		return;
	}

	//
	// Check if legal pin number
	//
	if (PadNumber >= GpioGroupInfo[GroupIndex].PadPerGroup) {
		printk(BIOS_ERR, "GPIO ERROR: Pin number (%d) exceeds possible "
				 "range for this group\n",
		       PadNumber);
		return;
	}
	//
	// Calculate RegOffset using Pad Ownership offset and GPIO Pad number.
	// One DWord register contains information for 8 pads.
	//
	RegOffset =
		GpioGroupInfo[GroupIndex].PadOwnOffset + (PadNumber >> 3) * 0x4;

	//
	// Calculate pad bit position within DWord register
	//
	PadNumber %= 8;
	Mask = ((1 << 1) | (1 << 0)) << (PadNumber * 4);

	PadOwnRegValue = read32((void *)PCH_PCR_ADDRESS(
		GpioGroupInfo[GroupIndex].Community, RegOffset));

	*PadOwnVal = (GPIO_PAD_OWN)((PadOwnRegValue & Mask) >> (PadNumber * 4));
}

void gpio_configure_dnv_pads(const struct dnv_pad_config *gpio, size_t num)
{
	/* Return if gpio not valid */
	if ((gpio == NULL) || (num == 0))
		return;

	uint32_t Index;
	uint32_t Dw0Reg;
	uint32_t Dw0RegMask;
	uint32_t Dw1Reg;
	uint32_t Dw1RegMask;
	uint32_t PadCfgReg;
	uint64_t HostSoftOwnReg[V_PCH_GPIO_GROUP_MAX];
	uint64_t HostSoftOwnRegMask[V_PCH_GPIO_GROUP_MAX];
	uint64_t GpiGpeEnReg[V_PCH_GPIO_GROUP_MAX];
	uint64_t GpiGpeEnRegMask[V_PCH_GPIO_GROUP_MAX];
	struct GPIO_GROUP_INFO *GpioGroupInfo;
	uint32_t GpioGroupInfoLength;
	GPIO_PAD GpioGroupOffset;
	uint32_t NumberOfGroups;
	GPIO_PAD_OWN PadOwnVal;
	struct dnv_pad_config *GpioData;
	GPIO_PAD Group;
	uint32_t GroupIndex;
	uint32_t PadNumber;
	uint32_t FinalValue;
	uint32_t Data32;
	uint32_t PadMode1, PadMode2;

	PadOwnVal = GpioPadOwnHost;

	memset(HostSoftOwnReg, 0, sizeof(HostSoftOwnReg));
	memset(HostSoftOwnRegMask, 0, sizeof(HostSoftOwnRegMask));
	memset(GpiGpeEnReg, 0, sizeof(GpiGpeEnReg));
	memset(GpiGpeEnRegMask, 0, sizeof(GpiGpeEnRegMask));

	GpioGroupInfo = GpioGetGroupInfoTable(&GpioGroupInfoLength);

	GpioGroupOffset = GPIO_DNV_GROUP_MIN;
	NumberOfGroups = V_PCH_GPIO_GROUP_MAX;

	for (Index = 0; Index < (uint32_t)num; Index++) {

		Dw0RegMask = 0;
		Dw0Reg = 0;
		Dw1RegMask = 0;
		Dw1Reg = 0;

		GpioData = (struct dnv_pad_config *)&(gpio[Index]);

		Group = GPIO_GET_GROUP_FROM_PAD(GpioData->GpioPad);
		GroupIndex = GPIO_GET_GROUP_INDEX_FROM_PAD(GpioData->GpioPad);
		PadNumber = GPIO_GET_PAD_NUMBER(GpioData->GpioPad);

		//
		// Check if group index argument exceeds GPIO group index range
		//
		if (GroupIndex >= V_PCH_GPIO_GROUP_MAX) {
			printk(BIOS_ERR, "GPIO ERROR: Invalid Group Index "
					 "(GroupIndex=%d, Pad=%d)!\n",
			       GroupIndex, PadNumber);
			continue;
		}

		//
		// Check if group argument exceeds GPIO group range
		//
		if ((Group < GpioGroupOffset) ||
		    (Group >= NumberOfGroups + GpioGroupOffset)) {
			printk(BIOS_ERR,
			       "GPIO ERROR: Invalid Group (Group=%d)!\n",
			       Group);
			return;
		}

		//
		// Check if legal pin number
		//
		if (PadNumber >= GpioGroupInfo[GroupIndex].PadPerGroup) {
			printk(BIOS_ERR, "GPIO ERROR: Invalid PadNumber "
					 "(PadNumber=%d)!\n",
			       PadNumber);
			return;
		}

		//
		// Check if selected GPIO Pad is not owned by CSME/ISH
		//
		GpioGetPadOwnership(GpioData->GpioPad, &PadOwnVal);

		if (PadOwnVal != GpioPadOwnHost) {
			printk(BIOS_ERR, "GPIO WARNING: Accessing pad not "
					 "owned by host (Group=%d, Pad=%d)!",
			       GroupIndex, PadNumber);
			if (PadOwnVal == GpioPadOwnCsme)
				printk(BIOS_ERR, "The owner is CSME\n");
			else if (PadOwnVal == GpioPadOwnIsh)
				printk(BIOS_ERR, "The owner is ISH\n");
			printk(BIOS_ERR, "** Please make sure the GPIO usage "
					 "in sync between CSME/ISH and Host IA "
					 "FW configuration.\n");
			printk(BIOS_ERR, "** All the GPIO occupied by CSME/ISH "
					 "should not do any configuration by "
					 "Host IA FW.\n");
			continue;
		}

		//
		// Configure Reset Type (PadRstCfg)
		//
		Dw0RegMask |=
			((((GpioData->GpioConfig.PowerConfig &
			    GPIO_CONF_RESET_MASK) >>
			   GPIO_CONF_RESET_BIT_POS) == GpioHardwareDefault)
				 ? 0x0
				 : B_PCH_GPIO_RST_CONF);
		Dw0Reg |= (((GpioData->GpioConfig.PowerConfig &
			     GPIO_CONF_RESET_MASK) >>
			    (GPIO_CONF_RESET_BIT_POS + 1))
			   << N_PCH_GPIO_RST_CONF);

		//
		// Configure how interrupt is triggered (RxEvCfg)
		//
		Dw0RegMask |=
			((((GpioData->GpioConfig.InterruptConfig &
			    GPIO_CONF_INT_TRIG_MASK) >>
			   GPIO_CONF_INT_TRIG_BIT_POS) == GpioHardwareDefault)
				 ? 0x0
				 : B_PCH_GPIO_RX_LVL_EDG);
		Dw0Reg |= (((GpioData->GpioConfig.InterruptConfig &
			     GPIO_CONF_INT_TRIG_MASK) >>
			    (GPIO_CONF_INT_TRIG_BIT_POS + 1))
			   << N_PCH_GPIO_RX_LVL_EDG);

		//
		// Configure interrupt generation (GPIRoutIOxAPIC/SCI/SMI/NMI)
		//
		Dw0RegMask |=
			((((GpioData->GpioConfig.InterruptConfig &
			    GPIO_CONF_INT_ROUTE_MASK) >>
			   GPIO_CONF_INT_ROUTE_BIT_POS) == GpioHardwareDefault)
				 ? 0x0
				 : (B_PCH_GPIO_RX_NMI_ROUTE |
				    B_PCH_GPIO_RX_SCI_ROUTE |
				    B_PCH_GPIO_RX_SMI_ROUTE |
				    B_PCH_GPIO_RX_APIC_ROUTE));
		Dw0Reg |= (((GpioData->GpioConfig.InterruptConfig &
			     GPIO_CONF_INT_ROUTE_MASK) >>
			    (GPIO_CONF_INT_ROUTE_BIT_POS + 1))
			   << N_PCH_GPIO_RX_NMI_ROUTE);

		// If CFIO is not Working as GPIO mode, Don't move TxDisable and
		// RxDisable
		if (GpioData->GpioConfig.PadMode == GpioPadModeGpio) {
			//
			// Configure GPIO direction (GPIORxDis and GPIOTxDis)
			//
			Dw0RegMask |= ((((GpioData->GpioConfig.Direction &
					  GPIO_CONF_DIR_MASK) >>
					 GPIO_CONF_DIR_BIT_POS) ==
					GpioHardwareDefault)
					       ? 0x0
					       : (B_PCH_GPIO_RXDIS |
						  B_PCH_GPIO_TXDIS));
			Dw0Reg |= (((GpioData->GpioConfig.Direction &
				     GPIO_CONF_DIR_MASK) >>
				    (GPIO_CONF_DIR_BIT_POS + 1))
				   << N_PCH_GPIO_TXDIS);
		}

		//
		// Configure GPIO input inversion (RXINV)
		//
		Dw0RegMask |= ((((GpioData->GpioConfig.Direction &
				  GPIO_CONF_INV_MASK) >>
				 GPIO_CONF_INV_BIT_POS) == GpioHardwareDefault)
				       ? 0x0
				       : B_PCH_GPIO_RXINV);
		Dw0Reg |= (((GpioData->GpioConfig.Direction &
			     GPIO_CONF_INV_MASK) >>
			    (GPIO_CONF_INV_BIT_POS + 1))
			   << N_PCH_GPIO_RXINV);

		//
		// Configure GPIO output state (GPIOTxState)
		//
		Dw0RegMask |=
			((((GpioData->GpioConfig.OutputState &
			    GPIO_CONF_OUTPUT_MASK) >>
			   GPIO_CONF_OUTPUT_BIT_POS) == GpioHardwareDefault)
				 ? 0x0
				 : B_PCH_GPIO_TX_STATE);
		Dw0Reg |= (((GpioData->GpioConfig.OutputState &
			     GPIO_CONF_OUTPUT_MASK) >>
			    (GPIO_CONF_OUTPUT_BIT_POS + 1))
			   << N_PCH_GPIO_TX_STATE);

		//
		// Configure GPIO RX raw override to '1' (RXRAW1)
		//
		Dw0RegMask |=
			((((GpioData->GpioConfig.OtherSettings &
			    GPIO_CONF_RXRAW_MASK) >>
			   GPIO_CONF_RXRAW_BIT_POS) == GpioHardwareDefault)
				 ? 0x0
				 : B_PCH_GPIO_RX_RAW1);
		Dw0Reg |= (((GpioData->GpioConfig.OtherSettings &
			     GPIO_CONF_RXRAW_MASK) >>
			    (GPIO_CONF_RXRAW_BIT_POS + 1))
			   << N_PCH_GPIO_RX_RAW1);

		//
		// Configure GPIO Pad Mode (PMode)
		//
		Dw0RegMask |=
			((((GpioData->GpioConfig.PadMode &
			    GPIO_CONF_PAD_MODE_MASK) >>
			   GPIO_CONF_PAD_MODE_BIT_POS) == GpioHardwareDefault)
				 ? 0x0
				 : B_PCH_GPIO_PAD_MODE);
		Dw0Reg |= (((GpioData->GpioConfig.PadMode &
			     GPIO_CONF_PAD_MODE_MASK) >>
			    (GPIO_CONF_PAD_MODE_BIT_POS + 1))
			   << N_PCH_GPIO_PAD_MODE);

		//
		// Configure GPIO termination (Term)
		//
		Dw1RegMask |= ((((GpioData->GpioConfig.ElectricalConfig &
				  GPIO_CONF_TERM_MASK) >>
				 GPIO_CONF_TERM_BIT_POS) == GpioHardwareDefault)
				       ? 0x0
				       : B_PCH_GPIO_TERM);
		Dw1Reg |= (((GpioData->GpioConfig.ElectricalConfig &
			     GPIO_CONF_TERM_MASK) >>
			    (GPIO_CONF_TERM_BIT_POS + 1))
			   << N_PCH_GPIO_TERM);

		//
		// Configure GPIO pad tolerance (padtol)
		//
		Dw1RegMask |=
			((((GpioData->GpioConfig.ElectricalConfig &
			    GPIO_CONF_PADTOL_MASK) >>
			   GPIO_CONF_PADTOL_BIT_POS) == GpioHardwareDefault)
				 ? 0x0
				 : B_PCH_GPIO_PADTOL);
		Dw1Reg |= (((GpioData->GpioConfig.ElectricalConfig &
			     GPIO_CONF_PADTOL_MASK) >>
			    (GPIO_CONF_PADTOL_BIT_POS + 1))
			   << N_PCH_GPIO_PADTOL);

		//
		// Check for additional requirements on setting PADCFG register
		//

		//
		// Create PADCFG register offset using group and pad number
		//
		PadCfgReg = 0x8 * PadNumber +
			    GpioGroupInfo[GroupIndex].PadCfgOffset;
		Data32 = read32((void *)PCH_PCR_ADDRESS(
			GpioGroupInfo[GroupIndex].Community, PadCfgReg));

		FinalValue = ((Data32 & (~Dw0RegMask)) | Dw0Reg);

		PadMode1 =
			(Data32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE;
		PadMode2 =
			(Dw0Reg & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE;

		if (((Data32 & B_PCH_GPIO_PAD_MODE) !=
		     (FinalValue & B_PCH_GPIO_PAD_MODE)) ||
		    (PadMode2 == 0)) {
			printk(BIOS_DEBUG, "Changing GpioPad PID: %x Offset: "
					   "0x%x PadModeP1: %d P2: %d ",
			       GpioGroupInfo[GroupIndex].Community, PadCfgReg,
			       PadMode1, PadMode2);
			printk(BIOS_DEBUG, "R: 0x%08x Fx%08x !\n", Data32,
			       FinalValue);
			//
			// Write PADCFG DW0 register``
			//
			mmio_andthenor32(
				(void *)(uint32_t)PCH_PCR_ADDRESS(
					GpioGroupInfo[GroupIndex].Community,
					PadCfgReg),
				~(uint32_t)Dw0RegMask, (uint32_t)Dw0Reg);
		}

		Data32 = read32((void *)PCH_PCR_ADDRESS(
			GpioGroupInfo[GroupIndex].Community, PadCfgReg + 0x4));
		FinalValue = ((Data32 & (~Dw1RegMask)) | Dw1Reg);
		if (Data32 != FinalValue) {
			//
			// Write PADCFG DW1 register
			//
			mmio_andthenor32(
				(void *)(uint32_t)PCH_PCR_ADDRESS(
					GpioGroupInfo[GroupIndex].Community,
					PadCfgReg + 0x4),
				~(uint32_t)Dw1RegMask, (uint32_t)Dw1Reg);
		}

		//
		// Update value to be programmed in HOSTSW_OWN register
		//
		HostSoftOwnRegMask[GroupIndex] |=
			((uint64_t)GpioData->GpioConfig.HostSoftPadOwn & 0x1) << PadNumber;
		HostSoftOwnReg[GroupIndex] |=
			((uint64_t)GpioData->GpioConfig.HostSoftPadOwn >> 0x1) << PadNumber;

		//
		// Update value to be programmed in GPI_GPE_EN register
		//
		GpiGpeEnRegMask[GroupIndex] |=
			((uint64_t)GpioData->GpioConfig.InterruptConfig & 0x1) << PadNumber;
		GpiGpeEnReg[GroupIndex] |=
			(((uint64_t)GpioData->GpioConfig.InterruptConfig & GpioIntSci) >> 3)
			<< PadNumber;
	}

	for (Index = 0; Index < NumberOfGroups; Index++) {
		//
		// Write HOSTSW_OWN registers
		//
		if (GpioGroupInfo[Index].HostOwnOffset !=
		    NO_REGISTER_FOR_PROPERTY) {
			mmio_andthenor32(
				(void *)PCH_PCR_ADDRESS(
					GpioGroupInfo[Index].Community,
					GpioGroupInfo[Index].HostOwnOffset),
				~(uint32_t)(HostSoftOwnRegMask[Index] &
					    0xFFFFFFFF),
				(uint32_t)(HostSoftOwnReg[Index] & 0xFFFFFFFF));
			mmio_andthenor32(
				(void *)PCH_PCR_ADDRESS(
					GpioGroupInfo[Index].Community,
					GpioGroupInfo[Index].HostOwnOffset +
						0x4),
				~(uint32_t)(HostSoftOwnRegMask[Index] >> 32),
				(uint32_t)(HostSoftOwnReg[Index] >> 32));
		}

		//
		// Write GPI_GPE_EN registers
		//
		if (GpioGroupInfo[Index].GpiGpeEnOffset !=
		    NO_REGISTER_FOR_PROPERTY) {
			mmio_andthenor32(
				(void *)PCH_PCR_ADDRESS(
					GpioGroupInfo[Index].Community,
					GpioGroupInfo[Index].GpiGpeEnOffset),
				~(uint32_t)(GpiGpeEnRegMask[Index] &
					    0xFFFFFFFF),
				(uint32_t)(GpiGpeEnReg[Index] & 0xFFFFFFFF));
			mmio_andthenor32(
				(void *)PCH_PCR_ADDRESS(
					GpioGroupInfo[Index].Community,
					GpioGroupInfo[Index].GpiGpeEnOffset +
						0x4),
				~(uint32_t)(GpiGpeEnRegMask[Index] >> 32),
				(uint32_t)(GpiGpeEnReg[Index] >> 32));
		}
	}
}
