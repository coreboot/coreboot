/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT6687D_HWM_H
#define SUPERIO_NUVOTON_NCT6687D_HWM_H

#include <assert.h>
#include <types.h>

#include "chip.h"
#include "nct6687d_ec.h"

/*
 * EC page number will be concatenated with the register offset to mark
 * which page the register belongs to.
 */
#define EC_PAGE(page)				((page) << 8)
#define EC_PAGE_REG(page, reg)			(EC_PAGE(page) + (reg))

/* EC page 1 */
#define SENSOR_READ_HI_REG(x)			EC_PAGE_REG(1, 0x00 + 2*(x))
#define SENSOR_READ_LO_REG(x)			EC_PAGE_REG(1, 0x01 + 2*(x))
#define FAN_RPM_READ_HI_REG(x)			EC_PAGE_REG(1, 0x40 + 2*((x) - 1))
#define FAN_RPM_READ_LO_REG(x)			EC_PAGE_REG(1, 0x41 + 2*((x) - 1))

#define FAN_DUTY_REG(fan) \
	(((fan) > 8)	? EC_PAGE_REG(8, 0xf0 + ((fan)-8)) \
			: EC_PAGE_REG(1, 0x60 + 2*((fan) - 1)))

#define SENSOR_MON_STS_REG			EC_PAGE_REG(1, 0x70)
#define   TEMP_OVERLOAD				(1 << 1)
#define   VOLT_OVERLOAD				(1 << 1)
#define   FAN_OVERLOAD				(1 << 2)

/* Errors and statuses */
#define PROCHOT_MON_STS_REG			EC_PAGE_REG(1, 0x71)
#define SENSOR_ERROR_STS_REG(x)			EC_PAGE_REG(1, 0x74 + (x))
#define TMPIN_IDLE_STS_REG(x)			EC_PAGE_REG(1, 0x78 + (x))
#define FANIN_ERROR_STS_REG(x)			EC_PAGE_REG(1, 0x7C + (x))
#define FANOUT_ERROR_STS_REG			EC_PAGE_REG(1, 0x7e)
#define FAN_INIT_STS_REG			EC_PAGE_REG(1, 0x7f)

#define HWM_CONFIG_REG				EC_PAGE_REG(1, 0x80)
#define   SMI_SCI_SEL				(1 << 2)
#define   LOCK_SENSOR_CFG			(1 << 5)
#define   HWM_EN				(1 << 7)

#define SENSOR_MON_VCC_DELAY_REG		EC_PAGE_REG(1, 0x81)
#define TEMPIN_IDLE_COUNTER_REG			EC_PAGE_REG(1, 0x82)
#define VIRTUAL_INPUT_CFG_REG			EC_PAGE_REG(1, 0x83)
#define TMPIN_POLLING_RATE_REG			EC_PAGE_REG(1, 0x85)
#define VIN_POLLING_RATE_REG			EC_PAGE_REG(1, 0x86)
#define FAN_POLLING_RATE_REG			EC_PAGE_REG(1, 0x87)

#define ADC_CLOCKK_DIV_REG			EC_PAGE_REG(1, 0x88)
#define ADC_AVG_NUM_REG				EC_PAGE_REG(1, 0x89)
#define ADC_SLOPE_HI_REG			EC_PAGE_REG(1, 0x8a)
#define ADC_SLOPE_LO_REG			EC_PAGE_REG(1, 0x8b)

#define PROCHOT_CFG_REG				EC_PAGE_REG(1, 0x8e)
#define   REFRESH_RATE_MASK			(3 << 0)
#define     REFRESH_RATE_100MS			(0 << 0)
#define     REFRESH_RATE_200MS			(1 << 0)
#define     REFRESH_RATE_500MS			(2 << 0)
#define     REFRESH_RATE_1S			(3 << 0)
#define   AVG_TIME_MASK				(3 << 2)
#define     AVG_TIME_1				(0 << 2)
#define     AVG_TIME_2				(1 << 2)
#define     AVG_TIME_4				(2 << 2)
#define     AVG_TIME_8				(3 << 2)
#define   MONITOR_MODE_MASK			(3 << 4)
#define     MONITOR_MODE_CAPTURE		(0 << 4)
#define     MONITOR_MODE_8BIT			(1 << 4)
#define     MONITOR_MODE_12BIT			(2 << 4)
#define     MONITOR_MODE_16BIT			(3 << 4)
#define   CLK_SRC_40K				(0 << 6)
#define   CLK_SRC_MCLK				(1 << 6)
#define   PROCHOT_EN				(1 << 7)

#define PROCHOT_OUT_REG				EC_PAGE_REG(1, 0x8f)

#define VIRTUAL_SENSOR_INPUT_REG(x)		EC_PAGE_REG(1, 0x90 + (x))

#define SENSOR_CFG_REG(x)			EC_PAGE_REG(1, 0xa0 + (x))
#define  FILTER_EN				(1 << 7)
#define  SENSOR_SRC_SEL_MASK			0x7f

#define FANIN_CFG_REG(fan)			EC_PAGE_REG(1, 0xc0 + ((fan) - 1))
#define   FANIN_PIN_SEL_MASK			0x1f
#define   FANIN_PULSE_REV_MASK			(3 << 5)
#define   FANIN_PULSE_REV_SHIFT			5
#define   FANIN_MONITOR_EN			(1 << 7)

#define FANOUT_CFG_REG(fan) \
	(((fan) > 8)	? EC_PAGE_REG(8, 0xf4 + ((fan) - 9)) \
			: EC_PAGE_REG(1, 0xd0 + ((fan) - 1)))
#define   FANOUT_PIN_SEL_MASK			0x1f
#define   FANOUT_TYPE_PP			(0 << 5)
#define   FANOUT_TYPE_OD			(1 << 5)
#define   FANOUT_INV_EN				(1 << 6)
#define   FANOUT_EN				(1 << 7)

#define FAN_INIT_RPM_HI_REG(fan)		EC_PAGE_REG(1, 0xe0 + 2*((fan) - 1))
#define FAN_INIT_RPM_LO_REG(fan)		EC_PAGE_REG(1, 0xe1 + 2*((fan) - 1))
#define FAN_INIT_DUTY_VAL_REG(fan)		EC_PAGE_REG(1, 0xf0 + ((fan) - 1))
#define FAN_INIT_MODE_REG			EC_PAGE_REG(1, 0xf8)
#define   FAN_INIT_MODE_RPM(fan)		(1 << ((fan) - 1))
#define   FAN_INIT_MODE_PWM(fan)		(0 << ((fan) - 1))

#define FAN_RPM_INIT_UNIT_REG			EC_PAGE_REG(1, 0xf9)
#define FAN_RPM_INIT_TOLERANCE_REG		EC_PAGE_REG(1, 0xfa)
#define FAN_RPM_INIT_STABLE_COUNTER_REG		EC_PAGE_REG(1, 0xfb)
#define FAN_RPM_INIT_TIME_COUNTER_REG		EC_PAGE_REG(1, 0xfc)

/* EC Page 2 - GPIO */

#define TIN_OFFSET_REG(x)			EC_PAGE_REG(2, 0x00 + (x))

#define GPI_FALLING_TRIG_EN_HI_REG		EC_PAGE_REG(2, 0xb0)
#define GPI_FALLING_TRIG_EN_LO_REG		EC_PAGE_REG(2, 0xb1)
#define GPI_RISING_TRIG_EN_HI_REG		EC_PAGE_REG(2, 0xb2)
#define GPI_RISING_TRIG_EN_LO_REG		EC_PAGE_REG(2, 0xb3)
#define GPI_DATA_HI_REG				EC_PAGE_REG(2, 0xb4)
#define GPI_DATA_LO_REG				EC_PAGE_REG(2, 0xb5)
#define GPI_SEL_REG(x)				EC_PAGE_REG(2, 0xc0 + (x))
#define GPO_TYPE_HI_REG				EC_PAGE_REG(2, 0xd0)
#define GPO_TYPE_LO_REG				EC_PAGE_REG(2, 0xd1)
#define GPO_DATA_HI_REG				EC_PAGE_REG(2, 0xd4)
#define GPO_DATA_LO_REG				EC_PAGE_REG(2, 0xd5)
#define GPO_SEL_REG(x)				EC_PAGE_REG(2, 0xe0 + (x))

/* EC page 3 */

