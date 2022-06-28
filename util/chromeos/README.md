# ChromeOS Scripts

These scripts can be used to access or generate ChromeOS resources, for example
to extract System Agent reference code and other blobs (e.g. `mrc.bin`, refcode,
VGA option roms) from a ChromeOS recovery image.

## crosfirmware.sh

`crosfirmware.sh` downloads a ChromeOS recovery image from the recovery
image server, unpacks it, extracts the firmware update shell archive,
extracts the firmware images from the shell archive.

To download all ChromeOS firmware images, run

    $ ./crosfirmware.sh


To download, e.g. the Panther firmware image, run

    $ ./crosfirmware.sh panther

## extract_blobs.sh

`extract_blobs.sh` extracts the blobs from a ChromeOS firmware image.

Right now it will produce the ME firmware blob, IFD, VGA option rom,
and `mrc.bin`.

## gen_test_hwid.sh

`gen_test_hwid.sh` generates a test-only identifier in ChromeOS HWID v2
compatible format.

Usage:

    $ ./gen_test_hwid.sh BOARD_NAME

Example:

    $ ./gen_test_hwid.sh Kukui
    KUKUI TEST 9847
