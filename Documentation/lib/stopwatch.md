# coreboot Timers, Stopwatch, Delays, and Associated Callbacks

## Introduction

coreboot provides several mechanisms for handling time, including
high-precision stopwatches for profiling, simple delay functions for
hardware timing, and a monotonic timer system that forms the foundation
for these features. It also supports scheduling timer callbacks for
deferred execution. These tools are crucial for performance
optimization, debugging, ensuring proper hardware timing, and managing
asynchronous events during the boot process.

This document describes the core monotonic timer, the delay functions
(`udelay`, `mdelay`, `delay`), the stopwatch API (`struct stopwatch`),
and the timer callback mechanism.


## Architecture Overview

The timing facilities are layered:

- **Platform Timer:** Hardware-specific implementations provide a raw
  time source (e.g., LAPIC timer, Time Base register). These are often
  abstracted by `timer_monotonic_get()`.
- **Monotonic Timer:** (`src/include/timer.h`, `src/lib/timer.c`)
  Provides a consistent time source (`struct mono_time`) counting
  microseconds since timer initialization. Requires
  `CONFIG(HAVE_MONOTONIC_TIMER)`.
- **Delay Functions:** (`src/include/delay.h`, `src/lib/delay.c`,
  `src/lib/timer.c`) Simple blocking delays (`udelay`, `mdelay`,
  `delay`). The generic `udelay` uses the stopwatch API if yielding via
  `thread_yield_microseconds()` is not performed.
- **Stopwatch API:** (`src/include/timer.h`) A lightweight wrapper
  around the monotonic timer for measuring durations and handling
  timeouts (`struct stopwatch`,`wait_us`,`wait_ms`).
- **Timer Callbacks:** (`src/include/timer.h`) Allows scheduling
  functions to be called after a specified delay
  (`struct timeout_callback`, `timer_sched_callback()`, `timers_run()`).

These APIs are generally designed to be thread-safe and usable across
different boot stages.


## Core Monotonic Timer

The foundation is the monotonic timer, enabled by
`CONFIG(HAVE_MONOTONIC_TIMER)`. It provides a time source that
continuously increases from an arbitrary starting point (usually near
timer initialization).

### Data Structures

#### struct mono_time

```c
struct mono_time {
    uint64_t microseconds;     // Time in microseconds since timer init
};
```
Represents a point in monotonic time. Direct field access outside core
timer code is discouraged; use helper functions.

### API Functions

#### timer_monotonic_get

```c
void timer_monotonic_get(struct mono_time *mt);
```
Retrieves the current monotonic time.

**Parameters:**
- `mt`: Pointer to a `struct mono_time` to store the current time.

**Preconditions:**
- `CONFIG(HAVE_MONOTONIC_TIMER)` must be enabled.
- The underlying platform timer must be functional. Platform
  implementations may require an `init_timer()` call.

**Postconditions:**
- `mt` contains the current monotonic time in microseconds.

**Note:** If `CONFIG(HAVE_MONOTONIC_TIMER)` is *not* enabled, functions
relying on it (like stopwatch functions) will generally fall back to
basic behavior (e.g., returning 0 durations, immediate expiration
checks).

#### init_timer

```c
void init_timer(void);
```

Platform-specific timer initialization. The generic version in
`src/lib/timer.c` is a weak empty function
`__weak void init_timer(void) { /* do nothing */ }`. Platforms needing
explicit timer setup (e.g., configuring frequency, enabling the
counter) must provide their own strong implementation. Check platform
code (`src/cpu/`, `src/soc/`) for details.

#### `mono_time` Helper Functions

`src/include/timer.h` also provides several inline helper functions for
manipulating `struct mono_time` values:
- `mono_time_set_usecs()`, `mono_time_set_msecs()`: Set time.
- `mono_time_add_usecs()`, `mono_time_add_msecs()`: Add duration.
- `mono_time_cmp()`: Compare two times.
- `mono_time_after()`, `mono_time_before()`: Check time ordering.
- `mono_time_diff_microseconds()`: Calculate difference between two
  times.