#define SENSOR_INT_CFG_REG(x)			EC_PAGE_REG(3, 0x10 + (x))
#define   TMPIN_INT_SEL_MASK			(7 << 0)
#define   TMPIN_INT_DISABLE			(0 << 0)
#define   TMPIN_INT_SHUTDOWN_MODE		(1 << 0)
#define   TMPIN_INT_SYST_FAN_COMP_MODE		(2 << 0)
#define   TMPIN_INT_CPUT_FAN_COMP_MODE		(3 << 0)
#define   TMPIN_INT_TWO_TIMES_MODE		(4 << 0)
#define   TMPIN_INT_ONE_TIME_MODE		(5 << 0)
#define   TMPIN_INT_OVT_COMP_MODE		(6 << 0)
#define   TMPIN_INT_OVT_MODE			(7 << 0)
#define   VIN_INT_EN				(1 << 7)

#define TMPIN_HYSTERSIS_REG(x)			EC_PAGE_REG(3, 0x30 + (x))
#define TMPIN_OVERTEMP_REG(x)			EC_PAGE_REG(3, 0x50 + (x))
#define TIN_SHUTDOWN_VIN_LIMIT_HI_REG(x)	EC_PAGE_REG(3, 0x70 + 2*(x))
#define TIN_SHUTDOWN_VIN_LIMIT_LO_REG(x)	EC_PAGE_REG(3, 0x71 + 2*(x))

#define FAN_RPM_INT_EN1_REG			EC_PAGE_REG(3, 0xb0)
#define FAN_RPM_INT_EN2_REG			EC_PAGE_REG(3, 0xb1)

#define FAN_RPM_LIMIT_HI_REG(fan)		EC_PAGE_REG(3, 0xb8 + 2*(fan))
#define FAN_RPM_LIMIT_LO_REG(fan)		EC_PAGE_REG(3, 0xb9 + 2*(fan))

/* EC Page 4 - SMBus */

#define PCH_ERR_STS_REG				EC_PAGE_REG(4, 0x00)
#define TSI_ERR_STS_REG				EC_PAGE_REG(4, 0x01)
#define SMBUS_MASTER_ERR_STS_REG		EC_PAGE_REG(4, 0x04)
#define SMBUS_THERM_SENSOR_ERR_STS_REG(x)	EC_PAGE_REG(4, 0x06 + (x))
#define SMBUS_DIMM_SENSOR_ERR_STS_REG(x)	EC_PAGE_REG(4, 0x0c + (x))

#define PCH_THERMAL_DATA_CFG_REG		EC_PAGE_REG(4, 0x10)
#define   PCH_BAUD_SEL_MASK			(7 << 0)
#define      PCH_BAUD_12_5K			(0  << 0)
#define      PCH_BAUD_25K			(1  << 0)
#define      PCH_BAUD_50K			(2  << 0)
#define      PCH_BAUD_100K			(3  << 0)
#define      PCH_BAUD_200K			(4  << 0)
#define      PCH_BAUD_400K			(5  << 0)
#define      PCH_BAUD_800K			(6  << 0)
#define      PCH_BAUD_1200K			(7  << 0)
#define   PCH_PORT_SEL_MASK			(3 << 4)
#define     PCH_THERMAL_PORT(x)			((x) << 4)
#define   PCH_ONE_BYTE_REPORT			(1 << 7)

#define PCH_DEVICE_ADDR_REG			EC_PAGE_REG(4, 0x11)
#define PCH_THERMAL_CMD_REG			EC_PAGE_REG(4, 0x12)

#define TSI_THERMAL_DATA_CFG_REG		EC_PAGE_REG(4, 0x14)
#define   TSI_BAUD_SEL_MASK			(7 << 0)
#define      TSI_BAUD_12_5K			(0  << 0)
#define      TSI_BAUD_25K			(1  << 0)
#define      TSI_BAUD_50K			(2  << 0)
#define      TSI_BAUD_100K			(3  << 0)
#define      TSI_BAUD_200K			(4  << 0)
#define      TSI_BAUD_400K			(5  << 0)
#define      TSI_BAUD_800K			(6  << 0)
#define      TSI_BAUD_1200K			(7  << 0)
#define   TSI_PORT_SEL_MASK			(3 << 4)
#define     TSI_THERMAL_PORT(x)			((x) << 4)

#define SMBUS_SENSOR_CFG_REG(x)			EC_PAGE_REG(4, 0x1a + 5*(x))
#define   SMB_BAUD_SEL_MASK			(7 << 0)
#define      SMB_BAUD_12_5K			(0  << 0)
#define      SMB_BAUD_25K			(1  << 0)
#define      SMB_BAUD_50K			(2  << 0)
#define      SMB_BAUD_100K			(3  << 0)
#define      SMB_BAUD_200K			(4  << 0)
#define      SMB_BAUD_400K			(5  << 0)
#define      SMB_BAUD_800K			(6  << 0)
#define      SMB_BAUD_1200K			(7  << 0)
#define   SMB_PORT_SEL_MASK			(3 << 4)
#define     SMB_SENSOR_PORT(x)			((x) << 4)
#define   SMB_SENSOR_DATA_LEN_1B		(0 << 6)
#define   SMB_SENSOR_DATA_LEN_2B		(1 << 6)
#define   SMB_SENSOR_DIRECT_ACCESS		(1 << 7)

#define SMBUS_SENSOR_ADDR_REG(x)		EC_PAGE_REG(4, 0x1b + 5*(x))
#define SMBUS_SENSOR_CMD_REG(x)			EC_PAGE_REG(4, 0x1c + 5*(x))
#define SMBUS_SENSOR_DATA_REG(x)		EC_PAGE_REG(4, 0x1d + 5*(x))
#define SMBUS_SENSOR_READ_CMD_REG(x)		EC_PAGE_REG(4, 0x1e + 5*(x))

#define SMBUS_DIMM_SENSOR_CFG0(x)		EC_PAGE_REG(4, 0x3a + 4*(x))
#define   SMB_DIMM_ADDR_MASK			0x07
#define   SMB_DIMM_BAUD_SEL_MASK		(7 << 4)
#define      SMB_DIMM_BAUD_12_5K		(0  << 0)
#define      SMB_DIMM_BAUD_25K			(1  << 0)
#define      SMB_DIMM_BAUD_50K			(2  << 0)
#define      SMB_DIMM_BAUD_100K			(3  << 0)
#define      SMB_DIMM_BAUD_200K			(4  << 0)
#define      SMB_DIMM_BAUD_400K			(5  << 0)
#define      SMB_DIMM_BAUD_800K			(6  << 0)
#define      SMB_DIMM_BAUD_1200K		(7  << 0)
#define   SMBUS_DIMM_SENSOR_EN			(1 << 7)

#define SMBUS_DIMM_SENSOR_CFG1_REG(x)		EC_PAGE_REG(4, 0x3b + 4*(x))
/* Bits 0-2 is address as in CFG0, bit 7 is DIMM sensor enable */
#define   SMD_DIMM_ADDR_MASK			0x07
#define   SMB_PORT_SEL_MASK			(3 << 4)
#define     SMB_SENSOR_PORT(x)			((x) << 4)

#define SMBUS_DIMM_SENSOR_CFG2_REG(x)		EC_PAGE_REG(4, 0x3c + 4*(x))
/* Bits 0-2 is address as in CFG0, bit 7 is DIMM sensor enable */

#define SMBUS_DIMM_SENSOR_CFG3_REG(x)		EC_PAGE_REG(4, 0x3d + 4*(x))
/* Bits 0-2 is address as in CFG0, bit 7 is DIMM sensor enable */

#define SMBUS_MASTER_CFG1_REG			EC_PAGE_REG(4, 0x60)
#define   SMB_MASTER_POLLRATE_MASK		0x07
#define     SMB_MASTER_POLLRATE_ONCE		(0 << 0)
#define     SMB_MASTER_POLLRATE_100MS		(0 << 0)
#define     SMB_MASTER_POLLRATE_200MS		(0 << 0)
#define     SMB_MASTER_POLLRATE_400MS		(0 << 0)
#define     SMB_MASTER_POLLRATE_800MS		(0 << 0)
#define     SMB_MASTER_POLLRATE_1S		(0 << 0)
#define     SMB_MASTER_POLLRATE_2S		(0 << 0)
#define     SMB_MASTER_POLLRATE_4S		(0 << 0)
#define   SMB_MASTER_CLEAR_BUF			(1 << 3)
#define   SMB_MASTER_HOLD_RDY			(1 << 5)
#define   SMB_MASTER_START			(1 << 6)
#define   SMB_MASTER_EN				(1 << 7)

