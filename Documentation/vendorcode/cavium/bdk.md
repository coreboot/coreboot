# Cavium's BDK

## BDK
A part of Cavium's BDK can be found in '''src/vendorcode/cavium/bdk'''.
It does the **DRAM init** in romstage and the **PCIe**, **QLM**, **SLI**,
**PHY**, **BGX**, **SATA** init in ramstage.

## Devicetree
The BDK does use it's own devicetree, as coreboot's devicetree isn't
compatible. The devicetree stores key-value pairs (see **bdk-devicetree.h**
for implementation details), where the key and the value are stored as strings.

The key-value pairs must be advertised in romstage and ramstage using the
'''bdk_config_set_fdt()''' method.

The tool '''util/cavium/devicetree_convert.py''' can be used to convert a
devicetree to a key-value array.

## Modifications

* The BDK has been modified to compile under coreboot's toolchain.
* Removed FDT devicetree support.
* Dropped files that aren't required for SoC bringup
* Added Kconfig values for verbose console output

## Debugging

You can enable verbose console output in *menuconfig*:

Go to **Chipset**, **BDK** and enable one or multiple stages.
