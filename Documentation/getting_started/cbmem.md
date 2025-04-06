# CBMEM high table memory manager

## Introduction

CBMEM (coreboot memory) is a dynamic memory infrastructure used in
coreboot to store runtime data structures, logs, and other information
that needs to persist across different boot stages, and even across warm
boots. CBMEM is crucial for maintaining state and information through
the coreboot boot process.

Its key responsibilities include:
- Providing a stable storage area for critical boot data
- Managing console logging
- Storing configuration tables for hand off to payloads
- Maintaining timestamps for performance analysis
- Preserving boot state during S3 suspend/resume cycles
- Storing data such as ACPI tables, SMBIOS tables and coreboot tables
  which are used at runtime


## Creation and Placement

CBMEM is initialized by coreboot during romstage, but is used mainly in
ramstage for storing any data that needs to be saved for more than a
short period of time.

For 32-bit builds, CBMEM is typically located at the highest usable DRAM
address below the 4GiB boundary. For 64-bit builds, while there is no
strict upper limit, it is advisable to follow the same guidelines to
prevent access or addressing issues. Regardless of the build type, the
CBMEM address must remain consistent between romstage and ramstage.

Each platform may need to implement its own method for determining the
`cbmem_top` address, as this can depend on specific hardware
configurations and memory layouts.


## Architecture Overview

Each CBMEM region is identified by a unique ID, allowing different
components to store and retrieve their data during runtime.

Upon creating an entry, a block of memory of the requested size is
allocated from the reserved CBMEM region. This region typically persists
across warm reboots, making it useful for debugging and passing
information to payloads.

CBMEM is implemented as a specialized in-memory database (IMD) system
that grows downward from a defined upper memory limit. This means that
only the latest added entry may be removed.

```text
High Memory
+----------------------+ <- cbmem_top
|  Root Pointer        |
|----------------------|
|  Root Structure      |
|----------------------|
|  Entry 1             |
|----------------------|
|  Entry 2             |
|----------------------|
|  ...                 |
|  (grows downward)    |
+----------------------+
```


### Core Components

The CBMEM system consists of several key components:

1. **Root Pointer**: Located at the very top of CBMEM memory space,
   contains a magic number and offset to the Root Structure

2. **Root Structure**: Contains metadata about the CBMEM region,
   including:
   - Entry alignment requirements
   - Maximum number of entries
   - Current number of entries
   - Address of the lowest allocated memory

3. **Entries**: Individual allocations within CBMEM, identified by:
   - 32-bit ID (often encoding ASCII characters for readability)
   - Size information
   - Magic validation value

4. **IMD Implementation**: The underlying memory management system
   that handles allocation, deallocation, and entry management


## Memory Layout and Initialization

CBMEM is positioned at the top of available system RAM, typically
just below the 4GiB boundary for 32-bit builds. This placement
ensures compatibility with legacy code while allowing CBMEM to
retain its contents across warm resets.


### CBMEM Location Determination

Each platform must implement a `cbmem_top_chipset()` function
that returns the physical address where CBMEM should be located.
This address must be consistent across coreboot stages and is
determined based on:

- Available physical memory
- Architecture-specific constraints
- BIOS/chipset requirements


### Initialization Process

CBMEM is initialized in a multi-step process:

1. **Early Initialization**: A small console buffer is created in during
   bootblock and romstage

2. **RAM Initialization**: The full CBMEM area is established in
   ramstage

3. **Normal Operation**:
   - In a normal boot, CBMEM is created fresh
   - Size and alignment values are specified
   - Initial entries are allocated

4. **Recovery Operation**:
   - During S3 resume, CBMEM structure is recovered from memory
   - Content is validated using magic numbers and checksums
   - All existing entries remain accessible


## Entry Management

CBMEM entries are identified by a 32-bit ID, often encoding ASCII
characters to indicate their purpose (for example, "CNSL" for console).

### Entry Creation

```c
void *cbmem_add(u32 id, u64 size);
```

This function:
- Searches for an existing entry with the given ID
- If found, returns the existing entry (size is ignored)
- If not found, allocates a new entry of the requested size
- Returns a pointer to the entry's data area


### Entry Access

```c
void *cbmem_find(u32 id);
```

This function:
- Searches for an entry with the specified ID
- Returns a pointer to the entry's data area if found
- Returns NULL if the entry does not exist


### Entry Removal

```c
int cbmem_entry_remove(const struct cbmem_entry *entry);
```

This function:
- Removes the specified entry if it was the last one added
- Returns 0 on success, negative value on failure
- Note: Due to the downward-growing design, only the most recently
  added entry can be removed


## CBMEM Console Implementation

The CBMEM console is a circular buffer used for capturing log messages
across all boot stages. It is one of the most widely used CBMEM
features. The size of this buffer is determined by the
`CONFIG_CBMEM_CONSOLE_SIZE` Kconfig option.


### Console Structure

```c
struct cbmem_console {
    u32 size;            // Size of the buffer
    u32 cursor;          // Current write position and flags
    u8  body[];          // Actual data buffer
};
```

Key features:
- The high bit of `cursor` indicates overflow condition
- Only the lower 28 bits of `cursor` are used as position
- Supports ring-buffer operation when full


### Console Operation