Refer to `src/include/timer.h` for their exact definitions.


## Delay Functions

coreboot provides simple functions for introducing delays.

### udelay

```c
void udelay(unsigned int usecs);
```
Delays execution for *at least* the specified number of microseconds.

**Parameters:**
- `usecs`: Number of microseconds to delay.

**Implementation Notes:**
- The generic implementation in `src/lib/timer.c` first attempts to
  yield using `thread_yield_microseconds()` if `CONFIG(HAS_THREADS)` is
  enabled. If yielding handles the required delay (function returns 0),
  `udelay` returns immediately, allowing other threads to run.
- If `thread_yield_microseconds()` does not handle the delay (returns
  non-zero), or if threading is disabled, the generic `udelay` falls
  back to a busy-wait using the stopwatch API
  (`stopwatch_init_usecs_expire()` and
  `stopwatch_wait_until_expired()`).
- Architecture-specific implementations (e.g., in `src/arch/`) may
  override the generic one for better precision or efficiency.
- Adds 1 microsecond internally to ensure the delay is *at least* the
  requested duration.

**Preconditions:**
- If relying on the stopwatch fallback,
  `CONFIG(HAVE_MONOTONIC_TIMER)` is needed.
- Underlying timer (if used) must be initialized (potentially via a
  platform `init_timer()`).
- The delay value should be reasonable (overly large values might cause
  issues or unexpected behavior depending on the implementation).

**Example:**
```c
#include <delay.h>

// Wait for 100 microseconds
udelay(100);
```

### mdelay

```c
void mdelay(unsigned int msecs);
```
Delays execution for *at least* the specified number of milliseconds.

**Parameters:**
- `msecs`: Number of milliseconds to delay.

**Implementation Notes:**
- The generic implementation in `src/lib/delay.c` simply calls
  `udelay(1000)` in a loop `msecs` times.
- Therefore, it inherits the behavior of `udelay`, including the attempt
  to yield first if `udelay` supports it.

**Preconditions:**
- Same as `udelay`.

**Example:**
```c
#include <delay.h>

// Wait for 50 milliseconds
mdelay(50);
```

### delay

```c
void delay(unsigned int secs);
```
Delays execution for *at least* the specified number of seconds.

**Parameters:**
- `secs`: Number of seconds to delay.

**Implementation Notes:**
- The generic implementation in `src/lib/delay.c` simply calls
  `mdelay(1000)` in a loop `secs` times.
- Inherits the behavior of `mdelay` and `udelay`.

**Preconditions:**
- Same as `udelay`.

**Example:**
```c
#include <delay.h>

// Wait for 2 seconds
delay(2);
```


## Stopwatch API

The stopwatch API provides a convenient way to measure time durations
and implement timeouts based on the monotonic timer.

### Data Structures

#### struct stopwatch

```c
#include <timer.h> // For struct stopwatch and struct mono_time

struct stopwatch {
    struct mono_time start;    // Time when stopwatch was started or initialized
    struct mono_time current;  // Time when stopwatch was last ticked
    struct mono_time expires;  // Expiration time for timeout operations
};
```
Holds the state for a stopwatch instance.

### API Functions

#### Initialization Functions

##### stopwatch_init

```c
#include <timer.h>

static inline void stopwatch_init(struct stopwatch *sw);
```
Initializes a stopwatch structure. `start`, `current`, and `expires` are
all set to the current monotonic time. Use this when you only need to
measure elapsed duration from initialization.

**Parameters:**
- `sw`: Pointer to the stopwatch structure to initialize.

**Preconditions:**
- `sw` must point to valid memory.
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled for meaningful
  timing.

**Postconditions:**
- The stopwatch is initialized.


##### stopwatch_init_usecs_expire

