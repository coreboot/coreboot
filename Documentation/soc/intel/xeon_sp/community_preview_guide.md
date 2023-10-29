Xeon Scalable processor coreboot Community Preview Guide
================================================

## Background

Sapphire Rapids coreboot was already merged into coreboot mainline.
Granite Rapids coreboot upstreaming is coming.

For Granite Rapids coreboot, we are going to perform a phased
upstreaming strategy according to the maturity of the code. A community
preview branch is opensource at:
https://review.coreboot.org/plugins/gitiles/intel-dev-pub/.

The community preview branch initially contains codes on legacy feature
enabling and matured patch set for platform support. More platform
support code will come with the platform development. The subsequent
upstreaming work will be based on this branch. It provides 2 board
targets,

1. Avenue City CRB (Granite Rapids-AP)
2. Beechnut City CRB (Granite Rapids-SP)

The above targets can pass build with Granite Rapids n-1 FSP headers,
which is a set of stub FSP headers used for compilation sanity check.

## Build steps

### Prepare workspace

```
# Create workspace

mkdir workspace && cd workspace

# Prepare coreboot codebase

git clone https://review.coreboot.org/intel-dev-pub ln -s intel-dev-pub/
coreboot

# Prepare stub binaries and update their path in
# configs/builder/config.intel.crb.avc.n-1

# Granite Rapids coreboot uses FSP 2.4, where FSP-I is newly introduced
# as an optional module to provide FSP based SMM capability. For FSP 2.4
# and FSP-I, please further refer to:
# https://cdrdv2-public.intel.com/736809/736809_FSP_EAS_v2.4_Errata_A.pdf

CONFIG_IFD_BIN_PATH=<path of intel flash descriptor blob>
CONFIG_CPU_UCODE_BINARIES=<path of ucode>
CONFIG_FSP_T_FILE=<path of FSP-T binary>
CONFIG_FSP_M_FILE=<path of FSP-M binary>
CONFIG_FSP_S_FILE=<path of FSP-S binary>
CONFIG_FSP_I_FILE=<path of FSP-I binary>
CONFIG_PAYLOAD_FILE=<path of payload binary>
```

### Download necessary submodules
```
git submodule update --init --checkout 3rdparty/vboot
```

### Or take below alternative if the network connection is not good
```
git clone https://github.com/coreboot/vboot.git 3rdparty/vboot/
```

### Build toolchain
```
make crossgcc-i386 CPUS=$(nproc)
```

### Build BIOS (Avenue City CRB)
```
make distclean

make defconfig KBUILD_DEFCONFIG=configs/builder/config.intel.crb.avc.n-1

make olddefconfig

make clean

make UPDATED_SUBMODULES=1 -j`nproc`
```

### Build BIOS (Beechnut City CRB)
```
make distclean

make defconfig KBUILD_DEFCONFIG=configs/builder/config.intel.crb.bnc.n-1

make olddefconfig

make clean

make UPDATED_SUBMODULES=1 -j`nproc`
```

### Obtain the preview patch list

```
# The tag ‘upstream’ points to the upstream commit that the preview
# branch is based on.

git format-patch upstream..HEAD
```

## About Granite Rapids n-1 FSP Headers

This is a set of stub FSP headers for Granite Rapids server, which will
be forward compatible with the formal Granite Rapids FSP headers which
will be opensource at a later stage. For the n-1 FSP headers, there are
no corresponding n-1 FSP binaries. To pass build, users need to use stub
binaries which could be generated in arbitrary ways.  Granite Rapids n-1
FSP headers are at: `src/vendorcode/intel/fsp/fsp2_0/graniterapids_n-1`.

For the formal Granite Rapids FSP headers and binaries, they will be
published at in
https://github.com/coreboot/coreboot/tree/main/src/vendorcode/intel/fsp/fsp2_0
(headers only) and https://github.com/intel/FSP (headers and binaries)
at a later stage.