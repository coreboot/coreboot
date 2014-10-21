#*****************************************************************************
#
# Copyright (c) 2012,        Advanced Micro Devices, Inc.
#               2013 - 2014, Sage Electronic Engineering, LLC
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Advanced Micro Devices, Inc. nor the names of
#       its contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

INCLUDES += -Isrc/southbridge/amd/agesa/hudson

romstage-y += smbus.c smbus_spd.c
ramstage-y += hudson.c
ramstage-y += usb.c
ramstage-y += lpc.c
ramstage-y += sm.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += hda.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sd.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
ramstage-y += reset.c
romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c
romstage-y += early_setup.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c smi_util.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c smi_util.c

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |PSPDIR ADDR|
# +-----------+
#
# EC ROM should be 64K aligned.

# HVB(Hardware Validated Boot) or Bypass
CONFIG_HVB=Bypass

HUDSON_FWM_POSITION=$(shell printf %u $(CONFIG_HUDSON_FWM_POSITION))

#assume the cbfs header is less than 128 bytes.
ROMSIG_SIZE=16
ifeq ($(CONFIG_HUDSON_XHCI_FWM), y)
HUDSON_XHCI_POSITION=$(call int-add,$(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) 128)
XHCI_FWM_SIZE=$(call file-size,$(CONFIG_HUDSON_XHCI_FWM_FILE))
else
HUDSON_XHCI_POSITION=0
XHCI_FWM_SIZE=0
endif

ifeq ($(CONFIG_HUDSON_GEC_FWM), y)
HUDSON_GEC_POSITION=$(call int-add,$(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) 128 \
	$(XHCI_FWM_SIZE) 128)
GEC_FWM_SIZE=$(call file-size,$(CONFIG_HUDSON_GEC_FWM_FILE))
else
HUDSON_GEC_POSITION=0
GEC_FWM_SIZE=0
endif

ifeq ($(CONFIG_HUDSON_IMC_FWM), y)
HUDSON_IMC_POSITION=$(call int-align,\
	$(call int-add,\
		$(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) 128 $(XHCI_FWM_SIZE)\
		128 $(GEC_FWM_SIZE) 128),\
	65536)
else
HUDSON_IMC_POSITION=0
endif

HUDSON_PSP_DIRECTORY_POSITION=0
ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
HUDSON_PSP_DIRECTORY_POSITION=$(call int-add,$(HUDSON_FWM_POSITION) 262144)
endif

$(obj)/coreboot_hudson_romsig.bin: $(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE)) \
			$(obj)/config.h
	echo "    Hudson FW  $@"
	for fwm in 1437226410 \
		$(HUDSON_IMC_POSITION) \
		$(HUDSON_GEC_POSITION) \
		$(HUDSON_XHCI_POSITION) \
		$(HUDSON_PSP_DIRECTORY_POSITION); do \
		echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done > $@

ifeq ($(CONFIG_HUDSON_FWM), y)
cbfs-files-y += hudson/fwm
hudson/fwm-file := $(obj)/coreboot_hudson_romsig.bin
hudson/fwm-position := $(HUDSON_FWM_POSITION)
hudson/fwm-type := raw
endif

ifeq ($(CONFIG_HUDSON_XHCI_FWM), y)
cbfs-files-y += hudson/xhci
hudson/xhci-file := $(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE))
hudson/xhci-position := $(HUDSON_XHCI_POSITION)
hudson/xhci-type := raw
hudson/xhci-required := Hudson XHCI firmware (available in coreboot/3rdparty if enabled)
endif

ifeq ($(CONFIG_HUDSON_IMC_FWM), y)
cbfs-files-y += hudson/imc
hudson/imc-file := $(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE))
hudson/imc-position := $(HUDSON_IMC_POSITION)
hudson/imc-type := raw
hudson/imc-required := Hudson IMC Firmware (available in coreboot/3rdparty if enabled)
endif

ifeq ($(CONFIG_HUDSON_GEC_FWM), y)
cbfs-files-y += hudson/gec
hudson/gec-file := $(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE))
hudson/gec-position := $(HUDSON_GEC_POSITION)
hudson/gec-type := raw
hudson/gec-required := Hudson Gigabit Ethernet Controller Firmware (Contact your AMD representative)
endif

