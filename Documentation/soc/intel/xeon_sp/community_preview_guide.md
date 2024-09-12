Xeon Scalable Processor coreboot Community Preview Guide
================================================

## Background

Xeon 6 basic boot supports are initially upstreamed at
https://review.coreboot.org/q/topic:%22Xeon6-Basic-Boot%22.

Full feature supports are previewed at
https://review.coreboot.org/admin/repos/intel-dev-pub,general

The supported platform status are as below,

1. Beechnut City CRB (Sierra Forest-SP)

- Buildable with n-1 FSP headers

- Bootable with real FSP headers/binaries

2. Avenue City CRB (Granite Rapids-AP)

- Buildable with n-1 FSP headers

## Build steps

### Prepare workspace

```
# Create workspace

mkdir workspace && cd workspace

# Prepare coreboot codebase

git clone https://review.coreboot.org/intel-dev-pub
ln -s intel-dev-pub/ coreboot

# Switch to the branches you would like to use

https://review.coreboot.org/admin/repos/intel-dev-pub,branches

3.0_branch                    - Support of real platform boot
3.0_feature_cxl               - Support of CXL Type-3 memory expander
3.0_feature_fsp_smm_ras       - Support of RAS by FSP-SMM
3.0_feature_gpio_vgpio        - Support of GPIO and virtual GPIO
3.0_feature_iio_res_rebalance - Support of customized IIO resource (bus/MMIO) window for smart NICs
3.0_feature_large_cbfs        - Support of 32MB/48MB CBFS for large cloud payloads
3.0_feature_smbios            - Support of SMBIOS
3.0_feature_snc               - Support of sub-NUMA clustering
3.0_feature_var_car_code_sz   - Support of user defined cache-as-RAM code size

# Prepare stub binaries and update their path in
# configs/builder/config.intel.crb.avc

# Granite Rapids coreboot uses FSP 2.4, where FSP-I is newly introduced
# as an optional module to provide FSP based SMM capability. For FSP 2.4
# and FSP-I, please further refer to:
# https://cdrdv2-public.intel.com/736809/736809_FSP_EAS_v2.4_Errata_A.pdf

CONFIG_IFD_BIN_PATH=<path of intel flash descriptor blob>
CONFIG_CPU_UCODE_BINARIES=<path of ucode>
CONFIG_FSP_T_FILE=<path of FSP-T binary>
CONFIG_FSP_M_FILE=<path of FSP-M binary>
CONFIG_FSP_S_FILE=<path of FSP-S binary>
CONFIG_FSP_I_FILE=<path of FSP-I binary, optional, to comment out if not using>
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

make defconfig KBUILD_DEFCONFIG=configs/builder/config.intel.crb.avc

make UPDATED_SUBMODULES=1 -j`nproc`
```

### Build BIOS (Beechnut City CRB)
```
make distclean

make defconfig KBUILD_DEFCONFIG=configs/builder/config.intel.crb.bnc

make UPDATED_SUBMODULES=1 -j`nproc`
```

### Obtain the preview patch list

```
# The tag ‘upstream’ points to the upstream commit that the preview
# branch is based on.

git format-patch upstream..HEAD
```

## About Granite Rapids n-1 FSP headers

This is a set of stub FSP headers for Granite Rapids server, which will
be forward compatible with the formal Granite Rapids FSP headers which
will be opensource at a later stage. For the n-1 FSP headers, there are
no corresponding n-1 FSP binaries. To pass build, users need to use stub
binaries which could be generated in arbitrary ways.  Granite Rapids n-1
FSP headers are at: `src/vendorcode/intel/fsp/fsp2_0/graniterapids`.

## About Granite Rapids real FSP headers

For the real Granite Rapids FSP headers and binaries, please contact
intel business interface to obtain. Then please update
configs/builder/config.intel.crb.avc and configs/builder/config.intel.crb.bnc
to apply.
