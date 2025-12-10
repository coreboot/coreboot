/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_CMD_DB_H__
#define __SOC_QUALCOMM_CMD_DB_H__

#include <types.h>

#define NUM_PRIORITY		2
#define MAX_SLV_ID		8
#define SLAVE_ID_MASK		0x7
#define SLAVE_ID_SHIFT		16
#define CMD_DB_STANDALONE_MASK	BIT(0)

#define SLAVE_ID(addr)		(((addr) >> 16) & 0xF)
#define VRM_ADDR(addr)		(((addr) >> 4) & 0xFFFF)

enum cmd_db_hw_type {
	CMD_DB_HW_INVALID = 0,
	CMD_DB_HW_MIN     = 3,
	CMD_DB_HW_ARC     = CMD_DB_HW_MIN,
	CMD_DB_HW_VRM     = 4,
	CMD_DB_HW_BCM     = 5,
	CMD_DB_HW_MAX     = CMD_DB_HW_BCM,
	CMD_DB_HW_ALL     = 0xff,
};

/**
 * cmd_db_ready - Indicates if command DB is available
 *
 * Return: CB_SUCCESS on success, CB_ERR otherwise
 */
enum cb_err cmd_db_ready(void);

/**
 * cmd_db_read_addr() - Query command db for resource id address.
 *
 * @id: resource id to query for address
 *
 * Return: resource address on success, 0 on error
 *
 * This is used to retrieve resource address based on resource id.
 */
u32 cmd_db_read_addr(const char *id);

/**
 * cmd_db_read_aux_data() - Query command db for aux data.
 *
 * @id: Resource to retrieve AUX Data on
 * @len: size of data buffer returned
 *
 * Return: pointer to data on success, NULL otherwise
 */
const void *cmd_db_read_aux_data(const char *id, size_t *len);

/**
 * cmd_db_match_resource_addr() - Compare if both Resource addresses are same
 *
 * @addr1: Resource address to compare
 * @addr2: Resource address to compare
 *
 * Return: true if two addresses refer to the same resource, false otherwise
 */
bool cmd_db_match_resource_addr(u32 addr1, u32 addr2);

/**
 * cmd_db_read_slave_id - Get the slave ID for a given resource address
 *
 * @id: Resource id to query the DB for version
 *
 * Return: cmd_db_hw_type enum on success, CMD_DB_HW_INVALID on error
 */
enum cmd_db_hw_type cmd_db_read_slave_id(const char *id);

/**
 * cmd_db_is_standalone - Check if command DB is in standalone mode
 *
 * Return: true if standalone, false otherwise
 */
bool cmd_db_is_standalone(void);

/**
 * cmd_db_init - Initialize the command DB
 *
 * @base: Physical base address of the command DB memory region
 * @size: Size of the command DB memory region
 *
 * Return: CB_SUCCESS on success, CB_ERR otherwise
 */
enum cb_err cmd_db_init(uintptr_t base, size_t size);

#endif /* __SOC_QUALCOMM_CMD_DB_H__ */
