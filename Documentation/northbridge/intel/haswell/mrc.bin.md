# mrc.bin

All Haswell boards supported by coreboot currently require a proprietary
blob in order to initialise the DRAM and a few other components. The
blob, named `mrc.bin`, largely consists of Intel's memory reference code
(MRC), but it has been tailored specifically for Chrome OS. It is just
under 200 KiB in size. Another name for `mrc.bin` is the system agent
binary.

Having a replacement for `mrc.bin` using native coreboot code is very
much desired, but it is not an easy task.

## Obtaining mrc.bin

Unfortunately, it is not currently possible to distribute `mrc.bin` as
part of coreboot. Though, it can be obtained from a Haswell Chromebook
firmware image like so, starting in the root of the coreboot directory:

```bash
make -C util/cbfstool
cd util/chromeos
./crosfirmware.sh peppy
../cbfstool/cbfstool coreboot-*.bin extract -f mrc.bin -n mrc.bin -r RO_SECTION
```

Now, place `mrc.bin` in the root of the coreboot directory.
Alternatively, place `mrc.bin` anywhere you want, and set `MRC_FILE` to
its location when building coreboot.

## ECC DRAM

When `mrc.bin` has finished executing, ECC is active on the channels
populated with ECC DIMMs. However, `mrc.bin` was tailored specifically
for Haswell Chromebooks and Chomeboxes, none of which support ECC DRAM.
While ECC likely functions correctly, it is advised to further validate
the correct operation of ECC if data integrity is absolutely critical.
