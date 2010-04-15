/*
 * This file is part of the coreboot project.
 *
 * (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

static void print_debug_pci_dev(unsigned dev)
{
	print_debug("PCI: ");
	print_debug_hex8((dev >> 16) & 0xff);
	print_debug_char(':');
	print_debug_hex8((dev >> 11) & 0x1f);
	print_debug_char('.');
	print_debug_hex8((dev >> 8) & 7);
}

static inline void print_pci_devices(void)
{
	device_t dev;
	for (dev = PCI_DEV(0, 0, 0);
	     dev <= PCI_DEV(0x00, 0x1f, 0x7); dev += PCI_DEV(0, 0, 1)) {
		u32 id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		print_debug_pci_dev(dev);
		print_debug("\n");
	}
}

static void dump_pci_device(unsigned dev)
{
	int i;
	print_debug_pci_dev(dev);
	print_debug("\n");

	for (i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = pci_read_config8(dev, i);
		print_debug_char(' ');
		print_debug_hex8(val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
	}
}

static inline void dump_pci_devices(void)
{
	device_t dev;
	for (dev = PCI_DEV(0, 0, 0);
	     dev <= PCI_DEV(0, 0x1f, 0x7); dev += PCI_DEV(0, 0, 1)) {
		u32 id;
		id = pci_read_config32(dev, PCI_VENDOR_ID);
		if (((id & 0xffff) == 0x0000) || ((id & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0xffff)
		    || (((id >> 16) & 0xffff) == 0x0000)) {
			continue;
		}
		dump_pci_device(dev);
	}
}


static inline void dump_io_resources(unsigned port)
{

	int i;
	print_debug_hex16(port);
	print_debug(":\n");
	for (i = 0; i < 256; i++) {
		u8 val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = inb(port);
		print_debug_char(' ');
		print_debug_hex8(val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
		port++;
	}
}
