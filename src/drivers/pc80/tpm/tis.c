/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * The code in this file has been heavily based on the article "Writing a TPM
 * Device Driver" published on http://ptgmedia.pearsoncmg.com and the
 * submission by Stefan Berger on Qemu-devel mailing list.
 *
 * One principal difference is that in the simplest config the other than 0
 * TPM localities do not get mapped by some devices (for instance, by
 * Infineon slb9635), so this driver provides access to locality 0 only.
 */

#include <commonlib/helpers.h>
#include <string.h>
#include <delay.h>
#include <device/mmio.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <device/device.h>
#include <console/console.h>
#include <security/tpm/tis.h>
#include <security/tpm/tss.h>
#include <device/pnp.h>
#include <drivers/tpm/tpm_ppi.h>
#include <timer.h>
#include "chip.h"

#define PREFIX "lpc_tpm: "

/* coreboot wrapper for TPM driver (start) */
#define	TPM_DEBUG(fmt, args...)		\
	if (CONFIG(DEBUG_TPM)) {		\
		printk(BIOS_DEBUG, PREFIX);		\
		printk(BIOS_DEBUG, fmt, ##args);	\
	}
#define TPM_DEBUG_IO_READ(reg_, val_) \
	TPM_DEBUG("Read reg 0x%x returns 0x%x\n", (reg_), (val_))
#define TPM_DEBUG_IO_WRITE(reg_, val_) \
	TPM_DEBUG("Write reg 0x%x with 0x%x\n", (reg_), (val_))
#define printf(x...) printk(BIOS_ERR, x)

/* coreboot wrapper for TPM driver (end) */

/* the macro accepts the locality value, but only locality 0 is operational */
#define TIS_REG(LOCALITY, REG) \
	(void *)(uintptr_t)(CONFIG_TPM_TIS_BASE_ADDRESS + (LOCALITY << 12) + REG)

/* hardware registers' offsets */
#define TIS_REG_ACCESS                 0x0
#define TIS_REG_INT_ENABLE             0x8
#define TIS_REG_INT_VECTOR             0xc
#define TIS_REG_INT_STATUS             0x10
#define TIS_REG_INTF_CAPABILITY        0x14
#define TIS_REG_STS                    0x18
#define TIS_REG_BURST_COUNT            0x19
#define TIS_REG_DATA_FIFO              0x24
#define TIS_REG_INTF_ID                0x30
#define TIS_REG_DID_VID                0xf00
#define TIS_REG_RID                    0xf04

/* Some registers' bit field definitions */
#define TIS_STS_VALID                  (1 << 7) /* 0x80 */
#define TIS_STS_COMMAND_READY          (1 << 6) /* 0x40 */
#define TIS_STS_TPM_GO                 (1 << 5) /* 0x20 */
#define TIS_STS_DATA_AVAILABLE         (1 << 4) /* 0x10 */
#define TIS_STS_EXPECT                 (1 << 3) /* 0x08 */
#define TIS_STS_RESPONSE_RETRY         (1 << 1) /* 0x02 */

#define TIS_ACCESS_TPM_REG_VALID_STS   (1 << 7) /* 0x80 */
#define TIS_ACCESS_ACTIVE_LOCALITY     (1 << 5) /* 0x20 */
#define TIS_ACCESS_BEEN_SEIZED         (1 << 4) /* 0x10 */
#define TIS_ACCESS_SEIZE               (1 << 3) /* 0x08 */
#define TIS_ACCESS_PENDING_REQUEST     (1 << 2) /* 0x04 */
#define TIS_ACCESS_REQUEST_USE         (1 << 1) /* 0x02 */
#define TIS_ACCESS_TPM_ESTABLISHMENT   (1 << 0) /* 0x01 */

/*
 * Error value returned if a tpm register does not enter the expected state
 * after continuous polling. No actual TPM register reading ever returns ~0,
 * so this value is a safe error indication to be mixed with possible status
 * register values.
 */
#define TPM_TIMEOUT_ERR			(~0)

/* Error value returned on various TPM driver errors */
#define TPM_DRIVER_ERR		(~0)

 /* 1 second is plenty for anything TPM does.*/
#define MAX_DELAY_US	USECS_PER_SEC

enum tpm_family {
	TPM_UNKNOWN = 0,
	TPM_1 = 1,
	TPM_2 = 2,
};

/*
 * Structures defined below allow creating descriptions of TPM vendor/device
 * ID information for run time discovery.
 */
struct device_name {
	u16 dev_id;
	enum tpm_family family;
	const char *const dev_name;
};

struct vendor_name {
	u16 vendor_id;
	const char *vendor_name;
	const struct device_name *dev_names;
};

static const struct device_name atmel_devices[] = {
	{0x3204, TPM_1, "AT97SC3204"},
	{0xffff}
};

static const struct device_name infineon_devices[] = {
	{0x000b, TPM_1, "SLB9635 TT 1.2"},
	{0x001a, TPM_1, "SLB9660 TT 1.2"},
	{0x001b, TPM_1, "SLB9670 TT 1.2"},
	{0x001a, TPM_2, "SLB9665 TT 2.0"},
	{0x001b, TPM_2, "SLB9670 TT 2.0"},
	{0xffff}
};

static const struct device_name nuvoton_devices[] = {
	{0x00fe, TPM_1, "NPCT420AA V2"},
	{0xffff}
};

static const struct device_name stmicro_devices[] = {
	{0x0000, TPM_1, "ST33ZP24" },
	{0xffff}
};

static const struct device_name swtpm_devices[] = {
	{0x0001, TPM_1, "SwTPM 1.2" },
	{0x0001, TPM_2, "SwTPM 2.0" },
	{0xffff}
};

static const struct vendor_name vendor_names[] = {
	{0x1114, "Atmel", atmel_devices},
	{0x15d1, "Infineon", infineon_devices},
	{0x1050, "Nuvoton", nuvoton_devices},
	{0x1014, "TPM Emulator", swtpm_devices},
	{0x104a, "ST Microelectronics", stmicro_devices},
};

/*
 * Cached vendor/device ID pair to indicate that the device has been already
 * discovered
 */
static u32 vendor_dev_id;

static inline u8 tpm_read_status(int locality)
{
	u8 value = read8(TIS_REG(locality, TIS_REG_STS));
	TPM_DEBUG_IO_READ(TIS_REG_STS, value);
	return value;
}

static inline void tpm_write_status(u8 sts, int locality)
{
	TPM_DEBUG_IO_WRITE(TIS_REG_STS, sts);
	write8(TIS_REG(locality, TIS_REG_STS), sts);
}

static inline u8 tpm_read_data(int locality)
{
	u8 value = read8(TIS_REG(locality, TIS_REG_DATA_FIFO));
	TPM_DEBUG_IO_READ(TIS_REG_DATA_FIFO, value);
	return value;
}

static inline void tpm_write_data(u8 data, int locality)
{
	TPM_DEBUG_IO_WRITE(TIS_REG_DATA_FIFO, data);
	write8(TIS_REG(locality, TIS_REG_DATA_FIFO), data);
}

static inline u16 tpm_read_burst_count(int locality)
{
	u16 count;
	count = read8(TIS_REG(locality, TIS_REG_BURST_COUNT));
	count |= read8(TIS_REG(locality, TIS_REG_BURST_COUNT + 1)) << 8;
	TPM_DEBUG_IO_READ(TIS_REG_BURST_COUNT, count);
	return count;
}

static inline u8 tpm_read_access(int locality)
{
	u8 value = read8(TIS_REG(locality, TIS_REG_ACCESS));
	TPM_DEBUG_IO_READ(TIS_REG_ACCESS, value);
	return value;
}

static inline void tpm_write_access(u8 data, int locality)
{
	TPM_DEBUG_IO_WRITE(TIS_REG_ACCESS, data);
	write8(TIS_REG(locality, TIS_REG_ACCESS), data);
}

static inline u32 tpm_read_intf_cap(int locality)
{
	u32 value = be32_to_cpu(read32(TIS_REG(locality, TIS_REG_INTF_CAPABILITY)));
	TPM_DEBUG_IO_READ(TIS_REG_INTF_CAPABILITY, value);
	return value;
}

static inline u32 tpm_read_intf_id(int locality)
{
	u32 value = be32_to_cpu(read32(TIS_REG(locality, TIS_REG_INTF_ID)));
	TPM_DEBUG_IO_READ(TIS_REG_INTF_ID, value);
	return value;
}

static inline u32 tpm_read_did_vid(int locality)
{
	u32 value = read32(TIS_REG(locality, TIS_REG_DID_VID));
	TPM_DEBUG_IO_READ(TIS_REG_DID_VID, value);
	return value;
}

static inline void tpm_write_int_vector(int vector, int locality)
{
	TPM_DEBUG_IO_WRITE(TIS_REG_INT_VECTOR, vector);
	write8(TIS_REG(locality, TIS_REG_INT_VECTOR), vector & 0xf);
}

static inline u8 tpm_read_int_vector(int locality)
{
	u8 value = read8(TIS_REG(locality, TIS_REG_INT_VECTOR));
	TPM_DEBUG_IO_READ(TIS_REG_INT_VECTOR, value);
	return value;
}

static inline void tpm_write_int_polarity(int polarity, int locality)
{
	/* Set polarity and leave all other bits at 0 */
	u32 value = (polarity & 0x3) << 3;
	TPM_DEBUG_IO_WRITE(TIS_REG_INT_ENABLE, value);
	write32(TIS_REG(locality, TIS_REG_INT_ENABLE), value);
}

static inline u32 tpm_read_int_polarity(int locality)
{
	/* Get polarity and leave all other bits */
	u32 value = read8(TIS_REG(locality, TIS_REG_INT_ENABLE));
	value = (value >> 3) & 0x3;
	TPM_DEBUG_IO_READ(TIS_REG_INT_ENABLE, value);
	return value;
}

/*
 * tis_wait_sts()
 *
 * Wait for at most a second for a status to change its state to match the
 * expected state. Normally the transition happens within microseconds.
 *
 * @locality - locality
 * @mask - bitmask for the bitfield(s) to watch
 * @expected - value the field(s) are supposed to be set to
 *
 * Returns 0 on success or TPM_TIMEOUT_ERR on timeout.
 */
static int tis_wait_sts(int locality, u8 mask, u8 expected)
{
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, MAX_DELAY_US);
	do {
		u8 value = tpm_read_status(locality);
		if ((value & mask) == expected)
			return 0;
		udelay(1);
	} while (!stopwatch_expired(&sw));
	return TPM_TIMEOUT_ERR;
}

