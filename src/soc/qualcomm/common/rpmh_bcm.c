/* SPDX-License-Identifier: GPL-2.0-only */
#include <device/device.h>
#include <console/console.h>
#include <soc/cmd_db.h>
#include <soc/rpmh.h>
#include <soc/rpmh_bcm.h>
#include <soc/tcs.h>

/**
 * rpmh_bcm_vote() - Send BCM (Bus Clock Manager) vote to RPMh
 * @dev: Device pointer for RPMh communication
 * @resource_name: BCM resource name (e.g., "MM0", "SH0")
 * @vote_value: Vote value to send
 *
 * This function sends a BCM vote to RPMh to enable bandwidth/clock requirements.
 * It looks up the resource address from Command DB and sends the vote via TCS command.
 *
 * Return: 0 on success, negative error code on failure
 */
int rpmh_bcm_vote(const char *resource_name, u32 vote_value)
{
	u32 bcm_addr;
	struct tcs_cmd cmd;
	int rc;

	if (!resource_name) {
		printk(BIOS_ERR, "BCM: Invalid parameters\n");
		return -1;
	}

	/* Get the RPMh address for the BCM resource from Command DB */
	bcm_addr = cmd_db_read_addr(resource_name);
	if (!bcm_addr) {
		printk(BIOS_ERR, "BCM: Could not find RPMh address for resource %s\n",
		       resource_name);
		return -1;
	}

	printk(BIOS_DEBUG, "BCM: Found address 0x%08X for resource %s\n",
	       bcm_addr, resource_name);

	/* Prepare TCS command */
	cmd.addr = bcm_addr;
	cmd.data = vote_value;
	cmd.wait = true;

	/* Send BCM vote via RPMh in active-only state */
	rc = rpmh_write(RPMH_ACTIVE_ONLY_STATE, &cmd, 1);
	if (rc) {
		printk(BIOS_ERR, "BCM: Failed to send vote for %s (addr=0x%08X, val=0x%08X): %d\n",
		       resource_name, bcm_addr, vote_value, rc);
		return rc;
	}

	printk(BIOS_INFO, "BCM: Successfully voted for %s (addr=0x%08X, val=0x%08X)\n",
	       resource_name, bcm_addr, vote_value);

	return 0;
}
