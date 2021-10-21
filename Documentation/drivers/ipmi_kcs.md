# IPMI KCS driver

The driver can be found in `src/drivers/ipmi/`. It works with BMC that provide
a KCS I/O interface as specified in the [IPMI] standard.

The driver detects the IPMI version, reserves the I/O space in coreboot's
resource allocator and writes the required ACPI and SMBIOS tables.

## For developers

To use the driver, select the `IPMI_KCS` Kconfig and add the following PNP
device under the LPC bridge device (in example for the KCS at 0xca2):

```
 chip drivers/ipmi
   device pnp ca2.0 on end         # IPMI KCS
 end
```

**Note:** The I/O base address needs to be aligned to 2.

The following registers can be set:

* `have_nv_storage`
  * Boolean
  * If true `nv_storage_device_address` will be added to SMBIOS type 38.
* `nv_storage_device_address`
  * Integer
  * The NV storage address as defined in SMBIOS spec for type 38.
* `bmc_i2c_address`
  * Integer
  * The i2c address of the BMC. zero if not applicable.
* `have_apic`
  * Boolean
  * If true the `apic_interrupt` will be added to SPMI table.
* `apic_interrupt`
  * Integer
  * The APIC interrupt used to notify about a change on the KCS.
* `have_gpe`
  * Boolean
  * If true the `gpe_interrupt` will be added to SPMI table.
* `gpe_interrupt`
  * Integer
  * The bit in GPE (SCI) used to notify about a change on the KCS.
* `wait_for_bmc`
  * Boolean
  * Wait for BMC to boot. This can be used if the BMC takes a long time to boot
    after PoR:
     - AST2400 on Supermicro X11SSH: 34 s
* `bmc_boot_timeout`
  * Integer
  * The timeout in seconds to wait for the IPMI service to be loaded.
    Will be used if wait_for_bmc is true.


[IPMI]: https://www.intel.com/content/dam/www/public/us/en/documents/product-briefs/ipmi-second-gen-interface-spec-v2-rev1-1.pdf
