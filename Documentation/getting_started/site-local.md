# Using the site-local Directory in coreboot

## Overview

The `site-local` directory is a powerful mechanism in coreboot that
allows developers to maintain local modifications, configurations, and
binary blobs separate from the main coreboot repository. This
separation ensures that your local changes never conflict with upstream
updates and can be versioned independently.

## Purpose and Benefits

- **Local Customization**: Store board-specific configurations and
  modifications
- **Binary Blobs**: Keep non-redistributable files (like firmware
  blobs) outside the main repository
- **Independent Versioning**: Version your local additions separately
  from coreboot
- **Clean Separation**: Avoid conflicts with upstream changes
- **Build Integration**: Seamlessly integrate local additions into the
  build process
- **Override default values**: Set custom Kconfig or Makefile values,
  overriding the general coreboot codebase

## Getting Started

### Directory Structure

Create a `site-local` directory at the top level of your coreboot
repository:

```
coreboot/
├── src/
├── util/
├── ...
└── site-local/  <-- Your local additions go here
```

### Key Files

The following files in your `site-local` directory are recognized by
coreboot's build system and parsed very early in the process to allow
default values to be set, overriding values that might be set
elsewhere in the coreboot tree.

1. **`site-local/Kconfig`**: Integrated early in the configuration
   process, allowing you to set custom defaults
2. **`site-local/Makefile.mk`**: Integrated into the build system for
   custom build rules

## Integration Methods

### 1. Using Symbolic Links

The most common approach for integrating local additions is using
symbolic links. This allows you to maintain a parallel directory
structure in `site-local` that mirrors coreboot's structure.

#### Steps:

1. Create your directory structure within `site-local` that mirrors
   coreboot's structure
2. Add a `symlink.txt` file at the root of each directory you want to
   link
3. In each `symlink.txt`, specify the path (relative to coreboot root)
   where it should be linked
4. Run `make symlink` to create the symbolic links

#### Example:

```
coreboot/
├── src/
│   └── soc/
│       └── test-soc-from-site-local -> ../../site-local/src/soc/test-soc-from-site-local/
└── site-local/
    ├── Kconfig
    ├── Makefile.mk
    └── src/
        └── soc/
            └── test-soc-from-site-local/
                ├── chip.h
                ├── soc.c
                └── symlink.txt  <-- Contains "src/soc/test-soc-from-site-local"
```

To keep symlinks updated automatically, add this to your
`site-local/Makefile.mk`:

```
site-local-target:: symlink
```

### 2. Direct Integration via Kconfig

Your `site-local/Kconfig` file is included early in coreboot's
configuration process, allowing you to:

- Set custom default configurations
- Override upstream defaults
- Define new configuration options

### 3. Build System Integration

The `site-local/Makefile.mk` file is included in the build system,
allowing you to:

- Add files to CBFS
- Define custom build targets
- Modify build behavior for specific boards

## Common Use Cases

### 1. Adding Binary Blobs to CBFS

For non-redistributable files like firmware blobs or option ROMs:

```makefile
# In site-local/Makefile.mk
cbfs-files-$(CONFIG_BOARD_VENDOR_BOARDNAME) += firmware.bin
firmware.bin-file := path/to/firmware.bin
firmware.bin-type := raw
```

### 2. Board-Specific Binary Files

Store board-specific binary files in your `site-local` directory:

```makefile
# Example from real configs
CONFIG_IFD_BIN_PATH="site-local/descriptor.bin"
CONFIG_ME_BIN_PATH="site-local/me.bin"
CONFIG_GBE_BIN_PATH="site-local/gbe.bin"
```

### 3. Custom Payloads

Specify custom payloads for your builds:

```makefile
CONFIG_PAYLOAD_FILE="site-local/custom/linuxboot_payload"
```

### 4. FSP Binaries

Store and reference Intel FSP binaries:

```makefile
CONFIG_FSP_T_FILE="site-local/board/Server_T.fd"
CONFIG_FSP_M_FILE="site-local/board/Server_M.fd"
CONFIG_FSP_S_FILE="site-local/board/Server_S.fd"
```

### 5. CPU Microcode Updates

Include CPU microcode updates:

```makefile
CONFIG_CPU_UCODE_BINARIES="site-local/board/microcode.bin"
```

## Example: Developing a New SoC Out-of-Tree

