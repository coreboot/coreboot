# ABI data consumption

This text describes the ABI coreboot presents to downstream users. Such
users are payloads and/or operating systems. Therefore, this text serves
at what can be relied on for downstream consumption. Anything not explicitly
listed as consumable is subject to change without notice.

## Background and Usage

coreboot passes information to downstream users using coreboot tables. These
table definitions can be found in src/include/boot/coreboot_tables.h and
payloads/libpayload/include/coreboot_tables.h respectively within coreboot
and libpayload. One of the most vital and important pieces of information
found within these tables is the memory map of the system indicating
available and reserved memory.

In 2009 cbmem was added to coreboot. The "CBMEM high table memory manager"
serves a way for coreboot to bookkeep its own internal data. While some
of this data may be exposed through the coreboot tables the data structures
used to manage the data within the cbmem area is subject to change.

Provided the above, if one needs a piece of data exposed to the OS
or payload it should reside within the coreboot tables. If it isn't there
then a code change will be required to add it to the coreboot tables.
