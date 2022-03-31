/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <assert.h>
#include <cbmem.h>
#include <commonlib/sdhci.h>
#include <commonlib/storage.h>
#include <console/console.h>
#include <lib.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/storage_test.h>
#include <timer.h>
#include <string.h>

#if CONFIG(STORAGE_LOG)
struct log_entry log[LOG_ENTRIES];
uint8_t log_index;
int log_full;
long log_start_time;
#endif

static uint8_t drivers_storage[256];

#define STORAGE_DEBUG  BIOS_DEBUG
#define LOG_DEBUG  (CONFIG(STORAGE_LOG) ? STORAGE_DEBUG : BIOS_NEVER)

#ifdef __SIMPLE_DEVICE__
uint32_t storage_test_init(pci_devfn_t dev, uint32_t *previous_bar,
	uint16_t *previous_command)
#else
uint32_t storage_test_init(struct device *dev, uint32_t *previous_bar,
	uint16_t *previous_command)
#endif
{
	uint32_t bar;

	/* Display the vendor/device IDs */
	printk(LOG_DEBUG, "Vendor ID: 0x%04x, Device ID: 0x%04x\n",
		pci_read_config16(dev, PCI_VENDOR_ID),
		pci_read_config16(dev, PCI_DEVICE_ID));

	/* Set the temporary base address */
	bar = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	*previous_bar = bar;
	bar &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
	if (!bar) {
		bar = SD_BASE_ADDRESS;
		pci_write_config32(dev, PCI_BASE_ADDRESS_0, bar);
	}

	/* Enable the SD/MMC controller */
	*previous_command = pci_read_config16(dev, PCI_COMMAND);
	pci_write_config16(dev, PCI_COMMAND, *previous_command
		| PCI_COMMAND_MEMORY);

	/* Return the controller address */
	return bar;
}

#ifdef __SIMPLE_DEVICE__
void storage_test_complete(pci_devfn_t dev, uint32_t previous_bar,
	uint16_t previous_command)
#else
void storage_test_complete(struct device *dev, uint32_t previous_bar,
	uint16_t previous_command)
#endif
{
	pci_write_config16(dev, PCI_COMMAND, previous_command);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, previous_bar);
}

#if !ENV_BOOTBLOCK
static void display_log(void)
{
	/* Determine the array bounds */
	if (CONFIG(STORAGE_LOG)) {
		long delta;
		uint8_t end;
		uint8_t index;
		uint8_t start;

		end = log_index;
		start = log_full ? log_index : 0;
		for (index = start; (log_full || (index != end)); index++) {
			log_full = 0;
			delta = log[index].time.microseconds - log_start_time;
			printk(BIOS_DEBUG, "%3ld.%03ld mSec, cmd: %2d 0x%08x%s",
				delta / 1000, delta % 1000,
				log[index].cmd.cmdidx,
				log[index].cmd.cmdarg,
				log[index].cmd_issued ? "" : "(not issued)");
			if (log[index].response_entries == 1)
				printk(BIOS_DEBUG, ", rsp: 0x%08x",
					log[index].response[0]);
			else if (log[index].response_entries == 4)
				printk(BIOS_DEBUG,
					", rsp: 0x%08x.%08x.%08x.%08x",
					log[index].response[3],
					log[index].response[2],
					log[index].response[1],
					log[index].response[0]);
			printk(BIOS_DEBUG, ", ret: %d\n", log[index].ret);
		}
	}
}

void sdhc_log_command(struct mmc_command *cmd)
{
	if (CONFIG(STORAGE_LOG)) {
		timer_monotonic_get(&log[log_index].time);
		log[log_index].cmd = *cmd;
		log[log_index].cmd_issued = 0;
		log[log_index].response_entries = 0;
		if ((log_index == 0) && (!log_full))
			log_start_time = log[0].time.microseconds;
	}
}

void sdhc_log_command_issued(void)
{
	if (CONFIG(STORAGE_LOG)) {
		log[log_index].cmd_issued = 1;
	}
}

