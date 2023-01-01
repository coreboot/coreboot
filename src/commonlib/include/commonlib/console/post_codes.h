/* SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * @file post_codes.h
 */

/*
 * This aims to be a central point for POST codes used throughout coreboot.
 * All POST codes should be declared here as macros, and post_code() should
 * be used with the macros instead of hardcoded values. This allows us to
 * quickly reference POST codes when nothing is working
 *
 * The format for a POST code macro is
 * #define POST_WHAT_WE_COMMUNICATE_IS_HAPPENING_WHEN_THIS_CODE_IS_POSTED
 * Lets's keep it at POST_* instead of POST_CODE_*
 *
 * This file is also included by early assembly files. Only use #define s;
 * no function prototypes allowed here
 *
 * DOCUMENTATION:
 * Please document any and all post codes using Doxygen style comments. We
 * want to be able to generate a verbose enough documentation that is useful
 * during debugging.
 *
 */

#ifndef POST_CODES_H
#define POST_CODES_H

/**
 * \brief Not an actual post-code - used to clear port80h
 *
 */
#define POST_CODE_CLEAR				0x00

/**
 * \brief  Entry into 'crt0.s'. reset code jumps to here
 *
 * First instruction that gets executed after the reset vector jumps.
 * This indicates that the reset vector points to the correct code segment.
 */
#define POST_RESET_VECTOR_CORRECT		0x01

/**
 * \brief Entry into protected mode
 *
 * Preparing to enter protected mode. This is POSTed right before changing to
 * protected mode.
 */
#define POST_ENTER_PROTECTED_MODE		0x10

/**
 * \brief Start copying coreboot to RAM with decompression if compressed
 *
 * POSTed before ramstage is about to be loaded into memory
 */
#define POST_PREPARE_RAMSTAGE			0x11

/**
 * \brief Copy/decompression finished; jumping to RAM
 *
 * This is called after ramstage is loaded in memory, and before
 * the code jumps there. This represents the end of romstage.
 */
#define POST_RAMSTAGE_IS_PREPARED		0x12

/**
 * \brief Entry into c_start
 *
 * c_start.S is the first code executing in ramstage.
 */
#define POST_ENTRY_C_START			0x13

/**
 * \brief Entry into bootblock cache-as-RAM code
 *
 */
#define POST_BOOTBLOCK_CAR			0x21

/**
 * \brief Entry into pci_scan_bus
 *
 * Entered pci_scan_bus()
 */
#define POST_ENTER_PCI_SCAN_BUS			0x24

/**
 * \brief Entry into pci_scan_bus
 *
 * Entered pci_scan_bus()
 */
#define POST_EXIT_PCI_SCAN_BUS			0x25

/**
 * \brief Pre-memory init preparation start
 *
 * Post code emitted in romstage before making callbacks to allow SoC/mainboard
 * to prepare params for FSP memory init.
 */
#define POST_MEM_PREINIT_PREP_START		0x34

/**
 * \brief Pre-memory init preparation end
 *
 * Post code emitted in romstage after returning from SoC/mainboard callbacks
 * to prepare params for FSP memory init.
 */
#define POST_MEM_PREINIT_PREP_END		0x36

/**
 * \brief Console is initialized
 *
 * The console is initialized and is ready for usage
 */
#define POST_CONSOLE_READY			0x39

/**
 * \brief Console boot message succeeded
 *
 * First console message has been successfully sent through the console backend
 * driver.
 */
#define POST_CONSOLE_BOOT_MSG			0x40

/**
 * \brief Before enabling the cache
 *
 * Going to enable the cache
 */
#define POST_ENABLING_CACHE			0x60

/**
 * \brief Pre call to RAM stage main()
 *
 * POSTed right before RAM stage main() is called from c_start.S
 */
#define POST_PRE_HARDWAREMAIN			0x6e

/**
 * \brief Entry into coreboot in RAM stage main()
 *
 * This is the first call in hardwaremain.c. If this code is POSTed, then
 * ramstage has successfully loaded and started executing.
 */
#define POST_ENTRY_HARDWAREMAIN			0x6f

/**
 * \brief Before Device Probe
 *
 * Boot State Machine: bs_pre_device()
 */
#define POST_BS_PRE_DEVICE			0x70

