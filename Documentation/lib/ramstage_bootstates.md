# coreboot Ramstage Bootstates & Bootstate Callbacks

## Introduction

The coreboot boot process is divided into several discrete phases, one
of which is **ramstage**. Ramstage is the phase where the main hardware
initialization and device setup occurs after memory initialization.
Within ramstage, a state machine called the **bootstate machine**
manages the sequence of operations needed to initialize the system,
configure devices, and prepare to load and execute the payload (such as
a bootloader, operating system, or firmware utility).

The bootstate machine provides a structured and extensible way to
organize code execution during the boot process. It allows for clear
separation of concerns between different initialization phases and
provides hooks for component-specific code to run at well-defined
points.

**Important Note:** The exact execution order of multiple callbacks
registered for the same state and sequence (entry/exit) is not
guaranteed. This means that you cannot depend on one call for the
state/sequence in any other calls to the same state/sequence. If this
ordering is required, join the calls to the two functions into a single
function which specifies the order and create a callback to call the
top-level function instead of the two individual callbacks.


## Bootstate Machine Architecture

The bootstate machine's public API is defined in
`src/include/bootstate.h`, and its core implementation resides in
`src/lib/hardwaremain.c`. At its core, it consists of:

1. A series of sequential states that represent phases of the boot process
2. A mechanism for callback registration to execute code during state transitions
3. A framework for blocking and unblocking state transitions
4. Timing and debugging facilities to measure and report performance during boot


### Key Data Structures

The primary public data structure for interacting with the bootstate
machine is `struct boot_state_callback`. The internal implementation
also uses `struct boot_state` and `struct boot_phase`.


#### Boot State Callback (Public API)

Callbacks that run during state transitions are defined by this
structure in `src/include/bootstate.h`:

```c
struct boot_state_callback {
    void *arg;                  // Argument to pass to the callback
    void (*callback)(void *arg); // Function pointer to the callback
    struct boot_state_callback *next; // Next callback in linked list (internal use)
#if CONFIG(DEBUG_BOOT_STATE)
    const char *location;       // Source location for debugging
#endif
};
```

#### Boot State Sequence (Public API)

The boot state sequence type, defined in `src/include/bootstate.h`,
specifies when a callback should run relative to the state's main
action:

```c
typedef enum {
    BS_ON_ENTRY,                // Execute before state function
    BS_ON_EXIT                  // Execute after state function
} boot_state_sequence_t;
```


#### Boot State (Internal Implementation)

The main internal data structure in `src/lib/hardwaremain.c` is
`struct boot_state`, which defines a single state in the bootstate
machine:

```c
struct boot_state {
    const char *name;           // Human-readable name of the state
    boot_state_t id;            // Enumerated identifier for the state
    u8 post_code;               // POST code to output during state execution
    struct boot_phase phases[2]; // Entry and exit phases (internal use)
    boot_state_t (*run_state)(void *arg); // Function to execute during the state
    void *arg;                  // Argument to pass to the run_state function
    int num_samples;            // Counter for timing samples (internal use)
    bool complete;              // Flag indicating if state has completed (internal use)
};
```


#### Boot Phase (Internal Implementation)

Each boot state has two internal phases ("entry" and "exit") represented
by `struct boot_phase` in `src/lib/hardwaremain.c`:

```c
struct boot_phase {
    struct boot_state_callback *callbacks; // Linked list of callbacks
    int blockers;               // Counter for blocking state transition
};
```

## Bootstate Sequence

The bootstate machine defines the following sequence of states, executed
in order by the `bs_walk_state_machine` function in
`src/lib/hardwaremain.c`. The sequence is defined by the `boot_state_t`
enum in `src/include/bootstate.h`:

1. **BS_PRE_DEVICE**: Initial state before any device operations begin
2. **BS_DEV_INIT_CHIPS**: Early chip initialization for critical components
3. **BS_DEV_ENUMERATE**: Device enumeration (discovering devices on buses)
4. **BS_DEV_RESOURCES**: Resource allocation for devices
5. **BS_DEV_ENABLE**: Enabling devices that were discovered
6. **BS_DEV_INIT**: Device initialization
7. **BS_POST_DEVICE**: All device operations have been completed
8. **BS_OS_RESUME_CHECK**: Check if we're resuming from a sleep state
9. **BS_OS_RESUME**: Handle OS resume process (if needed)
10. **BS_WRITE_TABLES**: Write system tables (e.g., ACPI, SMBIOS)
11. **BS_PAYLOAD_LOAD**: Load the payload into memory
12. **BS_PAYLOAD_BOOT**: Boot the payload

This sequence forms the backbone of the ramstage execution flow. Each
state performs a specific task, runs associated callbacks, and
transitions to the next state upon completion, unless blocked.


## Bootstate Details

### BS_PRE_DEVICE

**Purpose**: Serves as the initial state before any device tree
operations begin.

**Key Functions**:
- `bs_pre_device()`: Sets up initial environment and transitions to next
  state.

**Usage**: This state is used for initializing core components that need
to be set up before any device operations. Examples include:
- Setting up global NVRAM variables
- Initializing debugging facilities
- Preparing ACPI tables or other critical system structures


