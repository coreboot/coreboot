/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Mailbox EC communication interface for Google Chrome Embedded Controller.
 */

#ifndef _EC_GOOGLE_CHROMEEC_EC_H
#define _EC_GOOGLE_CHROMEEC_EC_H
#include <types.h>
#include <device/device.h>
#include "ec_commands.h"
#include <device/usbc_mux.h>

/* Fill in base and size of the IO port resources used. */
void google_chromeec_ioport_range(uint16_t *base, size_t *size);

int google_chromeec_i2c_xfer(uint8_t chip, uint8_t addr, int alen,
			     uint8_t *buffer, int len, int is_read);
uint64_t google_chromeec_get_wake_mask(void);
int google_chromeec_set_sci_mask(uint64_t mask);
int google_chromeec_set_smi_mask(uint64_t mask);
int google_chromeec_set_wake_mask(uint64_t mask);
enum host_event_code google_chromeec_get_event(void);

/* Check if EC supports feature EC_FEATURE_UNIFIED_WAKE_MASKS */
bool google_chromeec_is_uhepi_supported(void);
int google_ec_running_ro(void);
enum ec_image google_chromeec_get_current_image(void);
void google_chromeec_init(void);
/* Check for the current mux state in EC
 * in: int port physical port number of the type-c port
 * out: uint8_t flags representing the status of the mux such as
 *	usb capability, dp capability, cable type, etc
 */
int google_chromeec_usb_get_pd_mux_info(int port, uint8_t *flags);
/* Poll (up to `timeout_ms` ms) for DisplayPort to be ready
 * Return:	-1: Error. 0: Timeout.
 *              >=1: Bitmask of the ports that DP device is connected
 */
int google_chromeec_wait_for_displayport(long timeout_ms);
/* Poll (up to `timeout_ms` ms) for a Hot-Plug Detect (HPD)
 * event on the specified port.
 * Return: 0 on HPD ready, -1 on timeout */
int google_chromeec_wait_for_dp_hpd(int port, long timeout_ms);
/* Send command to EC to request to enter DisplayPort ALT mode on the
 * specified port.
 * Return: 0 on success, -1 on error */
int google_chromeec_typec_control_enter_dp_mode(int port);
/*
 * Obtain any USB-C mux data needed for the specified port
 * in: int port physical port number of the type-c port
 * out: struct usbc_mux_info mux_info stores USB-C mux data
 * Return: 0 on success, -1 on error
 */
int google_chromeec_get_usbc_mux_info(int port, struct usbc_mux_info *mux_info);

/* Device events */
uint64_t google_chromeec_get_device_enabled_events(void);
int google_chromeec_set_device_enabled_events(uint64_t mask);
uint64_t google_chromeec_get_device_current_events(void);

int google_chromeec_check_feature(int feature);
uint8_t google_chromeec_calc_checksum(const uint8_t *data, int size);

/**
 * google_chromeec_get_board_version() - Get the board version
 * @version: Out parameter to retrieve the board Version
 *
 * Return: 0 on success or -1 on failure/error.
 *
 * This function is used to get the board version information from EC.
 */
int google_chromeec_get_board_version(uint32_t *version);
uint32_t google_chromeec_get_sku_id(void);
int google_chromeec_set_sku_id(uint32_t skuid);
uint64_t  google_chromeec_get_events_b(void);
int google_chromeec_clear_events_b(uint64_t mask);
int google_chromeec_kbbacklight(int percent);
void google_chromeec_post(uint8_t postcode);
uint8_t google_chromeec_get_switches(void);
bool google_chromeec_get_ap_watchdog_flag(void);

/* Temporary secure storage commands */
int google_chromeec_vstore_supported(void);
int google_chromeec_vstore_info(uint32_t *locked);
int google_chromeec_vstore_read(int slot, uint8_t *data);
int google_chromeec_vstore_write(int slot, uint8_t *data, size_t size);