```c
#include <timer.h>

static inline void stopwatch_init_usecs_expire(struct stopwatch *sw, uint64_t us);
```
Initializes a stopwatch and sets an expiration time `us` microseconds
from now.

**Parameters:**
- `sw`: Pointer to the stopwatch structure.
- `us`: Timeout duration in microseconds.

**Preconditions:**
- `sw` must point to valid memory.
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled.

**Postconditions:**
- The stopwatch is initialized, and `expires` is set `us` microseconds
  after `start`.


##### stopwatch_init_msecs_expire

```c
#include <timer.h>

static inline void stopwatch_init_msecs_expire(struct stopwatch *sw, uint64_t ms);
```
Initializes a stopwatch and sets an expiration time `ms` milliseconds
from now.

**Parameters:**
- `sw`: Pointer to the stopwatch structure.
- `ms`: Timeout duration in milliseconds.

**Preconditions:**
- `sw` must point to valid memory.
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled.

**Postconditions:**
- The stopwatch is initialized, and `expires` is set `ms` milliseconds
  after `start`.


#### Time Measurement Functions

##### stopwatch_tick

```c
#include <timer.h>

static inline void stopwatch_tick(struct stopwatch *sw);
```
Updates the `current` time field in the stopwatch structure to the
current monotonic time. This is often called implicitly by other
stopwatch functions like `stopwatch_expired()` and
`stopwatch_duration_usecs()`.

**Parameters:**
- `sw`: Pointer to the stopwatch structure.

**Preconditions:**
- The stopwatch must be initialized.
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled.


##### stopwatch_expired

```c
#include <timer.h>

static inline int stopwatch_expired(struct stopwatch *sw);
```
Checks if the stopwatch's expiration time (`expires`) has passed. It
implicitly calls `stopwatch_tick()` to get the current time for
comparison.

**Parameters:**
- `sw`: Pointer to the stopwatch structure.

**Returns:**
- Non-zero (true) if `current` time >= `expires` time.
- Zero (false) otherwise.

**Preconditions:**
- The stopwatch must be initialized (preferably with an expiration
  time).
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled.


##### stopwatch_wait_until_expired

```c
#include <timer.h>

static inline void stopwatch_wait_until_expired(struct stopwatch *sw);
```
Blocks (busy-waits) until the stopwatch expires by repeatedly calling
`stopwatch_expired()`.

**Parameters:**
- `sw`: Pointer to the stopwatch structure.

**Preconditions:**
- The stopwatch must be initialized with an expiration time.
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled.

**Postconditions:**
- The function returns only after the stopwatch has expired.


#### Duration Measurement Functions

##### stopwatch_duration_usecs

```c
#include <timer.h>

static inline int64_t stopwatch_duration_usecs(struct stopwatch *sw);
```
Returns the elapsed time in microseconds between `start` and `current`.
If `current` hasn't been updated since `stopwatch_init`, it implicitly
calls `stopwatch_tick()` first.

**Parameters:**
- `sw`: Pointer to the stopwatch structure.

**Returns:**
- Elapsed time in microseconds (`current` - `start`).

**Preconditions:**
- The stopwatch must be initialized.
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled for a meaningful
  duration.


##### stopwatch_duration_msecs

```c
#include <timer.h>

static inline int64_t stopwatch_duration_msecs(struct stopwatch *sw);
```
Returns the elapsed time in milliseconds since the stopwatch was
started. It calls `stopwatch_duration_usecs()` and divides by 1000.

**Parameters:**
- `sw`: Pointer to the stopwatch structure.

**Returns:**
- Elapsed time in milliseconds.

**Preconditions:**
- The stopwatch must be initialized.
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled.


### Utility Macros

These macros combine stopwatch initialization and expiration checking with
a user-provided condition.

#### wait_us

```c
#include <timer.h>

#define wait_us(timeout_us, condition) ...
```
Waits until a condition becomes true or a timeout elapses, whichever
comes first. Internally uses a `struct stopwatch`.

