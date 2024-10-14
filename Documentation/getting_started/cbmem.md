# CBMEM high table memory manager

## Introduction
CBMEM (coreboot memory) is a dynamic memory infrastructure used in
coreboot to store runtime data structures, logs, and other information
that needs to persist across different boot stages. CBMEM is crucial
for maintaining state and logging information across different stages
of the coreboot boot process.

## Creation and Placement
CBMEM is initialized by coreboot during romstage, but is used mainly in
ramstage for storing data such as coreboot and ACPI tables, SMBIOS,
timestamps, stage information, vendor-specific data structures, etc.

For 32-bit builds, CBMEM is typically located at the highest usable
DRAM address below the 4GiB boundary. For 64-bit builds, while there
is no strict upper limit, it is advisable to follow the same guidelines
to prevent access or addressing issues. Regardless of the build type,
the CBMEM address must remain consistent between romstage and ramstage.

Each platform may need to implement its own method for determining the
`cbmem_top` address, as this can depend on specific hardware
configurations and memory layouts.

## Usage
Each CBMEM region is identified by a unique ID, allowing different
components to store and retrieve their data during runtime. The ID is a
32-bit value that optionally encodes characters to indicate the type or
source of the data.

Upon creating an entry, a block of memory is allocated of the requested
size from the reserved CBMEM region. This region persists across warm
reboots, making it useful for debugging and passing information to
payloads.

Note that CBMEM is implemented as imd (in-memory database), meaning
it grows downwards in memory from the provided upper limit, and only
the latest added entry may be removed.

The `cbmem` tool in `/util` can be used to list and access entries
using their ID.
