# Flashing firmware internally

**WARNING:** If you flash a broken firmware and have no recovery mechanism, you
             must use the **external method** to flash a working firmware again.

## Using flashrom
This method does only work on Linux, if it isn't locked down.
You may also need to boot with `iomem=relaxed` in the kernel command
line if CONFIG_IO_STRICT_DEVMEM is set.


For more details please also check [flashrom's wiki].
Use the programmer *internal* to flash *coreboot.rom* internally:

```bash
flashrom -p internal -w coreboot.rom
```

[flashrom's wiki]: https://www.flashrom.org/Flashrom