#define SMBUS_MASTER_CFG2_REG			EC_PAGE_REG(4, 0x61)
#define   SMB_MASTER_PORT_SEL_MASK		0x03
#define     SMB_MASTER_PORT(x)			((x) << 0)
#define     SMB_MASTER_PEC_EN			(1 << 3)

#define SMBUS_MASTER_BAUD_RATE_SEL_REG		EC_PAGE_REG(4, 0x62)
#define   SMB_MASTER_BAUD_SEL_MASK		(7 << 4)
#define      SMB_MASTER_BAUD_12_5K		(0  << 0)
#define      SMB_MASTER_BAUD_25K		(1  << 0)
#define      SMB_MASTER_BAUD_50K		(2  << 0)
#define      SMB_MASTER_BAUD_100K		(3  << 0)
#define      SMB_MASTER_BAUD_200K		(4  << 0)
#define      SMB_MASTER_BAUD_400K		(5  << 0)
#define      SMB_MASTER_BAUD_800K		(6  << 0)
#define      SMB_MASTER_BAUD_1200K		(7  << 0)

#define SMBUS_MASTER_PROTOCOL_SEL_REG		EC_PAGE_REG(4, 0x63)
#define   SMBUS_MASTER_QUICK_WRITE		0x00
#define   SMBUS_MASTER_SEND_BYTE		0x01
#define   SMBUS_MASTER_WRITE_BYTE		0x02
#define   SMBUS_MASTER_WRITE_WORD		0x03
#define   SMBUS_MASTER_BLOCK_WRITE		0x04
#define   SMBUS_MASTER_PROCESS_CALL		0x05
#define   SMBUS_MASTER_BLOCK_WRITE_READ		0x06
#define   SMBUS_MASTER_RECEIVE_BYTE		0x81
#define   SMBUS_MASTER_READ_BYTE		0x82
#define   SMBUS_MASTER_READ_WORD		0x83
#define   SMBUS_MASTER_BLOCK_READ		0x84

#define SMBUS_MASTER_BLOCK_WR_LEN_REG		EC_PAGE_REG(4, 0x64)
#define SMBUS_MASTER_DEV_ADDR_REG		EC_PAGE_REG(4, 0x65)
#define SMBUS_MASTER_CMD_REG			EC_PAGE_REG(4, 0x66)

#define SMBUS_MASTER_WRITE_BUFFER(x)		EC_PAGE_REG(4, 0x70 + (x))
#define SMBUS_MASTER_WRITE_BUFFER_LEN		64
#define SMBUS_MASTER_READ_BUFFER(x)		EC_PAGE_REG(4, 0xb0 + (x))
#define SMBUS_MASTER_READ_BUFFER_LEN		64

/* EC page 5 - PECI */

#define PECI_PING_INFO_REG			EC_PAGE_REG(5, 0x00)
#define   PECI_AGENT_ALIVE(agent)		(1 << (agent))
#define   PECI_OVER_SMBUS			(1 << 7)

#define PECI_INFO_STS_REG			EC_PAGE_REG(5, 0x01)
#define   PECI_AGENT_INFO_RDY(agent)		(1 << (agent))

#define PECI_DEVICE_INFO_REG			EC_PAGE_REG(5, 0x02)
#define   PECI_AGENT_TWO_DOMAIN_SUPPORT		(1 << 2)

#define PECI_REVISION_INFO_REG			EC_PAGE_REG(5, 0x03)
#define   PECI_REV_MINOR_MASK			0x0f
#define   PECI_REV_MINOR_SHIFT			0
#define   PECI_REV_MAJOR_MASK			0xf0
#define   PECI_REV_MAJOR_SHIFT			4

#define PECI_DOM0_MARGIN_HI_REG(agent)		EC_PAGE_REG(5, 0x20 + 2*(agent))
#define PECI_DOM0_MARGIN_LO_REG(agent)		EC_PAGE_REG(5, 0x21 + 2*(agent))
#define PECI_DOM1_MARGIN_HI_REG(agent)		EC_PAGE_REG(5, 0x28 + 2*(agent))
#define PECI_DOM1_MARGIN_LO_REG(agent)		EC_PAGE_REG(5, 0x29 + 2*(agent))

#define PECI_DOM0_TEMP_HI_REG(agent)		EC_PAGE_REG(5, 0x30 + 2*(agent))
#define PECI_DOM0_TEMP_LO_REG(agent)		EC_PAGE_REG(5, 0x31 + 2*(agent))
#define PECI_DOM1_TEMP_HI_REG(agent)		EC_PAGE_REG(5, 0x38 + 2*(agent))
#define PECI_DOM1_TEMP_LO_REG(agent)		EC_PAGE_REG(5, 0x39 + 2*(agent))

#define PECI_TCONTROL_REG(dom, agent)		EC_PAGE_REG(5, 0x40 + 4*(dom) + (agent))
#define PECI_TJMAX_REG(dom, agent)		EC_PAGE_REG(5, 0x48 + 4*(dom) + (agent))

#define PECI_PCS_ERR_STS_REG			EC_PAGE_REG(5, 0x52)
#define PECI_MASTER_ERR_STS_REG			EC_PAGE_REG(5, 0x53)

#define PECI_CFG_REG				EC_PAGE_REG(5, 0x60)
#define   PECI_SPEED_SEL_MASK			(3 << 0)
#define     PECI_SPEED_2MHZ			(0 << 0)
#define     PECI_SPEED_1_2MHZ			(1 << 0)
#define     PECI_SPEED_800KHZ			(2 << 0)
#define     PECI_SPEED_400KHZ			(3 << 0)
#define   PECI_USER_TJMAX_FROM_CPU		(0 << 5)
#define   PECI_USER_TJMAX_FROM_EC		(1 << 5)
#define   PECI_AGENT_INIT			(1 << 6)
#define   PECI_EN				(1 << 7)

#define PECI_AGENT_EN_REG			EC_PAGE_REG(5, 0x61)
#define   PECI_AGENT_EN_MASK			0xf
#define   PECI_AGENT_EN(agent)			(1 << (agent))

#define PECI_HOSTID_REG				EC_PAGE_REG(5, 0x62)
#define PECI_ERR_RETRY_CFG_REG			EC_PAGE_REG(5, 0x63)
#define PECI_MARGIN_CFG_REG			EC_PAGE_REG(5, 0x64)
#define   PECI_MARGIN_POLLRATE_MASK		0x07
#define     PECI_MARGIN_POLLRATE_ONCE		(0 << 0)
#define     PECI_MARGIN_POLLRATE_100MS		(1 << 0)
#define     PECI_MARGIN_POLLRATE_200MS		(2 << 0)
#define     PECI_MARGIN_POLLRATE_400MS		(3 << 0)
#define     PECI_MARGIN_POLLRATE_800MS		(4 << 0)
#define     PECI_MARGIN_POLLRATE_1S		(5 << 0)
#define     PECI_MARGIN_POLLRATE_2S		(6 << 0)
#define     PECI_MARGIN_POLLRATE_4S		(7 << 0)
#define   PECI_GET_MARGIN_EN			(1 << 7)

#define PECI_USER_TJMAX_REG(dom, agent)		EC_PAGE_REG(5, 0x68 + 4*(dom) + (agent))
#define PECI_PCS_CFG_REG			0x70
#define   PECI_PCS_POLLRATE_MASK		0x07
#define     PECI_PCS_POLLRATE_ONCE		(0 << 0)
#define     PECI_PCS_POLLRATE_100MS		(1 << 0)
#define     PECI_PCS_POLLRATE_200MS		(2 << 0)
#define     PECI_PCS_POLLRATE_400MS		(3 << 0)
#define     PECI_PCS_POLLRATE_800MS		(4 << 0)
#define     PECI_PCS_POLLRATE_1S		(5 << 0)
#define     PECI_PCS_POLLRATE_2S		(6 << 0)
#define     PECI_PCS_POLLRATE_4S		(7 << 0)
#define   PECI_PCS_STOP_ERR			(1 << 4)
#define   PECI_PCS_HOLD_RDY			(1 << 5)
#define   PECI_PCS_START			(1 << 6)
#define   PECI_PCS_EN				(1 << 7)

