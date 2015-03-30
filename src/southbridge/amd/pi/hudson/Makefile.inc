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

HUDSON_FWM_POSITION=$(shell printf %u $(CONFIG_HUDSON_FWM_POSITION))
CBFS_HEADER_SIZE=0x40

#assume the cbfs header is less than $(CBFS_HEADER_SIZE) bytes.
ROMSIG_SIZE=4096
ifeq ($(CONFIG_HUDSON_XHCI_FWM), y)
HUDSON_XHCI_POSITION=$(call int-add,$(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) $(CBFS_HEADER_SIZE))
XHCI_FWM_SIZE=$(call file-size,$(CONFIG_HUDSON_XHCI_FWM_FILE))
else
HUDSON_XHCI_POSITION=0
XHCI_FWM_SIZE=0
endif

ifeq ($(CONFIG_HUDSON_GEC_FWM), y)
HUDSON_GEC_POSITION=$(call int-add,$(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) $(CBFS_HEADER_SIZE) \
	$(XHCI_FWM_SIZE) $(CBFS_HEADER_SIZE))
GEC_FWM_SIZE=$(call file-size,$(CONFIG_HUDSON_GEC_FWM_FILE))
else
HUDSON_GEC_POSITION=0
GEC_FWM_SIZE=0
endif

ifeq ($(CONFIG_HUDSON_IMC_FWM), y)
HUDSON_IMC_POSITION=$(call int-align,\
	$(call int-add,\
		$(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) $(CBFS_HEADER_SIZE) $(XHCI_FWM_SIZE)\
		$(CBFS_HEADER_SIZE) $(GEC_FWM_SIZE) $(CBFS_HEADER_SIZE)),\
	65536)
IMC_FWM_SIZE=$(call file-size,$(CONFIG_HUDSON_IMC_FWM_FILE))
else
HUDSON_IMC_POSITION=0
IMC_FWM_SIZE=0
endif

ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
HUDSON_PSP_DIRECTORY_POSITION=$(call int-align,\
	$(call int-add,\
		$(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) $(CBFS_HEADER_SIZE) $(XHCI_FWM_SIZE)\
		$(CBFS_HEADER_SIZE) $(GEC_FWM_SIZE) $(CBFS_HEADER_SIZE) $(IMC_FWM_SIZE) $(CBFS_HEADER_SIZE)),\
	65536)
HUDSON_PSP_DIRECTORY_SIZE=256
else
HUDSON_PSP_DIRECTORY_POSITION=0
HUDSON_PSP_DIRECTORY_SIZE=0
endif

$(obj)/coreboot_hudson_romsig.bin: $(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE)) \
			$(obj)/config.h
	echo "    Hudson FW  $@"
	dd if=/dev/zero of=$@ count=$(ROMSIG_SIZE) ibs=1 status=noxfer 2>/dev/null; \
	for fwm in 1437226410 \
		$(HUDSON_IMC_POSITION) \
		$(HUDSON_GEC_POSITION) \
		$(HUDSON_XHCI_POSITION) \
		$(HUDSON_PSP_DIRECTORY_POSITION); do \
		echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done > $@.tmp; \
	dd if=$@.tmp of=$@ conv=notrunc status=noxfer 2>/dev/null; \
	rm $@.tmp

ifeq ($(CONFIG_HUDSON_FWM), y)
cbfs-files-y += fch/fwm
fch/fwm-file := $(obj)/coreboot_hudson_romsig.bin
fch/fwm-position := $(HUDSON_FWM_POSITION)
fch/fwm-size := $(ROMSIZE_SIG)
fch/fwm-type := raw
endif

ifeq ($(CONFIG_HUDSON_XHCI_FWM), y)
cbfs-files-y += fch/xhci
fch/xhci-file := $(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE))
fch/xhci-position := $(HUDSON_XHCI_POSITION)
fch/xhci-type := raw
fch/xhci-required := Hudson XHCI firmware (available in coreboot/3rdparty if enabled)
endif

ifeq ($(CONFIG_HUDSON_IMC_FWM), y)
cbfs-files-y += fch/imc
fch/imc-file := $(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE))
fch/imc-position := $(HUDSON_IMC_POSITION)
fch/imc-type := raw
fch/imc-required := Hudson IMC Firmware (available in coreboot/3rdparty if enabled)
endif

ifeq ($(CONFIG_HUDSON_GEC_FWM), y)
cbfs-files-y += fch/gec
fch/gec-file := $(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE))
fch/gec-position := $(HUDSON_GEC_POSITION)
fch/gec-type := raw
fch/gec-required := Hudson Gigabit Ethernet Controller Firmware (Contact your AMD representative)
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
AMDPUBKEY_POS=$(call int-add,$(HUDSON_PSP_DIRECTORY_POSITION) $(HUDSON_PSP_DIRECTORY_SIZE))
AMDPUBKEY_SIZE=$(call file-size,$(CONFIG_AMD_PUBKEY_FILE))

ifeq ($(CONFIG_CPU_AMD_PI_00730F01), y)
FIRMWARE_LOCATE=$(dir $(call strip_quotes, $(CONFIG_AMD_PUBKEY_FILE)))
FIRMWARE_TYPE=
endif