### BS_DEV_INIT_CHIPS

**Purpose**: Initializes critical chips early in the boot process.

**Key Functions**:
- `bs_dev_init_chips()`: Calls `dev_initialize_chips()` to initialize
  all chips in the device tree.

**Notes**: Chip initialization can disable unused devices, which is why
it happens before device enumeration.


### BS_DEV_ENUMERATE

**Purpose**: Discovers devices in the system.

**Key Functions**:
- `bs_dev_enumerate()`: Calls `dev_enumerate()` to probe and identify
  devices.

**Notes**: During this phase, the system scans buses and detects
connected devices.


### BS_DEV_RESOURCES

**Purpose**: Allocates and assigns resources (I/O, memory, IRQs) to
devices.

**Key Functions**:
- `bs_dev_resources()`: Calls `dev_configure()` to compute and assign
  bus resources.

**Notes**: Resource allocation resolves conflicts and ensures each
device has the resources it needs.


### BS_DEV_ENABLE

**Purpose**: Enables devices in the system.

**Key Functions**:
- `bs_dev_enable()`: Calls `dev_enable()` to enable devices on the bus.

**Notes**: Some devices may be selectively disabled based on hardware
configuration or policy.


### BS_DEV_INIT

**Purpose**: Initializes enabled devices.

**Key Functions**:
- `bs_dev_init()`: Calls `dev_initialize()` to initialize devices on the
  bus.

**Notes**: This state performs device-specific initialization routines
for all enabled devices.


### BS_POST_DEVICE

**Purpose**: Final state after all device operations have completed.

**Key Functions**:
- `bs_post_device()`: Calls `dev_finalize()` to complete any final
  device operations.

**Notes**: This state serves as a checkpoint that all device
initialization is complete.


### BS_OS_RESUME_CHECK

**Purpose**: Checks if the system should resume from a sleep state.

**Key Functions**:
- `bs_os_resume_check()`: Looks for a wake vector to determine if resume
  is needed.

**Notes**: This state branches the boot flow based on whether the system
is resuming from a sleep state.

### BS_OS_RESUME

**Purpose**: Handles the OS resume process.

**Key Functions**:
- `bs_os_resume()`: Calls `acpi_resume()` with the wake vector to resume
  the OS.

**Notes**: After successful resume, control is transferred to the OS and
does not return to coreboot.


### BS_WRITE_TABLES

**Purpose**: Writes configuration tables for the payload or OS.

**Key Functions**:
- `bs_write_tables()`: Calls `write_tables()` to generate system tables.

**Notes**: Tables include ACPI, SMBIOS, and other system configuration
data.


### BS_PAYLOAD_LOAD

**Purpose**: Loads the payload into memory.

**Key Functions**:
- `bs_payload_load()`: Calls `payload_load()` to load the payload.

**Notes**: The payload could be a bootloader, an operating system kernel,
or a firmware utility.


### BS_PAYLOAD_BOOT

**Purpose**: Final state that boots the loaded payload.

**Key Functions**:
- `bs_payload_boot()`: Calls `payload_run()` to execute the payload.

**Notes**: After successful execution, control is transferred to the
payload and does not return to coreboot. If execution returns (which
indicates an error), a boot failure message is printed.


## Driving the State Machine

The state machine is driven by the `main()` function in
`src/lib/hardwaremain.c`. After initial setup (like initializing the
console and CBMEM), it calls `bs_walk_state_machine()`.

`bs_walk_state_machine()` loops through the defined boot states:
1. It identifies the current state.
2. Runs all `BS_ON_ENTRY` callbacks for that state.
3. Executes the state's specific function (e.g., `bs_dev_enumerate()`).
4. Runs all `BS_ON_EXIT` callbacks for that state.
5. Transitions to the next state returned by the state function.

This loop continues until the final state (`BS_PAYLOAD_BOOT` or
`BS_OS_RESUME`) transfers control away from coreboot.


## External Functions (Public API)

The bootstate machine provides several functions in
`src/include/bootstate.h` for interacting with states:


### Callback Registration

```c
int boot_state_sched_on_entry(struct boot_state_callback *bscb, boot_state_t state_id);
```
Schedules a callback to run when entering a state (`BS_ON_ENTRY`).

```c
int boot_state_sched_on_exit(struct boot_state_callback *bscb, boot_state_t state_id);
```
Schedules a callback to run when exiting a state (`BS_ON_EXIT`).


### State Transition Control

```c
int boot_state_block(boot_state_t state, boot_state_sequence_t seq);
```
Blocks a state transition from occurring after the specified sequence
(entry or exit callbacks). The transition will pause until the block is
removed.

```c
int boot_state_unblock(boot_state_t state, boot_state_sequence_t seq);
```
Removes a previously set block on a state transition.


### Static Callback Registration

For registering callbacks at compile time, use the `BOOT_STATE_INIT_ENTRY`
macro defined in `src/include/bootstate.h`:

```c
BOOT_STATE_INIT_ENTRY(state, when, func, arg)
```

