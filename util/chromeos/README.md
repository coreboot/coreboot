Chrome OS binary extraction
===========================

These scripts can be used to extract System Agent reference code
and other blobs (e.g. mrc.bin, refcode, VGA option roms) from a
Chrome OS recovery image.

crosfirmware.sh
---------------

crosfirmware.sh downloads a Chrome OS recovery image from the recovery
image server, unpacks it, extracts the firmware update shell archive,
extracts the firmware images from the shell archive.

To download all Chrome OS firmware images, run
$ ./crosfirmware.sh

To download, e.g. the Panther firmware image, run
$ ./crosfirmware.sh panther

extract_blobs.sh
----------------

extract_blobs.sh extracts the blobs from a Chrome OS firmware image.

Right now it will produce the ME firmware blob, IFD, VGA option rom,
and mrc.bin
