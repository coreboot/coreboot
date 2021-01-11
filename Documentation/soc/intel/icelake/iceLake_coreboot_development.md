# Intel Ice Lake coreboot development

## Introduction

This document captures the coreboot development strategy for Intel SoC named Ice lake.

The Ice Lake processor family is the next generation Intel® Core processor family.
These processors are built using Intel's 10 nm+ process.

* [What is Ice Lake?](https://www.intel.in/content/www/in/en/design/products-and-solutions/processors-and-chipsets/ice-lake/overview.html)

## Development Strategy

Like any other Intel SoC, Ice Lake coreboot development is also based on "Intel common code development model".

1. Intel develops initial Firmware code for Ice Lake SoC.

2. Additionally provides Firmware code support for Intel Reference Platform (RVP), known as Ice lake RVP with same SoC.
   ```eval_rst
   :doc:`../../../mainboard/intel/icelake_rvp`
   ```

### Summary:
* SoC is Ice Lake.
* Reference platform is icelake_rvp.
* OEM board is Dragonegg.

## Create coreboot Image

1. Clone latest coreboot code as below
   ```bash
   $ git clone https://review.coreboot.org/coreboot.git
   ```

2. Place blobs (ucode, me.bin and FSP packages) in appropriate locations

   Note:
   Consider the fact that ucode and ME kit for Ice Lake SoC will be available from Intel VIP site.
   After product launch, FSP binary will be available externally as any other program.

3. Create coreboot .config

4. Build toolchain
   ```bash
   CPUS=$(nproc--ignore=1)  make  crossgcc-i386  iasl
   ```

5. Build image
   ```bash
   $ make # the image is generated as build/coreboot.rom
   ```

## Flashing coreboot

Flashing mechanism might be different between Intel RVP (Reference Validation Platform) and Chromebooks:

* Make use of dediprog while flashing coreboot image on Intel-RVP
* For Chromebook related platform like dragonegg, one can flash via servo:

```bash
   $ dut-control spi2_vref:pp3300 spi2_buf_en:on spi2_buf_on_flex_en:on warm_reset:on
   $ sudo flashrom -n -p ft2232_spi:type=servo-v2 -w <bios_image>
   $ dut-control spi2_vref:off spi2_buf_en:off spi2_buf_on_flex_en:off warm_reset:off
```
### References
* [flashrom](https://flashrom.org/Flashrom)
* [Servo](https://www.chromium.org/chromium-os/servo)
