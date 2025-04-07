# Device Operations in coreboot Firmware

## Introduction

The `device_operations` structure is a cornerstone of coreboot's
hardware abstraction layer. It represents a set of function pointers
defining how the firmware interacts with a specific hardware device
during various boot stages. This structure lets the coreboot
architecture establish a consistent interface for device initialization,
configuration, resource allocation, and ACPI table generation, while
allowing device-specific implementations behind this common interface.

At its core, `device_operations` applies the strategy pattern in
systems programming. It decouples algorithms (device operations) from
the core boot sequence, allowing devices to define their own behavior
while the boot process follows a predictable flow. This pattern enables
coreboot to support a wide range of hardware platforms with minimal
changes to the core boot sequence code.


## Structure Definition

The `device_operations` structure, defined in
`src/include/device/device.h`, consists of several function pointers,
each representing a specific operation performed on a device during
boot:

```c
struct device_operations {
    void (*read_resources)(struct device *dev);
    void (*set_resources)(struct device *dev);
    void (*enable_resources)(struct device *dev);
    void (*init)(struct device *dev);
    void (*final)(struct device *dev);
    void (*scan_bus)(struct device *bus);
    void (*enable)(struct device *dev);
    void (*vga_disable)(struct device *dev);
    void (*reset_bus)(struct bus *bus);

    int (*get_smbios_data)(struct device *dev, int *handle,
        unsigned long *current);
    void (*get_smbios_strings)(struct device *dev, struct smbios_type11 *t);

    unsigned long (*write_acpi_tables)(const struct device *dev,
        unsigned long start, struct acpi_rsdp *rsdp);
    void (*acpi_fill_ssdt)(const struct device *dev);
    const char *(*acpi_name)(const struct device *dev);
    const char *(*acpi_hid)(const struct device *dev);

    const struct pci_operations *ops_pci;
    const struct i2c_bus_operations *ops_i2c_bus;
    const struct spi_bus_operations *ops_spi_bus;
    const struct smbus_bus_operations *ops_smbus_bus;
    const struct pnp_mode_ops *ops_pnp_mode;
    const struct gpio_operations *ops_gpio;
    const struct mdio_bus_operations *ops_mdio;
};
```


### Core Resource Management Functions

*   `read_resources`: Discovers and collects resources required by the
    device (I/O ports, memory regions, IRQs, etc.). This typically
    involves reading configuration registers or static device tree
    information.
*   `set_resources`: Assigns finalized resources to the device after the
    resource allocation phase. This writes the assigned base addresses,
    lengths, and other parameters back to the device structure, but not
    necessarily to hardware registers yet.
*   `enable_resources`: Activates the assigned resources in the device's
    hardware registers (e.g., writing to PCI BARs, enabling memory
    decoding).


### Device Lifecycle Functions

*   `init`: Performs device-specific initialization, often requiring
    access to the device's assigned resources. This is called after
    resources have been enabled.
*   `final`: Executes final setup or cleanup operations before the
    payload is loaded. This is useful for tasks that depend on other
    devices being initialized.
*   `enable`: Enables the device in the hardware, often setting bits in
    configuration registers to make the device active. Called after
    `enable_resources`.


### Bus Management Functions

*   `scan_bus`: Enumerates child devices on a bus device (e.g., scanning
    a PCI bus for devices, probing I2C devices). Only applicable to
    devices that act as buses.
*   `reset_bus`: Resets all devices on a specific bus.
*   `vga_disable`: Disables VGA decoding on a PCI device when another VGA
    device is active. Used to manage legacy VGA resources.


### System Table Generation Functions

*   `get_smbios_data`: Provides SMBIOS data specific to the device for
    Type 9 (System Slots) or Type 41 (Onboard Devices Extended
    Information).
*   `get_smbios_strings`: Supplies string information for SMBIOS tables,
    often related to the data provided by `get_smbios_data`.
*   `write_acpi_tables`: Generates device-specific ACPI tables (like SSDTs)
    or contributes data to system-wide tables.
*   `acpi_fill_ssdt`: Adds device-specific objects (scopes, methods, data)
    to the Secondary System Description Table (SSDT).
*   `acpi_name`: Returns the ACPI name for the device (e.g.,
    `\_SB.PCI0.GFX0`). This defines the device's path in the ACPI
    namespace.
