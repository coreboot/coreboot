/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only */

zstd_support=n
ifeq ($(CONFIG_COMPRESS_RAMSTAGE_ZSTD),y)
zstd_support=y
endif
ifeq ($(CONFIG_COMPRESSED_PAYLOAD_ZSTD),y)
zstd_support=y
endif

ifeq ($(zstd_support),y)
all-y += decompress/huf_decompress.c
all-y += decompress/zstd_ddict.c
all-y += decompress/zstd_decompress_block.c
all-y += decompress/zstd_decompress.c

all-y += common/entropy_common.c
all-y += common/fse_decompress.c

CPPFLAGS_common += -DZSTD_DISABLE_ASM=1
# Measured on Intel Sandy Bridge i5-2540M at 800Mhz:
# Setting                                      code size reduction    time loss
# -DHUF_FORCE_DECOMPRESS_X1=1                  6.4 KiB                6%
# -DHUF_FORCE_DECOMPRESS_X2=1                  5.4 KiB                7%
# -DZSTD_FORCE_DECOMPRESS_SEQUENCES_SHORT=1    8.1 KiB                19%
# -DZSTD_FORCE_DECOMPRESS_SEQUENCES_LONG=1     7.8 KiB                15%

CPPFLAGS_common += -DHUF_FORCE_DECOMPRESS_X1=1
CPPFLAGS_common += -DZSTD_FORCE_DECOMPRESS_SEQUENCES_SHORT=1
CPPFLAGS_common += -DZSTD_STRIP_ERROR_STRINGS=1

# Huffman fast decode needs 64bit and LE
CPPFLAGS_x86_32 += -DHUF_DISABLE_FAST_DECODE=1
# Measured on Intel Sandy Bridge i5-2540M at 800Mhz (x86_64 only):
# Setting                                      code size reduction    time loss
# -DHUF_DISABLE_FAST_DECODE=1                  2.3 KiB                5%

CPPFLAGS_common += -DDYNAMIC_BMI2=0 -DSTATIC_BMI2=0 -DZSTD_DECODER_INTERNAL_BUFFER=2048
endif

CPPFLAGS_common += -I$(src)/commonlib/bsd/zstd
CPPFLAGS_common += -I$(src)/commonlib/bsd/zstd/common
CPPFLAGS_common += -I$(src)/commonlib/bsd/zstd/decompress