One possible use case for `site-local` is developing a new
SoC implementation out-of-tree before it's ready to be made public.
This allows you to:

- Keep proprietary or under-NDA code separate until it can be properly
  open-sourced
- Develop and test in the context of the full coreboot tree
- Collaborate with a team on the SoC without affecting the public
  codebase
- Gradually transition from private to public as code is cleared for
  release

### Directory Structure for a New SoC

Here's a comprehensive example of how to structure your `site-local`
directory for developing a new SoC (in this example, a fictional
"newvendor/newtarget" SoC):

```
coreboot/
└── site-local/
    ├── Kconfig                   # Global Kconfig overrides
    ├── Makefile.mk               # Global Makefile overrides
    └── src/
        ├── soc/
        │   └── newvendor/        # New vendor directory
        │       ├── common/       # Common code for vendor SoCs
        │       │   ├── include/
        │       │   │   └── soc/
        │       │   │       └── common_definitions.h
        │       │   ├── Kconfig
        │       │   ├── Makefile.mk
        │       │   ├── common_init.c
        │       │   └── symlink.txt  # Contains "src/soc/newvendor/common"
        │       │
        │       └── newtarget/    # Specific SoC implementation
        │           ├── include/
        │           │   └── soc/
        │           │       ├── addressmap.h
        │           │       ├── gpio.h
        │           │       └── soc_api.h
        │           ├── chip.h
        │           ├── Kconfig
        │           ├── Makefile.mk
        │           ├── romstage.c
        │           ├── ramstage.c
        │           ├── gpio.c
        │           ├── soc.c
        │           ├── memory.c
        │           ├── uart.c
        │           └── symlink.txt  # Contains "src/soc/newvendor/newtarget"
        │
        └── mainboard/
            └── newvendor/        # Reference mainboard for the new SoC
                └── devboard/     # Development board for the SoC
                    ├── devicetree.cb
                    ├── Kconfig
                    ├── Makefile.mk
                    ├── board.c
                    ├── romstage.c
                    ├── gpio.c
                    └── symlink.txt  # Contains "src/mainboard/newvendor/devboard"
```

### Key Files for SoC Implementation

Let's look at the content of some key files in this structure:

#### 1. SOC Kconfig (`site-local/src/soc/newvendor/newtarget/Kconfig`)

```kconfig
config SOC_NEWVENDOR_NEWTARGET
	bool
	help
	  NewVendor NewTarget SoC support

if SOC_NEWVENDOR_NEWTARGET

config SOC_SPECIFIC_OPTIONS
	def_bool y
	select ARCH_BOOTBLOCK_ARM64
	select ARCH_RAMSTAGE_ARM64
	select ARCH_ROMSTAGE_ARM64
	select ARCH_VERSTAGE_ARM64
	select ARM64_USE_ARM_TRUSTED_FIRMWARE
	select HAVE_UART_SPECIAL
	select COMMON_CBFS_SPI_WRAPPER
	select SOC_NEWVENDOR_COMMON  # For common vendor code

config VBOOT
	bool
	default y if VBOOT_SLOTS_RW_AB

config UART_FOR_CONSOLE
	int
	default 0

endif # SOC_NEWVENDOR_NEWTARGET
```

#### 2. SOC Makefile (`site-local/src/soc/newvendor/newtarget/Makefile.mk`)

```makefile
bootblock-y += bootblock.c
bootblock-y += uart.c
bootblock-y += gpio.c

romstage-y += romstage.c
romstage-y += memory.c
romstage-y += uart.c
romstage-y += gpio.c

ramstage-y += soc.c
ramstage-y += uart.c
ramstage-y += gpio.c

CPPFLAGS_common += -Isrc/soc/newvendor/newtarget/include
CPPFLAGS_common += -Isrc/soc/newvendor/common/include

# Include any vendor-specific binary blobs that can't be open-sourced yet
BL31_MAKEARGS += PLAT=newtarget

# Include private bootloader files for this SoC
cbfs-files-y += scp.bin
scp.bin-file := site-local/blobs/newvendor/newtarget/scp.bin
scp.bin-type := raw
scp.bin-position := 0x20000
```

#### 3. Global site-local Kconfig (`site-local/Kconfig`)