*   `acpi_hid`: Returns the ACPI Hardware ID (HID) for the device (e.g.,
    `PNP0A08`). Used by the OS to match drivers.


### Bus-Specific Operation Pointers

These fields point to bus-specific operation structures when a device
functions as a bus controller (or exposes bus-like functionality). See
the "Bus-Specific Operations" section for details.

*   `ops_pci`: Operations for PCI configuration space access.
*   `ops_i2c_bus`: Operations for I2C bus transactions (read, write,
    transfer).
*   `ops_spi_bus`: Operations for SPI bus transactions.
*   `ops_smbus_bus`: Operations for SMBus transactions.
*   `ops_pnp_mode`: Operations for Plug-and-Play device configuration.
*   `ops_gpio`: Operations for GPIO control (get, set, configure
    direction/pulls).
*   `ops_mdio`: Operations for MDIO (Management Data Input/Output) bus
    access, used for Ethernet PHYs.


## Device Lifecycle in coreboot

The function pointers in `device_operations` are called at specific
stages during the boot process, following a sequence defined in
coreboot's boot state machine (`src/lib/hardwaremain.c`). Understanding
this lifecycle helps developers implement appropriate behavior for each
function pointer.


### Boot Sequence and Device Operations

coreboot's main device initialization sequence involves these boot
states:

1.  **BS_DEV_INIT_CHIPS** (`dev_initialize_chips()`): Initializes chip
    drivers (`chip_operations`).
2.  **BS_DEV_ENUMERATE** (`dev_enumerate()`): Discovers and enumerates
    devices.
    *   Calls `scan_bus()` for each bus to detect child devices.
3.  **BS_DEV_RESOURCES** (`dev_configure()`): Allocates resources across
    all enumerated devices.
    *   Calls `read_resources()` for each device to discover required
        resources.
    *   Calls `set_resources()` for each device to assign allocated
        resources back to the `struct device`.
4.  **BS_DEV_ENABLE** (`dev_enable()`): Enables devices and their
    resources.
    *   Calls `enable_resources()` for each device to activate assigned
        resources in hardware.
    *   Calls `enable()` for each device to perform general hardware
        enablement.
5.  **BS_DEV_INIT** (`dev_initialize()`): Initializes devices.
    *   Calls `init()` for each device to perform device-specific setup.
6.  **BS_POST_DEVICE** (`dev_finalize()`): Finalizes devices before
    payload loading.
    *   Calls `final()` for each device for any final cleanup or setup.

The sequence is primarily driven by the `boot_states` array in
`src/lib/hardwaremain.c`:

```c
static struct boot_state boot_states[] = {
    /* ... other states ... */
    BS_INIT_ENTRY(BS_PRE_DEVICE, bs_pre_device),
    BS_INIT_ENTRY(BS_DEV_INIT_CHIPS, bs_dev_init_chips),
    BS_INIT_ENTRY(BS_DEV_ENUMERATE, bs_dev_enumerate),
    BS_INIT_ENTRY(BS_DEV_RESOURCES, bs_dev_resources),
    BS_INIT_ENTRY(BS_DEV_ENABLE, bs_dev_enable),
    BS_INIT_ENTRY(BS_DEV_INIT, bs_dev_init),
    BS_INIT_ENTRY(BS_POST_DEVICE, bs_post_device),
    /* ... other states ... */
};
```

Later stages include ACPI and SMBIOS table generation, where functions
like `write_acpi_tables()`, `acpi_fill_ssdt()`, `get_smbios_data()`, and
`get_smbios_strings()` are invoked as part of the table construction
process.


## Inheritance and Code Reuse Patterns

The `device_operations` structure enables several patterns for code
reuse:


### 1. Default Implementations

coreboot provides default implementations for common device types (like
root devices, PCI devices, PCI bridges), which can be used directly or
extended. Chip or mainboard code often assigns these defaults if no
specific driver is found.

```c
/* From src/device/root_device.c */
struct device_operations default_dev_ops_root = {
    .read_resources   = noop_read_resources,
    .set_resources    = noop_set_resources,
    .scan_bus         = scan_static_bus,
    .reset_bus        = root_dev_reset,
#if CONFIG(HAVE_ACPI_TABLES)
    .acpi_name        = root_dev_acpi_name,
#endif
};
```


### 2. No-op Functions

