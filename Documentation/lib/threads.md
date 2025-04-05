# coreboot Threads

## Thread Management

coreboot provides a cooperative threading system that allows for
concurrent execution of tasks during the boot process. The thread API is
particularly useful for implementing asynchronous operations and
managing hardware initialization sequences.


### Thread Creation and Management

#### thread_run

```c
int thread_run(struct thread_handle *handle, enum cb_err (*func)(void *), void *arg)
```
Creates and starts a new thread to execute the specified function.

**Parameters:**
- `handle`: Pointer to a thread handle structure to track thread state
  (Note: `struct thread_handle` is an opaque structure used by the API
  to manage the thread's state.)
- `func`: Function to execute in the new thread
- `arg`: Argument to pass to the function

**Returns:**
- 0 on success
- < 0 on failure

**Example:**
```c
struct thread_handle th;
enum cb_err thread_func(void *arg) {
    // Thread work here
    return CB_SUCCESS;
}

if (thread_run(&th, thread_func, NULL) < 0) {
    printk(BIOS_ERR, "Failed to create thread\n");
} else {
    // Wait for the thread to complete and check its status
    enum cb_err err = thread_join(&th);
    if (err != CB_SUCCESS) {
        printk(BIOS_ERR, "Thread failed with error %d\n", err);
    }
}
```


#### thread_run_until

```c
int thread_run_until(struct thread_handle *handle, enum cb_err (*func)(void *), void *arg,
                    boot_state_t state, boot_state_sequence_t seq)
```
Creates a thread that blocks boot state transitions until completion.

**Parameters:**
- `handle`: Pointer to a thread handle structure
- `func`: Function to execute
- `arg`: Argument to pass to the function
- `state`: Boot state to block
- `seq`: Boot state sequence to block

**Returns:**
- 0 on success
- < 0 on failure

**Example:**
```c
struct thread_handle th;
enum cb_err init_func(void *arg) {
    // Hardware initialization
    return CB_SUCCESS;
}

// Block BS_DEV_ENABLE until initialization completes
thread_run_until(&th, init_func, NULL, BS_DEV_ENABLE, 0);
```


### Thread Synchronization

#### thread_join

```c
enum cb_err thread_join(struct thread_handle *handle)
```
Waits for a thread to complete and returns its error code.

**Parameters:**
- `handle`: Thread handle to wait for

**Returns:**
- Thread's error code (e.g., `CB_SUCCESS`, `CB_ERR`). See
  `src/include/cb_err.h` for details.

**Example:**
```c
struct thread_handle th;
// ... create thread ...

enum cb_err err = thread_join(&th);
if (err != CB_SUCCESS) {
    printk(BIOS_ERR, "Thread failed with error %d\n", err);
}
```


### Thread Yielding

Yielding is crucial in a cooperative multitasking system like
coreboot's. Threads must explicitly yield control using `thread_yield`
or `thread_yield_microseconds` to allow other threads to run. Failure to
yield can lead to a single thread monopolizing the CPU, preventing other
tasks from executing.

#### thread_yield

```c
int thread_yield(void)
```
Yields the current thread's execution to allow other threads to run.

**Returns:**
- 0 on success
- < 0 if thread cannot yield

**Example:**
```c
while (!condition) {
    if (thread_yield() < 0) {
        printk(BIOS_ERR, "Failed to yield thread\n");
        break;
    }
}
```


#### thread_yield_microseconds

```c
int thread_yield_microseconds(unsigned int microsecs)
```
Yields the current thread for a specified number of microseconds.

**Parameters:**
- `microsecs`: Number of microseconds to yield

**Returns:**
- 0 on success
- < 0 if thread cannot yield

**Example:**
```c
// Wait for 100 microseconds
if (thread_yield_microseconds(100) < 0) {
    printk(BIOS_ERR, "Failed to yield thread\n");
}
```


### Thread Cooperation Control

#### thread_coop_enable

```c
void thread_coop_enable(void)
```
Enables cooperative behavior for the current thread.

**Example:**
```c
thread_coop_enable();  // Allow thread to yield
```

#### thread_coop_disable

```c
void thread_coop_disable(void)
```
Disables cooperative behavior for the current thread.

**Example:**
```c
thread_coop_disable();  // Prevent thread from yielding
```


### Thread Mutexes

#### thread_mutex_lock

```c
void thread_mutex_lock(struct thread_mutex *mutex)
```
Acquires a mutex lock, waiting if necessary.

**Parameters:**
- `mutex`: Mutex to lock

**Example:**
```c
struct thread_mutex mtx = THREAD_MUTEX_INITIALIZER; // Or = { .locked = false };
thread_mutex_lock(&mtx);
// Critical section
thread_mutex_unlock(&mtx);
```


#### thread_mutex_unlock

```c
void thread_mutex_unlock(struct thread_mutex *mutex)
```
Releases a mutex lock.

**Parameters:**
- `mutex`: Mutex to unlock

## Best Practices

1. **Thread Safety**:
   - Use mutexes to protect shared resources
   - Be careful with global variables in threaded code
   - Consider thread cooperation when implementing critical sections

2. **Resource Management**:
   - Always join threads that you create using `thread_run` to check
     their completion status and clean up resources. Threads started
     with `thread_run_until` are implicitly managed by the boot state
     machine and typically do not require explicit joining.
   - Consistently check return values from thread creation and operation
     functions (like `thread_run`, `thread_yield`, `thread_join`) to
     detect errors early.
   - Clean up resources allocated or used within thread functions before
     they exit.

3. **Performance Considerations**:
   - Use thread_yield_microseconds for precise timing
   - Minimize time spent in critical sections
   - Consider using thread_run_until for hardware initialization

4. **Error Handling**:
   - Check thread creation and operation return values (as noted in
     Resource Management).
   - Implement proper error handling within thread functions, returning
     appropriate `cb_err` values.
   - Use `thread_join` (for `thread_run` threads) to check the final
     completion status.


## Common Patterns

### Hardware Initialization

```c
struct thread_handle init_th;
enum cb_err init_hardware(void *arg) {
    // Initialize hardware
    if (hardware_init() != 0)
        return CB_ERR;
    return CB_SUCCESS;
}

// Run initialization in a thread
thread_run_until(&init_th, init_hardware, NULL, BS_DEV_ENABLE, 0);
```

### Asynchronous Operation

```c
struct thread_handle async_th;
enum cb_err async_operation(void *arg) {
    // Perform async operation
    while (!operation_complete()) {
        if (thread_yield() < 0)
            return CB_ERR;
    }
    return CB_SUCCESS;
}

// Start async operation
thread_run(&async_th, async_operation, NULL);
```


### Critical Section Protection

```c
struct thread_mutex resource_mtx = { .locked = false };

void access_shared_resource(void) {
    thread_mutex_lock(&resource_mtx);
    // Access shared resource
    thread_mutex_unlock(&resource_mtx);
}
```

## Limitations

1. The thread system is cooperative, not preemptive.
2. Threads must explicitly yield to allow other threads to run.
3. Thread operations are typically only available after RAM
   initialization (in ramstage and later). Check specific environment
   constraints if unsure.
4. Thread count is limited by the `CONFIG_NUM_THREADS` Kconfig option.
5. Thread stack size is fixed by the `CONFIG_STACK_SIZE` Kconfig option.