#define PECI_PCS_CFG2_REG			EC_PAGE_REG(5, 0x71)
#define   PECI_PCS_AGENT_SEL_MASK		0x03
#define   PECI_PCS_SEL_DOM0			(0 << 4)
#define   PECI_PCS_SEL_DOM1			(1 << 4)
#define   PECI_PCS_WRITE_PKG_CFG		(1 << 7)

#define PECI_PCS_IDX_REG			EC_PAGE_REG(5, 0x72)
#define PECI_PCS_PARAM_LO_REG			EC_PAGE_REG(5, 0x73)
#define PECI_PCS_PARAM_HI_REG			EC_PAGE_REG(5, 0x74)

#define PECI_PCS_COMPLETION_CODE_REG		EC_PAGE_REG(5, 0x75)
#define   PECI_PCS_DATA_VALID			0x40
#define   PECI_PCS_RESP_TIMEOUT_RETRY		0x80
#define   PECI_PCS_RESP_TIMEOUT			0x81
#define   PECI_PCS_INVALID_REQUEST		0x90
#define   PECI_PCS_PECI_CONTROL_HW		0x91

#define PECI_PCS_BUF_REG(x)			EC_PAGE_REG(5, 0x76 + (x))
#define PECI_PCS_BUF_LEN			4

#define PECI_MASTER_CFG_REG			EC_PAGE_REG(5, 0x7a)
#define   PECI_MASTER_POLLRATE_MASK		0x07
#define     PECI_MASTER_POLLRATE_ONCE		(0 << 0)
#define     PECI_MASTER_POLLRATE_100MS		(1 << 0)
#define     PECI_MASTER_POLLRATE_200MS		(2 << 0)
#define     PECI_MASTER_POLLRATE_400MS		(3 << 0)
#define     PECI_MASTER_POLLRATE_800MS		(4 << 0)
#define     PECI_MASTER_POLLRATE_1S		(5 << 0)
#define     PECI_MASTER_POLLRATE_2S		(6 << 0)
#define     PECI_MASTER_POLLRATE_4S		(7 << 0)
#define   PECI_MASTER_STOP_ERR			(1 << 4)
#define   PECI_MASTER_HOLD_RDY			(1 << 5)
#define   PECI_MASTER_START			(1 << 6)
#define   PECI_MASTER_EN			(1 << 7)

#define PECI_MASTER_CFG2_REG			EC_PAGE_REG(5, 0x7b)
#define   PECI_MASTER_CPL_CODE_SUPP		(1 << 6)
#define   PECI_MASTER_AWFCS_EN			(1 << 7)

#define PECI_MASTER_CLIENT_ADDR_REG		EC_PAGE_REG(5, 0x7c)
#define PECI_MASTER_CMD_CODE_REG		EC_PAGE_REG(5, 0x7d)
#define PECI_MASTER_WRITE_LEN_REG		EC_PAGE_REG(5, 0x7e)
#define PECI_MASTER_READ_LEN_REG		EC_PAGE_REG(5, 0x7f)

#define PECI_MASTER_WRITE_BUF(x)		EC_PAGE_REG(5, 0x80 + (x))
#define PECI_MASTER_WRITE_BUF_LEN		12
#define PECI_MASTER_READ_BUF(x)			EC_PAGE_REG(5, 0x90 + (x))
#define PECI_MASTER_READ_BUF_LEN		12

#define PECI_DRAM_TEMP_WR_STS_REG(x)		EC_PAGE_REG(5, 0xa0 + (x))
#define PECI_DIMM_AMB_TEMP_WR_STS_REG(x)	EC_PAGE_REG(5, 0xa4 + (x))
#define PECI_DIMM_TEMP_RD_STS_REG(x)		EC_PAGE_REG(5, 0xa8 + (x))
#define PECI_ACC_ENERGRY_RD_STS_REG(x)		EC_PAGE_REG(5, 0xac + (x))

#define PECI_DRAM_TEMP_WR_POLL_RATE_REG		EC_PAGE_REG(5, 0xb0)
#define   PECI_DRAM_POLLRATE_MASK		0x07
#define     PECI_DRAM_POLLRATE_ONCE		(0 << 0)
#define     PECI_DRAM_POLLRATE_100MS		(1 << 0)
#define     PECI_DRAM_POLLRATE_200MS		(2 << 0)
#define     PECI_DRAM_POLLRATE_400MS		(3 << 0)
#define     PECI_DRAM_POLLRATE_800MS		(4 << 0)
#define     PECI_DRAM_POLLRATE_1S		(5 << 0)
#define     PECI_DRAM_POLLRATE_2S		(6 << 0)
#define     PECI_DRAM_POLLRATE_4S		(7 << 0)

#define PECI_DIMM_AMB_TEMP_WR_POLL_RATE_REG	EC_PAGE_REG(5, 0xb1)
#define   PECI_DIMM_AMB_POLLRATE_MASK		0x07
#define     PECI_DIMM_AMB_POLLRATE_ONCE		(0 << 0)
#define     PECI_DIMM_AMB_POLLRATE_100MS	(1 << 0)
#define     PECI_DIMM_AMB_POLLRATE_200MS	(2 << 0)
#define     PECI_DIMM_AMB_POLLRATE_400MS	(3 << 0)
#define     PECI_DIMM_AMB_POLLRATE_800MS	(4 << 0)
#define     PECI_DIMM_AMB_POLLRATE_1S		(5 << 0)
#define     PECI_DIMM_AMB_POLLRATE_2S		(6 << 0)
#define     PECI_DIMM_AMB_POLLRATE_4S		(7 << 0)

#define PECI_ACC_ENERGRY_RD_POLL_RATE_REG	EC_PAGE_REG(5, 0xb2)
#define   PECI_ACC_ENERGRY_RD_POLLRATE_MASK	0x07
#define     PECI_ACC_ENERGRY_RD_POLLRATE_ONCE	(0 << 0)
#define     PECI_ACC_ENERGRY_RD_POLLRATE_100MS	(1 << 0)
#define     PECI_ACC_ENERGRY_RD_POLLRATE_200MS	(2 << 0)
#define     PECI_ACC_ENERGRY_RD_POLLRATE_400MS	(3 << 0)
#define     PECI_ACC_ENERGRY_RD_POLLRATE_800MS	(4 << 0)
#define     PECI_ACC_ENERGRY_RD_POLLRATE_1S	(5 << 0)
#define     PECI_ACC_ENERGRY_RD_POLLRATE_2S	(6 << 0)
#define     PECI_ACC_ENERGRY_RD_POLLRATE_4S	(7 << 0)

#define PECI_DRAM_TEMP_WR_CFG1_REG(x)		EC_PAGE_REG(5, 0xb8 + 2*(x))
#define   PECI_DRAM_TEMP_WR_CH_IDX_MASK		(7 << 0)
#define   PECI_DRAM_TEMP_WR_CH_IDX_SHIFT	0
#define   PECI_DRAM_TEMP_WR_DIMM_IDX_MASK	(7 << 3)
#define   PECI_DRAM_TEMP_WR_DIMM_IDX_SHIFT	3
#define   PECI_DRAM_TEMP_WR_POLLING		(1 << 6)
#define   PECI_DRAM_TEMP_WR_START		(1 << 7)

#define PECI_DRAM_TEMP_WR_CFG2_REG(x)		EC_PAGE_REG(5, 0xb9 + 2*(x))
#define   PECI_DRAM_TEMP_WR_AGENT_IDX_MASK	(3 << 1)
#define   PECI_DRAM_TEMP_WR_AGENT_IDX_SHIFT	1
#define   PECI_DRAM_TEMP_WR_DOM0		(0 << 6)
#define   PECI_DRAM_TEMP_WR_DOM1		(1 << 6)
#define   PECI_DRAM_TEMP_WR_CH_SEL		(1 << 7)

#define PECI_DIMM_AMB_TEMP_WR_CFG1_REG(x)	EC_PAGE_REG(5, 0xc0 + 2*(x))
#define   PECI_DIMM_AMB_TEMP_WR_CH_IDX_MASK	(7 << 0)
#define   PECI_DIMM_AMB_TEMP_WR_CH_IDX_SHIFT	0
#define   PECI_DIMM_AMB_TEMP_WR_POLLING		(1 << 6)
#define   PECI_DIMM_AMB_TEMP_WR_START		(1 << 7)