CONFIG_PUBSIGNEDKEY_FILE=$(top)/$(FIRMWARE_LOCATE)/RtmPubSigned$(FIRMWARE_TYPE).key
PUBSIGNEDKEY_POS=$(call int-align, \
	$(call int-add,$(AMDPUBKEY_POS) $(AMDPUBKEY_SIZE) $(CBFS_HEADER_SIZE)), \
	$(CBFS_HEADER_SIZE))
PUBSIGNEDKEY_SIZE=$(call file-size,$(CONFIG_PUBSIGNEDKEY_FILE))

CONFIG_PSPBTLDR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspBootLoader$(FIRMWARE_TYPE).Bypass.sbin
PSPBTLDR_POS=$(call int-align, \
	$(call int-add,$(PUBSIGNEDKEY_POS) $(PUBSIGNEDKEY_SIZE) $(CBFS_HEADER_SIZE)), \
	$(CBFS_HEADER_SIZE))
PSPBTLDR_SIZE=$(call file-size,$(CONFIG_PSPBTLDR_FILE))

CONFIG_PSPRCVR_FILE=$(top)/$(FIRMWARE_LOCATE)/PspRecovery$(FIRMWARE_TYPE).sbin
PSPRCVR_POS=$(call int-align, \
	$(call int-add,$(PSPBTLDR_POS) $(PSPBTLDR_SIZE) $(CBFS_HEADER_SIZE)), \
	$(CBFS_HEADER_SIZE))
PSPRCVR_SIZE=$(call file-size,$(CONFIG_PSPRCVR_FILE))

CONFIG_PSPSCUREOS_FILE=$(top)/$(FIRMWARE_LOCATE)/PspSecureOs$(FIRMWARE_TYPE).sbin
PSPSECUREOS_POS=$(call int-align, \
	$(call int-add,$(PSPRCVR_POS) $(PSPRCVR_SIZE) $(CBFS_HEADER_SIZE)), \
	$(CBFS_HEADER_SIZE))
PSPSECUREOS_SIZE=$(call file-size,$(CONFIG_PSPSCUREOS_FILE))

CONFIG_PSPNVRAM_FILE=$(top)/$(FIRMWARE_LOCATE)/PspNvram$(FIRMWARE_TYPE).bin
PSPNVRAM_POS=$(call int-align, \
	$(call int-add,$(PSPSECUREOS_POS) $(PSPSECUREOS_SIZE) $(CBFS_HEADER_SIZE)), \
	$(CBFS_HEADER_SIZE))
PSPNVRAM_SIZE=$(call file-size,$(CONFIG_PSPNVRAM_FILE))

CONFIG_SMUFWM_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuFirmware$(FIRMWARE_TYPE).sbin
SMUFWM_POS=$(call int-align, \
	$(call int-add,$(PSPNVRAM_POS) $(PSPNVRAM_SIZE) $(CBFS_HEADER_SIZE)), \
	$(CBFS_HEADER_SIZE))
SMUFWM_SIZE=$(call file-size,$(CONFIG_SMUFWM_FILE))

CONFIG_SMUSCS_FILE=$(top)/$(FIRMWARE_LOCATE)/SmuScs$(FIRMWARE_TYPE).bin
SMUSCS_POS=$(call int-align, \
	$(call int-add,$(SMUFWM_POS) $(SMUFWM_SIZE) $(CBFS_HEADER_SIZE)), \
	$(CBFS_HEADER_SIZE))
SMUSCS_SIZE=$(call file-size,$(CONFIG_SMUSCS_FILE))

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

cbfs-files-y += apu/pspdir
apu/pspdir-file := $(obj)/coreboot_psp_directory_combine_pubkey.bin
apu/pspdir-position := $(HUDSON_PSP_DIRECTORY_POSITION)
apu/pspdir-type := raw

# 5
cbfs-files-y += apu/pubsignedkey
apu/pubsignedkey-file := $(CONFIG_PUBSIGNEDKEY_FILE)
apu/pubsignedkey-position := $(PUBSIGNEDKEY_POS)
apu/pubsignedkey-type := raw

# 1
cbfs-files-y += apu/pspbtldr
apu/pspbtldr-file := $(CONFIG_PSPBTLDR_FILE)
apu/pspbtldr-position := $(PSPBTLDR_POS)
apu/pspbtldr-type := raw

#3
cbfs-files-y += apu/psprcvr
apu/psprcvr-file := $(CONFIG_PSPRCVR_FILE)
apu/psprcvr-position := $(PSPRCVR_POS)
apu/psprcvr-type := raw

# 2
cbfs-files-y += apu/pspsecureos
apu/pspsecureos-file := $(CONFIG_PSPSCUREOS_FILE)
apu/pspsecureos-position := $(PSPSECUREOS_POS)
apu/pspsecureos-type := raw

# 4
cbfs-files-y += apu/pspnvram
apu/pspnvram-file := $(CONFIG_PSPNVRAM_FILE)
apu/pspnvram-position := $(PSPNVRAM_POS)
apu/pspnvram-type := raw

#8
cbfs-files-y += apu/smufwm
apu/smufwm-file := $(CONFIG_SMUFWM_FILE)
apu/smufwm-position := $(SMUFWM_POS)
apu/smufwm-type := raw

cbfs-files-y += apu/smuscs
apu/smuscs-file := $(CONFIG_SMUSCS_FILE)
apu/smuscs-position := $(SMUSCS_POS)
apu/smuscs-type := raw

endif