static inline int tis_wait_ready(int locality)
{
	return tis_wait_sts(locality, TIS_STS_COMMAND_READY,
	                    TIS_STS_COMMAND_READY);
}

static inline int tis_wait_valid(int locality)
{
	return tis_wait_sts(locality, TIS_STS_VALID, TIS_STS_VALID);
}

static inline int tis_wait_valid_data(int locality)
{
	const u8 has_data = TIS_STS_DATA_AVAILABLE | TIS_STS_VALID;
	return tis_wait_sts(locality, has_data, has_data);
}

static inline int tis_has_valid_data(int locality)
{
	const u8 has_data = TIS_STS_DATA_AVAILABLE | TIS_STS_VALID;
	return (tpm_read_status(locality) & has_data) == has_data;
}

static inline int tis_expect_data(int locality)
{
	return !!(tpm_read_status(locality) & TIS_STS_EXPECT);
}

/*
 * tis_wait_access()
 *
 * Wait for at most a second for a access to change its state to match the
 * expected state. Normally the transition happens within microseconds.
 *
 * @locality - locality
 * @mask - bitmask for the bitfield(s) to watch
 * @expected - value the field(s) are supposed to be set to
 *
 * Returns 0 on success or TPM_TIMEOUT_ERR on timeout.
 */
