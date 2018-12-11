# Intel Ice Lake RVP (Reference Validation Platform)

This page describes how to run coreboot on the Intel icelake_rvp board.

Ice Lake RVP is based on Intel Ice Lake platform, please refer to below link to get more details
```eval_rst
:doc:`../../soc/intel/icelake/iceLake_coreboot_development`
```

## Building coreboot

* Follow build instructions mentioned in Ice Lake document
```eval_rst
:doc:`../../soc/intel/icelake/iceLake_coreboot_development`
```

* The default options for this board should result in a fully working image:
```bash
	# echo "CONFIG_VENDOR_INTEL=y" > .config
	# echo "CONFIG_BOARD_INTEL_ICELAKE_RVPU=y" >> .config
	# make olddefconfig && make
```

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Size                | 32 MiB     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+
```