**Parameters:**
- `timeout_us`: Timeout duration in microseconds.
- `condition`: A C expression that evaluates to true or false. The loop
  continues as long as the condition is false and the timeout has not
  expired.

**Returns:**
- 0: If the condition was still false when the timeout expired.
- >0: If the condition became true before the timeout. The return value
  is the approximate number of microseconds waited (at least 1).

**Preconditions:**
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled.


#### wait_ms

```c
#include <timer.h>

#define wait_ms(timeout_ms, condition) ...
```
Similar to `wait_us`, but the timeout is specified in milliseconds.

**Parameters:**
- `timeout_ms`: Timeout duration in milliseconds.
- `condition`: C expression to wait for.

**Returns:**
- 0: If the condition was still false after the timeout.
- >0: If the condition became true before the timeout. The return value
  is the approximate number of milliseconds waited (rounded up, at
  least 1).

**Preconditions:**
- `CONFIG(HAVE_MONOTONIC_TIMER)` should be enabled.


## Key differences between `mdelay` and `wait_ms`

While both can be used to wait, they serve different purposes:

### Purpose and Use Case

- `mdelay`: Provides an *unconditional* delay for a fixed duration. The
  generic implementation attempts to yield to other threads first (if
  `CONFIG(HAS_THREADS)` is enabled via `udelay`), but it always waits
  for (at least) the specified time, potentially using a busy-wait if
  yielding doesn't occur. Primarily used for hardware timing
  requirements or pacing operations.

- `wait_ms`: Provides a *conditional* wait. It waits for a specific C
  expression (`condition`) to become true, but *only up to* a maximum
  timeout duration. Used when polling for a status change or event, with
  a safeguard against waiting indefinitely.

### When to Use Which

#### Use `mdelay` when:

- You need a guaranteed minimum delay (e.g., waiting for hardware to
  settle after a register write).
- You are implementing a hardware initialization sequence with specific
  timing requirements between steps.
- You want a simple pause, potentially allowing other threads to run if
  the underlying `udelay` yields.

**Example:**
```c
#include <delay.h>
#include <arch/io.h> // For write_reg hypothetical function

// Initialize hardware with specific timing requirements
write_reg(REG_A, value);
mdelay(10);  // Must wait (at least) 10ms after writing REG_A
write_reg(REG_B, another_value);
```

#### Use `wait_ms` when:

- You are waiting for a hardware status bit to change or a condition to
  become true.
- You need to implement a timeout for an operation that might fail or
  take too long.
- You want to know approximately how long you waited for the condition
  to become true (via the return value).

**Example:**
```c
#include <timer.h>
#include <console/console.h>
#include <arch/io.h> // For read_reg hypothetical function

// Wait for hardware to become ready, but not forever
#define STATUS_REG 0x100
#define READY_BIT  (1 << 0)
int64_t waited_ms = wait_ms(100, (read_reg(STATUS_REG) & READY_BIT));

if (waited_ms > 0) {
    printk(BIOS_INFO, "Hardware ready after ~%lld ms\n", waited_ms);
} else {
    printk(BIOS_ERR, "Timeout: Hardware failed to become ready within 100 ms\n");
}
```

### Performance Considerations

- `mdelay`: Generally simpler if the underlying `udelay` performs a
  busy-wait. If it yields (`thread_yield_microseconds`), overhead depends
  on the scheduler. Always waits for the full duration (approximately).
- `wait_ms`: Involves repeated condition checking and stopwatch
  management (`stopwatch_expired`, which calls `timer_monotonic_get`),
  adding overhead within the loop. However, it can return early if the
  condition becomes true, potentially saving time compared to a fixed
  `mdelay`.

### Error Handling / Feedback

- `mdelay`: Provides no feedback. It simply waits.
- `wait_ms`: Returns whether the condition was met within the timeout
  and provides the approximate wait time if successful. This allows for
  explicit timeout handling.

### Summary