This macro creates a static entry in a special section (`.bs_init`) of
the binary. These entries are processed early in `main()` by
`boot_state_schedule_static_entries()` to register the callbacks before
the state machine starts running.


## Configuration Options

The bootstate machine behavior can be modified through Kconfig options:


### DEBUG_BOOT_STATE

```
config DEBUG_BOOT_STATE
    bool "Debug boot state machine"
    default n
    help
        Control debugging of the boot state machine. When selected displays
        the state boundaries in ramstage.
```

When enabled, this option causes the bootstate machine to output
debugging information via `printk`, including:
- State transition notifications (`Entering/Exiting <state> state.`)
- Callback execution details (address, source location, execution time)
- Timing information for state execution phases (entry, run, exit)


## Examples

### Adding a New Bootstate Callback

To register a function to be called when entering a specific state using
the static registration method:

```c
// Function to be called
static void my_init_function(void *arg)
{
    // Initialization code
    printk(BIOS_DEBUG, "My initialization running...\n");
}

// Register the callback at compile time
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, my_init_function, NULL);
```


### Runtime Callback Registration

For dynamic callback registration during runtime (e.g., within another
callback or state function):

```c
static void runtime_init(void *arg)
{
    // Do something
}

void register_my_callbacks(void)
{
    // Allocate or define a static callback structure
    static struct boot_state_callback bscb = {
        .callback = runtime_init,
        .arg = NULL,
        // .location is automatically handled if DEBUG_BOOT_STATE=y
    };

    // Schedule it
    boot_state_sched_on_entry(&bscb, BS_DEV_ENABLE);
}
```


### Blocking State Transition

To temporarily block a state from progressing until a condition is met,
often used with timers:

```c
#include <timer.h> // Required for timer functions

static void wait_for_device(void *arg)
{
    if (!device_is_ready()) {
        // Block the transition *after* BS_DEV_INIT exits
        boot_state_block(BS_DEV_INIT, BS_ON_EXIT);

        // Schedule a function to check again later (e.g., after 100us)
        // Assume schedule_timer exists and works appropriately
        schedule_timer(check_device_ready, NULL, 100);
    }
}

static void check_device_ready(void *arg)
{
    if (device_is_ready()) {
        // Device is ready, unblock the transition
        boot_state_unblock(BS_DEV_INIT, BS_ON_EXIT);
    } else {
        // Still not ready, check again later
        schedule_timer(check_device_ready, NULL, 100);
    }
}

// Register the initial check to run when entering BS_DEV_INIT
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, wait_for_device, NULL);
```


## Best Practices

### When Working with Bootstates

1. **Choose the appropriate state**: Register callbacks at the earliest
    state where all dependencies are guaranteed to be initialized, but no
    earlier. Check the state descriptions and the functions called by
    each state function (`bs_*`) in `hardwaremain.c`.

2. **Keep callbacks focused**: Each callback should perform a specific,
    related task and avoid complex operations that might significantly
    delay the boot process.

3. **Consider dependencies carefully**: Ensure any hardware, data
    structures, or other resources your callback needs are available and
    initialized at the chosen state and sequence (`BS_ON_ENTRY` vs.
    `BS_ON_EXIT`).

4. **Do not rely on callback order**: Remember that the execution order
    of callbacks within the same state and sequence is not guaranteed.
    Callbacks should be self-contained and not depend on side effects from
    other callbacks that might run before or after them in the same phase.

5. **Use blocking sparingly**: The blocking mechanism is powerful for
    synchronization but can complicate the boot flow and make debugging
    harder if overused. Always ensure a corresponding `boot_state_unblock`
    call will eventually run.

6. **Leverage compile-time registration**: Prefer using
    `BOOT_STATE_INIT_ENTRY` for callbacks whenever possible. It makes the
    registration explicit and easier to find. Runtime registration is
    necessary only when the need for the callback is determined dynamically.

7. **Debug with timestamps and `DEBUG_BOOT_STATE`**: Use the timestamp API
    (`timestamp_add_now()`) and enable `DEBUG_BOOT_STATE` to measure
    callback execution time, identify bottlenecks, and understand the
    flow during development.

8. **Document state-specific behavior**: When adding callbacks, add
    comments explaining why they are placed in a particular state and
    sequence.

9. **Be careful with late states**: Avoid registering non-essential
    callbacks in `BS_PAYLOAD_BOOT` or `BS_OS_RESUME`. Callbacks on
    `BS_ON_EXIT` for these states are disallowed by compile-time asserts,
    as coreboot is about to transfer control.


## Related Documentation

- [Boot Stages in coreboot](https://doc.coreboot.org/getting_started/architecture.html):
  Overview of all coreboot boot stages.


## References

- `src/include/bootstate.h`: Public API definitions (callbacks, enums,
  scheduling/blocking functions, static registration macro).
- `src/lib/hardwaremain.c`: Internal implementation (state machine driver,
  state definitions, state functions).
- `src/ec/google/wilco/chip.c`: Example of bootstate callback usage.
- `src/mainboard/prodrive/hermes/mainboard.c`: Examples of mainboard-specific
  bootstate callbacks.