/**
 * \brief Initializing Chips
 *
 * Boot State Machine: bs_dev_init_chips()
 */
#define POST_BS_DEV_INIT_CHIPS			0x71

/**
 * \brief Starting Device Enumeration
 *
 * Boot State Machine: bs_dev_enumerate()
 */
#define POST_BS_DEV_ENUMERATE			0x72

/**
 * \brief Device Resource Allocation
 *
 * Boot State Machine: bs_dev_resources()
 */
#define POST_BS_DEV_RESOURCES			0x73

/**
 * \brief Device Enable
 *
 * Boot State Machine: bs_dev_enable()
 */
#define POST_BS_DEV_ENABLE			0x74

/**
 * \brief Device Initialization
 *
 * Boot State Machine: bs_dev_init()
 */
#define POST_BS_DEV_INIT			0x75

/**
 * \brief After Device Probe
 *
 * Boot State Machine: bs_post_device()
 */
#define POST_BS_POST_DEVICE			0x76

/**
 * \brief OS Resume Check
 *
 * Boot State Machine: bs_os_resume_check()
 */
#define POST_BS_OS_RESUME_CHECK			0x77

/**
 * \brief OS Resume
 *
 * Boot State Machine: bs_os_resume()
 */
#define POST_BS_OS_RESUME			0x78

/**
 * \brief Write Tables
 *
 * Boot State Machine: bs_write_tables()
 */
#define POST_BS_WRITE_TABLES			0x79

/**
 * \brief Load Payload
 *
 * Boot State Machine: bs_payload_load()
 */
#define POST_BS_PAYLOAD_LOAD			0x7a

/**
 * \brief Boot Payload
 *
 * Boot State Machine: bs_payload_boot()
 */
#define POST_BS_PAYLOAD_BOOT			0x7b

/**
 * \brief Before calling FSP Notify (end of firmware)
 *
 * Going to call into FSP binary for Notify phase (end of firmware)
 */
#define POST_FSP_NOTIFY_BEFORE_END_OF_FIRMWARE	0x88

/**
 * \brief After calling FSP Notify (end of firmware)
 *
 * Going to call into FSP binary for Notify phase (end of firmware)
 */
#define POST_FSP_NOTIFY_AFTER_END_OF_FIRMWARE	0x89

/**
 * \brief Before calling FSP TempRamInit
 *
 * Going to call into FSP binary for TempRamInit phase
 */
#define POST_FSP_TEMP_RAM_INIT			0x90

/**
 * \brief Before calling FSP TempRamExit
 *
 * Going to call into FSP binary for TempRamExit phase
 */
#define POST_FSP_TEMP_RAM_EXIT			0x91

/**
 * \brief Before calling FSP MemoryInit
 *
 * Going to call into FSP binary for MemoryInit phase
 */
#define POST_FSP_MEMORY_INIT			0x92

/**
 * \brief Before calling FSP SiliconInit
 *
 * Going to call into FSP binary for SiliconInit phase
 */
#define POST_FSP_SILICON_INIT			0x93

/**
 * \brief Before calling FSP Notify (after PCI enumeration)
 *
 * Going to call into FSP binary for Notify phase (after PCI enumeration)
 */
#define POST_FSP_NOTIFY_BEFORE_ENUMERATE	0x94

/**
 * \brief Before calling FSP Notify (ready to boot)
 *
 * Going to call into FSP binary for Notify phase (ready to boot)
 */
#define POST_FSP_NOTIFY_BEFORE_FINALIZE		0x95

/**
 * \brief Indicate OS _PTS entry
 *
 * Called from _PTS asl method
 */
#define POST_OS_ENTER_PTS			0x96

/**
 * \brief Indicate OS _WAK entry
 *
 * Called from within _WAK method
 */
#define POST_OS_ENTER_WAKE			0x97

/**
 * \brief After calling FSP MemoryInit
 *
 * FSP binary returned from MemoryInit phase
 */
#define POST_FSP_MEMORY_EXIT			0x98

/**
 * \brief After calling FSP SiliconInit
 *
 * FSP binary returned from SiliconInit phase
 */
#define POST_FSP_SILICON_EXIT			0x99

/**
 * \brief Entry to write_pirq_table
 *
 * coreboot entered write_pirq_table
 */
#define POST_X86_WRITE_PIRQ_TABLE		0x9a

