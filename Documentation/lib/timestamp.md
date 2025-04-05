# Timestamps

## Introduction

The aim of the timestamp library is to make it easier for different
boards to save timestamps in cbmem / stash (until cbmem is brought up)
by providing a simple API to initialize, add, and sync timestamps. In
order to make the timestamps persistent and accessible from the kernel,
we need to ensure that all the saved timestamps end up in cbmem under
the CBMEM_ID_TIMESTAMP tag. However, until the cbmem area is available,
the timestamps can be saved to a SoC-defined `_timestamp` region if one
is defined in the board's `memlayout.ld`. The work of identifying the
right location for storing timestamps is done by the library and is not
exposed to the user.

Timestamps in coreboot are a critical feature for performance analysis,
debugging, and optimization of the boot process. They provide precise
timing information about various stages and operations during system
initialization, allowing developers to identify bottlenecks and optimize
boot performance. The timestamp system is designed to be lightweight,
accurate, and persistent across different boot stages.

Working of timestamp library from a user perspective can be outlined in
the following steps:
1. Initialize the base time and reset cbmem timestamp area using
   `timestamp_init()`.
2. Start adding timestamps using `timestamp_add()` or
   `timestamp_add_now()`.

Behind the scenes, the timestamp library takes care of:
1. Identifying the correct location for storing timestamps (`_timestamp`
   region before cbmem is ready, then cbmem region).
2. Add a new cbmem timestamp area based on whether a reset of the cbmem
   timestamp region is required or not.
3. Once cbmem is ready, ensuring that all timestamps are synced from the
   `_timestamp` region into the cbmem area.

Note that if `CONFIG_COLLECT_TIMESTAMPS` is disabled, all timestamp
functions are implemented as no-ops, and no timestamps will be
collected.


## Background

The timestamp implementation in coreboot has evolved over time to meet
the needs of the firmware development and performance analysis.
Initially designed as a simple timing mechanism, it has grown into a
sophisticated system that:

- Tracks boot stages and critical operations
- Supports multiple hardware platforms (x86, ARM, RISC-V)
- Provides persistent storage across boot stages
- Enables post-boot analysis of boot performance
- Integrates with vendor-specific firmware components


## Timestamp Architecture

### Transition from cache (`_timestamp` region) to cbmem

To move timestamps from the early `_timestamp` region to cbmem (and
initialize the cbmem area in the first place), we use the
`CBMEM_READY_HOOK` infrastructure of coreboot.

When cbmem is initialized (`cbmem_initialize` or `cbmem_recovery`), the
hook calls the `timestamp_reinit` function. This function allocates or
finds the `CBMEM_ID_TIMESTAMP` area in cbmem, copies all timestamps from
the `_timestamp` region (if used) using `timestamp_sync_cache_to_cbmem`,
and updates an internal global pointer (`glob_ts_table`) to point to the
cbmem table. Subsequent calls to `timestamp_add` will then write
directly to cbmem.

After such a transition, `timestamp_init()` must not be run again (it is
asserted to only run in `ENV_ROMSTAGE_OR_BEFORE`).


### Data structures used

Timestamps are stored using instances of `struct timestamp_table`. A
global pointer, `glob_ts_table`, points to the currently active table,
which is either the `_timestamp` memory region (during early boot) or
the CBMEM area.

The `_timestamp` region acts as an early cache before cbmem is ready.
Its size is determined by `REGION_SIZE(timestamp)` defined in the linker
script (`memlayout.ld`) and dictates the maximum number of entries that
can be stored early on.

For timestamps stored in the cbmem area, a `timestamp_table` is
allocated with space for a fixed number of entries (currently 192)
defined by `MAX_TIMESTAMPS` in `src/lib/timestamp.c`.


```c
struct timestamp_entry {
	uint32_t        entry_id;
	int64_t         entry_stamp;
} __packed;
```
>>> _Source: `/src/commonlib/include/commonlib/timestamp_serialized.h`_


```c
struct timestamp_table {
	uint64_t        base_time;
	uint16_t        max_entries;
	uint16_t        tick_freq_mhz;
	uint32_t        num_entries;
	struct timestamp_entry entries[]; /* Variable number of entries */
} __packed;
```
>>> _Source: `/src/commonlib/include/commonlib/timestamp_serialized.h`_


- `base_time`: Indicates the base time (offset) for all timestamp
  entries in this table.
- `max_entries`: Maximum number of entries that can be stored in this
  table instance.
- `tick_freq_mhz`: Timestamp tick frequency in MHz. Populated later in
  boot.
- `num_entries`: Total number of entries that currently exist in this
  table instance.
- `entries`: Array holding the actual timestamp entries.


