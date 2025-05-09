# The `chip_operations` Structure in coreboot

## Introduction

The `chip_operations` structure is a fundamental component of coreboot's
chipset abstraction layer. It provides a standardized interface for chipset-
specific code to interact with coreboot's device initialization framework.
This structure enables coreboot to support a wide variety of chipsets
while maintaining a consistent initialization flow across different hardware
platforms.

In coreboot's architecture, a "chip" refers to a collection of hardware
components that form a logical unit, such as a System-on-Chip (SoC),
a CPU, or a distinct southbridge/northbridge. The `chip_operations`
structure provides the hooks necessary for coreboot to discover, configure,
and initialize these components during the boot process.

The `chip_operations` structure is particularly crucial for the ramstage
portion of coreboot, where it connects the static device tree definitions
with the actual hardware initialization code. It serves as the bridge
between the declarative device descriptions and the imperative code that
brings those devices to life.


## Structure Definition

The `chip_operations` structure is defined in `src/include/device/device.h`
as follows:

```c
struct chip_operations {
    void (*enable_dev)(struct device *dev);
    void (*init)(void *chip_info);
    void (*final)(void *chip_info);
    unsigned int initialized : 1;
    unsigned int finalized : 1;
    const char *name;
};
```


### Field Descriptions

- **enable_dev**: A function pointer that takes a `struct device*`
parameter. This function is called for each device associated with the
chip during the device enumeration phase (specifically, within the
`scan_bus` operations triggered by `dev_enumerate`). Its primary
purpose is to set up device operations (`dev->ops`) based on the
device's role in the system.

- **init**: A function pointer that takes a `void*` parameter pointing to
the chip's configuration data (typically cast to a chip-specific struct).
This function is called during the chip initialization phase
(`BS_DEV_INIT_CHIPS`), before device enumeration. It usually performs
early hardware setup needed before individual devices can be configured.

- **final**: A function pointer that takes a `void*` parameter pointing to
the chip's configuration data (typically cast to a chip-specific struct).
This function is called during the final table writing phase of coreboot
initialization (`BS_WRITE_TABLES`), after all devices have been
initialized. It performs any necessary cleanup or late initialization
operations.

- **initialized**: A bit flag indicating whether the chip's init function
has been called.

- **finalized**: A bit flag indicating whether the chip's final function
has been called.

- **name**: A string containing the human-readable name of the chip, used
for debugging and logging purposes.


## Initialization Sequence and `chip_operations`

The `chip_operations` structure integrates with coreboot's boot state
machine, which is defined in `src/lib/hardwaremain.c`. The functions in
this structure are called at specific points during the boot process:

1. **BS_DEV_INIT_CHIPS** state: The `init` function is called for each
chip in the device tree. This is handled by `dev_initialize_chips()`
which iterates through all devices, identifies unique chip instances,
and invokes their `init` functions.

2. **BS_DEV_ENUMERATE** state: During the execution of this state,
`dev_enumerate()` is called, which triggers bus scanning
(e.g., `pci_scan_bus`). Within these scan routines, the `enable_dev`
function is called for devices associated with a chip. This commonly
assigns the appropriate `device_operations` structure to each device
based on its type and purpose.

3. **BS_WRITE_TABLES** state: The `final` function is called for each
chip by `dev_finalize_chips()` after all devices have been initialized
and just before payloads are loaded.

This sequence ensures that chips can perform necessary setup before their
individual devices are configured, and also perform cleanup or finalization
after all devices have been initialized but before the final tables are
written and the payload is executed.


## Relationship Between `chip_operations` and `device_operations`

It's important to understand the distinction and relationship between
`chip_operations` and `device_operations`:

- **chip_operations**: Operates at the chipset or SoC level, providing
hooks for chip-wide initialization. It's responsible for the overall
setup of a collection of devices that belong to the same logical chip.

- **device_operations**: Operates at the individual device level,
providing functions to manage specific devices within a chip. These
operations include resource allocation, device initialization, and device-
specific functionality.