static int tis_wait_access(int locality, u8 mask, u8 expected)
{
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, MAX_DELAY_US);
	do {
		u8 value = tpm_read_access(locality);
		if ((value & mask) == expected)
			return 0;
		udelay(1);
	} while (!stopwatch_expired(&sw));
	return TPM_TIMEOUT_ERR;
}

static inline int tis_wait_received_access(int locality)
{
	return tis_wait_access(locality, TIS_ACCESS_ACTIVE_LOCALITY,
	                       TIS_ACCESS_ACTIVE_LOCALITY);
}

static inline int tis_has_access(int locality)
{
	return !!(tpm_read_access(locality) & TIS_ACCESS_ACTIVE_LOCALITY);
}

static inline void tis_request_access(int locality)
{
	tpm_write_access(TIS_ACCESS_REQUEST_USE, locality);
}

/*
 * PC Client Specific TPM Interface Specification section 11.2.12:
 *
 *  Software must be prepared to send two writes of a "1" to command ready
 *  field: the first to indicate successful read of all the data, thus
 *  clearing the data from the ReadFIFO and freeing the TPM's resources,
 *  and the second to indicate to the TPM it is about to send a new command.
 *
 * In practice not all TPMs behave the same so it is necessary to be
 * flexible when trying to set command ready.
 *
 * Returns 0 on success if the TPM is ready for transactions.
 * Returns TPM_TIMEOUT_ERR if the command ready bit does not get set.
 */
