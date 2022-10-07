/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <stddef.h>

typedef struct {
	const unsigned char subclass_id;
	const char *subclass_name;
} PCI_SUBCLASS;

typedef struct {
	const unsigned char class_id;
	const PCI_SUBCLASS *subclass_list;
	const unsigned int subclass_entries;
	const char *class_name;
} PCI_CLASS;

static const PCI_SUBCLASS unclassified[] = {
	{ 0x00, "Non-VGA device" },
	{ 0x01, "VGA compatible device" }
};

static const PCI_SUBCLASS mass_storage[] = {
	{ 0x00, "SCSI storage controller" },
	{ 0x01, "IDE interface" },
	{ 0x02, "Floppy disk controller" },
	{ 0x03, "IPI bus controller" },
	{ 0x04, "RAID bus controller" },
	{ 0x05, "ATA controller" },
	{ 0x06, "SATA controller" },
	{ 0x07, "Serial Attached SCSI controller" },
	{ 0x08, "Non-Volatile memory controller" },
	{ 0x09, "Universal Flash Storage controller" },
	{ 0x80, "Mass storage controller" }
};

static const PCI_SUBCLASS network[] = {
	{ 0x00, "Ethernet controller" },
	{ 0x01, "Token ring network controller" },
	{ 0x02, "FDDI network controller" },
	{ 0x03, "ATM network controller" },
	{ 0x04, "ISDN controller" },
	{ 0x05, "WorldFip controller" },
	{ 0x06, "PICMG controller" },
	{ 0x07, "InfiniBand Controller" },
	{ 0x08, "Host fabric controller" },
	{ 0x80, "Network controller" }
};

static const PCI_SUBCLASS display[] = {
	{ 0x00, "VGA compatible controller" },
	{ 0x01, "XGA compatible controller" },
	{ 0x02, "3D controller" },
	{ 0x80, "Display controller" }
};

static const PCI_SUBCLASS multimedia[] = {
	{ 0x00, "Multimedia video controller" },
	{ 0x01, "Multimedia audio controller" },
	{ 0x02, "Computer telephony device" },
	{ 0x03, "Audio device" },
	{ 0x80, "Multimedia controller" }
};

static const PCI_SUBCLASS memory[] = {
	{ 0x00, "RAM memory" },
	{ 0x01, "FLASH memory" },
	{ 0x80, "Memory controller" }
};

static const PCI_SUBCLASS bridge[] = {
	{ 0x00, "Host bridge" },
	{ 0x01, "ISA bridge" },
	{ 0x02, "EISA bridge" },
	{ 0x03, "MicroChannel bridge" },
	{ 0x04, "PCI bridge" },
	{ 0x05, "PCMCIA bridge" },
	{ 0x06, "NuBus bridge" },
	{ 0x07, "CardBus bridge" },
	{ 0x08, "RACEway bridge" },
	{ 0x09, "Semi-transparent PCI-to-PCI bridge" },
	{ 0x0a, "InfiniBand to PCI host bridge" },
	{ 0x0b, "Advanced Switching to PCI host bridge" },
	{ 0x80, "Bridge" }
};

static const PCI_SUBCLASS communication[] = {
	{ 0x00, "Serial controller" },
	{ 0x01, "Parallel controller" },
	{ 0x02, "Multiport serial controller" },
	{ 0x03, "Modem" },
	{ 0x04, "GPIB controller" },
	{ 0x05, "Smard Card controller" },
	{ 0x80, "Communication controller" }
};

static const PCI_SUBCLASS generic[] = {
	{ 0x00, "PIC" },
	{ 0x01, "DMA controller" },
	{ 0x02, "Timer" },
	{ 0x03, "RTC" },
	{ 0x04, "PCI Hot-plug controller" },
	{ 0x05, "SD Host controller" },
	{ 0x06, "IOMMU" },
	{ 0x07, "Root Complex Event Collector" },
	{ 0x80, "System peripheral" }
};

static const PCI_SUBCLASS input_device[] = {
	{ 0x00, "Keyboard controller" },
	{ 0x01, "Digitizer Pen" },
	{ 0x02, "Mouse controller" },
	{ 0x03, "Scanner controller" },
	{ 0x04, "Gameport controller" },
	{ 0x80, "Input device controller" }
};

static const PCI_SUBCLASS docking_station[] = {
	{ 0x00, "Generic Docking Station" },
	{ 0x80, "Docking Station" }
};

static const PCI_SUBCLASS processor[] = {
	{ 0x00, "386" },
	{ 0x01, "486" },
	{ 0x02, "Pentium" },
	{ 0x10, "Alpha" },
	{ 0x20, "Power PC" },
	{ 0x30, "MIPS" },
	{ 0x40, "Co-processor" },
	{ 0x80, "Processor" }
};

static const PCI_SUBCLASS serial_bus[] = {
	{ 0x00, "FireWire (IEEE 1394)" },
	{ 0x01, "ACCESS Bus" },
	{ 0x02, "SSA" },
	{ 0x03, "USB controller" },
	{ 0x04, "Fibre Channel" },
	{ 0x05, "SMBus" },
	{ 0x06, "InfiniBand" },
	{ 0x07, "IPMI SMIC interface" },
	{ 0x08, "SERCOS interface" },
	{ 0x09, "CANBUS" },
	{ 0x0a, "MIPI I3C SM Host Controller Interface" },
	{ 0x80, "Serial Bus Controller" }
};