The key relationship is that `chip_operations.enable_dev` is typically
responsible for assigning the appropriate `device_operations` structure
to each device based on its type and function. This is where the bridge
between the chip-level and device-level abstractions occurs.

For example, a typical implementation of the `enable_dev` function might
look like this:

```c
static void soc_enable(struct device *dev)
{
    if (dev->path.type == DEVICE_PATH_DOMAIN)
        dev->ops = &pci_domain_ops;
    else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
        dev->ops = &cpu_bus_ops;
    else if (dev->path.type == DEVICE_PATH_GPIO)
        block_gpio_enable(dev);
    else if (dev->path.type == DEVICE_PATH_PCI &&
             dev->path.pci.devfn == PCH_DEVFN_PMC)
        dev->ops = &pmc_ops;
}
```

This function examines each device's path type and assigns the appropriate
operations based on the device's role in the system.


## Integration with the Devicetree

The `chip_operations` structure is tightly integrated with coreboot's
devicetree mechanism. The devicetree is a hierarchical description of the
hardware platform, defined in `.cb` files (typically `chipset.cb`,
`devicetree.cb`, and optionally `overridetree.cb`).

In the devicetree, a `chip` directive starts a collection of devices
associated with a particular chip driver. The path specified with the
`chip` directive corresponds to a directory in the coreboot source tree
that contains the chip driver code, including a `chip.c` file that defines
the `chip_operations` structure for that chip.

For example, a devicetree might contain:

```
chip soc/intel/cannonlake
    device domain 0 on
        device pci 00.0 on end  # Host Bridge
        device pci 12.0 on end  # Thermal Subsystem
        # ... more devices ...
    end
end
```

This connects the devices under this chip directive with the
`chip_operations` structure defined in
`src/soc/intel/cannonlake/chip.c`:

```c
struct chip_operations soc_intel_cannonlake_ops = {
    .name = "Intel Cannonlake",
    .enable_dev = &soc_enable,
    .init = &soc_init_pre_device,
};
```

During coreboot's build process, the `sconfig` utility processes the
devicetree files and generates code that links the devices defined in the
devicetree with their corresponding `chip_operations` structures.


## Chip Configuration Data

Each chip typically defines a configuration structure in a `chip.h` file
within its source directory. This structure contains configuration settings
that can be specified in the devicetree using `register` directives.

For example, a chip might define a configuration structure like:

```c
/* In src/soc/intel/cannonlake/chip.h */
struct soc_intel_cannonlake_config {
    uint8_t pcie_rp_aspm[CONFIG_MAX_ROOT_PORTS];
    uint8_t usb2_ports[16];
    uint8_t usb3_ports[10];
    /* ... more configuration options ... */
};
```

In the devicetree, you would configure these options using register
directives:

```
chip soc/intel/cannonlake
    register "pcie_rp_aspm[0]" = "ASPM_AUTO"
    register "usb2_ports[5]" = "USB2_PORT_MID(OC_SKIP)"
    # ... more register settings ...

    device domain 0 on
        # ... devices ...
    end
end
```

These configuration values are made available to the chip's `init` and
`final` functions through the `chip_info` parameter, which points to
an instance of the chip's configuration structure (after appropriate
casting from `void *`).


## Implementation Examples

### Minimal Implementation

Some chips may not need extensive initialization and can provide a
minimal implementation of the `chip_operations` structure:

```c
struct chip_operations soc_ucb_riscv_ops = {
    .name = "UCB RISC-V",
};
```

This implementation only provides a name for debugging purposes but
doesn't define any initialization functions.


### Basic Implementation with Initialization

A more typical implementation includes at least initialization hooks:

```c
struct chip_operations soc_amd_genoa_poc_ops = {
    .name = "AMD Genoa SoC Proof of Concept",
    .init = soc_init,
    .final = soc_final,
};
```

The `init` function might perform chip-wide initialization:

```c
static void soc_init(void *chip_info)
{
    default_dev_ops_root.write_acpi_tables = soc_acpi_write_tables;
    amd_opensil_silicon_init();
    data_fabric_print_mmio_conf();
    fch_init(chip_info);
}
```


### Complete Implementation

A complete implementation includes all three function pointers:

