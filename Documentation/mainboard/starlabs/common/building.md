## Building coreboot

### Preliminaries

Prior to building coreboot the following files are required:

#### StarBook series:
* Intel Flash Descriptor file (descriptor.bin)
* Intel Management Engine firmware (me.bin)
* ITE Embedded Controller firmware (ec.bin)

#### StarLite series:
* Intel Flash Descriptor file (descriptor.bin)
* IFWI Image (ifwi.rom)

The files listed below are optional:
- Splash screen image in Windows 3.1 BMP format (Logo.bmp)

These files exist in the correct location in the [StarLabsLtd/blobs](https://github.com/StarLabsLtd/blobs) repo on GitHub which is used in place of the standard 3rdparty/blobs repo.

### Build

The following commands will build a working image, where the last two words represent the
series and processor i.e. `lite_glkr`:

```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.starlabs_starbook_adl
make
```
