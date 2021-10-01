# x86 System Management Mode

## Introduction

The code running in System Management Mode (SMM) provides runtime services
to applications running in [ring0]. It has a higher privilege level than
[ring0] and resides in the SMRAM region which cannot be accessed from [ring0].

SMM can be entered by issuing System Management Interrupts (SMIs).

## Secure data exchange

In order to not leak SMM internals or accidentally overwrite parts of SMM,
[ring0] provided data (pointers, offsets, sizes, ...) must be checked before
using them in SMM.

There exist two methods to verify data:

```C
/* Returns true if the region overlaps with the SMM */
bool smm_region_overlaps_handler(struct region *r);
```

```C
/* Returns true if the memory pointed to overlaps with SMM reserved memory. */
static inline bool smm_points_to_smram(const void *ptr, const size_t len);
```

[ring0]: https://en.wikipedia.org/wiki/Protection_ring