/* Issue reboot command to EC with specified type and flags. Returns 0 on
   success, < 0 otherwise. */
int google_chromeec_reboot(enum ec_reboot_cmd type, uint8_t flags);

/**
 * Get data from Cros Board Info
 *
 * @param id/fw_config/buf [OUT] value from CBI.
 * @return 0 on success or negative integer for errors.
 */
int google_chromeec_cbi_get_oem_id(uint32_t *id);
int google_chromeec_cbi_get_sku_id(uint32_t *id);
int google_chromeec_cbi_get_fw_config(uint64_t *fw_config);
int google_chromeec_cbi_get_dram_part_num(char *buf, size_t bufsize);
int google_chromeec_cbi_get_oem_name(char *buf, size_t bufsize);
/* version may be stored in CBI as a smaller integer width, but the EC code
   handles it correctly. */
int google_chromeec_cbi_get_board_version(uint32_t *version);
int google_chromeec_cbi_get_ssfc(uint32_t *ssfc);

#define CROS_SKU_UNKNOWN	0xFFFFFFFF
#define CROS_SKU_UNPROVISIONED	0x7FFFFFFF
/* Returns CROS_SKU_UNKNOWN on failure. */
uint32_t google_chromeec_get_board_sku(void);
const char *google_chromeec_smbios_system_sku(void);

/* MEC uses 0x800/0x804 as register/index pair, thus an 8-byte resource. */
#define MEC_EMI_BASE		0x800
#define MEC_EMI_SIZE		8

/* For MEC, access ranges 0x800 thru 0x9ff using EMI interface instead of LPC */
#define MEC_EMI_RANGE_START EC_HOST_CMD_REGION0
#define MEC_EMI_RANGE_END   (EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SIZE)

int google_chromeec_set_usb_charge_mode(uint8_t port_id, enum usb_charge_mode mode);
int google_chromeec_set_usb_pd_role(uint8_t port, enum usb_pd_control_role role);
/*
 * Retrieve the charger type and max wattage.
 *
 * @param type      charger type
 * @param current_max charger max current
 * @param voltage_max charger max voltage
 * @return non-zero for error, otherwise 0.
 */
int google_chromeec_get_usb_pd_power_info(enum usb_chg_type *type,
					  uint16_t *current_max, uint16_t *voltage_max);

/*
 * Set max current and voltage of a dedicated charger.
 *
 * @param current_lim Max current in mA
 * @param voltage_lim Max voltage in mV
 * @return non-zero for error, otherwise 0.
 */
int google_chromeec_override_dedicated_charger_limit(uint16_t current_lim,
						     uint16_t voltage_lim);

/* internal structure to send a command to the EC and wait for response. */
struct chromeec_command {
	uint16_t    cmd_code;	  /* command code in, status out */
	uint8_t     cmd_version;  /* command version */
	const void* cmd_data_in;  /* command data, if any */
	void*	    cmd_data_out; /* command response, if any */
	uint16_t    cmd_size_in;  /* size of command data */
	uint16_t    cmd_size_out; /* expected size of command response in,
				   * actual received size out */
	int         cmd_dev_index;/* device index for passthru */
};

/*
 * There are transport level constraints for sending protov3 packets. Because
 * of this provide a way for the generic protocol layer to request buffers
 * so that there is zero copying being done through the layers.
 *
 * Request the buffer provided the size. If 'req' is non-zero then the
 * buffer requested is for EC requests. Otherwise it's for responses. Return
 * non-NULL on success, NULL on error.
 */
void *crosec_get_buffer(size_t size, int req);

/*
 * The lower level transport works on the buffers handed out to the
 * upper level. Therefore, only the size of the request and response
 * are required.
 */
typedef int (*crosec_io_t)(size_t req_size, size_t resp_size, void *context);
int crosec_command_proto(struct chromeec_command *cec_command,
			 crosec_io_t crosec_io, void *context);

