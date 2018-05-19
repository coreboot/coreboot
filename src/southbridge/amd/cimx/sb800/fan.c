/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Sage Electronic Engineering, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <southbridge/amd/cimx/cimx_util.h>
#include <device/device.h>
#include <device/pci.h>		/* device_operations */
#include "SBPLATFORM.h"
#include "sb_cimx.h"
#include "chip.h"		/* struct southbridge_amd_cimx_sb800_config */
#include "fan.h"

void init_sb800_MANUAL_fans(struct device *dev)
{
	int i;
	struct southbridge_amd_cimx_sb800_config *sb_chip =
		(struct southbridge_amd_cimx_sb800_config *)(dev->chip_info);

	/* Init Fan 0 */
	if (sb_chip->fan0_enabled)
		for (i = 0; i < FAN_REGISTER_COUNT; i++)
			pm2_iowrite(FAN_0_OFFSET + i, sb_chip->fan0_config_vals[i]);

	/* Init Fan 1 */
	if (sb_chip->fan1_enabled)
		for (i = 0; i < FAN_REGISTER_COUNT; i++)
			pm2_iowrite(FAN_1_OFFSET + i, sb_chip->fan1_config_vals[i]);

	/* Init Fan 2 */
	if (sb_chip->fan2_enabled)
		for (i = 0; i < FAN_REGISTER_COUNT; i++)
			pm2_iowrite(FAN_2_OFFSET + i, sb_chip->fan2_config_vals[i]);

	/* Init Fan 3 */
	if (sb_chip->fan3_enabled)
		for (i = 0; i < FAN_REGISTER_COUNT; i++)
			pm2_iowrite(FAN_3_OFFSET + i, sb_chip->fan3_config_vals[i]);

	/* Init Fan 4 */
	if (sb_chip->fan4_enabled)
		for (i = 0; i < FAN_REGISTER_COUNT; i++)
			pm2_iowrite(FAN_4_OFFSET + i, sb_chip->fan4_config_vals[i]);

}

