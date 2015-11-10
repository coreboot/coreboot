/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Marvell Inc.
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

#include <arch/io.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c.h>
#include <soc/common.h>
#include <soc/i2c.h>
#include <soc/clock.h>
#include <helpers.h>

#undef MV_DEBUG
//#define MV_DEBUG
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif
#define mv_os_printf(args...) printk(BIOS_INFO, args)

/* The TWSI interface supports both 7-bit and 10-bit addressing.            */
/* This enumerator describes addressing type.                               */
typedef enum _mv_twsi_addr_type {
	ADDR7_BIT, /* 7 bit address    */
	ADDR10_BIT /* 10 bit address   */
} MV_TWSI_ADDR_TYPE;

/* This structure describes TWSI address.                                   */
typedef struct _mv_twsi_addr {
	uint32_t address;       /* address          */
	MV_TWSI_ADDR_TYPE type; /* Address type     */
} MV_TWSI_ADDR;

/* This structure describes a TWSI slave.                                   */
typedef struct _mv_twsi_slave {
	MV_TWSI_ADDR slave_addr;
	int valid_offset; /* whether the slave has offset (i.e. Eeprom  etc.) */
	uint32_t offset;  /* offset in the slave. */
	int more_than256; /* whether the ofset is bigger then 256 */
} MV_TWSI_SLAVE;

/* This enumerator describes TWSI protocol commands.                        */
typedef enum _mv_twsi_cmd {
	MV_TWSI_WRITE, /* TWSI write command - 0 according to spec   */
	MV_TWSI_READ   /* TWSI read command  - 1 according to spec */
} MV_TWSI_CMD;

static void twsi_int_flg_clr(uint8_t chan_num);
static uint8_t twsi_main_int_get(uint8_t chan_num);
static void twsi_ack_bit_set(uint8_t chan_num);
static uint32_t twsi_sts_get(uint8_t chan_num);
static void twsi_reset(uint8_t chan_num);
static int twsi_addr7_bit_set(uint8_t chan_num,
			      uint32_t device_address,
			      MV_TWSI_CMD command);
static int twsi_addr10_bit_set(uint8_t chan_num,
			       uint32_t device_address,
			       MV_TWSI_CMD command);
static int twsi_data_transmit(uint8_t chan_num,
			      uint8_t *p_block,
			      uint32_t block_size);
static int twsi_data_receive(uint8_t chan_num,
			     uint8_t *p_block,
			     uint32_t block_size);
static int twsi_target_offs_set(uint8_t chan_num,
				uint32_t offset,
				uint8_t more_than256);
static int mv_twsi_start_bit_set(uint8_t chan_num);
static int mv_twsi_stop_bit_set(uint8_t chan_num);
static int mv_twsi_addr_set(uint8_t chan_num,
			    MV_TWSI_ADDR *twsi_addr,
			    MV_TWSI_CMD command);
static uint32_t mv_twsi_init(uint8_t chan_num,
			     uint32_t frequency,
			     uint32_t Tclk,
			     MV_TWSI_ADDR *twsi_addr,
			     uint8_t general_call_enable);
static int mv_twsi_read(uint8_t chan_num,
			MV_TWSI_SLAVE *twsi_slave,
			uint8_t *p_block,
			uint32_t block_size);
static int mv_twsi_write(uint8_t chan_num,
			 MV_TWSI_SLAVE *twsi_slave,
			 uint8_t *p_block,
			 uint32_t block_size);
static uint32_t who_am_i(void);
static int i2c_init(unsigned bus);
static void i2c_reset(unsigned bus);

static int m_initialized[MAX_I2C_NUM] = {0, 0};

static uint8_t twsi_timeout_chk(uint32_t timeout, const char *p_string)
{
	if (timeout >= TWSI_TIMEOUT_VALUE) {
		DB(mv_os_printf("%s", p_string));
		return MV_TRUE;
	}
	return MV_FALSE;
}

/*******************************************************************************
* mv_twsi_start_bit_set - Set start bit on the bus
*
* DESCRIPTION:
*       This routine sets the start bit on the TWSI bus.
*       The routine first checks for interrupt flag condition, then it sets
*       the start bit  in the TWSI Control register.
*       If the interrupt flag condition check previously was set, the function
*       will clear it.
*       The function then wait for the start bit to be cleared by the HW.
*       Then it waits for the interrupt flag to be set and eventually, the
*       TWSI status is checked to be 0x8 or 0x10(repeated start bit).
*
* INPUT:
*       chan_num - TWSI channel.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK if start bit was set successfuly on the bus.
*       MV_FAIL if start_bit not set or status does not indicate start
*       condition trasmitted.
*
*******************************************************************************/
static int mv_twsi_start_bit_set(uint8_t chan_num)
{
	uint8_t is_int_flag = MV_FALSE;
	uint32_t timeout, temp;

	DB(mv_os_printf("TWSI: mv_twsi_start_bit_set\n"));
	/* check Int flag */
	if (twsi_main_int_get(chan_num))
		is_int_flag = MV_TRUE;
	/* set start Bit */
	mrvl_reg_bit_set(TWSI_CONTROL_REG(chan_num), TWSI_CONTROL_START_BIT);

	/* in case that the int flag was set before i.e. repeated start bit */
	if (is_int_flag) {
		DB(mv_os_printf(
		    "TWSI: mv_twsi_start_bit_set repeated start Bit\n"));
		twsi_int_flg_clr(chan_num);
	}

	/* wait for interrupt */
	timeout = 0;
	while (!twsi_main_int_get(chan_num) && (timeout++ < TWSI_TIMEOUT_VALUE))
		;

	/* check for timeout */
	if (MV_TRUE ==
	    twsi_timeout_chk(timeout,
			     (const char *)"TWSI: Start Clear bit time_out.\n"))
		return MV_TIMEOUT;

	/* check that start bit went down */
	if ((mrvl_reg_read(TWSI_CONTROL_REG(chan_num)) &
	     TWSI_CONTROL_START_BIT) != 0) {
		mv_os_printf("TWSI: start bit didn't go down\n");
		return MV_FAIL;
	}

	/* check the status */
	temp = twsi_sts_get(chan_num);
	if ((TWSI_M_LOST_ARB_DUR_AD_OR_DATA_TRA == temp) ||
	    (TWSI_M_LOST_ARB_DUR_AD_TRA_GNL_CALL_AD_REC_ACK_TRA == temp)) {
		DB(mv_os_printf("TWSI: Lost Arb, status %x\n", temp));
		return MV_RETRY;
	} else if ((temp != TWSI_START_CON_TRA) &&
		   (temp != TWSI_REPEATED_START_CON_TRA)) {
		mv_os_printf("TWSI: status %x after Set Start Bit.\n", temp);
		return MV_FAIL;
	}

	return MV_OK;
}