/**
 * Performs light verification of the EC<->AP communication channel.
 *
 * @return		0 on success, -1 on error
 */
int google_chromeec_hello(void);

/**
 * Send a command to a CrOS EC
 *
 * @param cec_command: CrOS EC command to send
 * @return 0 for success. Non-zero for error.
 */
int google_chromeec_command(struct chromeec_command *cec_command);

struct google_chromeec_event_info {
	uint64_t log_events;
	uint64_t sci_events;
	uint64_t smi_events;
	uint64_t s3_wake_events;
	uint64_t s3_device_events;
	uint64_t s5_wake_events;
	uint64_t s0ix_wake_events;
};
void google_chromeec_events_init(const struct google_chromeec_event_info *info,
					bool is_s3_wakeup);

/*
 * Get next available MKBP event in ec_response_get_next_event. Returns 0 on
 * success, < 0 otherwise.
 */
int google_chromeec_get_mkbp_event(struct ec_response_get_next_event *event);

/* Log host events to eventlog based on the mask provided. */
void google_chromeec_log_events(uint64_t mask);

/**
 * Protect/un-protect EC flash regions.
 *
 * @param mask		Set/clear the requested bits in 'flags'
 * @param flags		Flash protection flags
 * @param resp		Pointer to response structure
 * @return		0 on success, -1 on error
 */
int google_chromeec_flash_protect(uint32_t mask, uint32_t flags,
				  struct ec_response_flash_protect *resp);
/**
 * Calculate image hash for vboot.
 *
 * @param hash_type	The hash types supported by the EC for vboot
 * @param offset	The offset to start hashing in flash
 * @param resp		Pointer to response structure
 * @return		0 on success, -1 on error
 */
int google_chromeec_start_vboot_hash(enum ec_vboot_hash_type hash_type,
				     uint32_t offset,
				     struct ec_response_vboot_hash *resp);
/**
 * Return the EC's vboot image hash.
 *
 * @param offset	Get hash for flash region beginning here
 * @param resp		Pointer to response structure
 * @return		0 on success, -1 on error
 *
 */
int google_chromeec_get_vboot_hash(uint32_t offset,
				   struct ec_response_vboot_hash *resp);

/**
 * Get offset and size of the specified EC flash region.
 *
 * @param region	Which region of EC flash
 * @param offset	Gets filled with region's offset
 * @param size		Gets filled with region's size
 * @return		0 on success, -1 on error
 */
int google_chromeec_flash_region_info(enum ec_flash_region region,
				      uint32_t *offset, uint32_t *size);
/**
 * Erase a region of EC flash.
 *
 * @param offset	Where to begin erasing
 * @param size		Size of area to erase
 * @return		0 on success, -1 on error
 */
int google_chromeec_flash_erase(uint32_t region_offset, uint32_t region_size);

/**
 * Return information about the entire flash.
 *
 * @param info		Pointer to response structure
 * @return		0 on success, -1 on error
 */
int google_chromeec_flash_info(struct ec_response_flash_info *info);

/**
 * Write a block into EC flash.
 *
 * @param data		Pointer to data to write to flash, prefixed by a
 *			struct ec_params_flash_write
 * @param offset        Offset to begin writing data
 * @param size		Number of bytes to be written to flash from data
 * @return		0 on success, -1 on error
 */
int google_chromeec_flash_write_block(const uint8_t *data, uint32_t size);

/**
 * Verify flash using EFS if available.
 *
 * @param region	Which flash region to verify
 * @return		0 on success, -1 on error
 */
int google_chromeec_efs_verify(enum ec_flash_region region);

/**
 * Command EC to perform battery cutoff.
 *
 * @param flags		Flags to pass to the EC
 * @return		0 on success, -1 on error
 */
int google_chromeec_battery_cutoff(uint8_t flags);

/**
 * Check if the EC is requesting the system to limit input power.
 *
 * @param limit_power	If successful, limit_power is 1 if EC is requesting
 *			input power limits, otherwise 0.
 * @return		0 on success, -1 on error
 */