```c
struct chip_operations soc_intel_xeon_sp_cpx_ops = {
    .name = "Intel Cooper Lake-SP",
    .enable_dev = chip_enable_dev,
    .init = chip_init,
    .final = chip_final,
};
```

The `enable_dev` function would typically assign device operations
based on device types:

```c
static void chip_enable_dev(struct device *dev)
{
    /* PCI root complex */
    if (dev->path.type == DEVICE_PATH_DOMAIN)
        dev->ops = &pci_domain_ops;
    /* CPU cluster */
    else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
        dev->ops = &cpu_cluster_ops;
    /* PCIe root ports */
    else if (dev->path.type == DEVICE_PATH_PCI &&
             PCI_SLOT(dev->path.pci.devfn) == PCIE_PORT1_SLOT)
        dev->ops = &pcie_rp_ops;
    /* ... other device types ... */
}
```


### Mainboard Implementation

It's also common for the mainboard-specific code (e.g.,
`src/mainboard/vendor/board/mainboard.c`) to define its own
`chip_operations`, often named `mainboard_ops`. The `mainboard_ops.init`
can perform early board-level setup, and `mainboard_ops.enable_dev` can
assign operations for devices specific to the mainboard or set default
operations.

```c
/* Example from src/mainboard/google/zork/mainboard.c */
struct chip_operations mainboard_ops = {
       .enable_dev = mainboard_enable,
       .init = mainboard_init,
       .final = mainboard_final,
};
```


## Device Registration and Discovery

The `chip_operations` structure plays a key role in device registration
and discovery within coreboot. Here's how it fits into this process:

1. **Static Device Definition**: Devices are statically defined in the
devicetree files (`chipset.cb`, `devicetree.cb`, `overridetree.cb`).

2. **Code Generation**: The `sconfig` utility processes these files and
generates code in `build/static.c` that creates the device structures
and links them to their corresponding chip configuration data.

3. **Chip Initialization**: During the `BS_DEV_INIT_CHIPS` boot state,
`dev_initialize_chips()` calls each chip's `init` function to perform
chip-wide setup.

4. **Device Enumeration and Enabling**: During the `BS_DEV_ENUMERATE`
    boot state, `dev_enumerate()` initiates bus scanning. The scan
    functions call the associated chip's `enable_dev` function for each
    device, which assigns the appropriate device operations (`dev->ops`).

5. **Device Configuration and Initialization**: Subsequent boot states
    (`BS_DEV_RESOURCES`, `BS_DEV_ENABLE`, `BS_DEV_INIT`) configure and
    initialize the devices according to their assigned device operations.

6. **Chip Finalization**: After all devices have been initialized,
    `dev_finalize_chips()` calls each chip's `final` function during the
    `BS_WRITE_TABLES` boot state.


## Build Process Integration

The `chip_operations` structures are integrated into the coreboot build
process through several mechanisms:

1. **Devicetree Processing**: The `sconfig` utility processes the
devicetree files and generates code that creates and links the device
structures.

2. **Static Structure Declaration**: Each chip (and often the mainboard)
    defines its `chip_operations` structure in its respective `.c` file.
    These structures are collected during the build process.

3. **External References**: The generated code in `build/static.c`
includes external references to these `chip_operations` structures.

4. **Linking**: The linker collects all the `chip_operations` structures
and includes them in the final firmware image.

This process ensures that the appropriate chip operations are available
during the boot process for each chip included in the devicetree.


## Best Practices for Implementing `chip_operations`

When implementing the `chip_operations` structure for a new chip,
follow these best practices:

1. **Provide a Meaningful Name**: The `name` field should be descriptive
and identify the chip clearly for debugging purposes.

2. **Implement `enable_dev` Correctly**: The `enable_dev` function should
assign the appropriate device operations based on device types and
functions. It should handle all device types that might be part of the chip.
Consider interactions with the mainboard `enable_dev`.

3. **Use Configuration Data**: The `init` and `final` functions should
make use of the chip configuration data passed via the `chip_info`
parameter (casting it to the correct type) to configure the chip
according to the settings specified in the devicetree.

