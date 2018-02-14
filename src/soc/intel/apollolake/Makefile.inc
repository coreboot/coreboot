ifeq ($(CONFIG_SOC_INTEL_APOLLOLAKE),y)

subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/x86/cache

bootblock-y += bootblock/bootblock.c
bootblock-y += car.c
bootblock-y += heci.c
bootblock-y += gspi.c
bootblock-y += i2c.c
bootblock-y += lpc.c
bootblock-y += mmap_boot.c
bootblock-y += pmutil.c
bootblock-y += spi.c
bootblock-$(CONFIG_SOC_UART_DEBUG) += uart.c
bootblock-$(CONFIG_FSP_CAR) += bootblock/cache_as_ram_fsp.S

romstage-y += car.c
romstage-$(CONFIG_PLATFORM_USES_FSP2_0) += romstage.c
romstage-y += gspi.c
romstage-y += heci.c
romstage-y += i2c.c
romstage-$(CONFIG_SOC_UART_DEBUG) += uart.c
romstage-y += memmap.c
romstage-y += meminit.c
ifeq ($(CONFIG_SOC_INTEL_GLK),y)
romstage-y += meminit_util_glk.c
else
romstage-y += meminit_util_apl.c
endif
romstage-y += mmap_boot.c
romstage-y += pmutil.c
romstage-y += reset.c
romstage-y += spi.c

smm-y += mmap_boot.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += spi.c
smm-$(CONFIG_SOC_UART_DEBUG) += uart.c
smm-y += elog.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += cpu.c
ramstage-y += chip.c
ramstage-y += cse.c
ramstage-y += elog.c
ramstage-y += graphics.c
ramstage-y += gspi.c
ramstage-y += heci.c
ramstage-y += i2c.c
ramstage-y += lpc.c
ramstage-y += memmap.c
ramstage-y += mmap_boot.c
ramstage-$(CONFIG_SOC_UART_DEBUG) += uart.c
ramstage-y += nhlt.c
ramstage-y += spi.c
ramstage-y += systemagent.c
ramstage-y += pmutil.c
ramstage-y += pnpconfig.c
ramstage-y += pmc.c
ramstage-y += reset.c
ramstage-y += xdci.c
ramstage-y += sd.c

postcar-y += memmap.c
postcar-y += mmap_boot.c
postcar-y += spi.c
postcar-y += i2c.c
postcar-$(CONFIG_SOC_UART_DEBUG) += uart.c

postcar-$(CONFIG_FSP_CAR) += exit_car_fsp.S

verstage-y += car.c
verstage-y += i2c.c
verstage-y += gspi.c
verstage-y += heci.c
verstage-y += memmap.c
verstage-y += mmap_boot.c
verstage-$(CONFIG_SOC_UART_DEBUG) += uart.c
verstage-y += pmutil.c
verstage-y += reset.c
verstage-y += spi.c

ifeq ($(CONFIG_SOC_INTEL_GLK),y)
bootblock-y += gpio_glk.c
romstage-y += gpio_glk.c
smm-y += gpio_glk.c
ramstage-y += gpio_glk.c
else
bootblock-y += gpio_apl.c
romstage-y += gpio_apl.c
smm-y += gpio_apl.c
ramstage-y += gpio_apl.c
endif

CPPFLAGS_common += -I$(src)/soc/intel/apollolake/include
ifeq ($(CONFIG_SOC_INTEL_GLK),y)
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp2_0/glk
else
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp2_0/apollolake
endif

# Since FSP-M runs in CAR we need to relocate it to a specific address
$(CONFIG_FSP_M_CBFS)-options := -b $(CONFIG_FSP_M_ADDR)

# Handle GLK paging requirements
ifeq ($(CONFIG_PAGING_IN_CACHE_AS_RAM),y)
cbfs-files-y += pt
pt-file := pt.c:struct
pt-type := raw
cbfs-files-y += pdpt
pdpt-file := pdpt.c:struct
pdpt-type := raw
endif

ifeq ($(CONFIG_NEED_LBP2),y)
files_added::
	$(CBFSTOOL) $(obj)/coreboot.rom write -r $(CONFIG_LBP2_FMAP_NAME) -f $(CONFIG_LBP2_FILE_NAME) --fill-upward