Use `mdelay` for fixed, unconditional delays, potentially allowing
thread yielding. Use `wait_ms` for conditional waits with a timeout and
feedback on success or failure. Choose based on whether you need to poll
for a condition or simply need to pause execution.


## Timer Callbacks

coreboot provides a mechanism to schedule functions (callbacks) to be
executed after a certain time has elapsed. This requires
`CONFIG(TIMER_QUEUE)`.

### Data Structures

#### struct timeout_callback

```c
#include <timer.h>

struct timeout_callback {
    void *priv;                     // Private data for the callback
    void (*callback)(struct timeout_callback *tocb); // Function to call
    /* Internal use by timer library: */
    struct mono_time expiration;    // Calculated expiration time
};
```

Represents a scheduled callback. The user initializes `priv` and
`callback`.

### API Functions

#### timer_sched_callback

```c
#include <timer.h>

int timer_sched_callback(struct timeout_callback *tocb, uint64_t us);
```
Schedules a callback function to run after a specified delay.

**Parameters:**
- `tocb`: Pointer to a `struct timeout_callback`. The `priv` and
  `callback` fields must be set by the caller. The structure must
  persist until the callback runs or is canceled (cancellation not
  directly supported by API).
- `us`: Delay in microseconds from the time of this call until the
  callback should run.

**Returns:**
- 0: Success.
- <0: Error (e.g., timer queue full, invalid arguments).

**Preconditions:**
- `CONFIG(TIMER_QUEUE)` and `CONFIG(HAVE_MONOTONIC_TIMER)` must be
  enabled.
- `tocb` must point to a valid structure with `callback` assigned.

**Postconditions:**
- The callback is added to a queue, to be executed when `timers_run()`
  is called after the expiration time.


#### timers_run

```c
#include <timer.h>

int timers_run(void);
```

Checks the timer callback queue and executes any callbacks whose
expiration time has passed. This function needs to be called
periodically from the main execution flow (e.g., in a boot state loop
or idle task) for callbacks to be processed.

**Returns:**
- 1: If callbacks were run or are still pending in the queue.
- 0: If the queue is empty and no callbacks were run.

**Preconditions:**
- `CONFIG(TIMER_QUEUE)` and `CONFIG(HAVE_MONOTONIC_TIMER)` must be
  enabled.

**Usage:**
Typically called in loops where deferred work might need processing:
```c
#include <timer.h>

// Example main loop structure
while (some_condition) {
    // ... do other work ...

    // Process any expired timer callbacks
    timers_run();

    // ... potentially yield or sleep ...
}
```


## Usage Examples

### Basic Timing Measurement Example

```c
#include <timer.h>
#include <console/console.h>

struct stopwatch sw;
stopwatch_init(&sw);

// ---> Code section to measure start
// ... perform some operations ...
// ---> Code section to measure end

// stopwatch_duration_usecs implicitly ticks the stopwatch if needed
int64_t duration_us = stopwatch_duration_usecs(&sw);
printk(BIOS_INFO, "Operation took %lld microseconds\n", duration_us);

// Or in milliseconds
int64_t duration_ms = stopwatch_duration_msecs(&sw);
printk(BIOS_INFO, "Operation took %lld milliseconds\n", duration_ms);
```


### Timeout Operation (Polling) Example

```c
#include <timer.h>
#include <console/console.h>
#include <stdint.h> // For uint8_t example

// Hypothetical hardware status check
extern uint8_t check_hardware_status(void);
#define HW_READY (1 << 0)

struct stopwatch sw;
// Initialize stopwatch with a 100 millisecond timeout
stopwatch_init_msecs_expire(&sw, 100);

uint8_t status = 0;
while (!(status & HW_READY)) {
    status = check_hardware_status();

    if (stopwatch_expired(&sw)) {
        printk(BIOS_ERR, "Operation timed out waiting for HW_READY\n");
        // Handle timeout error...
        status = 0; // Indicate failure perhaps
        break;
    }
    // Optional: Add a small delay here to avoid busy-spinning the CPU excessively
    // udelay(10); // e.g., wait 10us between checks
}

if (status & HW_READY) {
    printk(BIOS_DEBUG, "Hardware became ready.\n");
    // Continue with operation...
}
```


