# Star Labs LabTop

## Specs

- CPU (full processor specs available at https://ark.intel.com)
    - Intel i7-10710U (Comet Lake)
    - Intel i3-10110U (Comet Lake)
    - Intel i7-8550u  (Kaby Lake Refresh)
- EC
    - ITE IT8987E
    - Backlit Keyboard, with standard PS/2 keycodes and SCI hotkeys
    - Battery
    - Charger, using AC adapter or USB-C PD
    - Suspend / resume
- GPU
    - Intel UHD Graphics 620
    - GOP driver is recommended, VBT is provided
    - eDP 13-inch 1920x1080 LCD
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - 16GB on-board for Comet Lake platforms[^1]
    - 8GB on-board for Kaby Lake Refresh platform.
- Networking
    - AX201 CNVi WiFi / Bluetooth soldered to PCBA (Comet Lake)
    - 8265 PCIe WiFi / Bluetooth soldered to PCBA (Kaby Lake Refresh)
- Sound
    - Realtek ALC256
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - HDMI audio
    - USB-C DisplayPort audio
- Storage
    - M.2 PCIe SSD
    - RTS5129 MicroSD card reader
- USB
    - 1280x720 CCD camera
    - USB 3.1 Gen 2 Type-C (left)
    - USB 3.1 Gen 2 Type-A (left)
    - USB 3.1 Gen 1 Type-A (right)

[^1] The Comet Lake PCB supports multiple memory variations that are based on hardware configuration resistors see `src/mainboard/starlabs/labtop/variants/cml/romstage.c`

## Building coreboot

### Preliminaries

Prior to building coreboot the following files are required:

Comet Lake and Kaby Lake configurations:
- Intel Flash Descriptor file (descriptor.bin)
- Intel Management Engine firmware (me.bin)

Comet Lake configuration only:
- ITE IT8987E firmware (it8987-x.xx.bin)

All Star Labs platforms:
- Splash screen image in Windows 3.1 BMP format (Logo.bmp)

These files exist in the correct location in the StarLabsLtd/blobs repo on GitHub which is used in place of the standard 3rdparty/blobs repo.

### Build

The following commands will build a working image:

##### LabTop Mk IV (Comet Lake)

```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.starlabs_labtop_cml
make
```

##### LabTop Mk III (Kaby Lake)

```bash
make distclean
make defconfig KBUILD_DEFCONFIG=configs/config.starlabs_labtop_kbl
make
```

## Flashing coreboot

```eval_rst
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Model               | 25Q128JVSQ |
+---------------------+------------+
| Size                | 16 MiB     |
+---------------------+------------+
| Package             | SOIC-8     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | no         |
+---------------------+------------+

#### **Requirements:**

* Only available on Star Labtop Mk IV and Star LabTop Mk III
* fwupd version 1.5.6 or later
* The battery must be charged to at least 30%
* The charger must be connected (either USB-C or DC Jack)
* BIOS Lock must be disabled
* Supported Linux distribution (Ubuntu 20.04 +, Linux Mint 20.1 + elementaryOS 6 +, Manjaro 21+)

**fwupd 1.5.6 or later**
To check the version of **fwupd** you have installed, open a terminal window and enter the below command:

```
fwupdmgr --version
```

This will show the version number. **1.5.6** or greater will work.
![fwupd version](https://cdn.shopify.com/s/files/1/2059/5897/files/fwupdV.png?v=1611136423)
On Ubuntu 20.04, Ubuntu 20.10, Linux Mint 20.1 and elementaryOS 6, fwupd 1.5.6 can be installed from our PPA with the below terminal commands:

```
sudo add-apt-repository ppa:starlabs/ppa
sudo apt update
sudo apt install fwupd
```

On Manjaro:

```
sudo pacman -Sy fwupd-git flashrom-starlabs
```

Instructions for other distributions will be added once fwupd 1.5.6 is available. If you are not using one of the distributions listed above, it is possible to install coreboot using a Live USB.

**Disable BIOS Lock**
BIOS Lock must be disabled when switching from the standard AMI (American Megatrends Inc.) firmware to coreboot. To disable BIOS Lock:

1\. Start with your LabTop turned off\. Turn it on whilst holding the **F2** key to access the BIOS settings.
2\. When the BIOS settings load, use the arrow keys to navigate to the advanced tab\. Here you will see BIOS Lock\.
3\. Press `Enter` to change this setting from **Enabled** to **Disabled**

![Disable BIOS Lock](https://cdn.shopify.com/s/files/1/2059/5897/files/IMG_20210120_094049709_1.jpg?v=1611139567)

4\. Next, press the `F10` key to **Save & Exit** and then `Enter` to confirm.

#### **Switching Branch**

Switching branch refers to changing from AMI firmware to coreboot, or vice versa.

First, check for new firmware files with the below terminal command:

```
fwupdmgr refresh --force
```

Then, to change branch, enter the below terminal command:

```
fwupdmgr switch-branch
```

You can then select which branch you would like to use, by typing in the corresponding number:
![Switch Branch](https://cdn.shopify.com/s/files/1/2059/5897/files/SwitchBranch.png?v=1611138496)
You will be prompted to confirm, press `y` to continue or `n` to cancel.

Once the switch has been completed, you will be prompted to restart.

![Installed coreboot](https://cdn.shopify.com/s/files/1/2059/5897/files/Complete.png?v=1611138934)
The next reboot can take up to **5 minutes,** do not interrupt this process or disconnect the charger. Once the reboot is complete, that's it - you'll continue to receive updates for whichever branch you are using.

You can switch branch at any time.