static int tis_command_ready(u8 locality)
{
	u32 status;

	/* 1st attempt to set command ready */
	tpm_write_status(TIS_STS_COMMAND_READY, locality);

	/* Wait for response */
	status = tpm_read_status(locality);

	/* Check if command ready is set yet */
	if (status & TIS_STS_COMMAND_READY)
		return 0;

	/* 2nd attempt to set command ready */
	tpm_write_status(TIS_STS_COMMAND_READY, locality);

	return tis_wait_ready(locality);
}

/*
 * Probe the TPM device and try determining its manufacturer/device name.
 *
 * Returns 0 on success (the device is found or was found during an earlier
 * invocation) or TPM_DRIVER_ERR if the device is not found.
 */
static u32 pc80_tis_probe(void)
{
	const char *device_name = NULL;
	const char *vendor_name = NULL;
	const struct device_name *dev;
	u32 didvid, intf_id;
	u16 vid, did;
	u8 locality = 0, intf_type;
	int i;
	enum tpm_family family;
	const char *family_str;

	if (vendor_dev_id)
		return 0;  /* Already probed. */

	didvid = tpm_read_did_vid(0);
	if (!didvid || (didvid == 0xffffffff)) {
		printf("%s: No TPM device found\n", __func__);
		return TPM_DRIVER_ERR;
	}

	intf_id = tpm_read_intf_id(locality);
	intf_type = (intf_id & 0xf);
	if (intf_type == 0xf) {
		u32 intf_cap = tpm_read_intf_cap(locality);
		u8 intf_version = (intf_cap >> 28) & 0x7;
		switch (intf_version) {
		case 0:
		case 2:
			family = TPM_1;
			break;
		case 3:
			family = TPM_2;
			break;
		default:
			printf("%s: Unexpected TPM interface version: %d\n", __func__,
			       intf_version);
			return TPM_DRIVER_ERR;
		}
	} else if (intf_type == 0) {
		family = TPM_2;
	} else {
		printf("%s: Unexpected TPM interface type: %d\n", __func__, intf_type);
		return TPM_DRIVER_ERR;
	}

	vendor_dev_id = didvid;

	vid = didvid & 0xffff;
	did = (didvid >> 16) & 0xffff;
	for (i = 0; i < ARRAY_SIZE(vendor_names); i++) {
		int j = 0;
		if (vid == vendor_names[i].vendor_id) {
			vendor_name = vendor_names[i].vendor_name;
		} else {
			continue;
		}
		dev = &vendor_names[i].dev_names[j];
		while (dev->dev_id != 0xffff) {
			if (dev->dev_id == did && dev->family == family) {
				device_name = dev->dev_name;
				break;
			}
			j++;
			dev = &vendor_names[i].dev_names[j];
		}
		break;
	}

	family_str = (family == TPM_1 ? "TPM 1.2" : "TPM 2.0");
	if (vendor_name == NULL) {
		printk(BIOS_INFO, "Found %s 0x%04x by 0x%04x\n", family_str, did, vid);
	} else if (device_name == NULL) {
		printk(BIOS_INFO, "Found %s 0x%04x by %s (0x%04x)\n", family_str, did,
		       vendor_name, vid);
	} else {
		printk(BIOS_INFO, "Found %s %s (0x%04x) by %s (0x%04x)\n", family_str,
		       device_name, did, vendor_name, vid);
	}

	return 0;
}