ifdef CONFIG_HUDSON_AHCI_ROM
stripped_ahci_rom_id = $(call strip_quotes,$(CONFIG_AHCI_ROM_ID))
cbfs-files-y += pci$(stripped_ahci_rom_id).rom
pci$(stripped_ahci_rom_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_ROM_FILE))
pci$(stripped_ahci_rom_id).rom-type := optionrom
pci$(stripped_ahci_rom_id).rom-required := Hudson AHCI Option ROM (Contact your AMD representative)
endif

ifeq ($(CONFIG_HUDSON_PSP), y)

# 0
# catenate the pubkey and pspdir together to save some space.
AMDPUBKEY_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) 0x100)   #$(shell printf %u 0xFFb00100)
AMDPUBKEY_SIZE=$(call file-size,$(CONFIG_AMD_PUBKEY_FILE))

ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
FIRMWARE_LOCATE=$(dir $(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)))
FIRMWARE_TYPE=
endif
# 1
CONFIG_PSPBTLDR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspBootLoader$(FIRMWARE_TYPE).$(CONFIG_HVB).sbin
PSPBTLDR_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) 0x1000) #$(shell printf %u 0xFFb10000)
PSPBTLDR_SIZE=$(call file-size,$(CONFIG_PSPBTLDR_FILE))
cbfs-files-y += hudson/pspbtldr
hudson/pspbtldr-file := $(CONFIG_PSPBTLDR_FILE)
hudson/pspbtldr-position := $(PSPBTLDR_POS)
hudson/pspbtldr-type := raw

#8
CONFIG_SMUFWM_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware$(FIRMWARE_TYPE).sbin
SMUFWM_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) 0xb000) #$(shell printf %u 0xFFb20000)
SMUFWM_SIZE=$(call file-size,$(CONFIG_SMUFWM_FILE))
cbfs-files-y += hudson/smufwm
hudson/smufwm-file := $(CONFIG_SMUFWM_FILE)
hudson/smufwm-position := $(SMUFWM_POS)
hudson/smufwm-type := raw

#3
CONFIG_PSPRCVR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspRecovery$(FIRMWARE_TYPE).sbin
PSPRCVR_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) 0x30000) #$(shell printf %u 0xFFBb0000)
PSPRCVR_SIZE=$(call file-size,$(CONFIG_PSPRCVR_FILE))
cbfs-files-y += hudson/psprcvr
hudson/psprcvr-file := $(CONFIG_PSPRCVR_FILE)
hudson/psprcvr-position := $(PSPRCVR_POS)
hudson/psprcvr-type := raw

# 5
CONFIG_PUBSIGNEDKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/RtmPubSigned$(FIRMWARE_TYPE).key
PUBSIGNEDKEY_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) 0x400) #$(shell printf %u 0xFFb00400)
PUBSIGNEDKEY_SIZE=$(call file-size,$(CONFIG_PUBSIGNEDKEY_FILE))
cbfs-files-y += hudson/pubsignedkey
hudson/pubsignedkey-file := $(CONFIG_PUBSIGNEDKEY_FILE)
hudson/pubsignedkey-position := $(PUBSIGNEDKEY_POS)
hudson/pubsignedkey-type := raw

# 2
CONFIG_PSPSCUREOS_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureOs$(FIRMWARE_TYPE).sbin
PSPSECUREOS_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) 0x3A000) #$(shell printf %u 0xFFbc0000)
PSPSECUREOS_SIZE=$(call file-size,$(CONFIG_PSPSCUREOS_FILE))
cbfs-files-y += hudson/pspsecureos
hudson/pspsecureos-file := $(CONFIG_PSPSCUREOS_FILE)
hudson/pspsecureos-position := $(PSPSECUREOS_POS)
hudson/pspsecureos-type := raw

