/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Memory map for QEMU virt machine since
 * a578cdfbdd8f9beff5ced52b7826ddb1669abbbf (June 2019):
 *
 * 0..128MiB (0x0000_0000..0x0080_0000) is the space for a flash device.
 * 128MiB..256MiB (0x0080_0000..0x0100_0000) is used for miscellaneous device I/O.
 * 256MiB..1GiB (0x0100_0000..0x4000_0000) is reserved for possible future PCI support.
 * 1GiB.. (0x4000_0000) is RAM and the size depends on initial RAM and device memory settings
 *
 * 0x0000_0000..0x0080_0000: Flash memory
 * 0x0900_0000..0x0900_1000: UART (PL011)
 * 0x0901_0000..0x0901_1000: RTC (PL031)
 * 0x0903_0000..0x0903_1000: GPIO (PL061)
 * 0x0904_0000..0x0904_1000: Secure UART (PL011)
 * 0x0905_0000..0x0907_0000: SMMU (smmu-v3)
 * 0x0a00_0000..0x0a00_0200: MMIO (virtio)
 * 0x0c00_0000..0x0e00_0000: Platform bus
 * 0x0e00_0000..0x0eff_ffff: Secure SRAM
 * 0x4000_0000..: RAM
 */
#define VIRT_UART_BASE 0x09000000
#define VIRT_RTC_BASE 0x09010000
#define VIRT_GPIO_BASE 0x09030000
#define VIRT_SECURE_UART_BASE 0x09040000
#define VIRT_SMMU_BASE 0x09050000
#define VIRT_MMIO_BASE 0x0a000000
#define VIRT_PLATFORM_BUS_BASE 0x0c000000
#define VIRT_SECRAM_BASE 0xe000000
#define VIRT_PCIE_LOW_MMIO_BASE 0x10000000
#define VIRT_PCIE_LOW_MMIO_LIMIT 0x3efeffff
/*
 * From hw/arm/virt.c:
 * Highmem IO Regions: This memory map is floating, located after the RAM.
 * Each MemMapEntry base (GPA) will be dynamically computed, depending on the
 * top of the RAM, so that its base get the same alignment as the size,
 * ie. a 512GiB entry will be aligned on a 512GiB boundary. If there is
 * less than 256GiB of RAM, the floating area starts at the 256GiB mark.
 * Note the extended_memmap is sized so that it eventually also includes the
 * base_memmap entries (VIRT_HIGH_GIC_REDIST2 index is greater than the last
 * index of base_memmap).
 */
#define VIRT_PCIE_LOW_ECAM_BASE    0x3f000000 /* only used if highmem-ecam=off (QEMU cmdline) */
#define VIRT_PCIE_HIGH_ECAM_BASE 0x4010000000 /* only used if highmem-ecam=on  (default) */
#define VIRT_PCIE_ECAM_SIZE (256 * MiB)
#define VIRT_PCIE_HIGH_MMIO_BASE  0x8000000000ULL
#define VIRT_PCIE_HIGH_MMIO_LIMIT 0xffffffffffULL
