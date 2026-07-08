# Malloc & Free in coreboot

This document provides a guide to the implementation and usage of
`malloc` and `free` within the coreboot project.

## Historical Context

Because coreboot has a very short lived execution environment, it does
not employ a traditional operating system memory manager or a complex
heap allocator with capabilities like fragmentation reduction, multiple
bins, or coalescing freed blocks. The complex heap management simply
isn't needed for coreboot. Look at the
[libpayload](#malloc--free-in-libpayload) section below for a full heap
allocator with complete `malloc` & `free` implementations.

For the vast majority of coreboot's history, the `free()` function
literally did nothing but return. When it was initially checked into the
codebase in 2003, it was a dummy function with an empty body. In 2012,
it was removed entirely from `malloc.c` and replaced with an inline
stub, as the execution time of coreboot is so brief that memory never
actually needed to be freed. It wasn't until late 2019 (commit
`a347ea3787`) that `free()` was given its current behavior.

## Implementation

The current allocator (implemented in `src/lib/malloc.c`) uses a trivial
bump allocator (also known as a linear allocator). Three static pointers
track the allocator state:

- `free_mem_ptr` — points to the next free byte in the heap.
- `free_mem_end_ptr` — points to the end of the heap.
- `free_last_alloc_ptr` — points to the start of the most recent
  allocation, used by `free()` for its rewind feature.

The following diagram illustrates how these pointers move through a
sequence of allocations and a free:

```text
After malloc(A), malloc(B):

  _heap                                          _eheap
  |                                                 |
  v                                                 v
  +----------+----------+---------------------------+
  |    A     |    B     |     (free space)          |
  +----------+----------+---------------------------+
             ^          ^
  free_last_alloc_ptr   free_mem_ptr

After free(B) — rewinds because B was the last allocation:

  _heap                                          _eheap
  |                                                 |
  v                                                 v
  +----------+--------------------------------------+
  |    A     |          (free space)                |
  +----------+--------------------------------------+
             ^
    free_mem_ptr
   (free_last_alloc_ptr = NULL, no further rewind possible)
```

Because this is a bump allocator, memory from non-last allocations
cannot be reclaimed. This is by design — coreboot's brief execution
lifetime means the entire environment is torn down before handing
control to the OS payload, so long-term memory leaks don't exist in the
normal sense.

## API Reference

### `void *malloc(size_t size)`

A thin wrapper that calls `memalign(sizeof(u64), size)`, aligning each
allocation to an 8-byte boundary. Returns `NULL` on failure.

### `void *calloc(size_t nitems, size_t size)`

Allocates `nitems * size` bytes via `malloc()` and zeroes the memory
with `memset()`. Returns `NULL` on failure. Note that the implementation
does not check for integer overflow on `nitems * size`.

### `void *memalign(size_t boundary, size_t size)`

The core allocation function. All other allocation functions delegate to
`memalign()`. It aligns `free_mem_ptr` up to `boundary`, records the
aligned position as `free_last_alloc_ptr`, then advances `free_mem_ptr`
by `size`. Returns a pointer to the allocated region, or `NULL` if the
heap is exhausted (printing error messages at `BIOS_ERR` level).

Note that `memalign()` does not enforce any restrictions on the boundary
value — as the source code comment states, "This is firmware, you are
supposed to know what you are doing."

### `void free(void *ptr)`

Frees memory allocated by `malloc()`, `calloc()`, or `memalign()`. The
behavior is as follows:

1. If `ptr` is `NULL`, returns immediately (consistent with standard C).
2. If `ptr` falls outside the valid heap range (`_heap` to
   `free_mem_end_ptr`), prints a `BIOS_WARNING` and returns.
3. If `ptr` matches `free_last_alloc_ptr` (i.e., it was the most recent
   allocation), rewinds `free_mem_ptr` back to that location and sets
   `free_last_alloc_ptr` to `NULL`, effectively reclaiming the memory.
4. Otherwise, does nothing — the memory cannot be reclaimed by the bump
   allocator.

Because `free_last_alloc_ptr` is set to `NULL` after a successful
rewind, only a single rewind is possible. Calling `free()` again on an
earlier allocation will have no effect.

## Heap Initialization & Size

The heap in coreboot is not a complex dynamic structure initialized at
runtime. Instead, the start (`_heap`) and end (`_eheap`) symbols are
defined directly in the linker scripts (e.g., `src/lib/program.ld`).