/*******************************************************************************
* mv_twsi_stop_bit_set - Set stop bit on the bus
*
* DESCRIPTION:
*       This routine set the stop bit on the TWSI bus.
*       The function then wait for the stop bit to be cleared by the HW.
*       Finally the function checks for status of 0xF8.
*
* INPUT:
*	chan_num - TWSI channel
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE is stop bit was set successfuly on the bus.
*
*******************************************************************************/
static int mv_twsi_stop_bit_set(uint8_t chan_num)
{
	uint32_t timeout, temp;

	/* Generate stop bit */
	mrvl_reg_bit_set(TWSI_CONTROL_REG(chan_num), TWSI_CONTROL_STOP_BIT);

	twsi_int_flg_clr(chan_num);

	/* wait for stop bit to come down */
	timeout = 0;
	while (((mrvl_reg_read(TWSI_CONTROL_REG(chan_num)) &
		 TWSI_CONTROL_STOP_BIT) != 0) &&
	       (timeout++ < TWSI_TIMEOUT_VALUE))
		;

	/* check for timeout */
	if (MV_TRUE ==
	    twsi_timeout_chk(timeout,
			     (const char *)"TWSI: ERROR - Stop bit timeout\n"))
		return MV_TIMEOUT;

	/* check that the stop bit went down */
	if ((mrvl_reg_read(TWSI_CONTROL_REG(chan_num)) &
	     TWSI_CONTROL_STOP_BIT) != 0) {
		mv_os_printf(
		    "TWSI: ERROR - stop bit not went down\n");
		return MV_FAIL;
	}

	/* check the status */
	temp = twsi_sts_get(chan_num);
	if ((TWSI_M_LOST_ARB_DUR_AD_OR_DATA_TRA == temp) ||
	    (TWSI_M_LOST_ARB_DUR_AD_TRA_GNL_CALL_AD_REC_ACK_TRA == temp)) {
		DB(mv_os_printf("TWSI: Lost Arb, status %x\n", temp));
		return MV_RETRY;
	} else if (temp != TWSI_NO_REL_STS_INT_FLAG_IS_KEPT_0) {
		mv_os_printf(
		    "TWSI: ERROR - status %x after Stop Bit\n",
		    temp);
		return MV_FAIL;
	}

	return MV_OK;
}

/*******************************************************************************
* twsi_main_int_get - Get twsi bit from main Interrupt cause.
*
* DESCRIPTION:
*       This routine returns the twsi interrupt flag value.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE is interrupt flag is set, MV_FALSE otherwise.
*
*******************************************************************************/
static uint32_t who_am_i(void)
{
	return (read_mpidr() & 0x1);
}

static uint8_t twsi_main_int_get(uint8_t chan_num)
{
	uint32_t temp;

	/* get the int flag bit */
	temp = mrvl_reg_read(MV_TWSI_CPU_MAIN_INT_CAUSE(chan_num, who_am_i()));
	if (temp & (1 << CPU_MAIN_INT_TWSI_OFFS(chan_num)))
		return MV_TRUE;

	return MV_FALSE;
}

/*******************************************************************************
* twsi_int_flg_clr - Clear Interrupt flag.
*
* DESCRIPTION:
*       This routine clears the interrupt flag. It does NOT poll the interrupt
*       to make sure the clear. After clearing the interrupt, it waits for at
*       least 1 miliseconds.
*
* INPUT:
*	chan_num - TWSI channel
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
static void twsi_int_flg_clr(uint8_t chan_num)
{
	/* wait for 1ms to prevent TWSI register write after write problems */
	mdelay(1);
	/* clear the int flag bit */
	mrvl_reg_bit_reset(
		TWSI_CONTROL_REG(chan_num), TWSI_CONTROL_INT_FLAG_SET);
	/* wait for 1 mili sec for the clear to take effect */
	mdelay(1);
}

