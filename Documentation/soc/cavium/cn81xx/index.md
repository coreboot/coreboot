# Cavium CN81xx documentation

## Reference code

```eval_rst
The Cavium reference code is called `BDK`_ (board development kit) and is part
of the `Octeon-TX-SDK`_. Parts of the `BDK`_ have been integrated into coreoboot.
```

## SOC code

The SOC folder contains functions for:
* TWSI
* UART
* TIMER
* SPI
* MMU
* DRAM
* CLOCK
* GPIO
* Secondary CPUs
* PCI

All other hardware is initilized by the BDK code, which is invoked from
ramstage.

## Notes about the hardware

Cavium SoC do **not** have embedded SRAM. The **BOOTROM** setups the
L2 cache and loads 192KiB of firmware starting from 0x20000 to a fixed
location. It then jumps to the firmware.

```eval_rst
For more details have a look at `Cavium CN8XXX Bootflow`_.
```

## CAR setup

For Cache-as-RAM we only need to lock the cachelines which are used by bootblock
or romstage until DRAM has been set up. At the end of romstage the cachelines
are unlocked and the contents are flushed to DRAM.
Locked cachelines are never evicted.

The CAR setup is done in '''bootblock_custom.S''' and thus doesn't use the common
aarch64 '''bootblock.S''' code.

## DRAM setup

```eval_rst
The DRAM setup is done by the `BDK`_.
```

## PCI setup

The PCI setup is done using the MMCONF mechanism.
Besides configuring device visibility (secure/unsecure) the MSI-X interrupts
needs to be configured.

## Devicetree patching

The Linux devicetree needs to be patched, depending on the available hardware
and their configuration. Some values depends on fuses, some on user selectable
configuration.

The following SoC specific fixes are made:

1. Fix SCLK
2. Fix UUA refclock
3. Remove unused PEM entries
4. Remove unused QLM entries
5. Set local MAC address

## CN81xx quirks

The CN81xx needs some quirks that are not documented or hidden in the code.

### Violation of PCI spec

**Problem:**

* The PCI device 01:01.0 is disabled, but a multifunction device.
* The PCI device 01:01.2 - 00:01.7 is enabled and can't be found by the coreboot
  PCI allocator.

**Solution:**

The PCI Bus 0 and 1 are scanned manually in SOC's PCI code.


### Crash accessing SLI memory

**Problem:**

The SLI memory region decodes to attached PCIe devices.
Accessing the memory region results in 'Data Abort Exception' if the link of the
PCIe device never had been enabled.

**Solution:**

Enable the PCIe link at least once. (You can disabling the link and the SLI
memory reads as 0xffffffff.)


### RNG Data Abort Exception

**Problem:**

'Data Abort Exception' on accessing the enabled RNG.

**Solution**:

Read the BDK_RNM_CTL_STATUS register at least once after writing it.


```eval_rst
.. _Octeon-TX-SDK: https://github.com/Cavium-Open-Source-Distributions/OCTEON-TX-SDK
.. _Cavium CN8XXX Bootflow: ../bootflow.html
.. _BDK: ../../../vendorcode/cavium/bdk.html
```