The total size of the heap is statically determined at build time by the
`CONFIG_HEAP_SIZE` Kconfig option. The general default size is
`0x100000` (1MB), as set in `src/Kconfig`. However, specific platforms
may override this default. For example, several Intel SoCs (Alder Lake,
Meteor Lake, Panther Lake, Lunar Lake) default to `0x200000` (2MB),
Intel Xeon SP GNR defaults to `0x80000` (512KB), and the SiFive HiFive
Unmatched board defaults to `0x10000` (64KB).

Because the allocator's internal pointers (like `free_mem_ptr`) are
statically initialized to `&_heap`, the allocator is ready to use
immediately as soon as the C environment is set up for a stage. The heap
memory region itself is generally not zeroed out during stage loading
because the `.heap` section is marked as `(NOLOAD)` in the linker
script. If you require zero-initialized memory, you must use `calloc()`,
which explicitly zeroes the requested block.

## Usage Recommendations

Given this implementation, there can be questions from developers about
whether to bother writing `free()` at all.

The general guidance for `malloc` and `free` usage is as follows:

1. **Use `free()` when it matters**: If you are allocating a large,
   temporary buffer and you know you can free it *before* any other
   allocations are made, you should definitely call `free()`. This takes
   advantage of the bump allocator's rewind feature and saves space on
   the heap. Reviewers may specifically ask for a `free()` if they
   notice a non-trivial, temporary allocation that fits this criteria.
2. **You are not forced to `free()` everywhere**: If an allocation
   obviously cannot be freed usefully under the current implementation
   (e.g., other allocations have occurred since), you are not strictly
   required to include a `free()` call just to satisfy standard C
   conventions or static analyzers.
3. **Author's Discretion**: In general situations, adding a `free()`
   call is left up to the discretion of the author. It does no harm, and
   can be useful in specific cases.

In short, developers can ignore `free()` unless the last allocated space
can be freed and it's large enough to bother with. If you want to add it
for completeness, that is fine, but it is not a strict requirement for
all memory allocations in the coreboot source tree.

## Debugging the coreboot Heap

If you need to trace memory allocations, you can enable the
`DEBUG_MALLOC` Kconfig option. When enabled, the allocator will print
debugging information (such as the boundaries and pointers of each
allocation or free event) to the console at the `BIOS_SPEW` log level.

### Heap Poisoning and ASAN

Because the coreboot bump allocator is extremely rudimentary, it does
*not* support per-allocation heap poisoning or redzones natively. There
are no per-object redzones, and `free()` does not poison freed memory,
so use-after-free bugs within the heap will not be detected at the
allocator level.

However, coreboot does support AddressSanitizer (ASAN) during the
ramstage via the `CONFIG_ASAN_IN_RAMSTAGE` option. When enabled, ASAN
uses a shadow memory region to track which bytes are valid across the
entire ramstage memory map. It can detect out-of-bounds reads and writes
(e.g., accesses past `_eheap`), stack buffer overflows, and
use-after-scope errors. However, because ASAN does not hook individual
`malloc()` or `free()` calls in coreboot's bump allocator, it cannot
detect heap-specific errors such as intra-heap buffer overruns between
adjacent allocations or use-after-free within the heap region.

## Malloc & Free in libpayload

It is important to differentiate between the coreboot execution
environment (which is brief and uses the bump allocator described above)
and the payload execution environment. Payloads can run for the entire
duration of the system's uptime and can have significantly more complex
memory management needs.

The `libpayload` library (located at `payloads/libpayload/`) provides
its own separate implementation of `malloc` and `free` in
`payloads/libpayload/libc/malloc.c`.

Unlike the main coreboot heap, `libpayload` implements a fully
functional (albeit simple, O(N)) dynamic heap allocator:

- **Functional `free()`:** Calling `free()` in `libpayload` actually
  works. It marks blocks as free and consolidates adjacent free blocks
  (coalescing) to reduce fragmentation.
- **Header Tracking:** Each allocated block has a header that tracks its
  size, whether it is free, and includes a "magic" byte to detect basic
  buffer overruns or heap poisoning.
- **Extended Functions:** It fully supports `realloc()`, `calloc()`, and
  `memalign()`, as well as specialized DMA memory allocators
  (`dma_malloc()`, `dma_memalign()`).
- **Debugging:** `libpayload` has its own debug configuration,
  `CONFIG_LP_DEBUG_MALLOC`, which when enabled tracks the maximum memory
  consumption, warns about poisoned memory blocks, and provides a
  `print_malloc_map()` function to dump the entire state of the heap to
  the console.

In summary, while you can largely ignore `free()` in coreboot proper,
you must treat memory management with normal care when writing or
modifying `libpayload` and any payloads using it, ensuring you properly
`free()` memory to avoid leaks.