4. **Minimize Dependencies**: The `init` function should minimize
dependencies on other chips being initialized, as the order of chip
initialization is not guaranteed.

5. **Handle Resources Properly**: If the chip manages resources (memory
regions, I/O ports, etc.), ensure that these are properly allocated and
assigned to devices, usually within the associated `device_operations`.

6. **Implement Error Handling**: Include appropriate error handling in
the initialization functions to handle hardware initialization failures
gracefully.

7. **Document Special Requirements**: If the chip has special
requirements or dependencies, document these clearly in comments or
accompanying documentation.


## Troubleshooting `chip_operations` Issues

When implementing or debugging `chip_operations`, you might encounter
certain issues:

1. **Missing Device Operations**: If devices are not being initialized
properly, check that the `enable_dev` function is correctly
assigning device operations based on device types. Ensure it's being
called during bus scanning.

2. **Initialization Order Problems**: If a chip's initialization depends
on another chip being initialized first, you might need to adjust the
initialization sequence or add explicit dependencies, possibly using
boot state callbacks if necessary.

3. **Configuration Data Issues**: If chip configuration settings are not
being applied correctly, check that the configuration structure is
correctly defined in `chip.h`, that the register values in the
devicetree match the expected format, and that the `chip_info` pointer
is cast correctly in the `init`/`final` functions.

4. **Build Errors**: If you encounter build errors related to
`chip_operations`, check that the structure is correctly defined and
that all required symbols are properly exported and linked. Check for
conflicts if multiple files define the same symbol.

5. **Runtime Failures**: If the chip initialization fails at runtime,
add debug logging (using `printk`) to the `init`, `enable_dev`, and
`final` functions to identify the specific point of failure.


## Advanced `chip_operations` Patterns

### Hierarchical Chip Initialization

For complex chips with multiple components, you can implement a
hierarchical initialization pattern within the `init` function:

```c
static void soc_init(void *chip_info)
{
    /* Initialize common components first */
    common_init(chip_info);

    /* Initialize specific blocks */
    pcie_init(chip_info);
    usb_init(chip_info);
    sata_init(chip_info);

    /* Final SoC-wide configuration */
    power_management_init(chip_info);
}
```


### Variant Support

For chips with multiple variants, you can implement variant detection
and specific initialization within the `init` function:

```c
static void soc_init(void *chip_info)
{
    uint32_t variant = read_chip_variant();

    /* Common initialization */
    common_init(chip_info);

    /* Variant-specific initialization */
    switch (variant) {
    case VARIANT_A:
        variant_a_init(chip_info);
        break;
    case VARIANT_B:
        variant_b_init(chip_info);
        break;
    default:
        printk(BIOS_WARNING, "Unknown variant %u\\n", variant);
        break;
    }
}
```


### Conditional Feature Initialization

You can conditionally initialize features based on configuration settings
passed via `chip_info`:

```c
static void soc_init(void *chip_info)
{
    struct soc_config *config = chip_info;

    /* Always initialize core components */
    core_init();

    /* Conditionally initialize optional features */
    if (config->enable_xhci)
        xhci_init(config);

    if (config->enable_sata)
        sata_init(config);

    if (config->enable_pcie)
        pcie_init(config);
}
```


## Conclusion

The `chip_operations` structure is a fundamental component of coreboot's
chipset abstraction layer. It provides a standardized interface for chipset-
specific code to interact with coreboot's device initialization framework,
enabling support for a wide variety of chipsets while maintaining a
consistent initialization flow.

By implementing the `chip_operations` structure for a specific chipset
(and often for the mainboard), developers can integrate their
hardware-specific code with coreboot's device enumeration, configuration,
and initialization process. This structure serves as the bridge between
the declarative device descriptions in the devicetree and the imperative
code that initializes the hardware.

Understanding the `chip_operations` structure and its role in the
coreboot boot process is essential for anyone working on chipset or
mainboard support in coreboot. By following the best practices and
patterns outlined in this document, developers can create robust and
maintainable hardware support code that integrates seamlessly with the
coreboot firmware ecosystem.