Simple shim functions (often static inline) are provided for cases where
a device doesn't need to implement specific operations. Using these avoids
leaving function pointers NULL.

```c
/* From src/include/device/device.h */
static inline void noop_read_resources(struct device *dev) {}
static inline void noop_set_resources(struct device *dev) {}
```


### 3. Chain of Responsibility / Delegation

Some implementations delegate to parent devices or use helper functions
when they can't handle an operation themselves or when common logic can
be shared. For example, ACPI name generation often traverses up the
device tree.

```c
/* Simplified example logic */
const char *acpi_device_name(const struct device *dev)
{
    const char *name = NULL;
    const struct device *pdev = dev;

    /* Check for device specific handler */
    if (dev->ops && dev->ops->acpi_name) {
        name = dev->ops->acpi_name(dev);
        if (name)
            return name; /* Device handled it */
    }

    /* Walk up the tree to find if any parent can provide a name */
    while (pdev->upstream && pdev->upstream->dev) {
        pdev = pdev->upstream->dev;
        if (pdev->ops && pdev->ops->acpi_name) {
            /* Note: Parent's acpi_name might handle the original child 'dev' */
            name = pdev->ops->acpi_name(dev);
            if (name)
                return name; /* Parent handled it */
        }
    }

    /* Fallback or default logic if needed */
    return NULL;
}
```
This pattern allows parent devices (like buses) to provide default
behavior or naming schemes if a child device doesn't specify its own.


## Implementation Examples

These examples show typical `device_operations` assignments. Actual
implementations might involve more conditional compilation based on
Kconfig options.


### PCI Device Operations (Default)

```c
/* From src/device/pci_device.c */
struct device_operations default_pci_ops_dev = {
    .read_resources   = pci_dev_read_resources,
    .set_resources    = pci_dev_set_resources,
    .enable_resources = pci_dev_enable_resources,
#if CONFIG(HAVE_ACPI_TABLES)
    .write_acpi_tables = pci_rom_write_acpi_tables,
    .acpi_fill_ssdt    = pci_rom_ssdt,
#endif
    .init             = pci_dev_init,
    /* Assigns PCI-specific operations */
    .ops_pci          = &pci_dev_ops_pci,
};
```


### CPU Cluster Operations

```c
/* From src/soc/intel/alderlake/chip.c (representative example) */
static struct device_operations cpu_bus_ops = {
    .read_resources   = noop_read_resources,
    .set_resources    = noop_set_resources,
    .enable_resources = cpu_set_north_irqs,
#if CONFIG(HAVE_ACPI_TABLES)
    .acpi_fill_ssdt = cpu_fill_ssdt,
#endif
    /* CPU clusters often don't need scan_bus, init, etc. */
};
```


### GPIO Controller Operations

```c
/* From src/soc/intel/common/block/gpio/gpio_dev.c */
static struct gpio_operations gpio_ops = {
    .get         = gpio_get,
    .set         = gpio_set,
    /* ... other GPIO functions ... */
};

struct device_operations block_gpio_ops = {
    .read_resources = noop_read_resources,
    .set_resources  = noop_set_resources,
    /* Assigns GPIO-specific operations */
    .ops_gpio      = &gpio_ops,
};
```


## Registration and Discovery

How are `device_operations` structures associated with `struct device`
instances?


### 1. Static Assignment (via `chip_operations`)

For devices known at build time (defined in devicetree.cb), the
`device_operations` structure is often assigned in the SOC's or
mainboard's `chip_operations->enable_dev()` function based on the
device path type or other properties.