/*******************************************************************************
* twsi_ack_bit_set - Set acknowledge bit on the bus
*
* DESCRIPTION:
*       This routine set the acknowledge bit on the TWSI bus.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
static void twsi_ack_bit_set(uint8_t chan_num)
{
	/*Set the Ack bit */
	mrvl_reg_bit_set(TWSI_CONTROL_REG(chan_num), TWSI_CONTROL_ACK);
	/* Add delay of 1ms */
	mdelay(1);
}

/*******************************************************************************
* twsi_init - Initialize TWSI interface
*
* DESCRIPTION:
*       This routine:
*	-Reset the TWSI.
*	-Initialize the TWSI clock baud rate according to given frequency
*	 parameter based on Tclk frequency and enables TWSI slave.
*       -Set the ack bit.
*	-Assign the TWSI slave address according to the TWSI address Type.
*
* INPUT:
*	chan_num - TWSI channel
*       frequency - TWSI frequency in KHz. (up to 100_kHZ)
*
* OUTPUT:
*       None.
*
* RETURN:
*       Actual frequency.
*
*******************************************************************************/
static uint32_t mv_twsi_init(uint8_t chan_num,
			     uint32_t frequency,
			     uint32_t Tclk,
			     MV_TWSI_ADDR *p_twsi_addr,
			     uint8_t general_call_enable)
{
	uint32_t n, m, freq, margin, min_margin = 0xffffffff;
	uint32_t power;
	uint32_t actual_freq = 0, actual_n = 0, actual_m = 0, val;

	if (frequency > 100000)
		die("TWSI frequency is too high!");

	DB(mv_os_printf("TWSI: mv_twsi_init - Tclk = %d freq = %d\n", Tclk,
			frequency));
	/* Calucalte N and M for the TWSI clock baud rate */
	for (n = 0; n < 8; n++) {
		for (m = 0; m < 16; m++) {
			power = 2 << n; /* power = 2^(n+1) */
			freq = Tclk / (10 * (m + 1) * power);
			margin = ABS(frequency - freq);

			if ((freq <= frequency) && (margin < min_margin)) {
				min_margin = margin;
				actual_freq = freq;
				actual_n = n;
				actual_m = m;
			}
		}
	}
	DB(mv_os_printf("TWSI: mv_twsi_init - act_n %u act_m %u act_freq %u\n",
			actual_n, actual_m, actual_freq));
	/* Reset the TWSI logic */
	twsi_reset(chan_num);

	/* Set the baud rate */
	val = ((actual_m << TWSI_BAUD_RATE_M_OFFS) |
	       actual_n << TWSI_BAUD_RATE_N_OFFS);
	mrvl_reg_write(TWSI_STATUS_BAUDE_RATE_REG(chan_num), val);

	/* Enable the TWSI and slave */
	mrvl_reg_write(TWSI_CONTROL_REG(chan_num),
		       TWSI_CONTROL_ENA | TWSI_CONTROL_ACK);

	/* set the TWSI slave address */
	if (p_twsi_addr->type == ADDR10_BIT) {
		/* writing the 2 most significant bits of the 10 bit address */
		val = ((p_twsi_addr->address & TWSI_SLAVE_ADDR_10_BIT_MASK) >>
		       TWSI_SLAVE_ADDR_10_BIT_OFFS);
		/* bits 7:3 must be 0x11110 */
		val |= TWSI_SLAVE_ADDR_10_BIT_CONST;
		/* set GCE bit */
		if (general_call_enable)
			val |= TWSI_SLAVE_ADDR_GCE_ENA;
		/* write slave address */
		mrvl_reg_write(TWSI_SLAVE_ADDR_REG(chan_num), val);

		/* writing the 8 least significant bits of the 10 bit address */
		val = (p_twsi_addr->address << TWSI_EXTENDED_SLAVE_OFFS) &
		      TWSI_EXTENDED_SLAVE_MASK;
		mrvl_reg_write(TWSI_EXTENDED_SLAVE_ADDR_REG(chan_num), val);
	} else {
		/* set the 7 Bits address */
		mrvl_reg_write(TWSI_EXTENDED_SLAVE_ADDR_REG(chan_num), 0x0);
		val = (p_twsi_addr->address << TWSI_SLAVE_ADDR_7_BIT_OFFS) &
		      TWSI_SLAVE_ADDR_7_BIT_MASK;
		mrvl_reg_write(TWSI_SLAVE_ADDR_REG(chan_num), val);
	}

	/* unmask twsi int */
	mrvl_reg_bit_set(TWSI_CONTROL_REG(chan_num), TWSI_CONTROL_INT_ENA);

	/* unmask twsi int in Interrupt source control register */
	mrvl_reg_bit_set(CPU_INT_SOURCE_CONTROL_REG(
			CPU_MAIN_INT_CAUSE_TWSI(chan_num)), (
				1 << CPU_INT_SOURCE_CONTROL_IRQ_OFFS));

	/* Add delay of 1ms */
	mdelay(1);

	return actual_freq;
}

/*******************************************************************************
* twsi_sts_get - Get the TWSI status value.
*
* DESCRIPTION:
*       This routine returns the TWSI status value.
*
* INPUT:
*	chan_num - TWSI channel
*
* OUTPUT:
*       None.
*
* RETURN:
*       uint32_t - the TWSI status.
*
*******************************************************************************/
static uint32_t twsi_sts_get(uint8_t chan_num)
{
	return mrvl_reg_read(TWSI_STATUS_BAUDE_RATE_REG(chan_num));
}

