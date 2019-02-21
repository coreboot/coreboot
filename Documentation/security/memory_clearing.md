# Memory clearing

The main memory on computer platforms in high security environments contains
sensible data. On unexpected reboot the data might persist and could be
read by a malicious application in the bootflow or userspace.

In order to prevent leaking information from pre-reset, the boot firmware can
clear the main system memory on boot, wiping all information.

A common API indicates if the main memory has to be cleared. That could be
on user request or by a Trusted Execution Environment indicating that secrets
are in memory.

As every platform has different bring-up mechanisms and memory-layouts, every
The device must indicate support for memory clearing as part of the boot
process.

## Requirements

1. The platform must clear all platform memory (DRAM) if requested
2. Code that is placed in DRAM might be skipped (as workaround)
3. Stack that is placed in DRAM might be skipped (as workaround)
4. All DRAM is cleared with zeros

## Implementation

A platform that supports memory clearing selects Kconfig
``PLATFORM_HAS_DRAM_CLEAR`` and calls

```C
bool security_clear_dram_request(void);
```

to detect if memory should be cleared.

The memory is cleared in ramstage as part of `DEV_INIT` stage. It's possible to
clear it earlier on some platforms, but on x86 MTRRs needs to be programmed
first, which happens in `DEV_INIT`.

Without MTRRs (and caches enabled) clearing memory takes multiple seconds.
## Exceptions

As some platforms place code and stack in DRAM (FSP1.0), the regions can be
skipped.

## Architecture specific implementations

* [x86 PAE](../arch/x86/pae.md)