### Waiting for a Condition (Using `wait_ms`) Example

```c
#include <timer.h>
#include <console/console.h>
#include <stdint.h> // For uint8_t example

// Hypothetical hardware status check
extern uint8_t check_hardware_status(void);
#define HW_READY (1 << 0)

// Wait up to 100ms for the HW_READY bit
int64_t waited_ms = wait_ms(100, (check_hardware_status() & HW_READY));

if (waited_ms > 0) {
    printk(BIOS_INFO, "Condition met: HW_READY asserted after ~%lld ms\n", waited_ms);
    // Continue...
} else {
    printk(BIOS_ERR, "Timeout: HW_READY not asserted within 100 ms\n");
    // Handle timeout error...
}
```

### Scheduling a Timer Callback Example

```c
#include <timer.h>
#include <console/console.h>

// Data structure for our callback context
struct my_callback_data {
    int counter;
    const char *message;
};

// The callback function
static void my_callback_handler(struct timeout_callback *tocb)
{
    struct my_callback_data *data = tocb->priv;
    printk(BIOS_INFO, "Callback executed! Message: %s, Counter: %d\n",
           data->message, data->counter);
    // Note: 'tocb' can be reused here to reschedule if needed,
    // or the memory it points to can be freed if dynamically allocated.
}

// Somewhere in initialization code:
static struct timeout_callback my_timer;
static struct my_callback_data my_data;

void schedule_my_task(void)
{
    my_data.counter = 42;
    my_data.message = "Hello from timer";

    my_timer.priv = &my_data;
    my_timer.callback = my_callback_handler;

    // Schedule the callback to run after 500 milliseconds (500,000 us)
    int rc = timer_sched_callback(&my_timer, 500 * 1000);
    if (rc == 0) {
        printk(BIOS_DEBUG, "Scheduled my_callback_handler successfully.\n");
    } else {
        printk(BIOS_ERR, "Failed to schedule callback!\n");
    }
}

// Remember that timers_run() must be called periodically elsewhere
// for the callback to actually execute after the delay.
```

## Best Practices

1. **Enable Monotonic Timer:** For accurate timing and stopwatch
   functionality, ensure `CONFIG(HAVE_MONOTONIC_TIMER)` is enabled and a
   suitable platform timer is configured.
2. **Minimize Stopwatch Overhead:** While lightweight, frequent calls,
   especially `timer_monotonic_get()` implicitly called by stopwatch
   functions, can add up. Measure larger, significant code blocks rather
   than tiny ones in tight loops unless absolutely necessary.
3. **Use Appropriate Time Units:** Choose `usecs` or `msecs` variants
   based on the scale of time you are dealing with.
4. **Handle Timeouts Gracefully:** When using expiration or
   `wait_ms`/`wait_us`, always check the result and handle the timeout
   case appropriately. Don't assume success.
5. **Process Timer Callbacks:** If using `timer_sched_callback`, ensure
   `timers_run()` is called regularly in your main processing loops or
   idle states.
6. **Avoid `mdelay` for Polling:** Prefer `wait_ms` or manual polling
   with `stopwatch_expired` when waiting for conditions, as `mdelay`
   offers no timeout handling or feedback.
7. **Consider Platform Accuracy:** Timer resolution and accuracy vary
   between platforms. Don't assume microsecond precision unless verified
   for the target hardware.


## Limitations

1. **Monotonic Timer Dependence:** Stopwatch and timer callbacks rely
    heavily on `CONFIG(HAVE_MONOTONIC_TIMER)`. Without it, their
    behavior is limited.
2. **Timer Resolution:** Accuracy is limited by the underlying platform
    timer's resolution and the frequency of `timer_monotonic_get()`
    updates internally.