/*******************************************************************************
* twsi_reset - Reset the TWSI.
*
* DESCRIPTION:
*       Resets the TWSI logic and sets all TWSI registers to their reset values.
*
* INPUT:
*      chan_num - TWSI channel
*
* OUTPUT:
*       None.
*
* RETURN:
*       None
*
*******************************************************************************/
static void twsi_reset(uint8_t chan_num)
{
	/* Reset the TWSI logic */
	mrvl_reg_write(TWSI_SOFT_RESET_REG(chan_num), 0);

	/* wait for 2 mili sec */
	mdelay(2);
}

/*******************************************************************************
* mv_twsi_addr_set - Set address on TWSI bus.
*
* DESCRIPTION:
*       This function Set address (7 or 10 Bit address) on the Twsi Bus.
*
* INPUT:
*	chan_num - TWSI channel
*       p_twsi_addr - twsi address.
*	command	 - read / write .
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if setting the address completed successfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static int mv_twsi_addr_set(uint8_t chan_num,
			    MV_TWSI_ADDR *p_twsi_addr,
			    MV_TWSI_CMD command)
{
	DB(mv_os_printf(
	    "TWSI: mv_twsi_addr7_bit_set addr %x , type %d, cmd is %s\n",
	    p_twsi_addr->address, p_twsi_addr->type,
	    ((command == MV_TWSI_WRITE) ? "Write" : "Read")));
	/* 10 Bit address */
	if (p_twsi_addr->type == ADDR10_BIT)
		return twsi_addr10_bit_set(chan_num, p_twsi_addr->address,
					   command);
	/* 7 Bit address */
	else
		return twsi_addr7_bit_set(chan_num, p_twsi_addr->address,
					  command);
}

/*******************************************************************************
* twsi_addr10_bit_set - Set 10 Bit address on TWSI bus.
*
* DESCRIPTION:
*       There are two address phases:
*       1) Write '11110' to data register bits [7:3] and 10-bit address MSB
*          (bits [9:8]) to data register bits [2:1] plus a write(0) or read(1)
*bit
*          to the Data register. Then it clears interrupt flag which drive
*          the address on the TWSI bus. The function then waits for interrupt
*          flag to be active and status 0x18 (write) or 0x40 (read) to be set.
*       2) write the rest of 10-bit address to data register and clears
*          interrupt flag which drive the address on the TWSI bus. The
*          function then waits for interrupt flag to be active and status
*          0xD0 (write) or 0xE0 (read) to be set.
*
* INPUT:
*	chan_num - TWSI channel
*       device_address - twsi address.
*	command	 - read / write .
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if setting the address completed successfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static int twsi_addr10_bit_set(uint8_t chan_num,
			       uint32_t device_address,
			       MV_TWSI_CMD command)
{
	uint32_t val, timeout;

	/* writing the 2 most significant bits of the 10 bit address */
	val = ((device_address & TWSI_DATA_ADDR_10_BIT_MASK) >>
	       TWSI_DATA_ADDR_10_BIT_OFFS);
	/* bits 7:3 must be 0x11110 */
	val |= TWSI_DATA_ADDR_10_BIT_CONST;
	/* set command */
	val |= command;
	mrvl_reg_write(TWSI_DATA_REG(chan_num), val);
	/* WA add a delay */
	mdelay(1);

	/* clear Int flag */
	twsi_int_flg_clr(chan_num);

	/* wait for Int to be Set */
	timeout = 0;
	while (!twsi_main_int_get(chan_num) && (timeout++ < TWSI_TIMEOUT_VALUE))
		;

	/* check for timeout */
	if (MV_TRUE ==
	    twsi_timeout_chk(
		timeout, (const char *)"TWSI: addr (10_bit) Int time_out.\n"))
		return MV_TIMEOUT;

	/* check the status */
	val = twsi_sts_get(chan_num);
	if ((TWSI_M_LOST_ARB_DUR_AD_OR_DATA_TRA == val) ||
	    (TWSI_M_LOST_ARB_DUR_AD_TRA_GNL_CALL_AD_REC_ACK_TRA == val)) {
		DB(mv_os_printf("TWSI: Lost Arb, status %x\n", val));
		return MV_RETRY;
	} else if (((val != TWSI_AD_PLS_RD_BIT_TRA_ACK_REC) &&
		    (command == MV_TWSI_READ)) ||
		   ((val != TWSI_AD_PLS_WR_BIT_TRA_ACK_REC) &&
		    (command == MV_TWSI_WRITE))) {
		mv_os_printf("TWSI: status %x 1st addr (10 Bit) in %s mode.\n",
			     val,
			     ((command == MV_TWSI_WRITE) ? "Write" : "Read"));
		return MV_FAIL;
	}

	/* set  8 LSB of the address */
	val = (device_address << TWSI_DATA_ADDR_7_BIT_OFFS) &
	      TWSI_DATA_ADDR_7_BIT_MASK;
	mrvl_reg_write(TWSI_DATA_REG(chan_num), val);

	/* clear Int flag */
	twsi_int_flg_clr(chan_num);

	/* wait for Int to be Set */
	timeout = 0;
	while (!twsi_main_int_get(chan_num) && (timeout++ < TWSI_TIMEOUT_VALUE))
		;

	/* check for timeout */
	if (MV_TRUE ==
	    twsi_timeout_chk(timeout,
			     (const char *)"TWSI: 2nd (10 Bit) Int tim_out.\n"))
		return MV_TIMEOUT;

	/* check the status */
	val = twsi_sts_get(chan_num);
	if ((TWSI_M_LOST_ARB_DUR_AD_OR_DATA_TRA == val) ||
	    (TWSI_M_LOST_ARB_DUR_AD_TRA_GNL_CALL_AD_REC_ACK_TRA == val)) {
		DB(mv_os_printf("TWSI: Lost Arb, status %x\n", val));
		return MV_RETRY;
	} else if (((val != TWSI_SEC_AD_PLS_RD_BIT_TRA_ACK_REC) &&
		    (command == MV_TWSI_READ)) ||
		   ((val != TWSI_SEC_AD_PLS_WR_BIT_TRA_ACK_REC) &&
		    (command == MV_TWSI_WRITE))) {
		mv_os_printf("TWSI: status %x 2nd addr(10 Bit) in %s mode.\n",
			     val,
			     ((command == MV_TWSI_WRITE) ? "Write" : "Read"));
		return MV_FAIL;
	}

	return MV_OK;
}