### Memory Layout

Timestamps are stored in two locations during the boot process:

1.  **`_timestamp` Region**: Used during early boot stages (before CBMEM
    is available), *if* defined in `memlayout.ld`.
    - Located at the `_timestamp` symbol.
    - Persists across stage transitions within `ENV_ROMSTAGE_OR_BEFORE`.
    - Size determined by `REGION_SIZE(timestamp)` in the linker script,
      which limits the number of entries.

2.  **CBMEM**: Used after CBMEM is initialized.
    - Identified by `CBMEM_ID_TIMESTAMP`.
    - Provides persistent storage across warm reboots.
    - Supports a fixed maximum number of entries.
    - Automatically synchronized from the `_timestamp` region when CBMEM
      becomes available via `timestamp_reinit`.


## Function APIs

### Core Functions

#### timestamp_init

```c
void timestamp_init(uint64_t base);
```
>>> _Source: `/src/include/timestamp.h`_

Initializes the timestamp system with a base time. This function sets up
the timestamp cache (`_timestamp` region) and should be run in
bootblock. It must be called once in *one* of the
`ENV_ROMSTAGE_OR_BEFORE` stages. It will fail if no `timestamp` region
is defined in `memlayout.ld`.

Note that platform setup code on x86 or the decompressor on Arm can
measure some timestamps earlier and pass them in to
bootblock_main_with_timestamp().


#### timestamp_add

```c
void timestamp_add(enum timestamp_id id, int64_t ts_time);
```
>>> _Source: `/src/include/timestamp.h`_

Adds a new timestamp with the specified ID and time value. It stores the
timestamp in the currently active table (either `_timestamp` region or
cbmem). The time value must be an absolute time value (typically
obtained from `timestamp_get()`), as it will be adjusted by subtracting
the table's `base_time` before being stored as an entry.


#### timestamp_add_now

```c
void timestamp_add_now(enum timestamp_id id);
```
>>> _Source: `/src/include/timestamp.h`_

Adds a new timestamp with the current time. This function calls
`timestamp_add` with user-provided id and current time obtained from
`timestamp_get()`.


#### timestamp_rescale_table

```c
void timestamp_rescale_table(uint16_t N, uint16_t M);
```
>>> _Source: `/src/include/timestamp.h`_

Applies a scaling factor N/M to all recorded timestamps (including the
`base_time`).


#### get_us_since_boot

```c
uint32_t get_us_since_boot(void);
```
>>> _Source: `/src/include/timestamp.h`_

Returns the time since boot (relative to `base_time`) in microseconds.
Requires `tick_freq_mhz` to be populated.


#### timestamp_get

```c
uint64_t timestamp_get(void);
```
>>> _Source: `/src/include/timestamp.h`_

Returns the current raw timestamp value from the underlying hardware
timer (platform-specific weak implementation).


#### timestamp_tick_freq_mhz

```c
int timestamp_tick_freq_mhz(void);
```
>>> _Source: `/src/include/timestamp.h`_

Returns the timestamp tick frequency in MHz (platform-specific weak
implementation).


### Timestamp IDs

The system uses predefined timestamp IDs to mark various boot stages and
operations. These are organized in ranges:

- 1-500: Miscellaneous coreboot operations (e.g., `TS_POSTCAR_START`,
  `TS_DELAY_START`, `TS_READ_UCODE_START`)
- 500-600: Google/ChromeOS specific (e.g., `TS_VBOOT_START`,
  `TS_EC_SYNC_START`).

  Note many of the existing timestamps here are no longer
  Google-specific since many features originally added for Google
  vendorcode have since been migrated into general coreboot code.

- 900-940: AMD specific (e.g., `TS_AGESA_INIT_EARLY_START`)
- 940-950: Intel ME specific (e.g., `TS_ME_INFORM_DRAM_START`)
- 950-989: Intel FSP specific (e.g., `TS_FSP_MEMORY_INIT_START`)
- 990-999: Intel ME specific (continued) (e.g., `TS_ME_ROM_START`)
- 1000+: Payload specific
  - Depthcharge: 1000-1199
  - ChromeOS Hypervisor: 1200-1299

Refer to `src/commonlib/include/commonlib/timestamp_serialized.h` for
the complete list and descriptions.


## Use / Test Cases

The following cases describe the behavior based on the presence of the
`timestamp` region and when cbmem is initialized.


### Case 1: Timestamp Region Exists

This is the standard configuration for collecting early timestamps.
`timestamp_init` must be called in an `ENV_ROMSTAGE_OR_BEFORE` stage to
initialize the `_timestamp` region. When the `CBMEM_READY_HOOK` runs
`timestamp_reinit`, the contents of the `_timestamp` region are copied
to the cbmem table, and subsequent timestamps go directly to cbmem. The
cbmem table is reset on fresh boot or resume.