3. **64-bit Microsecond Counter:** While large, the
    `uint64_t microseconds` counter will eventually wrap around
    (after ~584,000 years), though this is not a practical concern for
    boot times. More relevant is the potential rollover of the
    *underlying hardware counter* between `timer_monotonic_get` calls,
    which the implementation must handle correctly (typically ok for
    intervals up to several seconds).
4. **Busy Waiting:** `stopwatch_wait_until_expired` and the internal
   loops of `wait_us`/`wait_ms` (and potentially the fallback in
   `udelay`/`mdelay`) perform busy-waits, consuming CPU cycles. Consider
   alternatives like interrupt-driven timers or yielding
   (`thread_yield_microseconds`) if power consumption or concurrency is
   critical.
5. **Callback Queue Size:** The timer callback queue has a fixed size
   defined by `CONFIG(TIMER_QUEUE_SIZE)`, limiting the number of pending
   callbacks.


## Troubleshooting

Common issues and solutions:

1. **Inaccurate Timing / Durations are Zero:**
  - Verify `CONFIG(HAVE_MONOTONIC_TIMER)` is enabled.
  - Check if a platform-specific `init_timer()` is required and being
    called.
  - Ensure the platform timer frequency is correctly configured.
  - Check for potential timer hardware issues or conflicts.

2. **Timeouts Not Working / `wait_ms` Never Returns > 0:**
  - Verify the timeout value is appropriate (not too short).
  - Double-check the `condition` logic in `wait_ms`/`wait_us`. Is it
    actually capable of becoming true?
  - Ensure the stopwatch (`sw` or the internal one in `wait_ms`) is
    properly initialized before the check loop.
  - Confirm `CONFIG(HAVE_MONOTONIC_TIMER)` is enabled.

3. **Timer Callbacks Not Running:**
  - Verify `CONFIG(TIMER_QUEUE)` is enabled.
  - Ensure `timer_sched_callback()` returned success (0).
  - **Crucially:** Check that `timers_run()` is being called
    periodically in a suitable loop after the callback was scheduled.
  - Make sure the `struct timeout_callback` structure persists in memory
    until the callback runs.

4. **Performance Impact:**
  - Reduce frequency of stopwatch checks or `wait_ms`/`wait_us` calls if
    possible.
  - Measure larger code blocks.
  - Investigate if the underlying `udelay` implementation is
    busy-waiting excessively; yielding (`thread_yield_microseconds`)
    might be preferable if available and appropriate.


## Platform-Specific Considerations

The core APIs are generic, but the underlying implementation relies on
platform code:
- **Timer Source:** Platforms implement `timer_monotonic_get` (often
  weakly linked) or provide the necessary hooks for it, using hardware
  like the APIC timer (x86), Time Base (PPC), architectural timers
  (ARM), etc.
- **`init_timer()`:** Platforms may need a custom `init_timer` to set up
  clocks, dividers, or enable the timer hardware.
- **`udelay`:** Platforms might provide optimized `udelay`
  implementations.

Refer to the documentation and code within specific `src/soc/`,
`src/cpu/`, or `src/arch/` directories for platform details.


## References

- `src/include/timer.h`: Monotonic timer, stopwatch, and callback
  declarations.
- `src/include/delay.h`: `udelay`, `mdelay`, `delay` declarations.
- `src/lib/timer.c`: Generic `udelay` (using stopwatch/yield), weak
  `init_timer`.
- `src/lib/delay.c`: Generic `mdelay` and `delay` implementations
  (calling `udelay`).
- `src/lib/hardwaremain.c`: Example usage of `timers_run()` in boot
  state machine.
- Platform timer implementations: Search for `timer_monotonic_get` or
  `init_timer` in `src/cpu/`, `src/soc/`, `src/arch/` directories
  (e.g., `src/cpu/x86/lapic/apic_timer.c`,
  `src/arch/arm64/timer.c`).