void sdhc_log_response(uint32_t entries, uint32_t *response)
{
	unsigned int entry;

	if (CONFIG(STORAGE_LOG)) {
		log[log_index].response_entries = entries;
		for (entry = 0; entry < entries; entry++)
			log[log_index].response[entry] = response[entry];
	}
}

void sdhc_log_ret(int ret)
{
	if (CONFIG(STORAGE_LOG)) {
		log[log_index].ret = ret;
		if (++log_index == 0)
			log_full = 1;
	}
}

void storage_test(uint32_t bar, int full_initialization)
{
	uint64_t blocks_read;
	uint8_t buffer[512];
	int err;
	struct storage_media *media;
	const char *name;
	unsigned int partition;
	unsigned int previous_partition;
	struct sdhci_ctrlr *sdhci_ctrlr;

	ASSERT(sizeof(struct sdhci_ctrlr) <= sizeof(drivers_storage));

	/* Get the structure addresses */
	media = NULL;
	if (ENV_CREATES_CBMEM)
		media = (struct storage_media *)drivers_storage;
	else
		media = cbmem_find(CBMEM_ID_STORAGE_DATA);
	sdhci_ctrlr = (void *)(((uintptr_t)(media + 1) + 0x7) & ~7);
	media->ctrlr = (struct sd_mmc_ctrlr *)sdhci_ctrlr;
	sdhci_ctrlr->ioaddr = (void *)bar;

	/* Initialize the controller */
	if (!full_initialization) {
		/* Perform fast initialization */
		sdhci_update_pointers(sdhci_ctrlr);
		sdhci_display_setup(sdhci_ctrlr);
		storage_display_setup(media);
	} else {
		/* Initialize the log */
		if (CONFIG(STORAGE_LOG)) {
			log_index = 0;
			log_full = 0;
		}

		printk(LOG_DEBUG, "Initializing the SD/MMC controller\n");
		err = sdhci_controller_init(sdhci_ctrlr, (void *)bar);
		if (err) {
			display_log();
			printk(BIOS_ERR, "Controller failed to initialize, err = %d\n",
				err);
			return;
		}

		/* Initialize the SD/MMC/eMMC card or device */
		printk(LOG_DEBUG, "Initializing the device\n");
		err = storage_setup_media(media, &sdhci_ctrlr->sd_mmc_ctrlr);
		if (err) {
			display_log();
			printk(BIOS_ERR, "Device failed to initialize, err = %d\n",
				err);
			return;
		}
		display_log();
	}

	/* Save the current partition */
	previous_partition = storage_get_current_partition(media);

	/* Read block 0 from each partition */
	for (partition = 0; partition < ARRAY_SIZE(media->capacity);
		partition++) {
		if (media->capacity[partition] == 0)
			continue;
		name = storage_partition_name(media, partition);
		printk(STORAGE_DEBUG, "%s%sReading block 0\n", name,
			name[0] ? ": " : "");
		err = storage_set_partition(media, partition);
		if (err)
			continue;
		blocks_read = storage_block_read(media, 0, 1, &buffer);
		if (blocks_read)
			hexdump(buffer, sizeof(buffer));
	}

	/* Restore the previous partition */
	storage_set_partition(media, previous_partition);
}
#endif

static void copy_storage_structures(int is_recovery)
{
	struct storage_media *media;
	struct sdhci_ctrlr *sdhci_ctrlr;
	size_t size = sizeof(drivers_storage);

	/* Locate the data structures in CBMEM */
	media = cbmem_add(CBMEM_ID_STORAGE_DATA, size);
	ASSERT(media != NULL);
	sdhci_ctrlr = (void *)(((uintptr_t)(media + 1) + 0x7) & ~7);

	/* Migrate the data into CBMEM */
	memcpy(media, drivers_storage, size);
	media->ctrlr = &sdhci_ctrlr->sd_mmc_ctrlr;
}

CBMEM_CREATION_HOOK(copy_storage_structures);