/*******************************************************************************
* twsi_addr7_bit_set - Set 7 Bit address on TWSI bus.
*
* DESCRIPTION:
*       This function writes 7 bit address plus a write or read bit to the
*       Data register. Then it clears interrupt flag which drive the address on
*       the TWSI bus. The function then waits for interrupt flag to be active
*       and status 0x18 (write) or 0x40 (read) to be set.
*
* INPUT:
*	chan_num - TWSI channel
*       device_address - twsi address.
*	command	 - read / write .
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if setting the address completed successfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static int twsi_addr7_bit_set(uint8_t chan_num,
			      uint32_t device_address,
			      MV_TWSI_CMD command)
{
	uint32_t val, timeout;

	/* set the address */
	val = (device_address << TWSI_DATA_ADDR_7_BIT_OFFS) &
	      TWSI_DATA_ADDR_7_BIT_MASK;
	/* set command */
	val |= command;
	mrvl_reg_write(TWSI_DATA_REG(chan_num), val);
	/* WA add a delay */
	mdelay(1);

	/* clear Int flag */
	twsi_int_flg_clr(chan_num);

	/* wait for Int to be Set */
	timeout = 0;
	while (!twsi_main_int_get(chan_num) && (timeout++ < TWSI_TIMEOUT_VALUE))
		;

	/* check for timeout */
	if (MV_TRUE ==
	    twsi_timeout_chk(
		timeout, (const char *)"TWSI: Addr (7 Bit) int time_out.\n"))
		return MV_TIMEOUT;

	/* check the status */
	val = twsi_sts_get(chan_num);
	if ((TWSI_M_LOST_ARB_DUR_AD_OR_DATA_TRA == val) ||
	    (TWSI_M_LOST_ARB_DUR_AD_TRA_GNL_CALL_AD_REC_ACK_TRA == val)) {
		DB(mv_os_printf("TWSI: Lost Arb, status %x\n", val));
		return MV_RETRY;
	} else if (((val != TWSI_AD_PLS_RD_BIT_TRA_ACK_REC) &&
		    (command == MV_TWSI_READ)) ||
		   ((val != TWSI_AD_PLS_WR_BIT_TRA_ACK_REC) &&
		    (command == MV_TWSI_WRITE))) {
		/* only in debug, since in boot we try to read the SPD of both
		   DRAM, and we don't
		   want error messeges in case DIMM doesn't exist. */
		DB(mv_os_printf(
		    "TWSI: status %x addr (7 Bit) in %s mode.\n", val,
		    ((command == MV_TWSI_WRITE) ? "Write" : "Read")));
		return MV_FAIL;
	}

	return MV_OK;
}

/*******************************************************************************
* twsi_data_write - Trnasmit a data block over TWSI bus.
*
* DESCRIPTION:
*       This function writes a given data block to TWSI bus in 8 bit
*       granularity.
*	first The function waits for interrupt flag to be active then
*       For each 8-bit data:
*        The function writes data to data register. It then clears
*        interrupt flag which drives the data on the TWSI bus.
*        The function then waits for interrupt flag to be active and status
*        0x28 to be set.
*
*
* INPUT:
*	chan_num - TWSI channel
*       p_block - Data block.
*	block_size - number of chars in p_block.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if transmiting the block completed successfully,
*	MV_BAD_PARAM - if p_block is NULL,
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static int twsi_data_transmit(uint8_t chan_num,
			      uint8_t *p_block,
			      uint32_t block_size)
{
	uint32_t timeout, temp, block_size_wr = block_size;

	if (NULL == p_block)
		return MV_BAD_PARAM;

	/* wait for Int to be Set */
	timeout = 0;
	while (!twsi_main_int_get(chan_num) && (timeout++ < TWSI_TIMEOUT_VALUE))
		;

	/* check for timeout */
	if (MV_TRUE ==
	    twsi_timeout_chk(timeout,
			     (const char *)"TWSI: Read Data Int time_out.\n"))
		return MV_TIMEOUT;

	while (block_size_wr) {
		/* write the data */
		mrvl_reg_write(TWSI_DATA_REG(chan_num), (uint32_t)*p_block);
		DB(mv_os_printf(
		    "TWSI: twsi_data_transmit place = %d write %x\n",
		    block_size - block_size_wr, *p_block));
		p_block++;
		block_size_wr--;

		twsi_int_flg_clr(chan_num);

		/* wait for Int to be Set */
		timeout = 0;
		while (!twsi_main_int_get(chan_num) &&
		       (timeout++ < TWSI_TIMEOUT_VALUE))
			;

		/* check for timeout */
		if (MV_TRUE == twsi_timeout_chk(
				   timeout, (const char *)"TWSI: time_out.\n"))
			return MV_TIMEOUT;

		/* check the status */
		temp = twsi_sts_get(chan_num);
		if ((TWSI_M_LOST_ARB_DUR_AD_OR_DATA_TRA == temp) ||
		    (TWSI_M_LOST_ARB_DUR_AD_TRA_GNL_CALL_AD_REC_ACK_TRA ==
		     temp)) {
			DB(mv_os_printf("TWSI: Lost Arb, status %x\n", temp));
			return MV_RETRY;
		} else if (temp != TWSI_M_TRAN_DATA_BYTE_ACK_REC) {
			mv_os_printf("TWSI: status %x in write trans\n", temp);
			return MV_FAIL;
		}
	}

	return MV_OK;
}

