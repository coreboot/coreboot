subdirs-y += dramc

CPPFLAGS_common += -Isrc/soc/mediatek/mt8195/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include
CPPFLAGS_common += -Isrc/vendorcode/mediatek/mt8195/include
CPPFLAGS_common += -DFOR_COREBOOT