/**
 * \brief Entry to write_mptable
 *
 * coreboot entered write_mptable
 */
#define POST_X86_WRITE_MPTABLE			0x9b

/**
 * \brief Entry to write_acpi_table
 *
 * coreboot entered write_acpi_table
 */
#define POST_X86_WRITE_ACPITABLE		0x9c

/**
 * \brief Before calling FSP Multiphase SiliconInit
 *
 * Going to call into FSP binary for Multiple phase SI Init
 */
#define POST_FSP_MULTI_PHASE_SI_INIT_ENTRY	0xa0

/**
 * \brief After calling FSP Multiphase SiliconInit
 *
 * FSP binary returned from Multiple phase SI Init
 */
#define POST_FSP_MULTI_PHASE_SI_INIT_EXIT	0xa1

/**
 * \brief After calling FSP Notify (after PCI enumeration)
 *
 * Going to call into FSP binary for Notify phase (after PCI enumeration)
 */
#define POST_FSP_NOTIFY_AFTER_ENUMERATE        0xa2

/**
 * \brief After calling FSP Notify (ready to boot)
 *
 * Going to call into FSP binary for Notify phase (ready to boot)
 */
#define POST_FSP_NOTIFY_AFTER_FINALIZE         0xa3

/**
 * \brief Invalid or corrupt ROM
 *
 * Set if firmware failed to find or validate a resource that is stored in ROM.
 */
#define POST_INVALID_ROM			0xe0

/**
 * \brief Invalid or corrupt CBFS
 *
 * Set if firmware failed to find or validate a resource that is stored in CBFS.
 */
#define POST_INVALID_CBFS			0xe1

/**
 * \brief Vendor binary error
 *
 * Set if firmware failed to find or validate a vendor binary, or the binary
 * generated a fatal error.
 */
#define POST_INVALID_VENDOR_BINARY		0xe2

/**
 * \brief RAM failure
 *
 * Set if RAM could not be initialized.  This includes RAM is missing,
 * unsupported RAM configuration, or RAM failure.
 */
#define POST_RAM_FAILURE			0xe3

/**
 * \brief Hardware initialization failure
 *
 * Set when a required hardware component was not found or is unsupported.
 */
#define POST_HW_INIT_FAILURE			0xe4

/**
 * \brief Video failure
 *
 * Video subsystem failed to initialize.
 */
#define POST_VIDEO_FAILURE			0xe5

/**
 * \brief TPM failure
 *
 * An error with the TPM, either unexpected state or communications failure.
 */
#define POST_TPM_FAILURE			0xed

/**
 * \brief Not supposed to get here
 *
 * A function that should not have returned, returned
 *
 * Check the console output for details.
 */
#define POST_DEAD_CODE				0xee

/**
 * \brief Resume from suspend failed
 *
 * This post code is sent when the firmware is expected to resume it is
 * unable to do so.
 */
#define POST_RESUME_FAILURE			0xef

/**
 * \brief Jumping to payload
 *
 * Called right before jumping to a payload. If the boot sequence stops with
 * this code, chances are the payload freezes.
 */
#define POST_JUMPING_TO_PAYLOAD			0xf3

/**
 * \brief Entry into elf boot
 *
 * This POST code is called right before invoking jmp_to_elf_entry()
 * jmp_to_elf_entry() invokes the payload, and should never return
 */
#define POST_ENTER_ELF_BOOT			0xf8

/**
 * \brief Final code before OS resumes
 *
 * Called right before jumping to the OS resume vector.
 */
#define POST_OS_RESUME				0xfd

/**
 * \brief Final code before OS boots
 *
 * This may not be called depending on the payload used.
 */
#define POST_OS_BOOT				0xfe

/**
 * \brief Elfload fail or die() called
 *
 * coreboot was not able to load the payload, no payload was detected
 * or die() was called.
 * \n
 * If this code appears before entering ramstage, then most likely
 * ramstage is corrupted, and reflashing of the ROM chip is needed.
 * \n
 * If this code appears after ramstage, there is a problem with the payload
 * If the payload was built out-of-tree, check that it was compiled as
 * a coreboot payload
 * \n
 * Check the console output to see exactly where the failure occurred.
 */
#define POST_DIE				0xff

#endif /* POST_CODES_H */