```kconfig
# Include our custom SoC in the mainboard selection process
source "src/soc/newvendor/*/Kconfig"

# Override build options for development
config COMPILER_GCC
	default y

config ALLOW_MANUAL_FIRMWARE_BLOB_SELECTION
	default y

# Add custom firmware verification options
config CUSTOM_FIRMWARE_VERIFICATION
	bool "Use custom firmware verification"
	default n
	help
	  Enable custom firmware verification process for NDA-covered
	  components
```

#### 4. Global site-local Makefile (`site-local/Makefile.mk`)

```makefile
# Always run the symlink target to keep links updated
site-local-target:: symlink

# Add a custom build step for the new SoC
prebuild-y += $(if $(CONFIG_SOC_NEWVENDOR_NEWTARGET), site-local-newtarget-prepare)

# Define custom build step
site-local-newtarget-prepare:
	@echo "Preparing NewTarget build environment..."
	$(MAKE) -C site-local/tools/newtarget
```

#### 5. Mainboard Kconfig (`site-local/src/mainboard/newvendor/devboard/Kconfig`)

```kconfig
if BOARD_NEWVENDOR_DEVBOARD

config BOARD_SPECIFIC_OPTIONS
	def_bool y
	select SOC_NEWVENDOR_NEWTARGET
	select BOARD_ROMSIZE_KB_16384
	select MAINBOARD_HAS_CHROMEOS
	select COMMON_CBFS_SPI_WRAPPER
	select DRIVERS_I2C_GENERIC
	select DRIVERS_USB_ACPI

config MAINBOARD_DIR
	string
	default "newvendor/devboard"

config MAINBOARD_PART_NUMBER
	string
	default "NewVendor Development Board"

config MAINBOARD_VENDOR
	string
	default "NewVendor"

config MAX_CPUS
	int
	default 8

endif # BOARD_NEWVENDOR_DEVBOARD
```

### Integration with coreboot

1. First, set up the symlinks:

```bash
cd coreboot
make symlink
```

This will create symbolic links from your `site-local` SoC
implementation into the main coreboot directory structure.

2. Configure coreboot to use your new board:

```bash
make menuconfig
```

Select:
- Mainboard → Mainboard vendor → NewVendor
- Mainboard → Mainboard model → NewVendor Development Board

3. Build coreboot with your new SoC:

```bash
make
```

### Transition to Upstream

When your SoC implementation is ready to be made public:

1. Move the code from `site-local` to the appropriate locations in the
   main coreboot tree
2. Remove the symlinks
3. Test to ensure everything still works
4. Commit the code to the main coreboot repository

This approach allows for a seamless transition from private to public
development.

## Useful Commands

- `make symlink`: Create symbolic links from `site-local` into the
  coreboot tree
- `make clean-symlink`: Remove symbolic links created by `make symlink`
- `make cleanall-symlink`: Remove all symbolic links in the coreboot
  tree

## Best Practices

1. **Version Control**: Consider keeping your `site-local` directory in
   a separate git repository
2. **Git Submodules**: You can add your `site-local` repo as a git
   submodule to your coreboot checkout
3. **Documentation**: Document your local additions within your
   `site-local` directory
4. **Organization**: Mirror coreboot's directory structure for clarity
5. **Minimal Changes**: Keep local modifications minimal to ease future
   updates

## Important Notes

- The `site-local` directory is intentionally excluded from coreboot's
  `.gitignore`
- coreboot's lint checks will fail if you try to commit the
  `site-local` directory to the main repository
- It's recommended to keep `site-local` in a separate repository and
  pull it in as needed

## Example: Complete `site-local` Setup

Here's a complete example of a `site-local` setup for a custom board:

```
coreboot/
└── site-local/
    ├── Kconfig                 # Custom Kconfig options
    ├── Makefile.mk             # Build system integration
    ├── blobs/                  # Binary blobs directory
    │   ├── board1/
    │   │   ├── descriptor.bin
    │   │   └── me.bin
    │   └── board2/
    │       └── microcode.bin
    └── src/                    # Custom source code
        └── mainboard/
            └── vendor/
                └── custom_board/
                    ├── devicetree.cb
                    ├── Kconfig
                    ├── Makefile.mk
                    └── symlink.txt  # Contains "src/mainboard/vendor/custom_board"
```

By leveraging the `site-local` mechanism effectively, you can maintain a
clean separation between upstream coreboot and your local
customizations, making it easier to update to new coreboot versions
while preserving your specific modifications.