# 4
CONFIG_PSPNVRAM_FILE=$(top)/$(FIRMWARE_LOCATE)/PspNvram$(FIRMWARE_TYPE).bin
PSPNVRAM_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) 0x64000) #$(shell printf %u 0xFFbf0000)
PSPNVRAM_SIZE=$(call file-size,$(CONFIG_PSPNVRAM_FILE))
cbfs-files-y += hudson/pspnvram
hudson/pspnvram-file := $(CONFIG_PSPNVRAM_FILE)
hudson/pspnvram-position := $(PSPNVRAM_POS)
hudson/pspnvram-type := raw

ifeq ($(CONFIG_HVB), HVB)
# 6
RTM_FILE=$(objcbfs)/bootblock.bin     #The file size need to be 256 bytes aligned.
RTM_SIZE=$(call file-size,$(RTM_FILE))
RTM_POS=$(call int-add,4294967296 -$(RTM_SIZE))

# 7
RTMSIGN_FILE=$(obj)/bootblock_sig.bin
RTMSIGN_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) 0x800) #$(shell printf %u 0xFFb00800)
RTMSIGN_SIZE=256 #it should be hardcoded to 256, otherwise circular dependency comes up.$(call file-size,$(RTMSIGN_FILE))
cbfs-files-y += hudson/rtmsign
hudson/rtmsign-file := $(RTMSIGN_FILE)
hudson/rtmsign-position := $(RTMSIGN_POS)
hudson/rtmsign-type := raw
endif

CONFIG_SMUSCS_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuScs$(FIRMWARE_TYPE).bin
SMUSCS_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) 0x6d000) #$(shell printf %u 0xFFC00000)
SMUSCS_SIZE=$(call file-size,$(CONFIG_SMUSCS_FILE))
cbfs-files-y += hudson/smuscs
hudson/smuscs-file := $(CONFIG_SMUSCS_FILE)
hudson/smuscs-position := $(SMUSCS_POS)
hudson/smuscs-type := raw

define output_hex
echo $(1) | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'
endef

$(obj)/coreboot_psp_directory.bin: $(obj)/config.h $(FLETCHER) $(RTM_FILE)
	echo "    PSPDir     $@"
	for fwm in 0 $(AMDPUBKEY_SIZE) $(AMDPUBKEY_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done > $@_tail.tmp
	for fwm in 1 $(PSPBTLDR_SIZE) $(PSPBTLDR_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
	for fwm in 8 $(SMUFWM_SIZE) $(SMUFWM_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
	for fwm in 3 $(PSPRCVR_SIZE) $(PSPRCVR_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
	for fwm in 5 $(PUBSIGNEDKEY_SIZE) $(PUBSIGNEDKEY_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
ifeq ($(CONFIG_HVB), HVB)
	for fwm in 6 $(RTM_SIZE) $(RTM_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
	for fwm in 7 $(RTMSIGN_SIZE) $(RTMSIGN_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
endif
	for fwm in 2 $(PSPSECUREOS_SIZE) $(PSPSECUREOS_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
	for fwm in 4 $(PSPNVRAM_SIZE) $(PSPNVRAM_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
	for fwm in 95 $(SMUSCS_SIZE) $(SMUSCS_POS) 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
	for fwm in 11 4294967295 0 0; do \
	echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done >> $@_tail.tmp
	for fwm in  1347637284 0 `ls -l $@_tail.tmp | awk '{printf("%d", $$5/16);}'` 0; do \
		echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done > $@_head.tmp
	cat $@_head.tmp $@_tail.tmp > $@.tmp
	$(FLETCHER) < $@.tmp > $@
	rm $@_head.tmp $@_tail.tmp $@.tmp

$(obj)/coreboot_psp_directory_combine_pubkey.bin: $(obj)/coreboot_psp_directory.bin
	cat  $(obj)/coreboot_psp_directory.bin > $@
	ls -l $(obj)/coreboot_psp_directory.bin | LC_ALL=C awk '{for (i=0; i<256-$$5; i++) {printf "%c", 255}}' >> $@
	cat  $(top)/$(FIRMWARE_LOCATE)/AmdPubKey$(FIRMWARE_TYPE).bin >> $@

cbfs-files-y += hudson/pspdir
hudson/pspdir-file := $(obj)/coreboot_psp_directory_combine_pubkey.bin
hudson/pspdir-position := $(HUDSON_PSP_DIRECTORY_POSITION)
hudson/pspdir-type := raw

endif