void init_sb800_IMC_fans(struct device *dev)
{

	AMDSBCFG sb_config;
	unsigned char *message_ptr;
	int i;
	struct southbridge_amd_cimx_sb800_config *sb_chip =
		(struct southbridge_amd_cimx_sb800_config *)(dev->chip_info);

	/*
	 * The default I/O address of the IMC configuration register index
	 *  port is 0x6E. Change the IMC Config port I/O Address if it
	 *  conflicts with other components in the system.
	 *
	 * Device 20, Function 3, Reg 0xA4
	 * [0]: if 1, the address specified in IMC_PortAddress is used.
	 * [15:1] IMC_PortAddress bits 15:1 (0x17 - address 0x2E )
	 */

	pci_write_config16(dev, 0xA4, sb_chip->imc_port_address | 0x01);


	/*
	 * Do an initial manual setup of the fans for things like polarity
	 * and frequency.
	 */
	init_sb800_MANUAL_fans(dev);

	/*
	 * FLAG for Func 81/83/85/89 support (1=On,0=Off)
	 * Bit0-3   = Func 81 Zone0-Zone3
	 * Bit4-7   = Func 83 Zone0-Zone3
	 * Bit8-11  = Func 85 Zone0-Zone3
	 * Bit12-15 = Func 89 Tempin Channel0-Channel3
	 */
	sb_config.Pecstruct.IMCFUNSupportBitMap = 0;

/*
 ********** Zone 0 **********
 */
if (sb_chip->imc_fan_zone0_enabled) {

	sb_config.Pecstruct.IMCFUNSupportBitMap |= IMC_ENABLE_ZONE0;

	/* EC LDN9 function 81 zone 0 */
	sb_config.Pecstruct.MSGFun81zone0MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun81zone0MSGREG1 = IMC_ZONE0;
	message_ptr = &sb_config.Pecstruct.MSGFun81zone0MSGREG2;
	for (i = 0; i < IMC_FAN_CONFIG_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone0_config_vals[i];

	/* EC LDN9 function 83 zone 0 - Temperature Thresholds */
	sb_config.Pecstruct.MSGFun83zone0MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun83zone0MSGREG1 = IMC_ZONE0;
	sb_config.Pecstruct.MSGFun83zone0MSGREGB = 0x00;
	message_ptr = &sb_config.Pecstruct.MSGFun83zone0MSGREG2;
	for (i = 0; i < IMC_FAN_THRESHOLD_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone0_thresholds[i];

	/*EC LDN9 function 85 zone 0 - Fan Speeds */
	sb_config.Pecstruct.MSGFun85zone0MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun85zone0MSGREG1 = IMC_ZONE0;
	message_ptr = &sb_config.Pecstruct.MSGFun85zone0MSGREG2;
	for (i = 0; i < IMC_FAN_SPEED_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone0_fanspeeds[i];

}

/*
 ********** Zone 1 **********
 */
if (sb_chip->imc_fan_zone1_enabled) {

	sb_config.Pecstruct.IMCFUNSupportBitMap |= IMC_ENABLE_ZONE1;

	/* EC LDN9 function 81 zone 1 */
	sb_config.Pecstruct.MSGFun81zone1MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun81zone1MSGREG1 = IMC_ZONE1;
	message_ptr = &sb_config.Pecstruct.MSGFun81zone1MSGREG2;
	for (i = 0; i < IMC_FAN_CONFIG_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone1_config_vals[i];

	/* EC LDN9 function 83 zone 1 - Temperature Thresholds */
	sb_config.Pecstruct.MSGFun83zone1MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun83zone1MSGREG1 = IMC_ZONE1;
	sb_config.Pecstruct.MSGFun83zone1MSGREGB = 0x00;
	message_ptr = &sb_config.Pecstruct.MSGFun83zone1MSGREG2;
	for (i = 0; i < IMC_FAN_THRESHOLD_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone1_thresholds[i];

	/* EC LDN9 function 85 zone 1 - Fan Speeds */
	sb_config.Pecstruct.MSGFun85zone1MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun85zone1MSGREG1 = IMC_ZONE1;
	message_ptr = &sb_config.Pecstruct.MSGFun85zone1MSGREG2;
	for (i = 0; i < IMC_FAN_SPEED_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone1_fanspeeds[i];

}


/*
 ********** Zone 2 **********
 */
if (sb_chip->imc_fan_zone2_enabled) {

	sb_config.Pecstruct.IMCFUNSupportBitMap |= IMC_ENABLE_ZONE2;

	/* EC LDN9 function 81 zone 2 */
	sb_config.Pecstruct.MSGFun81zone2MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun81zone2MSGREG1 = IMC_ZONE2;
	message_ptr = &sb_config.Pecstruct.MSGFun81zone2MSGREG2;
	for (i = 0; i < IMC_FAN_CONFIG_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone2_config_vals[i];

	/* EC LDN9 function 83 zone 2 */
	sb_config.Pecstruct.MSGFun83zone2MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun83zone2MSGREG1 = IMC_ZONE2;
	sb_config.Pecstruct.MSGFun83zone2MSGREGB = 0x00;
	message_ptr = &sb_config.Pecstruct.MSGFun83zone2MSGREG2;
	for (i = 0; i < IMC_FAN_THRESHOLD_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone2_thresholds[i];

	/* EC LDN9 function 85 zone 2 */
	sb_config.Pecstruct.MSGFun85zone2MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun85zone2MSGREG1 = IMC_ZONE2;
	message_ptr = &sb_config.Pecstruct.MSGFun85zone2MSGREG2;
	for (i = 0; i < IMC_FAN_SPEED_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone2_fanspeeds[i];

}

/*
 ********** Zone 3 **********
 */

if (sb_chip->imc_fan_zone3_enabled) {

	sb_config.Pecstruct.IMCFUNSupportBitMap |= IMC_ENABLE_ZONE3;

	/* EC LDN9 function 81 zone 3 */
	sb_config.Pecstruct.MSGFun81zone3MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun81zone3MSGREG1 = IMC_ZONE3;
	message_ptr = &sb_config.Pecstruct.MSGFun81zone3MSGREG2;
	for (i = 0; i < IMC_FAN_CONFIG_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone3_config_vals[i];

	/* EC LDN9 function 83 zone 3 */
	sb_config.Pecstruct.MSGFun83zone3MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun83zone3MSGREG1 = IMC_ZONE3;
	sb_config.Pecstruct.MSGFun83zone3MSGREGB = 0x00;
	message_ptr = &sb_config.Pecstruct.MSGFun83zone3MSGREG2;
	for (i = 0; i < IMC_FAN_THRESHOLD_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone3_thresholds[i];

	/* EC LDN9 function 85 zone 3 */
	sb_config.Pecstruct.MSGFun85zone3MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun85zone3MSGREG1 = IMC_ZONE3;
	message_ptr = &sb_config.Pecstruct.MSGFun85zone3MSGREG2;
	for (i = 0; i < IMC_FAN_SPEED_COUNT; i++ )
		*(message_ptr + i) = sb_chip->imc_zone3_fanspeeds[i];

}

	/*
	 * EC LDN9 function 89 - Set HWM TEMPIN Temperature Calculation Parameters
	 * This function provides the critical parameters of the HWM TempIn
	 * sensors, IMC would not perform temperature measurement using those
	 * sensors until the parameters are provided.
	 */

if (sb_chip->imc_tempin0_enabled) {

	sb_config.Pecstruct.IMCFUNSupportBitMap |= IMC_ENABLE_TEMPIN0;

	/* EC LDN9 function 89 TEMPIN channel 0 */
	sb_config.Pecstruct.MSGFun89zone0MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun89zone0MSGREG1 = 0x00;
	sb_config.Pecstruct.MSGFun89zone0MSGREG2 = ( sb_chip->imc_tempin0_at & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG3 = ((sb_chip->imc_tempin0_at >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG4 = ((sb_chip->imc_tempin0_at >> 16) & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG5 = ((sb_chip->imc_tempin0_at >> 24) & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG6 = ( sb_chip->imc_tempin0_ct & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG7 = ((sb_chip->imc_tempin0_ct >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG8 = ((sb_chip->imc_tempin0_ct >> 16)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG9 = ((sb_chip->imc_tempin0_ct >> 24)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREGA = sb_chip->imc_tempin0_tuning_param;
}

if (sb_chip->imc_tempin1_enabled) {

	sb_config.Pecstruct.IMCFUNSupportBitMap |= IMC_ENABLE_TEMPIN1;

	/* EC LDN9 function 89 TEMPIN channel 1 */
	sb_config.Pecstruct.MSGFun89zone1MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun89zone1MSGREG1 = 0x01;
	sb_config.Pecstruct.MSGFun89zone1MSGREG2 = ( sb_chip->imc_tempin1_at & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG3 = ((sb_chip->imc_tempin1_at >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG4 = ((sb_chip->imc_tempin1_at >> 16) & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG5 = ((sb_chip->imc_tempin1_at >> 24) & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG6 = ( sb_chip->imc_tempin1_ct & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG7 = ((sb_chip->imc_tempin1_ct >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG8 = ((sb_chip->imc_tempin1_ct >> 16)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG9 = ((sb_chip->imc_tempin1_ct >> 24)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREGA = sb_chip->imc_tempin1_tuning_param;
}

if (sb_chip->imc_tempin2_enabled) {

	sb_config.Pecstruct.IMCFUNSupportBitMap |= IMC_ENABLE_TEMPIN2;

	/* EC LDN9 function 89 TEMPIN channel 2 */
	sb_config.Pecstruct.MSGFun89zone2MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun89zone2MSGREG1 = 0x02;
	sb_config.Pecstruct.MSGFun89zone2MSGREG2 = ( sb_chip->imc_tempin2_at & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG3 = ((sb_chip->imc_tempin2_at >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG4 = ((sb_chip->imc_tempin2_at >> 16) & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG5 = ((sb_chip->imc_tempin2_at >> 24) & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG6 = ( sb_chip->imc_tempin2_ct & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG7 = ((sb_chip->imc_tempin2_ct >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG8 = ((sb_chip->imc_tempin2_ct >> 16)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG9 = ((sb_chip->imc_tempin2_ct >> 24)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREGA = sb_chip->imc_tempin2_tuning_param;
}

if (sb_chip->imc_tempin3_enabled) {

	sb_config.Pecstruct.IMCFUNSupportBitMap |= IMC_ENABLE_TEMPIN3;

	/* EC LDN9 function 89 TEMPIN channel 3 */
	sb_config.Pecstruct.MSGFun89zone3MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun89zone3MSGREG1 = 0x03;
	sb_config.Pecstruct.MSGFun89zone3MSGREG2 = ( sb_chip->imc_tempin3_at & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG3 = ((sb_chip->imc_tempin3_at >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG4 = ((sb_chip->imc_tempin3_at >> 16) & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG5 = ((sb_chip->imc_tempin3_at >> 24) & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG6 = ( sb_chip->imc_tempin3_ct & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG7 = ((sb_chip->imc_tempin3_ct >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG8 = ((sb_chip->imc_tempin3_ct >> 16)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG9 = ((sb_chip->imc_tempin3_ct >> 24)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREGA = sb_chip->imc_tempin3_tuning_param;
}

	/* Set up the sb_config structure for the fan control initialization */
	sb_config.StdHeader.Func = SB_EC_FANCONTROL;

	AmdSbDispatcher(&sb_config);

	return;
}