/*******************************************************************************
* twsi_data_receive - Receive data block from TWSI bus.
*
* DESCRIPTION:
*       This function receive data block from TWSI bus in 8bit granularity
*       into p_block buffer.
*	first The function waits for interrupt flag to be active then
*       For each 8-bit data:
*        It clears the interrupt flag which allows the next data to be
*        received from TWSI bus.
*	 The function waits for interrupt flag to be active,
*	 and status reg is 0x50.
*	 Then the function reads data from data register, and copies it to
*	 the given buffer.
*
* INPUT:
*	chan_num - TWSI channel
*       block_size - number of bytes to read.
*
* OUTPUT:
*       p_block - Data block.
*
* RETURN:
*       MV_OK - if receive transaction completed successfully,
*	MV_BAD_PARAM - if p_block is NULL,
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static int twsi_data_receive(uint8_t chan_num,
			     uint8_t *p_block,
			     uint32_t block_size)
{
	uint32_t timeout, temp, block_size_rd = block_size;

	if (NULL == p_block)
		return MV_BAD_PARAM;

	/* wait for Int to be Set */
	timeout = 0;
	while (!twsi_main_int_get(chan_num) && (timeout++ < TWSI_TIMEOUT_VALUE))
		;

	/* check for timeout */
	if (MV_TRUE ==
	    twsi_timeout_chk(timeout,
			     (const char *)"TWSI: Read Data int Time out .\n"))
		return MV_TIMEOUT;

	while (block_size_rd) {
		if (block_size_rd == 1)
			/* clear ack and Int flag */
			mrvl_reg_bit_reset(
				TWSI_CONTROL_REG(chan_num), TWSI_CONTROL_ACK);

		twsi_int_flg_clr(chan_num);
		/* wait for Int to be Set */
		timeout = 0;
		while ((!twsi_main_int_get(chan_num)) &&
		       (timeout++ < TWSI_TIMEOUT_VALUE))
			;

		/* check for timeout */
		if (MV_TRUE ==
		    twsi_timeout_chk(timeout, (const char *)"TWSI: Timeout.\n"))
			return MV_TIMEOUT;

		/* check the status */
		temp = twsi_sts_get(chan_num);
		if ((TWSI_M_LOST_ARB_DUR_AD_OR_DATA_TRA == temp) ||
		    (TWSI_M_LOST_ARB_DUR_AD_TRA_GNL_CALL_AD_REC_ACK_TRA ==
		     temp)) {
			DB(mv_os_printf("TWSI: Lost Arb, status %x\n", temp));
			return MV_RETRY;
		} else if ((temp != TWSI_M_REC_RD_DATA_ACK_TRA) &&
			   (block_size_rd != 1)) {
			mv_os_printf("TWSI: status %x in read trans\n", temp);
			return MV_FAIL;
		} else if ((temp != TWSI_M_REC_RD_DATA_ACK_NOT_TRA) &&
			   (block_size_rd == 1)) {
			mv_os_printf("TWSI: status %x in Rd Terminate\n", temp);
			return MV_FAIL;
		}

		/* read the data */
		*p_block = (uint8_t)mrvl_reg_read(TWSI_DATA_REG(chan_num));
		DB(mv_os_printf("TWSI: twsi_data_receive  place %d read %x\n",
				block_size - block_size_rd, *p_block));
		p_block++;
		block_size_rd--;
	}

	return MV_OK;
}

