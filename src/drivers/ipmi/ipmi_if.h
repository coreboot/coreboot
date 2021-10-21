/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __IPMI_IF_H
#define __IPMI_IF_H

/* Common API and code for different IPMI interfaces in different stages */

#include <stdint.h>

#define IPMI_NETFN_CHASSIS 0x00
#define IPMI_NETFN_BRIDGE 0x02
#define IPMI_NETFN_SENSOREVENT 0x04
#define IPMI_NETFN_APPLICATION 0x06
#define  IPMI_BMC_GET_DEVICE_ID 0x01
#define   IPMI_IPMI_VERSION_MINOR(x) ((x) >> 4)
#define   IPMI_IPMI_VERSION_MAJOR(x) ((x) & 0xf)
#define  IPMI_BMC_GET_SELFTEST_RESULTS 0x04
#define   IPMI_APP_SELFTEST_RESERVED             0xFF
#define   IPMI_APP_SELFTEST_NO_ERROR             0x55
#define   IPMI_APP_SELFTEST_NOT_IMPLEMENTED      0x56
#define   IPMI_APP_SELFTEST_ERROR                0x57
#define   IPMI_APP_SELFTEST_FATAL_HW_ERROR       0x58

#define IPMI_NETFN_FIRMWARE 0x08
#define IPMI_NETFN_STORAGE 0x0a
#define   IPMI_READ_FRU_DATA 0x11
#define   IPMI_ADD_SEL_ENTRY 0x44
#define IPMI_NETFN_TRANSPORT 0x0c

#define IPMI_CMD_ACPI_POWERON 0x06

struct ipmi_rsp {
	uint8_t lun;
	uint8_t cmd;
	uint8_t completion_code;
} __packed;

/* Get Device ID */
struct ipmi_devid_rsp {
	struct ipmi_rsp resp;
	uint8_t device_id;
	uint8_t device_revision;
	uint8_t fw_rev1;
	uint8_t fw_rev2;
	uint8_t ipmi_version;
	uint8_t additional_device_support;
	uint8_t manufacturer_id[3];
	uint8_t product_id[2];
} __packed;

/* Get Self Test Results */
struct ipmi_selftest_rsp {
	struct ipmi_rsp resp;
	uint8_t result;
	uint8_t param;
} __packed;

struct device;

/*
 * Sends a command and reads its response. Input buffer is for payload, but
 * output includes `struct ipmi_rsp` as a header. Returns number of bytes copied
 * into the buffer or -1.
 */
int ipmi_message(int port, int netfn, int lun, int cmd,
		 const unsigned char *inmsg, int inlen,
		 unsigned char *outmsg, int outlen);

/* Run basic IPMI init functions in romstage from the provided PnP device,
 * returns CB_SUCCESS on success and CB_ERR if an error occurred. */
enum cb_err ipmi_premem_init(const uint16_t port, const uint16_t device);

int ipmi_get_device_id(const struct device *dev, struct ipmi_devid_rsp *rsp);

int ipmi_process_self_test_result(const struct device *dev);

void ipmi_bmc_version(uint8_t *ipmi_bmc_major_revision, uint8_t *ipmi_bmc_minor_revision);

#endif /* __IPMI_IF_H */
