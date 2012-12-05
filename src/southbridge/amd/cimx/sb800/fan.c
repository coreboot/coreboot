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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <southbridge/amd/cimx/cimx_util.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>
#include <southbridge/amd/cimx/sb800/sb_cimx.h>
#include <southbridge/amd/cimx/sb800/fan.h>
#include <arch/io.h>
#include <arch/romcc_io.h>

/*
 * For a new motherboard, copy the fan_config.example.h into the mainboard
 * directory, rename it to fan_config.h and customize it.
 */
#include "fan_config.h"


void init_sb800_MANUAL_fans(void)
{
	int i;

	/* Init Fan 0 */
#if FAN0_ENABLED
	unsigned char	fan0_values[FAN_REGISTER_COUNT] = FAN0_REG_VALUES;

	for (i = 0; i < FAN_REGISTER_COUNT; i++)
		pm2_iowrite(FAN_0_OFFSET + i, fan0_values[i]);

#endif

	/* Init Fan 1 */
#if FAN1_ENABLED
	unsigned char	fan1_values[FAN_REGISTER_COUNT] = FAN1_REG_VALUES;

	for (i = 0; i < FAN_REGISTER_COUNT; i++)
		pm2_iowrite(FAN_1_OFFSET + i, fan1_values[i]);

#endif

	/* Init Fan 2 */
#if FAN2_ENABLED
	unsigned char	fan2_values[FAN_REGISTER_COUNT] = FAN2_REG_VALUES;

	for (i = 0; i < FAN_REGISTER_COUNT; i++)
		pm2_iowrite(FAN_2_OFFSET + i, fan2_values[i]);

#endif

	/* Init Fan 3 */
#if FAN3_ENABLED
	unsigned char	fan3_values[FAN_REGISTER_COUNT] = FAN3_REG_VALUES;

	for (i = 0; i < FAN_REGISTER_COUNT; i++)
		pm2_iowrite(FAN_3_OFFSET + i, fan3_values[i]);

#endif

	/* Init Fan 4 */
#if FAN4_ENABLED
	unsigned char	fan4_values[FAN_REGISTER_COUNT] = FAN4_REG_VALUES;

	for (i = 0; i < FAN_REGISTER_COUNT; i++)
		pm2_iowrite(FAN_4_OFFSET + i, fan4_values[i]);

#endif


}

