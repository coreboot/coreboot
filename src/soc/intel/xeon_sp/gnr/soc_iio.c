 /* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/iio.h>

#include <fsp/util.h>
#include <IioPcieConfigUpd.h>

static IIO_BOARD_SETTINGS_HOB iio_upd_hob;

void soc_config_iio_pe_ports(FSPM_UPD *mupd, const struct iio_pe_config *config_table,
				unsigned int num_entries)
{
	int i;
	uint8_t socket, pe, port;

	const struct iio_pe_config *board_pe_config;
	const struct iio_port_config *board_port_config;

	IIO_BOARD_SETTINGS_PER_PE *upd_pe_config;
	IIO_BOARD_SETTINGS_PER_PORT *upd_port_config;

	for (i = 0; i < num_entries; i++) {
		board_pe_config = &config_table[i];
		socket = board_pe_config->socket;
		pe = board_pe_config->pe;

		upd_pe_config = &(iio_upd_hob.Socket[socket].Pe[pe]);

		if ((socket >= MAX_SOCKET) || (pe >= MAX_IIO_PCIE_PER_SOCKET))
			continue;

		for (port = 0; port < MAX_IIO_PORTS_PER_STACK; port++) {
			upd_port_config = &(upd_pe_config->Port[port]);
			board_port_config = &(board_pe_config->port_config[port]);

			upd_pe_config->Bifurcation = board_pe_config->bifurcation;
			upd_pe_config->CxlSupportInUba = board_pe_config->cxl_support;

			upd_port_config->Vpp.Address = board_port_config->vpp_address;
			upd_port_config->Vpp.Port = board_port_config->vpp_port;
			upd_port_config->Vpp.MuxAddress	= board_port_config->vpp_mux_address;
			upd_port_config->Vpp.MuxChannel	= board_port_config->vpp_mux_channel;

			upd_port_config->Slot.Eip = board_port_config->slot_eip;
			upd_port_config->Slot.HotPlugSurprise = board_port_config->slot_hps;
			upd_port_config->Slot.PowerInd = board_port_config->slot_pind;
			upd_port_config->Slot.AttentionInd = board_port_config->slot_aind;
			upd_port_config->Slot.PowerCtrl = board_port_config->slot_pctl;
			upd_port_config->Slot.AttentionBtn = board_port_config->slot_abtn;

			upd_port_config->VppEnabled = board_port_config->vpp_enabled;
			upd_port_config->VppExpType = board_port_config->vpp_exp_type;

			upd_port_config->SlotImplemented = board_port_config->slot_implemented;
			upd_port_config->HotPlug = board_port_config->hot_plug;
			upd_port_config->MrlSensorPresent = board_port_config->mrl_sensor_present;
			upd_port_config->SlotPowerLimitScale = board_port_config->slot_power_limit_scale;
			upd_port_config->SlotPowerLimitValue = board_port_config->slot_power_limit_value;
			upd_port_config->PhysicalSlotNumber = board_port_config->physical_slot_number;
		}
	}

	mupd->FspmConfig.IioBoardSettingsHobPtr = (UINT32)&iio_upd_hob;
	mupd->FspmConfig.IioBoardSettingsHobLength = sizeof(iio_upd_hob);
}