1. **Initialization**: A console entry is created in CBMEM with ID
   `CBMEM_ID_CONSOLE` (0x434f4e53 - 'CONS')

2. **Writing**: Log messages are written byte-by-byte using
   `cbmemc_tx_byte()` which:
   - Adds data to the current cursor position
   - Advances the cursor
   - Sets the overflow flag when wrapping around

3. **Stage Transition**: When transitioning between boot stages:
   - Pre-RAM console contents are copied to the main CBMEM console
   - Any overflow condition is preserved and noted
   - The process ensures no log messages are lost


## Common CBMEM Entry Types

CBMEM contains various data structures used by different coreboot
components:

- **Console**: Log messages from all boot stages (`CBMEM_ID_CONSOLE`)
- **Timestamps**: Performance metrics (`CBMEM_ID_TIMESTAMP`)
- **ACPI Tables**: ACPI data for OS handoff (`CBMEM_ID_ACPI`)
- **coreboot Tables**: System information (`CBMEM_ID_CBTABLE`)
- **Memory Information**: RAM configuration (`CBMEM_ID_MEMINFO`)
- **Stage Cache**: Code/data for faster S3 resume
- **ROM/CBFS Cache**: Cached ROM content
- **Vendor-specific**: Platform-dependent structures

A complete list of IDs is defined in
`src/commonlib/bsd/include/commonlib/bsd/cbmem_id.h`.


## Integration with Boot Stages

CBMEM interacts differently with each coreboot boot stage.


### Bootblock/Romstage

- Uses cache-as-RAM for temporary console storage
- Limited CBMEM functionality before RAM initialization
- Sets up initial timestamp entries


### Ramstage

- Full CBMEM initialization or recovery
- All entries become accessible
- Most coreboot subsystems interact with CBMEM
- Console logging is fully operational


### Payload Handoff

- CBMEM contents are preserved when transferring to the payload
- Entries are made available via coreboot tables
- Common payloads (SeaBIOS, GRUB, etc.) can access CBMEM data


## Platform-Specific Considerations

Different platforms have unique requirements for CBMEM implementation.


### x86 Platforms

- CBMEM typically located just below 4GiB
- Often integrates with ACPI resume and SMM operations
- May need to accommodate memory reserved by legacy components


### ARM/RISC-V Platforms

- More flexibility in CBMEM placement
- Must coordinate with platform-specific memory controllers
- Memory topology can vary significantly between implementations


## CBMEM Hooks

CBMEM provides a hook mechanism to allow subsystems to perform
initialization or recovery operations when CBMEM becomes available:

```c
CBMEM_CREATION_HOOK(hook_function);    // First-time creation only
CBMEM_READY_HOOK(hook_function);       // Any CBMEM initialization
CBMEM_READY_HOOK_EARLY(hook_function); // Early CBMEM initialization
```

These macros register functions to be called when CBMEM is initialized,
allowing components to set up their CBMEM entries at the appropriate time.


## Debugging and Utilities

### CBMEM Utility

The `cbmem` utility provides direct access to CBMEM contents on a
running system. It needs to be built from the coreboot source tree using
`make -C util/cbmem`. Common uses include:

- Listing all CBMEM entries (`cbmem -l`)
- Viewing console logs (`cbmem -c`)
- Analyzing timestamps (`cbmem -t`)
- Extracting specific entries by ID (`cbmem -e <ID>`)


### Debugging Techniques

- CBMEM console contents can be dumped to UART for debugging if serial
  output is enabled.
- The console overflow flag (`cbmem -c` output) helps identify if logs
  were truncated.
- Size validation within CBMEM helps detect potential memory corruption.
- Magic numbers provide integrity validation for CBMEM structures.
- Enabling `CONFIG_CBMEM_CHECKS` in Kconfig adds extra sanity checks
  that can help catch issues during development.


## Limitations

While CBMEM is a powerful tool, it has some inherent limitations:

- **Downward Growth**: The stack-like allocation (growing downwards)
  means that only the most recently added entry can be removed. This
  prevents fragmentation but limits flexibility in freeing memory.
- **Fixed Size**: Once CBMEM is initialized in ramstage, its total size
  and top address (`cbmem_top`) are fixed. Entries cannot be resized
  after allocation.
- **Platform Complexity**: Determining the correct `cbmem_top` can be
  complex on some platforms due to varying memory maps and reserved
  regions.


## Best Practices for Developers

When working with the CBMEM subsystem:

1. **Alignment**: Always respect the alignment requirements of the
   CBMEM tier (`IMD_ROOT` vs `IMD_SMALL`) you are using.

2. **ID Selection**: Use unique, meaningful IDs for new entries,
   preferably encoding ASCII characters for readability (see
   `cbmem_id.h`).

3. **Size Estimation**: Allocate sufficient space initially, as
   entries cannot be resized.

4. **Memory Conservation**: Be mindful of limited memory resources,
   especially on constrained platforms. Avoid storing excessively large
   data structures in CBMEM unless necessary.

5. **Persistence**: Remember that CBMEM contents persist across
   warm reboots (like S3 resume) but not across full system resets
   (cold boots).

6. **Entry Ordering**: Consider that only the most recently added
   entry can be removed, which might influence your allocation strategy
   if you anticipate needing to free space.