static const PCI_SUBCLASS wireless[] = {
	{ 0x00, "IRDA controller" },
	{ 0x01, "Consumer IR controller" },
	{ 0x10, "RF controller" },
	{ 0x11, "Bluetooth" },
	{ 0x12, "Broadband" },
	{ 0x20, "802.1a controller" },
	{ 0x21, "802.1b controller" },
	{ 0x40, "Cellular controller/modem" },
	{ 0x41, "Cellular controller/modem plus Ethernet (802.11)" },
	{ 0x80, "Wireless controller" }
};

static const PCI_SUBCLASS intellegient_controller[] = {
	{ 0x00, "I2O" }
};

static const PCI_SUBCLASS satellite_controller[] = {
	{ 0x01, "Satellite TV controller" },
	{ 0x02, "Satellite audio communication controller" },
	{ 0x03, "Satellite voice communication controller" },
	{ 0x04, "Satellite data communication controller" }
};

static const PCI_SUBCLASS encryption[] = {
	{ 0x00, "Network and computing encryption device" },
	{ 0x10, "Entertainment encryption device" },
	{ 0x80, "Encryption controller" }
};

static const PCI_SUBCLASS signal_processing[] = {
	{ 0x00, "DPIO module" },
	{ 0x01, "Performance counters" },
	{ 0x10, "Communication synchronizer" },
	{ 0x20, "Signal processing management" },
	{ 0x80, "Signal processing controller" }
};

static const PCI_CLASS class_list[] = {
	{ 0x00, &unclassified[0], ARRAY_SIZE(unclassified),
		"Unclassified device" },
	{ 0x01, &mass_storage[0], ARRAY_SIZE(mass_storage), "Mass storage" },
	{ 0x02, &network[0], ARRAY_SIZE(network), "Network" },
	{ 0x03, &display[0], ARRAY_SIZE(display), "Display" },
	{ 0x04, &multimedia[0], ARRAY_SIZE(multimedia), "Multimedia" },
	{ 0x05, &memory[0], ARRAY_SIZE(memory), "Memory" },
	{ 0x06, &bridge[0], ARRAY_SIZE(bridge), "Bridge" },
	{ 0x07, &communication[0], ARRAY_SIZE(communication), "Communication" },
	{ 0x08, &generic[0], ARRAY_SIZE(generic), "Generic system peripheral" },
	{ 0x09, &input_device[0], ARRAY_SIZE(input_device), "Input device" },
	{ 0x0a, &docking_station[0], ARRAY_SIZE(docking_station),
		"Docking station" },
	{ 0x0b, &processor[0], ARRAY_SIZE(processor), "Processor" },
	{ 0x0c, &serial_bus[0], ARRAY_SIZE(serial_bus), "Serial bus" },
	{ 0x0d, &wireless[0], ARRAY_SIZE(wireless), "Wireless" },
	{ 0x0e, &intellegient_controller[0],
			ARRAY_SIZE(intellegient_controller),
			"Intelligent controller" },
	{ 0x0f, &satellite_controller[0], ARRAY_SIZE(satellite_controller),
			"Satellite communications" },
	{ 0x10, &encryption[0], ARRAY_SIZE(encryption), "Encryption" },
	{ 0x11, &signal_processing[0], ARRAY_SIZE(signal_processing),
			"Signal processing" },
	{ 0xff, NULL, 0, "Unassigned class" }
};
static const unsigned int class_entries = ARRAY_SIZE(class_list);

static const PCI_CLASS *get_pci_class_entry(struct device *dev)
{
	unsigned char class;
	const PCI_CLASS *class_entry;
	const PCI_CLASS *class_list_end;

	/* Get the PCI device class */
	class = pci_read_config8(dev, PCI_CLASS_DEVICE+1);

	/* Locate the class entry */
	class_entry = &class_list[0];
	class_list_end = &class_entry[class_entries];
	while (class_list_end > class_entry) {
		if (class_entry->class_id == class)
			return class_entry;
		class_entry += 1;
	}
	return NULL;
}

const char *get_pci_class_name(struct device *dev)
{
	const PCI_CLASS *class_entry;

	class_entry = get_pci_class_entry(dev);
	return class_entry ? class_entry->class_name : "???";
}

const char *get_pci_subclass_name(struct device *dev)
{
	const PCI_CLASS *class_entry;
	unsigned char subclass;
	const PCI_SUBCLASS *subclass_entry;
	const PCI_SUBCLASS *subclass_list_end;
	const char *subclass_name;

	/* Get the PCI device subclass */
	subclass = pci_read_config8(dev, PCI_CLASS_DEVICE);

	/* Locate the subclass name */
	subclass_name = "???";
	class_entry = get_pci_class_entry(dev);
	subclass_entry = class_entry ? class_entry->subclass_list : NULL;
	if (subclass_entry != NULL) {
		subclass_list_end =
			&subclass_entry[class_entry->subclass_entries];
		while (subclass_list_end > subclass_entry) {
			if (subclass_entry->subclass_id == subclass) {
				subclass_name = subclass_entry->subclass_name;
				break;
			}
			subclass_entry += 1;
		}
	}
	return subclass_name;
}