```c
/* Example from src/soc/intel/alderlake/chip.c */
static void soc_enable(struct device *dev)
{
    /* Assign ops based on the device's role in the tree */
    if (dev->path.type == DEVICE_PATH_DOMAIN)
        dev->ops = &pci_domain_ops; /* Handles PCI domain resources */
    else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
        dev->ops = &cpu_bus_ops;   /* Handles CPU cluster setup */
    else if (dev->path.type == DEVICE_PATH_GPIO)
        block_gpio_enable(dev); /* Assigns block_gpio_ops */
    /* ... other assignments for specific PCI devices, etc. ... */
}
```
The `enable_dev` function is part of `struct chip_operations`, which
handles broader chip-level initialization (see "Relationship with
`chip_operations`" section).


### 2. Dynamic Detection (PCI Drivers)

For PCI devices discovered during bus scanning (`scan_bus`), coreboot
looks through a list of registered PCI drivers (`_pci_drivers` array)
to find one matching the device's vendor and device IDs.

```c
/* Logic from src/device/pci_device.c::set_pci_ops() */
static void set_pci_ops(struct device *dev)
{
    struct pci_driver *driver;

    /* Check if ops already assigned (e.g., by chip_ops->enable_dev) */
    if (dev->ops)
        return;

    /* Look through registered PCI drivers */
    for (driver = &_pci_drivers[0]; driver != &_epci_drivers[0]; driver++) {
        if ((driver->vendor == dev->vendor) &&
            device_id_match(driver, dev->device)) {
            /* Found a matching driver, assign its ops */
            dev->ops = (struct device_operations *)driver->ops;
            printk(BIOS_SPEW, "%s: Assigned ops from driver for %04x:%04x\n",
                   dev_path(dev), driver->vendor, driver->device);
            return; /* Stop searching */
        }
    }

    /* Fall back to default operations if no specific driver found */
    if (!dev->ops) {
        if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_BRIDGE) {
            dev->ops = get_pci_bridge_ops(dev); /* Special ops for bridges */
        } else {
            dev->ops = &default_pci_ops_dev; /* Default for normal devices */
        }
        printk(BIOS_SPEW, "%s: Assigned default PCI ops\n", dev_path(dev));
    }
}
```


## Build Process Integration

The `device_operations` structures are integrated into the coreboot
build process:

1.  **Static Device Tree**: The mainboard's `devicetree.cb` defines the
    initial device hierarchy. The build process converts this into C
    code (`static.c`) containing `struct device` instances.
2.  **PCI Driver Registration**: PCI drivers (containing their own
    `device_operations`) register themselves using the `__pci_driver`
    linker set macro.

    ```c
    /* Example pattern */
    struct pci_driver example_pci_driver __pci_driver = {
        .ops    = &example_device_ops, /* Pointer to device_operations */
        .vendor = VENDOR_ID,
        .device = DEVICE_ID, /* Or .devices for a list */
    };
    ```
3.  **Linking**: The build system collects all structures placed in the
    `__pci_driver` section and creates the `_pci_drivers` array used by
    `set_pci_ops()`. It ensures all necessary code (default ops, driver
    ops, core device functions) is linked into the final firmware image.


## Relationship with `chip_operations`

It's important to distinguish `device_operations` from
`chip_operations` (defined in `src/include/chip.h`).

*   `chip_operations`: Defines operations related to the overall chipset
    or mainboard logic. It includes functions called earlier in the boot
    process, like `enable_dev`, `init`, and `final`.
    *   `chip_operations->enable_dev()` is crucial as it often performs
        initial setup for static devices and is the primary place where
        `device_operations` pointers are *assigned* for non-PCI devices
        based on their path or type.
    *   `chip_operations->init()` runs during `BS_DEV_INIT_CHIPS`, before
        most `device_operations` functions.
*   `device_operations`: Defines operations for *individual* devices
    within the device tree. These are called *after* the corresponding
    `chip_operations` stage and operate on a specific `struct device`.

Essentially, `chip_operations` sets the stage at the SoC/mainboard level,
including assigning the correct `device_operations` to static devices,
while `device_operations` handles the specific actions for each device
later in the boot process.


## Bus-Specific Operations

The `ops_*` pointers within `struct device_operations` (e.g., `ops_pci`,
`ops_i2c_bus`, `ops_spi_bus`, `ops_gpio`) provide a way for devices that
act as bus controllers or expose bus-like interfaces to offer
standardized access methods.

*   **Purpose:** They abstract the low-level details of interacting with
    that specific bus type. For example, a PCI host bridge device will
    implement `struct pci_operations` via its `ops_pci` pointer,
    allowing other code to perform PCI config reads/writes through it
    without knowing the exact hardware mechanism. Similarly, an I2C
    controller device implements `struct i2c_bus_operations` via
    `ops_i2c_bus` to provide standard `read`, `write`, and `transfer`
    functions for that bus segment.
*   **Usage:** Code needing to interact with a bus first finds the
    controller `struct device` in the tree, then accesses the relevant
    bus operations through the appropriate `ops_*` pointer, passing the
    target address or parameters. For instance, to talk to an I2C device
    at address `0x50` on the bus controlled by `i2c_controller_dev`, one
    might call:
    `i2c_controller_dev->ops->ops_i2c_bus->transfer(...)`. Helper
    functions often wrap this access pattern.
*   **Implementation:** The structures like `struct pci_operations`,
    `struct i2c_bus_operations`, etc., are defined in corresponding
    header files (e.g., `src/include/device/pci_ops.h`,
    `src/include/drivers/i2c/i2c_bus.h`). Devices acting as controllers
    provide concrete implementations of these functions, tailored to their
    hardware.

This mechanism allows coreboot to manage diverse bus types using a
consistent device model, where the controller device itself exposes the
necessary functions for interacting with devices on its bus.


## Best Practices

When implementing `device_operations`:

1.  **Leverage Defaults/No-ops**: Use default or no-op implementations
    whenever possible. Only override functions that require custom
    behavior for your specific device.
2.  **Error Handling**: Check return values from functions called within
    your ops implementations and handle errors gracefully (e.g., log an
    error, return an error code if applicable).
3.  **Resource Management**: In `read_resources`, accurately declare all
    resources (MMIO, I/O ports, IRQs) your device needs, specifying
    flags like fixed vs. alignment, or bridge vs. standard device.
    Incorrect resource declaration is a common source of issues.
4.  **Initialization Order**: Be mindful of dependencies in `init`. If
    your device relies on another device being fully initialized, consider
    deferring that part of the initialization to the `final` callback,
    which runs later.
5.  **Minimal Implementation**: Only implement the functions relevant to
    your device type. A simple MMIO device might only need
    `read_resources`, `set_resources`, `enable_resources`, and perhaps
    ACPI functions. A bus device additionally needs `scan_bus`.
6.  **Bus Operations**: If implementing a bus controller, correctly
    implement the corresponding bus operations structure (e.g.,
    `struct pci_operations`, `struct i2c_bus_operations`) and assign it
    to the appropriate `ops_*` field.
7.  **ACPI/SMBIOS**: If the device needs OS visibility via ACPI or
    SMBIOS, implement the relevant functions (`acpi_name`, `acpi_hid`,
    `acpi_fill_ssdt`, `get_smbios_data`, etc.). Ensure ACPI names and
    HIDs are correct according to specifications and platform needs.


## Logging and Debugging

Use coreboot's logging facilities (`printk`) within your `device_operations`
functions to provide visibility during development and debugging. Use
appropriate log levels (e.g., `BIOS_DEBUG`, `BIOS_INFO`, `BIOS_ERR`).

```c
static void example_device_init(struct device *dev)
{
    printk(BIOS_DEBUG, "%s: Initializing device at %s\n", __func__,
           dev_path(dev));

    /* ... Device initialization code ... */
    if (/* some condition */) {
        printk(BIOS_SPEW, "%s: Condition met, applying setting X\n",
               dev_path(dev));
        /* ... */
    }

    if (/* error condition */) {
        printk(BIOS_ERR, "%s: Failed to initialize feature Y!\n",
               dev_path(dev));
        /* Handle error */
    }

    printk(BIOS_DEBUG, "%s: Initialization complete for %s\n", __func__,
           dev_path(dev));
}
```
Consistent logging helps trace the boot process and pinpoint where issues
occur.


## Common Troubleshooting

*   **Missing Resource Declarations**:
    *   *Problem*: Device fails to function, or conflicts arise because a
        required resource (MMIO range, I/O port, IRQ) was not declared
        in `read_resources`. The resource allocator is unaware of the
        need.
    *   *Solution*: Verify that `read_resources` correctly calls functions
        like `pci_dev_read_resources` or manually adds all necessary
        resources using functions like `mmio_resource()`,
        `io_resource()`, etc. Check PCI BARs or device datasheets.
*   **Initialization Order Issues**:
    *   *Problem*: `init()` fails because it depends on another device
        that hasn't been fully initialized yet (e.g., accessing a shared
        resource like SMBus before the SMBus controller is ready).
    *   *Solution*: Move the dependent initialization code to the `final`
        callback if possible. Alternatively, ensure the dependency is met
        by careful ordering in the device tree or using boot state
        callbacks if necessary for complex scenarios.
