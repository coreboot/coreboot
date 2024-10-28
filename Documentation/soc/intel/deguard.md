# Intel BootGuard bypass on Skylake/Kabylake

Run `git clone "https://review.coreboot.org/deguard"` to get the deguard source code.

## How deguard works

Deguard makes use of [CVE-2017-5705].

This vulnerability has been fixed by Intel in newer ME v11.x.x.x firmware releases, however ME
11 hardware has no protection against downgrading the ME version by overwriting the SPI flash
physically, thus we can downgrade to a vulnerable version.

After downgrade, we exploit the bring-up (BUP) module of the vulnerable firmware, overwriting
the copy of field programmable fuses (FPF) stored in SRAM, resulting in the fused BootGuard
configuration being replaced with our desired one (which has Bootguard disabled).

## Supported boards

The list of supported boards can be seen by examining the degaurd repo:

    ls -l data/delta

If your board is already supported, you can skip to `Generating a deguarded ME image` below.

If not, then you need to generate the delta data for your board first.

## Adding a new board/target

If your board is not yet supported, you will need to generate the delta data between the default
ME configuration and vendor-provided board-specific ME configuration.

This data is located in the `data/delta/<target>` directory for each target.

To generate the delta data, dump the vendor firmware from your board, and execute:

    ./generatedelta.py --input <dump> --output data/delta/<target>

NOTE:
The delta generation process only takes your factory dump ME firmware as an input. This is
because an ME firmware image contains both the default and system specific configuration, which
are then compared by deguard.

You *must discard* the `/home/secureboot` directory from the delta for the zero FPF config to work.

You can optionally also discard `home/{amt,fwupdate,pavp,ptt}` from the delta.

Please submit delta data for new boards to the deguard repo using the regular Gerrit review
process.

## Generating a deguarded ME image

Once you have the delta data for your board, you will need to obtain a donor image for your
platform variant with a supported/vulnerable ME version (see URLs below).

This can either be a full image with a flash descriptor or just a bare ME region.

Afterwards, execute the following command and enjoy:

    ./finalimage.py --delta data/delta/<target> --version <donor version> --pch <H or LP PCH type> --sku <2M or 5M SKU> --fake-fpfs data/fpfs/zero --input <donor> --output <output>

Example: `./finalimage.py --delta data/delta/thinkpad_t480 --version 11.6.0.1126 --pch LP --sku 2M --fake-fpfs data/fpfs/zero --input me_donor.bin --output me_deguarded.bin`

NOTE:
The output will be a bare deguard-patched ME region which requires **the HAP bit to be enabled**
in your flash descriptor to work.

If you do not enable the HAP bit, the deguard-patched ME will not load, and the system will not
boot.

## Note on field programmable fuses

This document recommends faking a set of FPFs that are all zero as a BootGuard bypass strategy.

This causes the platform to work in legacy mode, and does not require dumping the fuses from the
PCH.

It is also possible to enable measured mode instead (there is some example FPF data for this).

Theoretically it is possible to even re-enable BootGuard with a custom private key (with the
caveat that it is obviously insecure against physical access).

## Donor images

This section lists some URLs to recommended and tested donor images. Any image with a supported
firmware version and variant ought to work, but the path of least resistance is for everyone to
use the same images.

|Version|Variant|SKU|URL|Notes|
|-|-|-|-|-|
|11.6.0.1126|H (Desktop)|2M|[link](https://web.archive.org/web/20230822134231/https://download.asrock.com/BIOS/1151/H110M-DGS(7.30)ROM.zip)|Zipped flash image|
|11.6.0.1126|LP (Laptop)|2M|[link](https://dl.dell.com/FOLDER04573471M/1/Inspiron_5468_1.3.0.exe)|Dell BIOS update (use [Dell_PFS_Extract.py](https://github.com/vuquangtrong/Dell-PFS-BIOS-Assembler/blob/master/Dell_PFS_Extract.py))|

## Thanks

Thanks goes to PT Research and Youness El Alaoui for previous work on exploiting Intel SA 00086.

- [IntelTXE-PoC](https://github.com/kakaroto/IntelTXE-PoC)
- [MFSUtil](https://github.com/kakaroto/MFSUtil)

[CVE-2017-5705]: https://www.intel.com/content/www/us/en/security-center/advisory/intel-sa-00086.html