/*
 * tis_senddata()
 *
 * send the passed in data to the TPM device.
 *
 * @data - address of the data to send, byte by byte
 * @len - length of the data to send
 *
 * Returns 0 on success, TPM_DRIVER_ERR on error (in case the device does
 * not accept the entire command).
 */
static u32 tis_senddata(const u8 *const data, u32 len)
{
	u32 offset = 0;
	u16 burst = 0;
	u8 locality = 0;

	if (tis_wait_ready(locality)) {
		printf("%s:%d - failed to get 'command_ready' status\n",
		       __FILE__, __LINE__);
		return TPM_DRIVER_ERR;
	}
	burst = tpm_read_burst_count(locality);

	while (1) {
		unsigned int count;
		struct stopwatch sw;

		/* Wait till the device is ready to accept more data. */
		stopwatch_init_usecs_expire(&sw, MAX_DELAY_US);
		while (!burst) {
			if (stopwatch_expired(&sw)) {
				printf("%s:%d failed to feed %u bytes of %u\n",
				       __FILE__, __LINE__, len - offset, len);
				return TPM_DRIVER_ERR;
			}
			udelay(1);
			burst = tpm_read_burst_count(locality);
		}

		/*
		 * Calculate number of bytes the TPM is ready to accept in one
		 * shot.
		 *
		 * We want to send the last byte outside of the loop (hence
		 * the -1 below) to make sure that the 'expected' status bit
		 * changes to zero exactly after the last byte is fed into the
		 * FIFO.
		 */
		count = MIN(burst, len - offset - 1);
		while (count--)
			tpm_write_data(data[offset++], locality);

		if (tis_wait_valid(locality) || !tis_expect_data(locality)) {
			printf("%s:%d TPM command feed overflow\n",
			       __FILE__, __LINE__);
			return TPM_DRIVER_ERR;
		}

		burst = tpm_read_burst_count(locality);
		if ((offset == (len - 1)) && burst)
			/*
			 * We need to be able to send the last byte to the
			 * device, so burst size must be nonzero before we
			 * break out.
			 */
			break;
	}

	/* Send the last byte. */
	tpm_write_data(data[offset++], locality);

	/*
	 * Verify that TPM does not expect any more data as part of this
	 * command.
	 */
	if (tis_wait_valid(locality) || tis_expect_data(locality)) {
		printf("%s:%d unexpected TPM status 0x%x\n",
		       __FILE__, __LINE__, tpm_read_status(locality));
		return TPM_DRIVER_ERR;
	}

	/* OK, sitting pretty, let's start the command execution. */
	tpm_write_status(TIS_STS_TPM_GO, locality);

	return 0;
}

/*
 * tis_readresponse()
 *
 * read the TPM device response after a command was issued.
 *
 * @buffer - address where to read the response, byte by byte.
 * @len - pointer to the size of buffer
 *
 * On success stores the number of received bytes to len and returns 0. On
 * errors (misformatted TPM data or synchronization problems) returns
 * TPM_DRIVER_ERR.
 */
static u32 tis_readresponse(u8 *buffer, size_t *len)
{
	u16 burst_count;
	u32 offset = 0;
	u8 locality = 0;
	u32 expected_count = *len;
	int max_cycles = 0;

	/* Wait for the TPM to process the command */
	if (tis_wait_valid_data(locality)) {
		printf("%s:%d failed processing command\n", __FILE__, __LINE__);
		return TPM_DRIVER_ERR;
	}

	do {
		while ((burst_count = tpm_read_burst_count(locality)) == 0) {
			if (max_cycles++ == MAX_DELAY_US) {
				printf("%s:%d TPM stuck on read\n",
				       __FILE__, __LINE__);
				return TPM_DRIVER_ERR;
			}
			udelay(1);
		}

		max_cycles = 0;

		while (burst_count-- && (offset < expected_count)) {
			buffer[offset++] = tpm_read_data(locality);
			if (offset == 6) {
				/*
				 * We got the first six bytes of the reply,
				 * let's figure out how many bytes to expect
				 * total - it is stored as a 4 byte number in
				 * network order, starting with offset 2 into
				 * the body of the reply.
				 */
				u32 real_length;
				memcpy(&real_length,
				       buffer + 2,
				       sizeof(real_length));
				expected_count = be32_to_cpu(real_length);

				if ((expected_count < offset) ||
				    (expected_count > *len)) {
					printf("%s:%d bad response size %u\n",
					       __FILE__, __LINE__,
					       expected_count);
					return TPM_DRIVER_ERR;
				}
			}
		}

		/* Wait for the next portion */
		if (tis_wait_valid(locality)) {
			printf("%s:%d failed to read response\n",
			       __FILE__, __LINE__);
			return TPM_DRIVER_ERR;
		}

		if (offset == expected_count)
			break;	/* We got all we need */

		/*
		 * Certain TPMs seem to need some delay between tis_wait_valid()
		 * and tis_has_valid_data(), or some race-condition-related
		 * issue will occur.
		 */
		if (CONFIG(TPM_RDRESP_NEED_DELAY))
			udelay(10);

	} while (tis_has_valid_data(locality));

	/* * Make sure we indeed read all there was. */
	if (tis_has_valid_data(locality)) {
		printf("%s:%d wrong receive status: %x %u bytes left\n",
		       __FILE__, __LINE__, tpm_read_status(locality),
	               tpm_read_burst_count(locality));
		return TPM_DRIVER_ERR;
	}

	/* Tell the TPM that we are done. */
	if (tis_command_ready(locality) == TPM_TIMEOUT_ERR)
		return TPM_DRIVER_ERR;

	*len = offset;
	return 0;
}

