# IPMI Block Transfer (BT) driver

The driver can be found in `src/drivers/ipmi/` (same as KCS). It works with BMC
that provides a BT I/O interface as specified in the [IPMI] standard.  See
"Intelligent Platform Management Interface Specification", v2.0, Rev. 1.1 for
more details on the interface and IPMI in general.

The driver detects the IPMI version and reserves the I/O space in coreboot's
resource allocator.

## For developers

To use the driver, select the `IPMI_BT` Kconfig and add the following PNP
device (in example for the BT at 0xe4):

```
chip drivers/ipmi
    device pnp e4.0 on end        # IPMI BT
end
```

**Note:** The I/O base address must be aligned to 4.

The following settings can be set in a device tree:

```{eval-rst}
+------------------+--------------+-------------------------------------------+
|  Setting         | Type/Default | Description/Purpose                       |
+==================+==============+===========================================+
| wait_for_bmc     | | Boolean    | Wait for BMC to boot. This can be used if |
|                  | | false      | the BMC takes a long time to boot after   |
|                  |              | PoR.                                      |
+------------------+--------------+-------------------------------------------+
| bmc_boot_timeout | | Integer    | The timeout in seconds to wait for the    |
|                  | | 0          | IPMI service to be loaded.  Will be used  |
|                  |              | if wait_for_bmc is true.                  |
+------------------+--------------+-------------------------------------------+
```

## Debugging/testing the driver

`ipmi_sim` from [OpenIPMI] project can be used by running `ipmi_sim -d` in one
console to watch what's being sent/received and starting QEMU like this in
another console:

```
qemu-system-x86_64 \
    -M q35,smm=on \
    -bios build/coreboot.rom \
    -chardev socket,id=ipmichr0,host=localhost,port=9002,reconnect=10 \
    -device ipmi-bmc-extern,chardev=ipmichr0,id=bmc0 \
    -device isa-ipmi-bt,bmc=bmc0,irq=0 \
    -serial stdio
```

A simpler alternative is to use QEMU's builtin BMC simulator:

```
qemu-system-x86_64 \
    -M q35,smm=on \
    -bios build/coreboot.rom \
    -device ipmi-bmc-sim,id=bmc0 \
    -device isa-ipmi-bt,bmc=bmc0,irq=0 \
    -serial stdio
```

## References

Useful links on the subject:
 * README of `ipmi_sim`:
   <https://github.com/wrouesnel/openipmi/blob/master/lanserv/README.ipmi_sim>
 * slides about OpenIPMI:
   <https://www.linux-kvm.org/images/7/76/03x08-Juniper-Corey_Minyard-UsingIPMIinQEMU.ods.pdf>
 * a usage example: <https://github.com/dhilst/qemu-ipmi>
 * old docs (the options are still there, but no longer have a dedicated page in
   modern documentation): <https://hskinnemoen.github.io/qemu/specs/ipmi.html>

[IPMI]: https://www.intel.com/content/dam/www/public/us/en/documents/specification-updates/ipmi-intelligent-platform-mgt-interface-spec-2nd-gen-v2-0-spec-update.pdf
[OpenIPMI]: https://github.com/wrouesnel/openipmi