*   **Resource Conflicts**:
    *   *Problem*: Boot fails during resource allocation, or devices
        misbehave because multiple devices requested the same
        non-sharable resource (e.g., conflicting fixed MMIO regions).
    *   *Solution*: Review resource declarations in `read_resources` across
        all relevant devices. Ensure fixed resources don't overlap. Check
        if bridge windows are correctly defined and large enough. Use
        coreboot's resource reporting logs to identify overlaps.
*   **ACPI Table Generation Errors**:
    *   *Problem*: The operating system fails to recognize the device,
        assigns the wrong driver, or the device doesn't function correctly
        (e.g., power management issues).
    *   *Solution*: Double-check the `acpi_name`, `acpi_hid`, `_CRS`
        (generated from assigned resources), and `acpi_fill_ssdt`
        implementations. Verify names match the ACPI hierarchy and HIDs
        match expected driver bindings. Ensure SSDT methods correctly
        access hardware. Use OS debugging tools (e.g., `acpidump`, Device
        Manager errors) to diagnose.
*   **Incorrect `ops` Pointer Assigned**:
    *   *Problem*: Device behaves incorrectly because the wrong
        `device_operations` structure was assigned (e.g., default PCI ops
        assigned to a device needing a specific driver's ops).
    *   *Solution*: Check the logic in `chip_operations->enable_dev` (for
        static devices) or the PCI driver registration (`__pci_driver`
        macro and `set_pci_ops` fallback logic) to ensure the correct
        `ops` structure is being selected and assigned based on device
        type, path, or PCI ID. Add debug prints to verify which `ops`
        structure is assigned.


## Advanced Usage

### Complex Device Hierarchies

For devices with non-standard interactions or complex initialization,
custom `device_operations` can be created, often inheriting from defaults
but overriding specific functions.

```c
static void advanced_device_init(struct device *dev)
{
    /* First, perform standard PCI init */
    pci_dev_init(dev);

    /* Then, add custom initialization steps */
    printk(BIOS_DEBUG, "%s: Performing advanced init\n", dev_path(dev));
    /* ... custom register writes, configuration ... */
}

static const char *advanced_device_acpi_name(const struct device *dev)
{
    /* Provide a custom ACPI name based on some property */
    if (/* condition */)
        return "ADV0001";
    else
        return "ADV0002";
}

/* Combine default and custom operations */
static struct device_operations advanced_device_ops = {
    /* Inherit resource handling from default PCI ops */
    .read_resources   = pci_dev_read_resources,
    .set_resources    = pci_dev_set_resources,
    .enable_resources = pci_dev_enable_resources,

    /* Override init */
    .init             = advanced_device_init,

    /* Override ACPI naming */
    .acpi_name        = advanced_device_acpi_name,
    /* Other functions might use defaults or no-ops */
};
```

### Dynamic Configuration based on Probing

Some `init` or other op implementations might probe the device's
capabilities or read configuration data (e.g., from SPD, VPD, or straps)
and alter their behavior accordingly.

```c
static void conditional_device_init(struct device *dev)
{
    uint8_t feature_flags;

    /* Read capability register from the device */
    feature_flags = pci_read_config8(dev, EXAMPLE_CAP_REG);

    printk(BIOS_DEBUG, "%s: Feature flags: 0x%02x\n", dev_path(dev),
           feature_flags);

    /* Conditional initialization based on detected features */
    if (feature_flags & FEATURE_X_ENABLED) {
        printk(BIOS_INFO, "%s: Initializing Feature X\n", dev_path(dev));
        init_feature_x(dev);
    }
    if (feature_flags & FEATURE_Y_ENABLED) {
        printk(BIOS_INFO, "%s: Initializing Feature Y\n", dev_path(dev));
        init_feature_y(dev);
    }
}
```


## Conclusion

The `device_operations` structure is a powerful abstraction mechanism in
coreboot. It enables consistent handling of diverse hardware while
allowing for device-specific behavior. By providing a standard interface
for device operations throughout the boot process, it simplifies the
codebase, enhances maintainability, and provides the extensibility needed
to support new hardware platforms.

Understanding this structure, its relationship with `chip_operations`,
and its role in the boot process is essential for coreboot developers,
particularly when adding support for new devices or debugging hardware
initialization issues. By following the patterns and best practices
outlined here, developers can create robust and reusable device driver
implementations that integrate smoothly into the coreboot architecture.
