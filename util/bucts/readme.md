# What is bucts?
Many Intel southbridges provide a mechanism called Back Up Control Top Swap (`BUC.TS`).
This functionality allows to have the southbridge fetch the reset vector or
the beginning of the bootblock at a 64K/128/256K offset from the usual top of flash.

This can be useful in different ways:
- Have a backup bootblock in case of bootblock hacking;
- Some vendor BIOS only write protect their bootblock so this tool makes it
  possible to circumvent this protection to allow flashing and booting coreboot.

The BUC.TS status is stored in a nvram bit. To clear it one has to remove the RTC battery.

# Operation Description
First compile bucts by running make:

	$ make

Then you can view the current system settings:

	$ ./bucts --print

To flip the decode address of the bootblock, by setting `BUC.TS` to 1:

	$ ./bucts --set

To set the behavior the regular mapping, by setting `BUC.TS` to 0

	$ ./bucts --unset


# Details
Example Bootblock size of 64KB (this is only configurable to be something else
like 128 or 256K on PCH Intel targets).

<pre>
    +-------------+ -> 0x200000         +-------------+ -> 0xFFFFFFFF       +-------------+ -> 0xFFFFFFFF
    |             |                     |             |                     |             |
    | bootblock_0 |                     | bootblock_0 |                     | bootblock_1 |
    |             |                     |             |                     |             |
    +-------------+ -> 0x1F0000         +-------------+ -> 0xFFFF0000       +-------------+ -> 0xFFFF0000
    |             |                     |             |                     |             |
    | bootblock_1 |                     | bootblock_1 |                     | bootblock_0 |
    |             |                     |             |                     |             |
    +-------------+ -> 0x1E0000         +-------------+ -> 0xFFFE0000       +-------------+ -> 0xFFFE0000
    |             |                     |             |                     |             |
    |             |                     |             |                     |             |
    Z             Z                     Z             Z                     Z             Z
    Z             Z                     Z             Z                     Z             Z
    |             |                     |             |                     |             |
    |             |                     |             |                     |             |
    FLASH (2M)                          Memory Map BUC.TS=0               Memory Map BUC.TS=1
</pre>
