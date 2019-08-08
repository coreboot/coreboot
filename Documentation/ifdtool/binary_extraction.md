# Intel IFD Binary Extraction Tutorial

## Part 1: Extracting Binaries

To begin extracting the binaries, first create a directory labeled "binaries"
in the coreboot directory (i.e. /path/to/coreboot/binaries/).

Now, execute the following commands to extract the binaries from a ROM image.
**Note:** Make sure you are in the root coreboot directory.

    cd /path/to/coreboot/util/ifdtool
    ./ifdtool COREBOOT_IMAGE
    ./ifdtool -d COREBOOT_IMAGE
    ./ifdtool -x COREBOOT_IMAGE

In the above steps, COREBOOT_IMAGE is the name of the ROM image to extract the
binaries from, including the file path (ex. /build/coreboot.rom).

Copy the extracted .bin files to the binaries directory you created previously.
**Note:** You may want to rename your various .bin files to more clearly
indicate what they are and their purpose.

To extract the mrc.bin, move to the /coreboot/build directory and run the
following command:

    cd /path/to/coreboot/build/
    ./cbfstool COREBOOT_IMAGE extract -n mrc.bin -f /path/to/destination/filename

where COREBOOT_IMAGE is the filepath to the ROM image (same image as above),
/path/to/destination is the filepath to the destination directory and filename
is the output filename. An example command is given below:

    ./cbfstool coreboot.rom extract -n mrc.bin -f /path/to/coreboot/binaries/mrc.bin

## Part 2: Using extract_blobs.sh

To simplify some of the steps above, there is a script in the
/path/to/coreboot/util/chromeos/ directory called extract_blobs.sh what will
extract the flashdescriptor.bin and intel_me.bin files.

To run this script, switch to the /path/to/coreboot/util/chromeos/ directory
and execute the script providing a coreboot image as an argument.

    cd /path/to/coreboot/util/chromeos/
    ./extract_blobs.sh COREBOOT_IMAGE

Executing those commands will result in two binary blobs to appear in the
/path/to/coreboot/util/chromeos/ directory under the names
'flashdescriptor.bin' and 'me.bin'.

## Part 3: Changing the coreboot configuration settings

To begin using the binaries extracted above, enable the use of binary
repositories in the menuconfig. From the main coreboot directory, run
'make menuconfig'. Select "General Setup", then select "Allow use of
binary-only repository", then exit to the main menu.

To configure the ROM image for a specific board, select "Mainboard". Select
"Mainboard vendor" and scroll to the correct vendor. Then select "Mainboard
model" and select the name of the board model. Exit back to the main menu.

To add the binaries you extracted, select "Chipset". Scroll and select "Add a
System Agent Binary" and set the filepath to your mrc.bin file's filepath.
Scroll and select "Add Intel descriptor.bin file" and type the filepath for
your descriptor.bin file. Scroll down and select "Add Intel ME/TXE firmware
file" and type the filepath for your ME file. Exit to the main menu.

Select "Exit", and select "Yes" when prompted to save your configuration.