/*******************************************************************************
* twsi_target_offs_set - Set TWST target offset on TWSI bus.
*
* DESCRIPTION:
*       The function support TWSI targets that have inside address space (for
*       example EEPROMs). The function:
*       1) Convert the given offset into p_block and size.
*		in case the offset should be set to a TWSI slave which support
*		more then 256 bytes offset, the offset setting will be done
*		in 2 transactions.
*       2) Use twsi_data_transmit to place those on the bus.
*
* INPUT:
*	chan_num - TWSI channel
*       offset - offset to be set on the EEPROM device.
*	more_than256 - whether the EEPROM device support more then 256 byte
*offset.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - if setting the offset completed successfully.
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static int twsi_target_offs_set(uint8_t chan_num,
				uint32_t offset,
				uint8_t more_than256)
{
	uint8_t off_block[2];
	uint32_t off_size;

	if (more_than256 == MV_TRUE) {
		off_block[0] = (offset >> 8) & 0xff;
		off_block[1] = offset & 0xff;
		off_size = 2;
	} else {
		off_block[0] = offset & 0xff;
		off_size = 1;
	}
	DB(mv_os_printf(
	    "TWSI: twsi_target_offs_set off_size = %x addr1 = %x addr2 = %x\n",
	    off_size, off_block[0], off_block[1]));
	return twsi_data_transmit(chan_num, off_block, off_size);
}

/*******************************************************************************
* mv_twsi_read - Read data block from a TWSI Slave.
*
* DESCRIPTION:
*       The function calls the following functions:
*       -) mv_twsi_start_bit_set();
*	if (EEPROM device)
*	-) mv_twsi_addr_set(w);
*	-) twsi_target_offs_set();
*	-) mv_twsi_start_bit_set();
*	-) mv_twsi_addr_set(r);
*	-) twsi_data_receive();
*	-) mv_twsi_stop_bit_set();
*
* INPUT:
*	chan_num - TWSI channel
*	p_twsi_slave - Twsi Slave structure.
*	block_size - number of bytes to read.
*
* OUTPUT:
*	p_block - Data block.
*
* RETURN:
*	MV_OK - if EEPROM read transaction completed successfully,
*	MV_BAD_PARAM - if p_block is NULL,
*	MV_FAIL otherwmise.
*
*******************************************************************************/
static int mv_twsi_read(uint8_t chan_num,
			MV_TWSI_SLAVE *p_twsi_slave,
			uint8_t *p_block,
			uint32_t block_size)
{
	int rc;
	int ret = MV_FAIL;
	uint32_t counter = 0;

	if ((NULL == p_block) || (NULL == p_twsi_slave))
		return MV_BAD_PARAM;

	do {
		/* wait for 1 mili sec for the clear to take effect */
		if (counter > 0)
			mdelay(1);
		ret = mv_twsi_start_bit_set(chan_num);

		if (MV_RETRY == ret)
			continue;
		else if (MV_OK != ret) {
			mv_twsi_stop_bit_set(chan_num);
			DB(mv_os_printf(
			    "mv_twsi_read:mv_twsi_start_bit_set failed\n"));
			return MV_FAIL;
		}

		DB(mv_os_printf(
		    "TWSI: mv_twsi_eeprom_read after mv_twsi_start_bit_set\n"));

		/* in case offset exsist (i.e. eeprom ) */
		if (MV_TRUE == p_twsi_slave->valid_offset) {
			rc = mv_twsi_addr_set(chan_num,
					      &(p_twsi_slave->slave_addr),
					      MV_TWSI_WRITE);
			if (MV_RETRY == rc)
				continue;
			else if (MV_OK != rc) {
				mv_twsi_stop_bit_set(chan_num);
				DB(mv_os_printf(
				    "mv_twsi_addr_set(%d,0x%x,%d) rc=%d\n",
				    chan_num,
				    (uint32_t) &(p_twsi_slave->slave_addr),
				    MV_TWSI_WRITE, rc));
				return MV_FAIL;
			}

			ret =
			    twsi_target_offs_set(chan_num, p_twsi_slave->offset,
						 p_twsi_slave->more_than256);
			if (MV_RETRY == ret)
				continue;
			else if (MV_OK != ret) {
				mv_twsi_stop_bit_set(chan_num);
				DB(mv_os_printf(
				    "TWSI: twsi_target_offs_set Failed\n"));
				return MV_FAIL;
			}
			DB(mv_os_printf("TWSI: after twsi_target_offs_set\n"));
			ret = mv_twsi_start_bit_set(chan_num);
			if (MV_RETRY == ret)
				continue;
			else if (MV_OK != ret) {
				mv_twsi_stop_bit_set(chan_num);
				DB(mv_os_printf(
				    "TWSI: mv_twsi_start_bit_set failed\n"));
				return MV_FAIL;
			}
			DB(mv_os_printf("TWSI: after mv_twsi_start_bit_set\n"));
		}
		ret = mv_twsi_addr_set(chan_num, &(p_twsi_slave->slave_addr),
				       MV_TWSI_READ);
		if (MV_RETRY == ret)
			continue;
		else if (MV_OK != ret) {
			mv_twsi_stop_bit_set(chan_num);
			DB(mv_os_printf(
			    "mv_twsi_read: mv_twsi_addr_set 2 Failed\n"));
			return MV_FAIL;
		}
		DB(mv_os_printf(
		    "TWSI: mv_twsi_eeprom_read after mv_twsi_addr_set\n"));

		ret = twsi_data_receive(chan_num, p_block, block_size);
		if (MV_RETRY == ret)
			continue;
		else if (MV_OK != ret) {
			mv_twsi_stop_bit_set(chan_num);
			DB(mv_os_printf(
			    "mv_twsi_read: twsi_data_receive Failed\n"));
			return MV_FAIL;
		}
		DB(mv_os_printf(
		    "TWSI: mv_twsi_eeprom_read after twsi_data_receive\n"));

		ret = mv_twsi_stop_bit_set(chan_num);
		if (MV_RETRY == ret)
			continue;
		else if (MV_OK != ret) {
			DB(mv_os_printf(
			    "mv_twsi_read: mv_twsi_stop_bit_set 3 Failed\n"));
			return MV_FAIL;
		}
		counter++;
	} while ((MV_RETRY == ret) && (counter < MAX_RETRY_CNT));

	if (counter == MAX_RETRY_CNT)
		DB(mv_os_printf("mv_twsi_write: Retry Expire\n"));

	twsi_ack_bit_set(chan_num);

	DB(mv_os_printf(
	    "TWSI: mv_twsi_eeprom_read after mv_twsi_stop_bit_set\n"));

	return MV_OK;
}

