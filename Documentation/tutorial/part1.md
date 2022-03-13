Tutorial, part 1: Starting from scratch
===========================================

This tutorial will guide you through the process of setting up a working
coreboot toolchain. In same cases you will find specific instructions for Debian (apt-get),
Fedora (dnf) and Arch Linux (pacman) based package management systems. Use the
instructions according to your system.

Download, configure, and build coreboot
---------------------------------------

### Step 1 - Install tools and libraries needed for coreboot
    $ sudo apt-get install -y bison build-essential curl flex git gnat libncurses5-dev m4 zlib1g-dev
    $ sudo pacman -S base-devel curl git gcc-ada ncurses zlib
    $ sudo dnf install git make gcc-gnat flex bison xz bzip2 gcc g++ ncurses-devel wget zlib-devel patch

### Step 2 - Download coreboot source tree
    $ git clone https://review.coreboot.org/coreboot
    $ cd coreboot

### Step 3 - Build the coreboot toolchain
Please note that this can take a significant amount of time. Use `CPUS=` to
specify number of `make` jobs to run in parallel.

This will list toolchain options and supported architectures:

    $ make help_toolchain

Here are some examples:

    $ make crossgcc-i386 CPUS=$(nproc)       # build i386 toolchain
    $ make crossgcc-aarch64 CPUS=$(nproc)    # build Aarch64 toolchain
    $ make crossgcc-riscv CPUS=$(nproc)      # build RISC-V toolchain

Note that the i386 toolchain is currently used for all x86 platforms, including
x86_64.

Also note that you can possibly use your system toolchain, but the results are
not reproducible, and may have issues, so this is not recommended.  See step 5
to use your system toolchain.

### Step 4 - Build the payload - coreinfo
    $ make -C payloads/coreinfo olddefconfig
    $ make -C payloads/coreinfo

### Step 5 - Configure the build

##### Configure your mainboard
    $ make menuconfig
       select 'Mainboard' menu
       Beside 'Mainboard vendor' should be '(Emulation)'
       Beside 'Mainboard model' should be 'QEMU x86 i440fx/piix4'
       select < Exit >
These should be the default selections, so if anything else was set, run
`make distclean` to remove your old config file and start over.

##### Optionally use your system toolchain (Again, not recommended)
       select 'General Setup' menu
       select 'Allow building with any toolchain'
       select < Exit >

##### Select the payload
       select 'Payload' menu
       select 'Add a Payload'
       choose 'An Elf executable payload'
       select 'Payload path and filename'
       enter 'payloads/coreinfo/build/coreinfo.elf'
       select < Exit >
       select < Exit >
       select < Yes >

##### Check your configuration (optional step):

    $ make savedefconfig
    $ cat defconfig

There should only be two lines (or 3 if you're using the system toolchain):

    CONFIG_PAYLOAD_ELF=y
    CONFIG_PAYLOAD_FILE="payloads/coreinfo/build/coreinfo.elf"

### Step 6 - build coreboot
    $ make

At the end of the build, you should see:

    Build emulation/qemu-i440fx (QEMU x86 i440fx/piix4)

This means your build was successful. The output from the build is in the build
directory. build/coreboot.rom is the full rom file.

Test the image using QEMU
-------------------------

### Step 7 - Install QEMU
    $ sudo apt-get install -y qemu
    $ sudo pacman -S qemu
    $ sudo dnf install qemu

### Step 8 - Run QEMU
Start QEMU, and point it to the ROM you just built:

    $ qemu-system-x86_64 -bios build/coreboot.rom -serial stdio

You should see the serial output of coreboot in the original console window, and
a new window will appear running the coreinfo payload.

Summary
-------

### Step 1 summary - Install tools and libraries needed for coreboot
Depending on your distribution you have installed the minimum additional
software requirements to continue with downloading and building coreboot.
Not every distribution has the tools, that would be required,
installed by default. In the following we shortly introduce the purpose of the
installed packages:

* `build-essential` or `base-devel` are the basic tools for building software.
* `git` is needed to download coreboot from the coreboot git repository.
* `libncurses5-dev` or `ncurses` is needed to build the menu for 'make menuconfig'
* `m4, bison, curl, flex, zlib1g-dev, gcc, gnat` and `g++` or `clang`
are needed to build the coreboot toolchain. `gcc` and `gnat` have to be
of the same version.

If you started with a different distribution or package management system you
might need to install other packages. Most likely they are named slightly
different. If that is the case for you, we'd like to encourage you to contribute
to the project and submit a pull request with an update for this documentation
for your system.

### Step 2 summary - Download coreboot source tree
This will download a 'read-only' copy of the coreboot tree. This just means
that if you made changes to the coreboot tree, you couldn't immediately
contribute them back to the community. To pull a copy of coreboot that would
allow you to contribute back, you would first need to sign up for an account on
gerrit.

### Step 3 summary - Build the coreboot toolchain.
This builds one of the coreboot cross-compiler toolchains for X86 platforms.
Because of the variability of compilers and the other required tools between
the various operating systems that coreboot can be built on, coreboot supplies
and uses its own cross-compiler toolchain to build the binaries that end up as
part of the coreboot ROM. The toolchain provided by the operating system (the
'host toolchain') is used to build various tools that will run on the local
system during the build process.

### Step 4 summary - Build the payload
To actually do anything useful with coreboot, you need to build a payload to
include into the rom. The idea behind coreboot is that it does the minimum amount
possible before passing control of the machine to a payload. There are various
payloads such as grub or SeaBIOS that are typically used to boot the operating
system. Instead, we used coreinfo, a small demonstration payload that allows the
user to look at various things such as memory and the contents of the coreboot
file system (CBFS) - the pieces that make up the coreboot rom.

### Step 5 summary - Configure the build
This step configures coreboot's build options using the menuconfig interface to
Kconfig. Kconfig is the same configuration program used by the linux kernel. It
allows you to enable, disable, and change various values to control the coreboot
build process, including which mainboard(motherboard) to use, which toolchain to
use, and how the runtime debug console should be presented and saved.
Anytime you change mainboards in Kconfig, you should always run `make distclean`
before running `make menuconfig`. Due to the way that Kconfig works, values will
be kept from the previous mainboard if you skip the clean step. This leads to a
hybrid configuration which may or may not work as expected.

### Step 6 summary - Build coreboot
You may notice that a number of other pieces are downloaded at the beginning of
the build process. These are the git submodules used in various coreboot builds.
By default, the _blobs_ submodule is not downloaded. This git submodule may be
required for other builds for microcode or other binaries. To enable downloading
this submodule, select the option "Allow use of binary-only repository" in the
"General Setup" menu of Kconfig
This attempts to build the coreboot rom. The rom file itself ends up in the
build directory as 'coreboot.rom'. At the end of the build process, the build
displayed the contents of the rom file.

### Step 7 summary - Install QEMU
QEMU is a processor emulator which we can use to show the coreboot boot
process in a virtualised environment.

### Step 8 summary - Run QEMU
Here's the command line instruction broken down:
* `qemu-system-x86_64`
This starts the QEMU emulator with the i440FX host PCI bridge and PIIX3 PCI to
ISA bridge.
* `-bios build/coreboot.rom`
Use the coreboot rom image that we just built. If this flag is left out, the
standard SeaBIOS image that comes with QEMU is used.
* `-serial stdio`
Send the serial output to the console. This allows you to view the coreboot
boot log.