int google_chromeec_read_limit_power_request(int *limit_power);

/**
 * Get information about the protocol that the EC speaks.
 *
 * @param resp		Filled with host command protocol information.
 * @return		0 on success, -1 on error
 */
int google_chromeec_get_protocol_info(
	struct ec_response_get_protocol_info *resp);

/**
 * Get available versions of the specified command.
 *
 * @param command	Command ID
 * @param pmask		Pointer to version mask
 * @return		0 on success, -1 on error
 */
int google_chromeec_get_cmd_versions(int command, uint32_t *pmask);

/**
 * Get number of PD-capable USB ports from EC.
 *
 * @param *num_ports	If successful, num_ports is the number
 *			of PD-capable USB ports according to the EC.
 * @return		0 on success, -1 on error
 */
int google_chromeec_get_num_pd_ports(unsigned int *num_ports);

/* Structure representing the capabilities of a USB-PD port */
struct usb_pd_port_caps {
	enum ec_pd_power_role_caps power_role_cap;
	enum ec_pd_try_power_role_caps try_power_role_cap;
	enum ec_pd_data_role_caps data_role_cap;
	enum ec_pd_port_location port_location;
};

/**
 * Get role-based capabilities for a USB-PD port
 *
 * @param port			Which port to get information about
 * @param *power_role_cap	The power-role capability of the port
 * @param *try_power_role_cap	The Try-power-role capability of the port
 * @param *data_role_cap	The data role capability of the port
 * @param *port_location	Location of the port on the device
 * @return			0 on success, -1 on error
 */
int google_chromeec_get_pd_port_caps(int port,
				struct usb_pd_port_caps *port_caps);

/**
 * Get the keyboard configuration / layout information from EC
 *
 * @param *keybd	If successful, this is filled with EC filled parameters
 * @return		0 on success, -1 on error
 */
int google_chromeec_get_keybd_config(struct ec_response_keybd_config *keybd);

/**
 * Send EC command to perform AP reset
 *
 * @return	0 on success, -1 on error
 */
int google_chromeec_ap_reset(void);

/**
 * Configure the regulator as enabled / disabled.
 *
 * @param index		Regulator ID
 * @param enable	Set to enable / disable the regulator
 * @return		0 on success, -1 on error
 */
int google_chromeec_regulator_enable(uint32_t index, uint8_t enable);

/**
 * Query if the regulator is enabled.
 *
 * @param index		Regulator ID
 * @param *enabled	If successful, enabled indicates enable/disable status.
 * @return		0 on success, -1 on error
 */
int google_chromeec_regulator_is_enabled(uint32_t index, uint8_t *enabled);

/**
 * Set voltage for the voltage regulator within the range specified.
 *
 * @param index		Regulator ID
 * @param min_mv	Minimum voltage
 * @param max_mv	Maximum voltage
 * @return		0 on success, -1 on error
 */
int google_chromeec_regulator_set_voltage(uint32_t index, uint32_t min_mv,
					  uint32_t max_mv);

/**
 * Get the currently configured voltage for the voltage regulator.
 *
 * @param index		Regulator ID
 * @param *voltage_mv	If successful, voltage_mv is filled with current voltage
 * @return		0 on success, -1 on error
 */
int google_chromeec_regulator_get_voltage(uint32_t index, uint32_t *voltage_mv);

#if CONFIG(HAVE_ACPI_TABLES)
/**
 * Writes USB Type-C PD related information to the SSDT
 *
 * @param dev			EC device
 */
void google_chromeec_fill_ssdt_generator(const struct device *dev);

/**
 * Returns the ACPI name for the EC device.
 *
 * @param dev			EC device
 */
const char *google_chromeec_acpi_name(const struct device *dev);

#endif /* HAVE_ACPI_TABLES */

#endif /* _EC_GOOGLE_CHROMEEC_EC_H */
