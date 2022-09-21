# Adding new devices to a device tree

## Introduction

ACPI exposes a platform-independent interface for operating systems to perform
power management and other platform-level functions.  Some operating systems
also use ACPI to enumerate devices that are not immediately discoverable, such
as those behind I2C or SPI buses (in contrast to PCI).  This document discusses
the way that coreboot uses the concept of a "device tree" to generate ACPI
tables for usage by the operating system.

## Devicetree and overridetree (if applicable)

For mainboards that are organized around a "reference board" or "baseboard"
model (see ``src/mainboard/google/octopus`` or ``hatch`` for examples), there is
typically a devicetree.cb file that all boards share, and any differences for a
specific board ("variant") are captured in the overridetree.cb file.  Any
settings changed in the overridetree take precedence over those in the main
devicetree.  Note, not all mainboards will have the devicetree/overridetree
distinction, and may only have a devicetree.cb file.  Or you can always just
write the ASL (ACPI Source Language) code yourself.

### Naming and referencing devices

When declaring a device, it can optionally be given an alias that can be
referred to elsewhere. This is particularly useful to declare a device in one
device tree while allowing its configuration to be more easily changed in an
overlay. For instance, the AMD Picasso SoC definition
(`soc/amd/picasso/chipset.cb`) declares an IOMMU on a PCI bus that is disabled
by default:

```
chip soc/amd/picasso
	device domain 0 on
		...
		device pci 00.2 alias iommu off end
		...
	end
end
```

A device based on this SoC can override the configuration for the IOMMU without
duplicating addresses, as in
`mainboard/google/zork/variants/baseboard/devicetree_trembyle.cb`:

```
chip soc/amd/picasso
	device domain 0
		...
		device ref iommu on end
		...
	end
end
```

In this example the override simply enables the IOMMU, but it could also
set additional properties (or even add child devices) inside the IOMMU `device`
block.

---

It is important to note that devices that use `device ref` syntax to override
previous definitions of a device by alias must be placed at **exactly the same
location in the device tree** as the original declaration. If not, this will
actually create another device rather than overriding the properties of the
existing one. For instance, if the above snippet from `devicetree_trembyle.cb`
were written as follows:

```
chip soc/amd/picasso
	# NOTE: not inside domain 0!
	device ref iommu on end
end
```

Then this would leave the SoC's IOMMU disabled, and instead create a new device
with no properties as a direct child of the SoC.

## Device drivers

Platform independent device drivers are hooked up via entries in a devicetree.
See [Driver Devicetree Entries](drivers/dt_entries.md) for more info.

## Notes

 - **All fields that are left unspecified in the devicetree are initialized to
   zero.**
