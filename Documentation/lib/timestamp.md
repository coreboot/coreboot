# Timestamps

## Introduction

The aim of the timestamp library is to make it easier for different
boards to save timestamps in cbmem / stash (until cbmem is brought up)
by providing a simple API to initialize, add and sync timestamps. In
order to make the timestamps persistent and accessible from the kernel,
we need to ensure that all the saved timestamps end up in cbmem under
the CBMEM_ID_TIMESTAMP tag. However, until the cbmem area is available,
the timestamps can be saved to a SoC-defined \_timestamp region or in a
local stage-specific stash. The work of identifying the right location
for storing timestamps is done by the library and is not exposed to the
user.

Working of timestamp library from a user perspective can be outlined in
the following steps:
1. Initialize the base time and reset cbmem timestamp area
2. Start adding timestamps

Behind the scenes, the timestamp library takes care of:
1. Identifying the correct location for storing timestamps (cbmem or
   timestamp region or local stash).
2. Once cbmem is up, ensure that all timestamps are synced from
   timestamp region or local stash into the cbmem area.
3. Add a new cbmem timestamp area based on whether a reset of the cbmem
   timestamp region is required or not.

### Transition from cache to cbmem

To move timestamps from the cache to cbmem (and initialize the cbmem
area in the first place), we use the CBMEM_INIT_HOOK infrastructure of
coreboot.

When cbmem is initialized, the hook is called, which creates the area,
copies all timestamps to cbmem and disables the cache.

After such a transition, timestamp_init() must not be run again.


## Data structures used

The main structure that maintains information about the timestamp cache
is:

```c
struct __packed timestamp_cache {
        uint16_t cache_state;
        struct timestamp_table table;
        struct timestamp_entry entries[MAX_TIMESTAMP_CACHE];
};
```

### cache_state

The state of the cache is maintained by `cache_state` attribute which
can be any one of the following:

```c
enum {
        TIMESTAMP_CACHE_UNINITIALIZED = 0,
        TIMESTAMP_CACHE_INITIALIZED,
        TIMESTAMP_CACHE_NOT_NEEDED,
};
```

By default, if the cache is stored in local stash (bss area), then it
will be reset to uninitialized state. However, if the cache is stored in
timestamp region, then it might have garbage in any of the attributes.
Thus, if the timestamp region is being used by any board, it is
initialized to default values by the library.

Once the cache is initialized, its state is set to
`CACHE_INITIALIZED`. Henceforth, the calls to cache i.e. `timestamp_add`
know that the state reflected is valid and timestamps can be directly
saved in the cache.

Once the cbmem area is up (i.e. call to
`timestamp_sync_cache_to_cbmem`), we do not need to store the timestamps
in local stash / timestamp area anymore. Thus, the cache state is set to
`CACHE_NOT_NEEDED`, which allows `timestamp_add` to store all timestamps
directly into the cbmem area.


### table

This field is represented by a structure which provides overall
information about the entries in the timestamp area:

```c
struct timestamp_table {
        uint64_t        base_time;
        uint32_t        max_entries;
        uint32_t        num_entries;
        struct timestamp_entry entries[0]; /* Variable number of entries */
} __packed;
```

It indicates the base time for all timestamp entries, maximum number of
entries that can be stored, total number of entries that currently exist
and an entry structure to hold variable number of entries.


### entries

This field holds the details of each timestamp entry, up to a maximum of
`MAX_TIMESTAMP_CACHE` which is defined as 16 entries. Each entry is
defined by:

```c
struct timestamp_entry {
        uint32_t        entry_id;
        uint64_t        entry_stamp;
} __packed;
```

`entry_id` holds the timestamp id corresponding to this entry and
`entry_stamp` holds the actual timestamp.


For timestamps stored in the cbmem area, a `timestamp_table` is
allocated with space for `MAX_TIMESTAMPS` equal to 30. Thus, the cbmem
area holds `base_time`, `max_entries` (which is 30), current number of
entries and the actual entries represented by `timestamp_entry`.


## Function APIs

### timestamp_init

This function initializes the timestamp cache and should be run as early
as possible. On platforms with SRAM, this might mean in bootblock, on
x86 with its CAR backed memory in romstage, this means romstage before
memory init.

### timestamp_add

This function accepts from user a timestamp id and time to record in the
timestamp table. It stores the entry in the appropriate table in cbmem
or `_timestamp` region or local stash.


### timestamp_add_now

This function calls `timestamp_add` with user-provided id and current
time.


## Use / Test Cases

The following cases have been considered while designing the timestamp
library. It is important to ensure that any changes made to this library
satisfy each of the following use cases:

### Case 1: Timestamp Region Exists (Fresh Boot / Resume)

In this case, the library needs to call `timestamp_init` as early as
possible to enable the timestamp cache. Once cbmem is available, the
values will be transferred automatically.

All regions are automatically reset on initialization.

### Case 2: No timestamp region, fresh boot, cbmem_initialize called after timestamp_init

`timestamp_init` will set up a local cache. cbmem must be initialized
before that cache vanishes - as happens when jumping to the next stage.

### Case 3: No timestamp region, fresh boot, cbmem_initialize called before timestamp_init

This case is not supported right now, just don't call `timestamp_init`
after `cbmem_initialize`. (Patches to make this more robust are
welcome.)

### Case 4: No timestamp region, resume, cbmem_initialize called after timestamp_init

We always reset the cbmem region before using it, so pre-suspend
timestamps will be gone.

### Case 5: No timestamp region, resume, cbmem_initialize called before timestamp_init

We always reset the cbmem region before using it, so pre-suspend
timestamps will be gone.
