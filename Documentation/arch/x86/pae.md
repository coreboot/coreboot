# x86_32 PAE documentation

Due to missing x86_64 support it's required to use PAE enabled x86_32 code.
The corresponding functions can be found in ``src/cpu/x86/pae/``.

## Memory clearing helper functions

To clear all DRAM on request of the
[Security API](../../security/memory_clearing.md), a helper function can be used
called `memset_pae`.
The function has additional requirements in contrast to `memset`, and has more
overhead as it uses virtual memory to access memory above 4GiB.
Memory is cleared in 2MiB chunks, which might take a while.

Make sure to enable caches through MTRRs, otherwise `memset_pae` will be slow!