#define PECI_DIMM_AMB_TEMP_WR_CFG2_REG(x)	EC_PAGE_REG(5, 0xc1 + 2*(x))
#define   PECI_DIMM_AMB_TEMP_WR_AGENT_IDX_MASK	(3 << 1)
#define   PECI_DIMM_AMB_TEMP_WR_AGENT_IDX_SHIFT	1
#define   PECI_DIMM_AMB_TEMP_WR_DOM0		(0 << 6)
#define   PECI_DIMM_AMB_TEMP_WR_DOM1		(1 << 6)
#define   PECI_DIMM_AMB_TEMP_WR_CH_SEL		(1 << 7)

#define PECI_DIMM_TEMP_RD_CFG1_REG(x)		EC_PAGE_REG(5, 0xc8 + (x))
#define   PECI_DIMM_TEMP_RD_AGENT_IDX_MASK	(3 << 0)
#define   PECI_DIMM_TEMP_RD_AGENT_IDX_SHIFT	0
#define   PECI_DIMM_TEMP_RD_DOM0		(0 << 2)
#define   PECI_DIMM_TEMP_RD_DOM1		(1 << 2)
#define   PECI_DIMM_TEMP_RD_DIMM_SEL_MASK	(3 << 4)
#define     PECI_DIMM_TEMP_RD_DIMM0		(0 << 4)
#define     PECI_DIMM_TEMP_RD_DIMM1		(1 << 4)
#define     PECI_DIMM_TEMP_RD_DIMM0_MAX		(2 << 4)
#define     PECI_DIMM_TEMP_RD_DIMM1_MAX		(3 << 4)
#define   PECI_DIMM_TEMP_RD_DIMM_CH0		(0 << 6)
#define   PECI_DIMM_TEMP_RD_DIMM_CH1		(1 << 6)

#define PECI_ACC_ENERGRY_RD_CFG_REG(x)		EC_PAGE_REG(5, 0xcc + (x))
#define   PECI_ACC_ENERGRY_RD_AGENT_IDX_MASK	(3 << 0)
#define   PECI_ACC_ENERGRY_RD_AGENT_IDX_SHIFT	0
#define   PECI_ACC_ENERGRY_RD_DOM0		(0 << 2)
#define   PECI_ACC_ENERGRY_RD_DOM1		(1 << 2)
#define   PECI_ACC_ENERGRY_RD_REPORT_CURR	(0 << 3)
#define   PECI_ACC_ENERGRY_RD_REPORT_INCR	(1 << 3)
#define   PECI_DIMM_TEMP_RD_PWR_PLANE_SEL_MASK	(3 << 4)
#define     PECI_DIMM_TEMP_RD_PWR_PLANE0	(0 << 4)
#define     PECI_DIMM_TEMP_RD_PWR_PLANE1	(1 << 4)
#define     PECI_DIMM_TEMP_RD_PKG_PWR_PLANE0	(2 << 4)
#define     PECI_DIMM_TEMP_RD_PKG_PWR_PLANE1	(3 << 4)
#define     PECI_ACC_ENERGRY_RD_POLLING		(1 << 6)
#define     PECI_ACC_ENERGRY_RD_START		(1 << 7)

#define PECI_ACC_ENERGRY_RD_VAL_B0_REG(x)	EC_PAGE_REG(5, 0xd0 + 4*(x))
#define PECI_ACC_ENERGRY_RD_VAL_B1_REG(x)	EC_PAGE_REG(5, 0xd1 + 4*(x))
#define PECI_ACC_ENERGRY_RD_VAL_B2_REG(x)	EC_PAGE_REG(5, 0xd2 + 4*(x))
#define PECI_ACC_ENERGRY_RD_VAL_B3_REG(x)	EC_PAGE_REG(5, 0xd3 + 4*(x))

/* EC Page 6 - EC FW Information and Configuration */

#define CHIP_ID0_REG				EC_PAGE_REG(6, 0x00)
#define CHIP_ID1_REG				EC_PAGE_REG(6, 0x01)
#define CUSTOMER_ID0_REG			EC_PAGE_REG(6, 0x02)
#define CUSTOMER_ID1_REG			EC_PAGE_REG(6, 0x03)
#define FW_BUILD_YEAR_REG			EC_PAGE_REG(6, 0x04)
#define FW_BUILD_MONTH_REG			EC_PAGE_REG(6, 0x05)
#define FW_BUILD_DAY_REG			EC_PAGE_REG(6, 0x06)
#define FW_BUILD_SERIALNUM_REG			EC_PAGE_REG(6, 0x07)
#define FW_VER0_REG				EC_PAGE_REG(6, 0x08)
#define FW_VER1_REG				EC_PAGE_REG(6, 0x09)
#define PROFILE_VER_REG				EC_PAGE_REG(6, 0x0a)
#define ROM_VER0_REG				EC_PAGE_REG(6, 0x0c)
#define ROM_VER1_REG				EC_PAGE_REG(6, 0x0d)
#define ROM_VER2_REG				EC_PAGE_REG(6, 0x0e)
#define ROM_VER3_REG				EC_PAGE_REG(6, 0x0f)
#define ISP_BUILD_YEAR_REG			EC_PAGE_REG(6, 0x12)
#define ISP_BUILD_MONTH_REG			EC_PAGE_REG(6, 0x13)
#define ISP_BUILD_DAY_REG			EC_PAGE_REG(6, 0x14)
#define ISP_BUILD_SERIALNUM_REG			EC_PAGE_REG(6, 0x15)
#define ISP_VER0_REG				EC_PAGE_REG(6, 0x16)
#define ISP_VER1_REG				EC_PAGE_REG(6, 0x17)

#define OEM_VER_REG				EC_PAGE_REG(6, 0x18)
#define   OEM_VER_LEN				8

#define EC_ALIVE_COUNTER_REG			EC_PAGE_REG(6, 0x20)

#define EC_HEARTBEAT_CFG_REG			EC_PAGE_REG(6, 0x2e)
#define   EC_HEARBEAT_TOGGLE_RATE_MASK		0x0f
#define   EC_HEARBEAT_TYPE_OD			(0 << 6)
#define   EC_HEARBEAT_TYPE_PP			(1 << 6)
#define   EC_HEARBEAT_EN			(1 << 7)

#define EC_HEARTBEAT_GPSEL_REG			EC_PAGE_REG(6, 0x2f)
#define   EC_HEARBEAT_GPIO_PIN_MASK		0x0f
#define   EC_HEARBEAT_GPIO_PIN_SHIFT		0
#define   EC_HEARBEAT_GPIO_GROUP_MASK		0xf0
#define   EC_HEARBEAT_GPIO_GROUP_SHIFT		4

#define EC_MCU_SPEED_REG			EC_PAGE_REG(6, 0x38)
#define   EC_MCU_SPEED_MCLK_MASK		0x0f
#define     EC_MCU_SPEED_MCLK_10KHZ		(0 << 0)
#define     EC_MCU_SPEED_MCLK_160KHZ		(1 << 0)
#define     EC_MCU_SPEED_MCLK_1MHZ		(2 << 0)
#define     EC_MCU_SPEED_MCLK_4MHZ		(3 << 0)
#define     EC_MCU_SPEED_MCLK_8MHZ		(4 << 0)
#define     EC_MCU_SPEED_MCLK_16MHZ		(5 << 0)
#define     EC_MCU_SPEED_MCLK_24MHZ		(6 << 0)
#define     EC_MCU_SPEED_MCLK_48MHZ		(7 << 0)
#define   EC_MCU_CACHE_EN			(1 << 6)

#define EC_FLASH_SPEED_REG			EC_PAGE_REG(6, 0x39)
#define   EC_FLASH_CLK_DIV_MASK			(3 << 4)
#define     EC_FLASH_CLK_DIV_1			(0 << 4)
#define     EC_FLASH_CLK_DIV_2			(1 << 4)
#define     EC_FLASH_CLK_DIV_4			(2 << 4)
#define     EC_FLASH_CLK_DIV_16			(3 << 4)
#define   EC_FLASH_FETCH_MODE_MASK		(3 << 6)
#define     EC_FLASH_FETCH_MODE_READ		(0 << 6)
#define     EC_FLASH_FETCH_MODE_FAST_READ	(1 << 6)
#define     EC_FLASH_FETCH_MODE_DUAL_OUT	(2 << 6)
#define     EC_FLASH_FETCH_MODE_DUAL_IN_OUT	(3 << 6)