void init_sb800_IMC_fans(void)
{

	AMDSBCFG sb_config;
	unsigned char *message_ptr;
	int i;

	/*
	 * The default I/O address of the IMC configuration register index
	 *  port is 0x6E. Change the IMC Config port I/O Address if it
	 *  conflicts with Other Components in the system.
	 *
	 * Device 20, Function 3, Reg 0xA4
	 * [0]: if 1, the address specified in IMC_PortAddress is used.
	 * [15:1] IMC_PortAddress bits 15:1 (0x17 - address 0x2E )
	 */

	pci_write_config16(PCI_DEV(0, 20, 3), 0xA4, IMC_PORT_ADDRESS | 0x01);

	/*
	 * Do an initial manual setup of the fans for things like polarity
	 * and frequency.
	 */
	init_sb800_MANUAL_fans();

	/*
	 * FLAG for Func 81/83/85/89 support (1=On,0=Off)
	 * Bit0-3   = Func 81 Zone0-Zone3
	 * Bit4-7   = Func 83 Zone0-Zone3
	 * Bit8-11  = Func 85 Zone0-Zone3
	 * Bit12-15 = Func 89 Tempin Channel0-Channel3
	 */
	sb_config.Pecstruct.IMCFUNSupportBitMap =
		(IMC_FAN_ZONE0_ENABLED * 0x111) | (IMC_FAN_ZONE1_ENABLED * 0x222) |
		(IMC_FAN_ZONE2_ENABLED * 0x444) | (IMC_FAN_ZONE3_ENABLED * 0x888) |
		(IMC_TEMPIN0_ENABLED << 12)     | (IMC_TEMPIN1_ENABLED << 13)  |
		(IMC_TEMPIN2_ENABLED << 14)     | (IMC_TEMPIN3_ENABLED << 15);

/*
 ********** Zone 0 **********
 */
#if IMC_FAN_ZONE0_ENABLED
	unsigned char	fan0_config_vals[IMC_FAN_CONFIG_COUNT] = ZONE0_CONFIG_VALS;
	unsigned char	fan0_thresholds[IMC_FAN_THRESHOLD_COUNT] = ZONE0_THRESHOLDS;
	unsigned char	fan0_speeds[IMC_FAN_SPEED_COUNT]= ZONE0_FANSPEEDS;

	/* EC LDN9 function 81 zone 0 */
	sb_config.Pecstruct.MSGFun81zone0MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun81zone0MSGREG1 = IMC_ZONE0;
	message_ptr = &sb_config.Pecstruct.MSGFun81zone0MSGREG2;
	for (i = 0; i < IMC_FAN_CONFIG_COUNT ; i++ )
		*(message_ptr + i) = fan0_config_vals[i];

	/* EC LDN9 function 83 zone 0 - Temperature Thresholds */
	sb_config.Pecstruct.MSGFun83zone0MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun83zone0MSGREG1 = IMC_ZONE0;
	sb_config.Pecstruct.MSGFun83zone0MSGREGB = 0x00;
	message_ptr = &sb_config.Pecstruct.MSGFun83zone0MSGREG2;
	for (i = 0; i < IMC_FAN_THRESHOLD_COUNT ; i++ )
		*(message_ptr + i) = fan0_thresholds[i];

	/*EC LDN9 function 85 zone 0 - Fan Speeds */
	sb_config.Pecstruct.MSGFun85zone0MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun85zone0MSGREG1 = IMC_ZONE0;
	message_ptr = &sb_config.Pecstruct.MSGFun85zone0MSGREG2;
	for (i = 0; i < IMC_FAN_SPEED_COUNT ; i++ )
		*(message_ptr + i) = fan0_speeds[i];

#endif /* IMC_FAN_ZONE0_ENABLED */

/*
 ********** Zone 1 **********
 */
#if IMC_FAN_ZONE1_ENABLED
	unsigned char	fan1_config_vals[IMC_FAN_CONFIG_COUNT] = ZONE1_CONFIG_VALS;
	unsigned char	fan1_thresholds[IMC_FAN_THRESHOLD_COUNT] = ZONE1_THRESHOLDS;
	unsigned char	fan1_speeds[IMC_FAN_SPEED_COUNT]= ZONE1_FANSPEEDS;

	/* EC LDN9 function 81 zone 1 */
	sb_config.Pecstruct.MSGFun81zone1MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun81zone1MSGREG1 = IMC_ZONE1;
	message_ptr = &sb_config.Pecstruct.MSGFun81zone1MSGREG2;
	for (i = 0; i < IMC_FAN_CONFIG_COUNT ; i++ )
		*(message_ptr + i) = fan1_config_vals[i];

	/* EC LDN9 function 83 zone 1 - Temperature Thresholds */
	sb_config.Pecstruct.MSGFun83zone1MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun83zone1MSGREG1 = IMC_ZONE1;
	sb_config.Pecstruct.MSGFun83zone1MSGREGB = 0x00;
	message_ptr = &sb_config.Pecstruct.MSGFun83zone1MSGREG2;
	for (i = 0; i < IMC_FAN_THRESHOLD_COUNT ; i++ )
		*(message_ptr + i) = fan1_thresholds[i];

	/* EC LDN9 function 85 zone 1 - Fan Speeds */
	sb_config.Pecstruct.MSGFun85zone1MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun85zone1MSGREG1 = IMC_ZONE1;
	message_ptr = &sb_config.Pecstruct.MSGFun85zone1MSGREG2;
	for (i = 0; i < IMC_FAN_SPEED_COUNT ; i++ )
		*(message_ptr + i) = fan1_speeds[i];


#endif /* IMC_FAN_ZONE1_ENABLED */


/*
 ********** Zone 2 **********
 */
#if IMC_FAN_ZONE2_ENABLED
	unsigned char	fan2_config_vals[IMC_FAN_CONFIG_COUNT] = ZONE2_CONFIG_VALS;
	unsigned char	fan2_thresholds[IMC_FAN_THRESHOLD_COUNT] = ZONE2_THRESHOLDS;
	unsigned char	fan2_speeds[IMC_FAN_SPEED_COUNT]= ZONE2_FANSPEEDS;

	/* EC LDN9 function 81 zone 2 */
	sb_config.Pecstruct.MSGFun81zone2MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun81zone2MSGREG1 = IMC_ZONE2;
	message_ptr = &sb_config.Pecstruct.MSGFun81zone2MSGREG2;
	for (i = 0; i < IMC_FAN_CONFIG_COUNT ; i++ )
		*(message_ptr + i) = fan2_config_vals[i];

	/* EC LDN9 function 83 zone 2 */
	sb_config.Pecstruct.MSGFun83zone2MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun83zone2MSGREG1 = IMC_ZONE2;
	sb_config.Pecstruct.MSGFun83zone2MSGREGB = 0x00;
	message_ptr = &sb_config.Pecstruct.MSGFun83zone2MSGREG2;
	for (i = 0; i < IMC_FAN_THRESHOLD_COUNT ; i++ )
		*(message_ptr + i) = fan2_thresholds[i];

	/* EC LDN9 function 85 zone 2 */
	sb_config.Pecstruct.MSGFun85zone2MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun85zone2MSGREG1 = IMC_ZONE2;
	message_ptr = &sb_config.Pecstruct.MSGFun85zone2MSGREG2;
	for (i = 0; i < IMC_FAN_SPEED_COUNT ; i++ )
		*(message_ptr + i) = fan2_speeds[i];

#endif /* IMC_FAN_ZONE2_ENABLED */


/*
 ********** Zone 3 **********
 */

#if IMC_FAN_ZONE3_ENABLED
	unsigned char	fan3_config_vals[IMC_FAN_CONFIG_COUNT] = ZONE3_CONFIG_VALS;
	unsigned char	fan3_thresholds[IMC_FAN_THRESHOLD_COUNT] = ZONE3_THRESHOLDS;
	unsigned char	fan3_speeds[IMC_FAN_SPEED_COUNT]= ZONE3_FANSPEEDS;

	/* EC LDN9 function 81 zone 3 */
	sb_config.Pecstruct.MSGFun81zone3MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun81zone3MSGREG1 = IMC_ZONE3;
	message_ptr = &sb_config.Pecstruct.MSGFun81zone3MSGREG2;
	for (i = 0; i < IMC_FAN_CONFIG_COUNT ; i++ )
		*(message_ptr + i) = fan3_config_vals[i];

	/* EC LDN9 function 83 zone 3 */
	sb_config.Pecstruct.MSGFun83zone3MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun83zone3MSGREG1 = IMC_ZONE3;
	sb_config.Pecstruct.MSGFun83zone3MSGREGB = 0x00;
	message_ptr = &sb_config.Pecstruct.MSGFun83zone3MSGREG2;
	for (i = 0; i < IMC_FAN_THRESHOLD_COUNT ; i++ )
		*(message_ptr + i) = fan1_thresholds[i];

	/* EC LDN9 function 85 zone 3 */
	sb_config.Pecstruct.MSGFun85zone3MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun85zone3MSGREG1 = IMC_ZONE3;
	message_ptr = &sb_config.Pecstruct.MSGFun85zone3MSGREG2;
	for (i = 0; i < IMC_FAN_SPEED_COUNT ; i++ )
		*(message_ptr + i) = fan3_speeds[i];

#endif /* IMC_FAN_ZONE3_ENABLED */


	/*
	 * EC LDN9 funtion 89 - Set HWM TEMPIN Temperature Calculation Parameters
	 * This function provides the critical parameters of the HWM TempIn
	 * sensors, IMC would not perform temperature measurement using those
	 * sensors until the parameters are provided.
	 */

#if IMC_TEMPIN0_ENABLED
	/* EC LDN9 funtion 89 TEMPIN channel 0 */
	sb_config.Pecstruct.MSGFun89zone0MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun89zone0MSGREG1 = 0x00;
	sb_config.Pecstruct.MSGFun89zone0MSGREG2 = ( TEMPIN0_AT & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG3 = ((TEMPIN0_AT >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG4 = ((TEMPIN0_AT >> 16) & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG5 = ((TEMPIN0_AT >> 24) & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG6 = ( TEMPIN0_CT & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG7 = ((TEMPIN0_CT >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG8 = ((TEMPIN0_CT >> 16)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREG9 = ((TEMPIN0_CT >> 24)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone0MSGREGA = TEMPIN0_TUNING_PARAM;
#endif

#if IMC_TEMPIN1_ENABLED
	/* EC LDN9 funtion 89 TEMPIN channel 1 */
	sb_config.Pecstruct.MSGFun89zone1MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun89zone1MSGREG1 = 0x01;
	sb_config.Pecstruct.MSGFun89zone1MSGREG2 = ( TEMPIN1_AT & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG3 = ((TEMPIN1_AT >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG4 = ((TEMPIN1_AT >> 16) & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG5 = ((TEMPIN1_AT >> 24) & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG6 = ( TEMPIN1_CT & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG7 = ((TEMPIN1_CT >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG8 = ((TEMPIN1_CT >> 16)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREG9 = ((TEMPIN1_CT >> 24)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone1MSGREGA = TEMPIN1_TUNING_PARAM;
#endif

#if IMC_TEMPIN2_ENABLED
	/* EC LDN9 funtion 89 TEMPIN channel 2 */
	sb_config.Pecstruct.MSGFun89zone2MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun89zone2MSGREG1 = 0x02;
	sb_config.Pecstruct.MSGFun89zone2MSGREG2 = ( TEMPIN2_AT & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG3 = ((TEMPIN2_AT >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG4 = ((TEMPIN2_AT >> 16) & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG5 = ((TEMPIN2_AT >> 24) & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG6 = ( TEMPIN2_CT & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG7 = ((TEMPIN2_CT >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG8 = ((TEMPIN2_CT >> 16)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREG9 = ((TEMPIN2_CT >> 24)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone2MSGREGA = TEMPIN2_TUNING_PARAM;
#endif

#if IMC_TEMPIN3_ENABLED
	/* EC LDN9 funtion 89 TEMPIN channel 3 */
	sb_config.Pecstruct.MSGFun89zone3MSGREG0 = 0x00;
	sb_config.Pecstruct.MSGFun89zone3MSGREG1 = 0x03;
	sb_config.Pecstruct.MSGFun89zone3MSGREG2 = ( TEMPIN3_AT & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG3 = ((TEMPIN3_AT >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG4 = ((TEMPIN3_AT >> 16) & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG5 = ((TEMPIN3_AT >> 24) & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG6 = ( TEMPIN3_CT & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG7 = ((TEMPIN3_CT >> 8)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG8 = ((TEMPIN3_CT >> 16)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREG9 = ((TEMPIN3_CT >> 24)  & 0xff);
	sb_config.Pecstruct.MSGFun89zone3MSGREGA = TEMPIN3_TUNING_PARAM;
#endif


	/* Set up the sb_config structure for the fan control initialization */
	sb_config.StdHeader.Func = SB_EC_FANCONTROL;

	AmdSbDispatcher(&sb_config);

	return;
}