/*
 * Requests access to locality 0 for the caller.
 *
 * Returns 0 on success, TPM_DRIVER_ERR on failure.
 */
static int pc80_tis_open(void)
{
	u8 locality = 0; /* we use locality zero for everything */

	if (!tis_has_access(locality)) {
		/* request access to locality */
		tis_request_access(locality);

		/* did we get a lock? */
		if (tis_wait_received_access(locality)) {
			printf("%s:%d - failed to lock locality %u\n",
			__FILE__, __LINE__, locality);
			return TPM_DRIVER_ERR;
		}

		/* Certain TPMs seem to need some delay here or they hang... */
		udelay(10);
	}

	if (tis_command_ready(locality) == TPM_TIMEOUT_ERR)
		return TPM_DRIVER_ERR;

	return 0;
}

/*
 * Send the requested data to the TPM and then try to get its response
 *
 * @sendbuf - buffer of the data to send
 * @send_size size of the data to send
 * @recvbuf - memory to save the response to
 * @recv_len - pointer to the size of the response buffer
 *
 * Returns 0 on success (and places the number of response bytes at recv_len)
 * or TPM_DRIVER_ERR on failure.
 */
static int pc80_tpm_sendrecv(const uint8_t *sendbuf, size_t send_size,
			     uint8_t *recvbuf, size_t *recv_len)
{
	if (tis_senddata(sendbuf, send_size)) {
		printf("%s:%d failed sending data to TPM\n",
		       __FILE__, __LINE__);
		return TPM_DRIVER_ERR;
	}

	return tis_readresponse(recvbuf, recv_len);
}

/*
 * tis_probe()
 *
 * Probe for the TPM device and set it up for use within locality 0. Returns
 * pointer to send-receive function on success or NULL on failure.
 */
tis_sendrecv_fn tis_probe(void)
{
	if (pc80_tis_probe())
		return NULL;

	if (pc80_tis_open())
		return NULL;

	return &pc80_tpm_sendrecv;
}

/*
 * tis_setup_interrupt()
 *
 * Set up the interrupt vector and polarity for locality 0 and
 * disable all interrupts so they are unused in firmware but can
 * be enabled by the OS.
 *
 * The values used here must match what is passed in the TPM ACPI
 * device if ACPI is used on the platform.
 *
 * @vector - TPM interrupt vector
 * @polarity - TPM interrupt polarity
 *
 * Returns 0 on success, TPM_DRIVER_ERR on failure.
 */
static int tis_setup_interrupt(int vector, int polarity)
{
	u8 locality = 0;

	if (tlcl_lib_init())
		return TPM_DRIVER_ERR;

	/* Set TPM interrupt vector */
	tpm_write_int_vector(vector, locality);

	/* Set TPM interrupt polarity and disable interrupts */
	tpm_write_int_polarity(polarity, locality);

	return 0;
}

static void lpc_tpm_read_resources(struct device *dev)
{
	/* Static 5K memory region specified in Kconfig */
	mmio_resource_kb(dev, 0, CONFIG_TPM_TIS_BASE_ADDRESS >> 10, 0x5000 >> 10);
}