#define EC_FLASH_MANUFACTURE_REG		EC_PAGE_REG(6, 0x3a)
#define EC_FLASH_MEMORY_TYPE_REG		EC_PAGE_REG(6, 0x3b)
#define EC_FLASH_CAPACITY_REG			EC_PAGE_REG(6, 0x3c)

#define EC_MCU_SPEED_CTL_VSB_REG		EC_PAGE_REG(6, 0x40)
#define   EC_FLASH_CLK_VSB_DIV_MASK		(3 << 0)
#define     EC_FLASH_CLK_VSB_DIV_1		(0 << 0)
#define     EC_FLASH_CLK_VSB_DIV_2		(1 << 0)
#define     EC_FLASH_CLK_VSB_DIV_4		(2 << 0)
#define     EC_FLASH_CLK_VSB_DIV_16		(3 << 0)
#define   EC_FLASH_FETCH_MODE_VSB_MASK		(3 << 2)
#define     EC_FLASH_FETCH_MODE_VSB_READ	(0 << 2)
#define     EC_FLASH_FETCH_MODE_VSB_FAST_READ	(1 << 2)
#define     EC_FLASH_FETCH_MODE_VSB_DUAL_OUT	(2 << 2)
#define     EC_FLASH_FETCH_MODE_VSB_DUAL_IN_OUT	(3 << 2)
#define   EC_MCU_SPEED_MCLK_VSB_MASK		(3 << 4)
#define     EC_MCU_SPEED_MCLK_VSB_1MHZ		(0 << 4)
#define     EC_MCU_SPEED_MCLK_VSB_4MHZ		(1 << 4)
#define     EC_MCU_SPEED_MCLK_VSB_24MHZ		(2 << 4)
#define     EC_MCU_SPEED_MCLK_VSB_48MHZ		(3 << 4)
#define   EC_MCU_CACHE_VSB_EN			(1 << 6)

#define EC_MCU_SPEED_CTL_VCC_REG		EC_PAGE_REG(6, 0x42)
#define   EC_MCU_SPEED_MCLK_VCC_MASK		0x0f
#define     EC_MCU_SPEED_MCLK_VCC_10KHZ		(0 << 0)
#define     EC_MCU_SPEED_MCLK_VCC_160KHZ	(1 << 0)
#define     EC_MCU_SPEED_MCLK_VCC_1MHZ		(2 << 0)
#define     EC_MCU_SPEED_MCLK_VCC_4MHZ		(3 << 0)
#define     EC_MCU_SPEED_MCLK_VCC_8MHZ		(4 << 0)
#define     EC_MCU_SPEED_MCLK_VCC_16MHZ		(5 << 0)
#define     EC_MCU_SPEED_MCLK_VCC_24MHZ		(6 << 0)
#define     EC_MCU_SPEED_MCLK_VCC_48MHZ		(7 << 0)
#define   EC_MCU_CACHE_VCC_EN			(1 << 6)
#define   EC_MCU_SPEED_VCC_LOAD_CFG		(1 << 7)

#define EC_FLASH_SPEED_CTL_VCC_REG		EC_PAGE_REG(6, 0x43)
#define   EC_FLASH_CLK_VCC_DIV_MASK		(3 << 2)
#define     EC_FLASH_CLK_VCC_DIV_1		(0 << 2)
#define     EC_FLASH_CLK_VCC_DIV_2		(1 << 2)
#define     EC_FLASH_CLK_VCC_DIV_4		(2 << 2)
#define     EC_FLASH_CLK_VCC_DIV_16		(3 << 2)
#define   EC_FLASH_FETCH_MODE_VCC_MASK		(3 << 4)
#define     EC_FLASH_FETCH_MODE_VCC_READ	(0 << 4)
#define     EC_FLASH_FETCH_MODE_VCC_FAST_READ	(1 << 4)
#define     EC_FLASH_FETCH_MODE_VCC_DUAL_OUT	(2 << 4)
#define     EC_FLASH_FETCH_MODE_VCC_DUAL_IN_OUT	(3 << 4)
#define   EC_FLASH_SPEED_VCC_LOAD_CFG		(1 << 7)

#define EC_FLASH_WREN_CMD_REG			EC_PAGE_REG(6, 0x47)

#define EC_FLASH_WRITE_LOCK_REG			EC_PAGE_REG(6, 0x4f)
#define   EC_FLASH_WRITE_LOCK			(1 << 0)
#define   EC_FLASH_WRITE_LOCK_EN		(1 << 7)

/* EC Page 8 - NCT6687D specific, not present in NCT6686D */

#define FAN9_FAN10_MANUAL_EN_REG		EC_PAGE_REG(8, 0x0f)

#define FAN_STEP_UP_TIME_REG(fan)		EC_PAGE_REG(8, 0x1a + ((fan) - 1))
#define FAN_STEP_DOWN_TIME_REG(fan)		EC_PAGE_REG(8, 0x24 + ((fan) - 1))

/* These are also included in macros in page 1, 10 and 12 */
#define FAN9_FUNCTION_CTL_REG			EC_PAGE_REG(8, 0xe0)
#define FAN10_FUNCTION_CTL_REG			EC_PAGE_REG(8, 0xe1)
#define FAN9_MANUAL_OUTPUT_VAL_REG		EC_PAGE_REG(8, 0xe8)
#define FAN10_MANUAL_OUTPUT_VAL_REG		EC_PAGE_REG(8, 0xe9)
#define FAN9_DUTY_REG				EC_PAGE_REG(8, 0xf0)
#define FAN10_DUTY_REG				EC_PAGE_REG(8, 0xf1)
#define	FAN9OUT_CFG_REG				EC_PAGE_REG(8, 0xf4)
#define	FAN10OUT_CFG_REG			EC_PAGE_REG(8, 0xf5)

/* EC Page 9 - Fan Algorithm Function Control */

#define FAN_ALG_FUNCTRL_REG(fan)		EC_PAGE_REG(9, 0x00 + ((fan) - 1))
#define   FAN_ALG_FUNCTRL_DTS_UB_EN		(1 << 2)
#define   FAN_ALG_FUNCTRL_DTS2_EN		(1 << 3)
#define   FAN_ALG_FUNCTRL_DTS1_EN		(1 << 4)
#define   FAN_ALG_FUNCTRL_TEMP_ERR		(1 << 5)
#define   FAN_ALG_FUNCTRL_CRIT_TEMP_EN		(1 << 6)
#define   FAN_ALG_FUNCTRL_ALG_EN		(1 << 7)

/* Main temperature zone decision */
#define FAN_MTZ_DCS_DATA_REG(x, fan)		EC_PAGE_REG(9, 0x10 + (x) + 4*((fan) - 1))
/* Ambient temperature zone decision */
#define FAN_ATZ_DCS_DATA_REG(x, fan)		EC_PAGE_REG(9, 0x50 + (x) 4*((fan) - 1))

#define FAN_INITIAL_VALUE_REG(fan)		EC_PAGE_REG(9, 0x90 + ((fan) - 1))
#define FAN_CRIT_TEMP_CFG_REG(fan)		EC_PAGE_REG(9, 0xA0 + ((fan) - 1))
#define FAN_CRIT_TEMP_TOLERANCE_REG(fan)	EC_PAGE_REG(9, 0xB0 + ((fan) - 1))
#define FAN_TEMP_ERR_DUTY_VAL_REG(fan)		EC_PAGE_REG(9, 0xC0 + ((fan) - 1))

#define FAN_MODE_SEL_REG(fan)			EC_PAGE_REG(9, 0xD0 + ((fan) - 1))
#define   FAN_MODE_SEL_MASK			0xf
#define     FAN_MODE_MODE_MANUAL		(0 << 0)
#define     FAN_MODE_THERMAL_CRUISE		(1 << 0)
#define     FAN_MODE_SPEED_CRUISE		(2 << 0)
#define     FAN_MODE_SMART_FAN_IV		(3 << 0)
#define     FAN_MODE_PID_CONTROL		(4 << 0)

/* EC Page 10 - Fan Function Control & Weight Matrix */

#define FAN_MANUAL_EN_REG(fan) \
	(((fan) > 8)	? EC_PAGE_REG(8, 0x0f) \
			: EC_PAGE_REG(10, 0x00))
#define   FAN_MANUAL_EN(fan)			(1 << (((fan) - 1) % 8))