### Case 2: No Timestamp Region Defined

If no `timestamp` region is defined in `memlayout.ld`, attempts to call
`timestamp_init` will fail (specifically, `timestamp_cache_get()` will
return NULL). No timestamps can be collected before cbmem is ready.
Timestamps added after `timestamp_reinit` has run (via the
`CBMEM_READY_HOOK`) will be added directly to the cbmem table, but there
will be no `base_time` established from early boot.


### Case 3: Resume

On resume (e.g., x86 S3), `timestamp_reinit` is typically called again.
If `ENV_CREATES_CBMEM` is true for the resume path (as it is for x86
S3), a new cbmem table is allocated by `timestamp_alloc_cbmem_table`,
effectively clearing any pre-suspend timestamps. The `_timestamp` region
content (if any) is copied over, but this usually contains stale data
from the previous boot's early stages.


## Configuration

### Kconfig Options

- `CONFIG_COLLECT_TIMESTAMPS`: Enable/disable timestamp collection
  globally. If disabled, timestamp functions become no-ops.
- `CONFIG_TIMESTAMPS_ON_CONSOLE`: Print timestamps to console during
  boot as they are added.


### Memory Layout

Collecting timestamps before cbmem is ready requires an `_timestamp`
region in the memory layout, defined in the `memlayout.ld` linker
script. Depending on the platform, the memory layout can be for the
board, the SOC, or the Architecture. Any of them will typically follow
the following pattern:

```text
#include <memlayout.h>

...
TIMESTAMP(., 0x200)
...

```

The size allocated to this region determines the maximum number of
timestamps that can be stored before cbmem is available.

The cbmem timestamp table (`CBMEM_ID_TIMESTAMP`) has a fixed size,
currently allowing up to 192 entries. This limit is defined by
`MAX_TIMESTAMPS` in `src/lib/timestamp.c`.


### Hardware Considerations

- x86: `timestamp_init` must be called before CAR (Cache-as-RAM) is torn
  down if called from bootblock or separate romstage. The library
  includes checks (`timestamp_should_run`) to ensure timestamps are only
  added by the primary processor during early boot on AP systems.
- ARM: No special considerations noted in the code.
- RISC-V: No special considerations noted in the code.


## Examples

### Initializing Timestamps (in bootblock)

```c
/* In src/mainboard/$(MAINBOARDDIR)/bootblock.c */
#include <timestamp.h>
#include <timer.h> /* For timestamp_get() default implementation */

void bootblock_mainboard_init(void)
{
	/* Initialize timestamp region with current time as base. */
	timestamp_init(timestamp_get());

	/* Add first timestamp */
	timestamp_add_now(TS_BOOTBLOCK_START);

	/* ... other bootblock code ... */
}
```

Note: `timestamp_get()` here provides the initial base time. Requires
`CONFIG_COLLECT_TIMESTAMPS=y` and a `timestamp` region.


### Adding Custom Timestamps

```c
#include <timestamp.h>

void my_custom_function(void)
{
	timestamp_add_now(TS_DEVICE_INITIALIZE); /* Use a relevant ID */
	// ... perform initialization ...
	timestamp_add_now(TS_DEVICE_DONE);       /* Use a relevant ID */
}
```


## Best Practices

1.  **Initialization**:
    - Enable `CONFIG_COLLECT_TIMESTAMPS` if needed.
    - Define a `timestamp` region in `memlayout.ld` if early
      timestamps (before cbmem) are required. Ensure it's large enough
      for the expected number of early entries.
    - Call `timestamp_init()` exactly once in the earliest possible
      `ENV_ROMSTAGE_OR_BEFORE` stage (e.g., `bootblock`).
    - Use a consistent base time, typically `timestamp_get()`.

2.  **Adding Timestamps**:
    - Use appropriate predefined timestamp IDs from
      `timestamp_serialized.h` whenever possible. Add custom IDs if
      necessary, avoiding conflicts.
    - Add timestamps for significant operations or stage transitions
      using `timestamp_add_now()`.
    - Be mindful of the entry limits: the size of the `_timestamp`
      region for early timestamps, and the fixed limit for the cbmem
      table. Check for "Timestamp table full" errors in the log.

3.  **Analysis**:
    - Use the `cbmem -t` utility in the OS (if using LinuxBoot/NERF)
      to read and display timestamps stored in CBMEM.
    - Consider the `tick_freq_mhz` (also available in the `cbmem -t`
      output) when converting raw timestamp differences (`entry_stamp`)
      to time units. The raw values are offsets from `base_time`.