/*******************************************************************************
* mv_twsi_write - Write data block to a TWSI Slave.
*
* DESCRIPTION:
*       The function calls the following functions:
*       -) mv_twsi_start_bit_set();
*       -) mv_twsi_addr_set();
*	-)if (EEPROM device)
*	-) twsi_target_offs_set();
*       -) twsi_data_transmit();
*       -) mv_twsi_stop_bit_set();
*
* INPUT:
*	chan_num - TWSI channel
*	eeprom_address - eeprom address.
*       block_size - number of bytes to write.
*	p_block - Data block.
*
* OUTPUT:
*	None
*
* RETURN:
*       MV_OK - if EEPROM read transaction completed successfully.
*	MV_BAD_PARAM - if p_block is NULL,
*	MV_FAIL otherwmise.
*
* NOTE: Part of the EEPROM, required that the offset will be aligned to the
*	max write burst supported.
*******************************************************************************/
static int mv_twsi_write(uint8_t chan_num,
			 MV_TWSI_SLAVE *p_twsi_slave,
			 uint8_t *p_block,
			 uint32_t block_size)
{
	int ret = MV_FAIL;
	uint32_t counter = 0;

	if ((NULL == p_block) || (NULL == p_twsi_slave))
		return MV_BAD_PARAM;

	do {
		if (counter >
		    0) /* wait for 1 mili sec for the clear to take effect */
			mdelay(1);
		ret = mv_twsi_start_bit_set(chan_num);

		if (MV_RETRY == ret)
			continue;

		else if (MV_OK != ret) {
			mv_twsi_stop_bit_set(chan_num);
			DB(mv_os_printf(
			    "mv_twsi_write: mv_twsi_start_bit_set failed\n"));
			return MV_FAIL;
		}

		ret = mv_twsi_addr_set(chan_num, &(p_twsi_slave->slave_addr),
				       MV_TWSI_WRITE);
		if (MV_RETRY == ret)
			continue;
		else if (MV_OK != ret) {
			mv_twsi_stop_bit_set(chan_num);
			DB(mv_os_printf(
			    "mv_twsi_write: mv_twsi_addr_set failed\n"));
			return MV_FAIL;
		}

		/* in case offset exsist (i.e. eeprom ) */
		if (MV_TRUE == p_twsi_slave->valid_offset) {
			ret =
			    twsi_target_offs_set(chan_num, p_twsi_slave->offset,
						 p_twsi_slave->more_than256);
			if (MV_RETRY == ret)
				continue;
			else if (MV_OK != ret) {
				mv_twsi_stop_bit_set(chan_num);
				DB(mv_os_printf(
				    "TWSI: twsi_target_offs_set failed\n"));
				return MV_FAIL;
			}
		}

		ret = twsi_data_transmit(chan_num, p_block, block_size);
		if (MV_RETRY == ret)
			continue;
		else if (MV_OK != ret) {
			mv_twsi_stop_bit_set(chan_num);
			DB(mv_os_printf(
			    "mv_twsi_write: twsi_data_transmit failed\n"));
			return MV_FAIL;
		}
		ret = mv_twsi_stop_bit_set(chan_num);
		if (MV_RETRY == ret)
			continue;
		else if (MV_OK != ret) {
			DB(mv_os_printf(
			    "mv_twsi_write: failed to set stopbit\n"));
			return MV_FAIL;
		}
		counter++;
	} while ((MV_RETRY == ret) && (counter < MAX_RETRY_CNT));

	if (counter == MAX_RETRY_CNT)
		DB(mv_os_printf("mv_twsi_write: Retry Expire\n"));

	return MV_OK;
}

static int i2c_init(unsigned bus)
{
	if (bus >= MAX_I2C_NUM)
		return 1;

	if (!m_initialized[bus]) {
		/* TWSI init */
		MV_TWSI_ADDR slave;

		slave.type = ADDR7_BIT;
		slave.address = 0;
		mv_twsi_init(bus, TWSI_SPEED, mv_tclk_get(), &slave, 0);
		m_initialized[bus] = 1;
	}

	return 0;
}

static void i2c_reset(unsigned bus)
{
	if (bus < MAX_I2C_NUM)
		m_initialized[bus] = 0;
}

int platform_i2c_transfer(unsigned bus, struct i2c_seg *segments, int seg_count)
{
	struct i2c_seg *seg = segments;
	int ret = 0;
	MV_TWSI_SLAVE twsi_slave;

	if (i2c_init(bus))
		return 1;

	while (!ret && seg_count--) {
		twsi_slave.slave_addr.address = seg->chip;
		twsi_slave.slave_addr.type = ADDR7_BIT;
		twsi_slave.more_than256 = MV_FALSE;
		twsi_slave.valid_offset = MV_FALSE;
		if (seg->read)
			ret =
			    mv_twsi_read(bus, &twsi_slave, seg->buf, seg->len);
		else
			ret =
			    mv_twsi_write(bus, &twsi_slave, seg->buf, seg->len);
		seg++;
	}

	if (ret) {
		i2c_reset(bus);
		DB(mv_os_printf("mv_twsi_read/mv_twsi_write failed\n"));
		return 1;
	}

	return 0;
}
