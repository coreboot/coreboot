# Flashing firmware standalone

If none of the other methods work, there are three possibilities:

## Desolder
You must remove or desolder the flash IC before you can flash it.
It's recommended to solder a socket in place of the flash IC.

When flashing the IC, always connect all input pins.
If in doubt, pull /WP, /HOLD, /RESET and alike up towards Vcc.

## SPI flash emulator
If you are a developer, you might want to use an [EM100Pro] instead, which sets
the onboard flash on hold, and allows to run custom firmware.
It provides a very fast development cycle without actually writing to flash.

## SPI flash overwrite
It is possible to set the onboard flash on hold and use another flash chip.
Connect all lines one-to-one, except /HOLD. Pull /HOLD of the soldered flash IC
low, and /HOLD of your replacement flash IC high.


[EM100Pro]: https://www.dediprog.com/product/EM100Pro