endif

# Bootblock on Apollolake platform lies in the IFWI region. In order to place
# the bootblock at the right location in IFWI image -
# a. Using ifwitool:
#    1. Create IFWI image (ifwi.bin.tmp) from input image
#       (CONFIG_IFWI_FILE_NAME).
#    2. Delete OBBP sub-partition, if present.
#    3. Replace IBBL directory entry in IBBP sub-partition with currently
#       generated bootblock.bin.
# b. Using cbfstool:
#    1. Write ifwi.bin.tmp to coreboot.rom using CONFIG_IFWI_FMAP_NAME.
ifeq ($(CONFIG_NEED_IFWI),y)
files_added:: $(IFWITOOL)
	$(IFWITOOL) $(CONFIG_IFWI_FILE_NAME) create -f $(objcbfs)/ifwi.bin.tmp
	$(IFWITOOL) $(objcbfs)/ifwi.bin.tmp delete -n OBBP
	$(IFWITOOL) $(objcbfs)/ifwi.bin.tmp replace -n IBBP -f $(objcbfs)/bootblock.bin -d -e IBBL
	$(CBFSTOOL) $(obj)/coreboot.rom write -r $(CONFIG_IFWI_FMAP_NAME) -f $(objcbfs)/ifwi.bin.tmp --fill-upward
endif

# DSP firmware settings files.
ifeq ($(CONFIG_SOC_INTEL_GLK),y)
NHLT_BLOB_PATH = 3rdparty/blobs/soc/intel/glk/nhlt-blobs
else
NHLT_BLOB_PATH = 3rdparty/blobs/soc/intel/apollolake/nhlt-blobs
endif
DMIC_1CH_48KHZ_16B = dmic-1ch-48khz-16b.bin
DMIC_2CH_48KHZ_16B = dmic-2ch-48khz-16b.bin
DMIC_4CH_48KHZ_16B = dmic-4ch-48khz-16b.bin
MAX98357_RENDER = max98357-render-2ch-48khz-24b.bin
DA7219_RENDER_CAPTURE = dialog-2ch-48khz-24b.bin
RT5682_RENDER_CAPTURE = rt5682-2ch-48khz-24b.bin

cbfs-files-$(CONFIG_NHLT_DMIC_1CH_16B) += $(DMIC_1CH_48KHZ_16B)
$(DMIC_1CH_48KHZ_16B)-file := $(NHLT_BLOB_PATH)/$(DMIC_1CH_48KHZ_16B)
$(DMIC_1CH_48KHZ_16B)-type := raw

cbfs-files-$(CONFIG_NHLT_DMIC_2CH_16B) += $(DMIC_2CH_48KHZ_16B)
$(DMIC_2CH_48KHZ_16B)-file := $(NHLT_BLOB_PATH)/$(DMIC_2CH_48KHZ_16B)
$(DMIC_2CH_48KHZ_16B)-type := raw

cbfs-files-$(CONFIG_NHLT_DMIC_4CH_16B) += $(DMIC_4CH_48KHZ_16B)
$(DMIC_4CH_48KHZ_16B)-file := $(NHLT_BLOB_PATH)/$(DMIC_4CH_48KHZ_16B)
$(DMIC_4CH_48KHZ_16B)-type := raw

cbfs-files-$(CONFIG_NHLT_MAX98357) += $(MAX98357_RENDER)
$(MAX98357_RENDER)-file := $(NHLT_BLOB_PATH)/$(MAX98357_RENDER)
$(MAX98357_RENDER)-type := raw

cbfs-files-$(CONFIG_NHLT_DA7219) += $(DA7219_RENDER_CAPTURE)
$(DA7219_RENDER_CAPTURE)-file := $(NHLT_BLOB_PATH)/$(DA7219_RENDER_CAPTURE)
$(DA7219_RENDER_CAPTURE)-type := raw

cbfs-files-$(CONFIG_NHLT_RT5682) += $(RT5682_RENDER_CAPTURE)
$(RT5682_RENDER_CAPTURE)-file := $(NHLT_BLOB_PATH)/$(RT5682_RENDER_CAPTURE)
$(RT5682_RENDER_CAPTURE)-type := raw

endif