#define FAN_CFG_CTRL_REG			EC_PAGE_REG(10, 0x01)
#define   FAN_CFG_DONE				(1 << 6)
#define   FAN_CFG_REQUEST			(1 << 7)

#define FAN_FUNCTION_CTRL(fan) \
	(((fan) > 8)	? EC_PAGE_REG(8, 0xe0 + ((fan) - 9)) \
			: EC_PAGE_REG(10, 0x08 + ((fan) - 1)))
#define   FAN_FUN_MINDUTY_EN			(1 << 0)
#define   FAN_FUN_MANUAL_OFFSET_EN		(1 << 1)
#define   FAN_FUN_STARTUP_EN			(1 << 2)
#define   FAN_FUN_AMBIENT_FLOOR_EN		(1 << 3)
#define   FAN_FUN_MARKUP_TRACK_EN		(1 << 4)
#define   FAN_FUN_FAST_TRACK_EN			(1 << 5)
#define   FAN_FUN_SMART_TRACK_EN		(1 << 6)
#define   FAN_FUN_UNIT_PWM_DUTY			(0 << 7)
#define   FAN_FUN_UNIT_RPM			(1 << 7)

/* The unit for below Ambient Floor register is 100 RPM */
#define FAN_AMB_FLOOR_RPM_TO_REG(rpm)		((rpm) ? (rpm) / 100 : 1)
#define FAN_AMB_FLOOR_MIN_OUT_START_REG(fan)	EC_PAGE_REG(10, 0x10 + ((fan) - 1))
#define FAN_AMB_FLOOR_MIN_OUT_END_REG(fan)	EC_PAGE_REG(10, 0x18 + ((fan) - 1))
#define FAN_AMB_FLOOR_MAX_OUT_REG(fan)		EC_PAGE_REG(10, 0x20 + ((fan) - 1))

#define FAN_MANUAL_VALUE_REG(fan) \
	(((fan) > 8)	? EC_PAGE_REG(8, 0xe8 + ((fan) - 9)) \
			: EC_PAGE_REG(10, 0x28 + ((fan) - 1)))

#define FAN_MANUAL_OFFSET_REG(fan)		EC_PAGE_REG(10, 0x30 + ((fan) - 1))
#define FAN_MIN_DUTY_REG(fan)			EC_PAGE_REG(10, 0x38 + ((fan) - 1))
#define FAN_STARTUP_DUTY_REG(fan)		EC_PAGE_REG(10, 0x40 + ((fan) - 1))

/* Smart Tracking registers */

#define FAN_LOW_RPM_SPEED_BOUNDARY_HI_REG	EC_PAGE_REG(10, 0x48)
#define FAN_LOW_RPM_SPEED_BOUNDARY_LO_REG	EC_PAGE_REG(10, 0x49)
#define FAN_HIGH_RPM_SPEED_BOUNDARY_HI_REG	EC_PAGE_REG(10, 0x4a)
#define FAN_HIGH_RPM_SPEED_BOUNDARY_LO_REG	EC_PAGE_REG(10, 0x4b)

#define FAN_LOW_RPM_TOLERANCE_REG		EC_PAGE_REG(10, 0x4c)
#define FAN_MID_RPM_TOLERANCE_REG		EC_PAGE_REG(10, 0x4d)
#define FAN_HIGH_RPM_TOLERANCE_REG		EC_PAGE_REG(10, 0x4e)

#define FAN_TRACKING_STEP_REG			EC_PAGE_REG(10, 0x4f)
#define   FAN_TRACK_STEP_DOWN_MASK		0x0f
#define   FAN_TRACK_STEP_DOWN_SHIFT		0
#define   FAN_TRACK_STEP_UP_MASK		0x0f
#define   FAN_TRACK_STEP_UP_SHIFT		4

#define FAN_FAST_TRACK_TEMP_BOUNDARY_REG	EC_PAGE_REG(10, 0x50)

/* Below definitions are dividers for fast tracking weight registers */
#define FAN_FAST_TRACK_WEIGHT_DIV_1		1
#define FAN_FAST_TRACK_WEIGHT_DIV_2		2
#define FAN_FAST_TRACK_WEIGHT_DIV_4		3
#define FAN_FAST_TRACK_WEIGHT_DIV_8		4
#define FAN_FAST_TRACK_WEIGHT_DIV_16		5
#define FAN_FAST_TRACK_WEIGHT_DIV_32		6
#define FAN_FAST_TRACK_WEIGHT_DIV_64		7

/* Below definitions are shifts and masks for fast tracking weight registers */
#define FAN_FAST_TRACK_WEIGHT_DOWN_MASK		0x07
#define FAN_FAST_TRACK_WEIGHT_DOWN_SHIFT	0
#define FAN_FAST_TRACK_WEIGHT_UP_MASK		0x07
#define FAN_FAST_TRACK_WEIGHT_UP_SHIFT		4

#define FAN_LOW_RPM_FAST_TRACK_WEIGHT_REG	EC_PAGE_REG(10, 0x51)
#define FAN_MID_RPM_FAST_TRACK_WEIGHT_REG	EC_PAGE_REG(10, 0x52)
#define FAN_HIGH_RPM_FAST_TRACK_WEIGHT_REG	EC_PAGE_REG(10, 0x53)

/* Below definitions are shifts and masks for fast tracking duty step registers */
#define   FAN_FAST_TRACK_DUTY_STEP_MASK		0x0f
#define   FAN_FAST_TRACK_DUTY_STEP_SHIFT	0

#define FAN_LOW_RPM_FAST_TRACK_DUTY_STEP_REG	EC_PAGE_REG(10, 0x54)
#define FAN_MID_RPM_FAST_TRACK_DUTY_STEP_REG	EC_PAGE_REG(10, 0x55)
#define FAN_HIGH_RPM_FAST_TRACK_DUTY_STEP_REG	EC_PAGE_REG(10, 0x56)

#define FAN_MARKUP_TRACK_AMB_TEMP_BOUNDARY_REG	EC_PAGE_REG(10, 0x57)
#define FAN_MARKUP_TRACK_WEIGHT_REG		EC_PAGE_REG(10, 0x58)
#define   FAN_MARKUP_TRACK_WEIGHT_MASK		0x07
#define   FAN_MARKUP_TRACK_WEIGHT_SHIFT		0

/* Intel Sensor Based (DTS) Fan Control registers */

#define ITL_TEMP_START_POINT_REG		EC_PAGE_REG(10, 0x59)
#define ITL_TEMP_END_POINT_REG			EC_PAGE_REG(10, 0x5a)
#define ITL_AMB_TEMP_START_POINT_REG(x)		EC_PAGE_REG(10, 0x5b + (x))

#define ITL_RPM_START_POINT_HI_REG(x)		EC_PAGE_REG(10, 0x60 + 2*(x))
#define ITL_RPM_START_POINT_LO_REG(x)		EC_PAGE_REG(10, 0x61 + 2*(x))
#define ITL_RPM_END_POINT_HI_REG(x)		EC_PAGE_REG(10, 0x6a + 2*(x))
#define ITL_RPM_END_POINT_LO_REG(x)		EC_PAGE_REG(10, 0x6a + 2*(x))
#define ITL_RPM_MAX_SPEED_HI_REG		EC_PAGE_REG(10, 0x74)
#define ITL_RPM_MAX_SPEED_LO_REG		EC_PAGE_REG(10, 0x75)

#define ITL_DTS_CFG_REG				EC_PAGE_REG(10, 0x76)
#define   ITL_DTS_PECI_AGENT_IDX_SEL_MASK	0x0f
#define   ITL_DTS_PECI_AGENT_IDX_SEL_SHIFT	0
#define   ITL_DTS_PECI_CFG_ADJUSTMENT		(1 << 7)

#define AMBIENT_FLOOR_TEMP_START_POINT_REG	EC_PAGE_REG(10, 0x77)
#define AMBIENT_FLOOR_TEMP_END_POINT_REG	EC_PAGE_REG(10, 0x78)

/* DTS 2.0 sensor registers */

