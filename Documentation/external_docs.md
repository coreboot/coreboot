# External Resources

This is a list of resources that could be useful to coreboot developers.
These are not endorsed or officially recommended by the coreboot project,
but simply listed here in the hopes that someone will find something
useful.

Please add any helpful or informational links and sections as you see fit.

## Articles

* External Interrupts in the x86 system.
  * [Part 1: Interrupt controller evolution](https://habr.com/en/post/446312/)
  * [Part 2: Linux kernel boot options](https://habr.com/en/post/501660/)
  * [Part 3: Interrupt routing setup in a chipset](https://habr.com/en/post/501912/)
* System address map initialization in x86/x64 architecture.
  * [Part 1: PCI-based systems](https://resources.infosecinstitute.com/topic/system-address-map-initialization-in-x86x64-architecture-part-1-pci-based-systems/)
  * [Part 2: PCI express-based systems](https://resources.infosecinstitute.com/topic/system-address-map-initialization-x86x64-architecture-part-2-pci-express-based-systems/)
  * [PCIe elastic buffer](https://www.mindshare.com/files/resources/mindshare_pcie_elastic_buffer.pdf)
* [Boot Guard and PSB have user-hostile defaults](https://mjg59.dreamwidth.org/58424.html)


## General Information

* [OS Dev](https://wiki.osdev.org/Categorized_Main_Page)
* [Interface BUS](http://www.interfacebus.com/)
* Open course material for a variety of topics such as assembly, firmware,
  security, debugging, and more.
  * [Open Security Training](https://opensecuritytraining.info/Training.html),
  * [Open Security Training 2](https://p.ost2.fyi/)


## Firmware Specifications & Information

* [System Management BIOS - SMBIOS](https://www.dmtf.org/standards/smbios)
* [Desktop and Mobile Architecture for System Hardware - DASH](https://www.dmtf.org/standards/dash)
* [PNP BIOS](https://www.intel.com/content/dam/support/us/en/documents/motherboards/desktop/sb/pnpbiosspecificationv10a.pdf)


### ACPI

* [ACPI Specs](https://uefi.org/acpi/specs)
* [ACPI in Linux](https://www.kernel.org/doc/ols/2005/ols2005v1-pages-59-76.pdf)
* [ACPI 5 Linux](https://blog.linuxplumbersconf.org/2012/wp-content/uploads/2012/09/LPC2012-ACPI5.pdf)
* [ACPI 6 Linux](https://events.static.linuxfound.org/sites/events/files/slides/ACPI_6_and_Linux_0.pdf)


### Security

* [Intel Boot Guard](https://edk2-docs.gitbook.io/understanding-the-uefi-secure-boot-chain/secure_boot_chain_in_uefi/intel_boot_guard)


## Hardware information

* [WikiChip](https://en.wikichip.org/wiki/WikiChip)
* [Sandpile](https://www.sandpile.org/)
* [CPU-World](https://www.cpu-world.com/index.html)
* [CPU-Upgrade](https://www.cpu-upgrade.com/index.html)


### Hardware Specifications & Standards

* [Bluetooth](https://www.bluetooth.com/specifications/specs/) - Bluetooth SIG
* [eMMC](https://www.jedec.org/)  - JEDEC - (LOGIN REQUIRED)
* [eSPI](https://cdrdv2.intel.com/v1/dl/getContent/645987) - Intel
* [I2c Spec](https://web.archive.org/web/20170704151406/https://www.nxp.com/docs/en/user-guide/UM10204.pdf),
  [Appnote](https://www.nxp.com/docs/en/application-note/AN10216.pdf) - NXP
* [I2S](https://www.nxp.com/docs/en/user-manual/UM11732.pdf) - NXP
* [I3C](https://www.mipi.org/specifications/i3c-sensor-specification) - MIPI Alliance (LOGIN REQUIRED)
* [Memory](https://www.jedec.org/)  - JEDEC - (LOGIN REQUIRED)
* [NVMe](https://nvmexpress.org/developers/) - NVMe Specifications
* [LPC](https://www.intel.com/content/dam/www/program/design/us/en/documents/low-pin-count-interface-specification.pdf) - Intel
* [PCI / PCIe / M.2](https://pcisig.com/specifications) -  PCI-SIG - (LOGIN REQUIRED)
* [Power Delivery](https://www.usb.org/documents) - USB Implementers Forum
* [SATA](https://sata-io.org/developers/purchase-specification) - SATA-IO (LOGIN REQUIRED)
* [SMBus](http://www.smbus.org/specs/) - System Management Interface Forum
* [Smart Battery](http://smartbattery.org/specs/) - Smart Battery System Implementers Forum
* [USB](https://www.usb.org/documents) - USB Implementers Forum
* [WI-FI](https://www.wi-fi.org/discover-wi-fi/specifications) - Wi-Fi Alliance


### Chip Vendor Documentation

* AMD
  * [Developer Guides, Manuals & ISA Documents](https://developer.amd.com/resources/developer-guides-manuals/)
  * [AMD Tech Docs - Official Documentation Page](https://www.amd.com/en/support/tech-docs)
* ARM
  * [Tools and Software - Specifications](https://developer.arm.com/tools-and-software/software-development-tools/specifications)
* Intel
  * [Developer Zone](https://www.intel.com/content/www/us/en/developer/overview.html)
  * [Resource & Documentation Center](https://www.intel.com/content/www/us/en/resources-documentation/developer.html)
  * [Architecture Software Developer Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
  * [Intel specific ACPI](https://www.intel.com/content/www/us/en/standards/processor-vendor-specific-acpi-specification.html)

* Rockchip
  * [Open Source Wiki](https://opensource.rock-chips.com/wiki_Main_Page)


## Software

 * [Fiedka](https://github.com/fiedka/fiedka) - A graphical Firmware Editor
 * [IOTools](https://github.com/adurbin/iotools) - Command line tools to access hardware registers
 * [UEFITool](https://github.com/LongSoft/UEFITool) - Editor for UEFI PI compliant firmware images
 * [CHIPSEC](https://chipsec.github.io) - Framework for analyzing platform level security & configuration
 * [SPDEditor](https://github.com/integralfx/SPDEditor) - GUI to edit DDR3 SPD files
 * [DDR4XMPEditor](https://github.com/integralfx/DDR4XMPEditor) - Editor for DDR4 SPD and XMP
* [overclockSPD](https://github.com/baboomerang/overclockSPD) - Fast and easy way to read and write data to RAM SPDs.
* [VBiosFinder](https://github.com/coderobe/VBiosFinder) - This tool attempts to extract a VBIOS from a BIOS update.


## Infrastructure software

* [Kconfig](https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html)
* [GNU Make](https://www.gnu.org/software/make/manual/)