static void lpc_tpm_set_resources(struct device *dev)
{
	tpm_config_t *config = (tpm_config_t *)dev->chip_info;
	DEVTREE_CONST struct resource *res;

	for (res = dev->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_ASSIGNED))
			continue;

		if (res->flags & IORESOURCE_IRQ) {
			/* Set interrupt vector */
			tis_setup_interrupt((int)res->base,
					    config->irq_polarity);
		} else {
			continue;
		}

#if !DEVTREE_EARLY
		res->flags |= IORESOURCE_STORED;
		report_resource_stored(dev, res, " <tpm>");
#endif
	}
}

#if CONFIG(HAVE_ACPI_TABLES)
static void lpc_tpm_fill_ssdt(const struct device *dev)
{
	/* Windows 11 requires the following path for TPM to be detected */
	const char *path = "\\_SB_.PCI0";

	/* Device */
	acpigen_write_scope(path);
	acpigen_write_device(acpi_device_name(dev));

	if (CONFIG(TPM2)) {
		acpigen_write_name_string("_HID", "MSFT0101");
		acpigen_write_name_string("_CID", "MSFT0101");
	} else {
		acpigen_write_name("_HID");
		acpigen_emit_eisaid("PNP0C31");

		acpigen_write_name("_CID");
		acpigen_emit_eisaid("PNP0C31");
	}

	acpi_device_write_uid(dev);

	u32 did_vid = tpm_read_did_vid(0);
	if (did_vid > 0 && did_vid < 0xffffffff)
		acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);
	else
		acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_OFF);

	u16 port = dev->path.pnp.port;

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpigen_write_mem32fixed(1, CONFIG_TPM_TIS_BASE_ADDRESS, 0x5000);
	if (port)
		acpigen_write_io16(port, port, 1, 2, 1);

	if (CONFIG_TPM_PIRQ) {
		/*
		 * PIRQ: Update interrupt vector with configured PIRQ
		 * Active-Low Level-Triggered Shared
		 */
		struct acpi_irq tpm_irq_a = ACPI_IRQ_LEVEL_LOW(CONFIG_TPM_PIRQ);
		acpi_device_write_interrupt(&tpm_irq_a);
	} else if (tpm_read_int_vector(0) > 0) {
		u8 int_vec = tpm_read_int_vector(0);
		u8 int_pol = tpm_read_int_polarity(0);
		struct acpi_irq tpm_irq = ACPI_IRQ_LEVEL_LOW(int_vec);

		if (int_pol & 1)
			tpm_irq.polarity = ACPI_IRQ_ACTIVE_LOW;
		else
			tpm_irq.polarity = ACPI_IRQ_ACTIVE_HIGH;

		if (int_pol & 2)
			tpm_irq.mode = ACPI_IRQ_EDGE_TRIGGERED;
		else
			tpm_irq.mode = ACPI_IRQ_LEVEL_TRIGGERED;

		acpi_device_write_interrupt(&tpm_irq);
	}


	acpigen_write_resourcetemplate_footer();

	if (!CONFIG(CHROMEOS))
		tpm_ppi_acpi_fill_ssdt(dev);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

#if !DEVTREE_EARLY
	printk(BIOS_INFO, "%s.%s: %s %s\n", path, acpi_device_name(dev),
	       dev->chip_ops->name, dev_path(dev));
#endif
}

static const char *lpc_tpm_acpi_name(const struct device *dev)
{
	return "TPM";
}
#endif

static struct device_operations lpc_tpm_ops = {
	.read_resources   = lpc_tpm_read_resources,
	.set_resources    = lpc_tpm_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = lpc_tpm_acpi_name,
	.acpi_fill_ssdt   = lpc_tpm_fill_ssdt,
#endif
};

static struct pnp_info pnp_dev_info[] = {
	{ .flags = PNP_IRQ0 }
};

static void enable_dev(struct device *dev)
{
	if (CONFIG(TPM))
		pnp_enable_devices(dev, &lpc_tpm_ops,
			ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations drivers_pc80_tpm_ops = {
	CHIP_NAME("LPC TPM")
	.enable_dev = enable_dev
};