#define DTS2_TARGET_MARGIN_REG			EC_PAGE_REG(10, 0x59)
#define DTS2_TCONTROL_REG			EC_PAGE_REG(10, 0x5a)
#define DTS2_TCONTROL_OFFSET_REG		EC_PAGE_REG(10, 0x5b)
#define DTS2_TARGET_TOLERANCE_REG		EC_PAGE_REG(10, 0x5c)
#define DTS2_DIVISOR_REG			EC_PAGE_REG(10, 0x5d)
#define DTS2_STEP_SPEED_HI_REG			EC_PAGE_REG(10, 0x5e)
#define DTS2_STEP_SPEED_LO_REG			EC_PAGE_REG(10, 0x5f)
#define DTS2_MIN_SPEED_HI_REG			EC_PAGE_REG(10, 0x60)
#define DTS2_MIN_SPEED_LO_REG			EC_PAGE_REG(10, 0x61)
#define DTS2_DELAY_TIME_COUNTER_REG		EC_PAGE_REG(10, 0x62)

/* Weight Matrix Configuration */

#define FAN_ALG_ENGINE_WEIGHT_EN_REG(fan) \
	(((fan) > 8)	? EC_PAGE_REG(10, 0xC1 + 8*((fan) - 9)) \
			: EC_PAGE_REG(10, 0x80 + 8*((fan) - 1)))
#define   FAN_ALG_ENGINE_WEIGHT_CHANNEL_EN(fan)	(1 << (((fan) - 1) % 8))

#define FAN_ALG_ENGINE_WEIGHT_VAL_REG(fan) \
	(((fan) > 8)	? EC_PAGE_REG(10, 0xC3 + ((fan) - 9)/2 + 8*((fan) - 9)) \
			: EC_PAGE_REG(10, 0x84 + ((fan) - 1)/2 + 8*((fan) - 1)))
#define   FAN_ALG_ENGINE_WEIGHT_MASK(fan)	(0xf << (4 * (((fan) - 1) % 2)))
#define   FAN_ALG_ENGINE_WEIGHT_SHIFT(fan)	(4 * (((fan) - 1) % 2))

/* EC Page 11 - Thermal Cruise, Speed Cruise, Smart Fan IV */

#define FAN_PWM_PERCENT_TO_HEX(x)		((x) * 255 / 100)


/* For x larger than 10, the registers are placed in page 12 */
/* Thermal Cruise (TC) Registers */

#define FAN_TC_TARGET_TEMP_TOLERANCE_REG(fan)	EC_PAGE_REG(11, 0x00 + 24*((fan) - 1))
#define   FAN_TC_TEMP_TOLERANCE_MASK		0xf
#define   FAN_TC_TEMP_TOLERANCE_SHIFT		0

#define FAN_TC_TARGET_TEMP_REG(fan)		EC_PAGE_REG(11, 0x01 + 24*((fan) - 1))
#define   FAN_TC_TARGET_TEMP_MASK		0x7f
#define   FAN_TC_TARGET_TEMP_SHIFT		0
#define   FAN_TC_KEEP_MIN_OUTPUT		(1 << 7)

#define FAN_TC_STOP_VALUE_REG(fan)		EC_PAGE_REG(11, 0x02 + 24*((fan) - 1))

#define FAN_TC_TIME_TO_SECONDS(x)		((x)*10)
#define FAN_TC_STOP_TIME_REG(fan)		EC_PAGE_REG(11, 0x03 + 24*((fan) - 1))
#define FAN_TC_STEP_DOWN_TIME_REG(fan)		EC_PAGE_REG(11, 0x04 + 24*((fan) - 1))
#define FAN_TC_STEP_UP_TIME_REG(fan)		EC_PAGE_REG(11, 0x05 + 24*((fan) - 1))

/* Speed Cruise (SC) Registers */

#define FAN_SC_TARGET_RPM_HI_REG(fan)		EC_PAGE_REG(11, 0x00 + 24*((fan) - 1))
#define FAN_SC_TARGET_RPM_LO_REG(fan)		EC_PAGE_REG(11, 0x01 + 24*((fan) - 1))
#define FAN_SC_TARGET_RPM_TOLERANCE_HI_REG(fan)	EC_PAGE_REG(11, 0x02 + 24*((fan) - 1))
#define FAN_SC_TARGET_RPM_TOLERANCE_LO_REG(fan)	EC_PAGE_REG(11, 0x03 + 24*((fan) - 1))

#define FAN_SC_TIME_TO_SECONDS(x)		((x)*10)
#define FAN_SC_STEP_DOWN_TIME_REG(fan)		EC_PAGE_REG(11, 0x04 + 24*((fan) - 1))
#define FAN_SC_STEP_UP_TIME_REG(fan)		EC_PAGE_REG(11, 0x05 + 24*((fan) - 1))

/* Smart Fan IV (SF4) Registers */

#define FAN_SF4_TEMP_LVL_REG(fan, level)	EC_PAGE_REG(11, 0x00 + (level) + 24*((fan) - 1))
#define FAN_SF4_PWM_RPM_LVL_HI_REG(fan, level)	EC_PAGE_REG(11, 0x07 + (level)*2 + 24*((fan) - 1))
#define FAN_SF4_PWM_RPM_LVL_LO_REG(fan, level)	EC_PAGE_REG(11, 0x08 + (level)*2 + 24*((fan) - 1))
#define FAN_SF4_TEMP_OFF_HYSTHERESIS_REG(fan)	EC_PAGE_REG(11, 0x15 + 24*((fan) - 1))
#define FAN_SF4_TEMP_CUT_OFF_REG(fan)		EC_PAGE_REG(11, 0x16 + 24*((fan) - 1))
#define FAN_SF4_TEMP_OFF_DELAY_REG(fan)		EC_PAGE_REG(11, 0x17 + 24*((fan) - 1))

/* EC Page 12 - Fan Function Control */

#define FAN_ENGINE_STS_REG			EC_PAGE_REG(12, 0xf8)
#define   FAN_PECI_CFG_ADJUSTED			(1 << 1)
#define   FAN_UNFINISHED_FLAG			(1 << 2)
#define   FAN_CFG_PHASE				(1 << 3)
#define   FAN_CFG_INVALID			(1 << 4)
#define   FAN_CFG_CHECK_DONE			(1 << 5)
#define   FAN_CFG_LOCK				(1 << 6)
#define   FAN_DRIVE_BY_MOD_SEL			(0 << 7)
#define   FAN_DRIVE_BY_DEFAULT_VAL		(1 << 7)

#define FAN_LAST_ERROR_CODE_REG			EC_PAGE_REG(12, 0xf9)
#define   FAN_NO_ERROR				0x00
#define   FAN_ERR_MODE_SELECT			0x01
#define   FAN_ERR_CRIT_TEMP_PROTECT		0x02
#define   FAN_ERR_ITL_FAN_CONTROL		0x04
#define   FAN_ERR_SMART_TRACKING		0x05
#define   FAN_ERR_THERMAL_CRUISE		0x10
#define   FAN_ERR_SPEED_CRUISE			0x20
#define   FAN_ERR_SMART_FAN_IV			0x40
#define   FAN_ERR_PID_CONTROL			0x50

#define FAN_CHANNEL_EN_REG(fan) \
	(((fan) > 8)	? EC_PAGE_REG(12, 0xfb) \
			: EC_PAGE_REG(12, 0xfc))
#define   FAN_CHANNEL_EN(fan)			(1 << (((fan) - 1) % 8))

#define FAN_DEFAULT_VAL_REG			EC_PAGE_REG(12, 0xfd)

extern uint16_t nct6687d_hwm_base;

static __always_inline void hwm_reg_and_or(uint16_t page_reg, uint8_t and_mask, uint8_t or_mask)
{
	assert(nct6687d_hwm_base != 0);
	nct6687d_ec_and_or_page(nct6687d_hwm_base, page_reg >> 8, page_reg & 0xff,
				and_mask, or_mask);
}

static __always_inline void hwm_reg_set_bits(uint16_t page_reg, uint8_t bits)
{
	assert(nct6687d_hwm_base != 0);
	nct6687d_ec_and_or_page(nct6687d_hwm_base, page_reg >> 8, page_reg & 0xff,
				~bits, bits);
}

static __always_inline void hwm_reg_write(uint16_t page_reg, uint8_t value)
{
	assert(nct6687d_hwm_base != 0);
	nct6687d_ec_write_page(nct6687d_hwm_base, page_reg >> 8, page_reg & 0xff, value);
}

static __always_inline uint8_t hwm_reg_read(uint16_t page_reg)
{
	assert(nct6687d_hwm_base != 0);
	return nct6687d_ec_read_page(nct6687d_hwm_base, page_reg >> 8, page_reg & 0xff);
}

#endif /* SUPERIO_NUVOTON_NCT6687D_HWM_H */
