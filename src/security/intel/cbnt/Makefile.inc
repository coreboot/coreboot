ifeq ($(CONFIG_INTEL_CBNT_SUPPORT),y)

ramstage-y += cmos.c

# The private key also contains the public key, so use that if a private key is provided.
ifeq ($(CONFIG_INTEL_CBNT_NEED_KM_PRIV_KEY),y)
$(obj)/km_pub.pem: $(call strip_quotes, $(CONFIG_INTEL_CBNT_KM_PRIV_KEY_FILE))
	openssl pkey -in $< -pubout > $@
else ifeq ($(CONFIG_INTEL_CBNT_NEED_KM_PUB_KEY),y)
$(obj)/km_pub.pem: $(call strip_quotes, $(CONFIG_INTEL_CBNT_KM_PUB_KEY_FILE))
	cp $< $@
endif

# The private key also contains the public key, so use that if a private key is provided.
ifeq ($(CONFIG_INTEL_CBNT_NEED_BPM_PRIV_KEY),y)
$(obj)/bpm_pub.pem: $(call strip_quotes, $(CONFIG_INTEL_CBNT_BPM_PRIV_KEY_FILE))
	openssl pkey -in $< -pubout > $@
else ifeq ($(CONFIG_INTEL_CBNT_NEED_BPM_PUB_KEY),y)
$(obj)/bpm_pub.pem: $(call strip_quotes, $(CONFIG_INTEL_CBNT_BPM_PUB_KEY_FILE))
	cp $< $@
endif

BG_PROV:=$(obj)/bg-prov
CBNT_CFG:=$(obj)/cbnt.json

$(BG_PROV):
	printf "    BG_PROV    building tool\n"
	cd 3rdparty/intel-sec-tools; \
	GO111MODULE=on go mod download; \
	GO111MODULE=on go mod verify; \
	GO111MODULE=on go build -o $(top)/$@ cmd/bg-prov/*.go

$(CBNT_CFG): $(call strip_quotes, $(CONFIG_INTEL_CBNT_BG_PROV_CFG_FILE))
	cp $(CONFIG_INTEL_CBNT_BG_PROV_CFG_FILE) $@

ifeq ($(CONFIG_INTEL_CBNT_GENERATE_BPM),y)
ifeq ($(CONFIG_INTEL_CBNT_BG_PROV_BPM_USE_CFG_FILE),y)
$(obj)/bpm_unsigned.bin: $(obj)/coreboot.rom $(BG_PROV) $(CBNT_CFG)
	printf "    BG_PROV    creating unsigned BPM using config file\n"
	$(BG_PROV) bpm-gen $@ $< --config=$(CBNT_CFG) --cut
else
$(obj)/bpm_unsigned.bin: $(obj)/coreboot.rom $(BG_PROV)
	printf "    BG_PROV    creating unsigned BPM\n"
	# SHA256, SHA1, SHA384 for digest
	$(BG_PROV) bpm-gen $@ $< --revision=$(CONFIG_INTEL_CBNT_BPM_REVISION) \
				 --svn=$(CONFIG_INTEL_CBNT_BPM_SVN) \
				 --acmsvn=$(CONFIG_INTEL_CBNT_ACM_SVN) \
				 --nems=$(CONFIG_INTEL_CBNT_NUM_NEM_PAGES) \
				 --pbet=$(CONFIG_INTEL_CBNT_PBET) \
				 --ibbflags=$(CONFIG_INTEL_CBNT_IBB_FLAGS) \
				 --entrypoint=$(shell printf "%d" 0xfffffff0) \
				 --ibbhash={11,4,12} \
				 --ibbsegbase=$(call int-add, $(call int-subtract, 0xffffffff $(CONFIG_C_ENV_BOOTBLOCK_SIZE)) 1) \
				 --ibbsegsize=$(shell printf "%d" $(CONFIG_C_ENV_BOOTBLOCK_SIZE)) \
				 --ibbsegflag=0 \
				 --sintmin=$(CONFIG_INTEL_CBNT_SINIT_SVN) \
				 --txtflags=0 \
				 --powerdowninterval=$(CONFIG_INTEL_CBNT_PD_INTERVAL) \
				 --acpibaseoffset=$(shell printf "%d" $(CONFIG_INTEL_ACPI_BASE_ADDRESS)) \
				 --powermbaseoffset=$(shell printf "%d" $(CONFIG_INTEL_PCH_PWRM_BASE_ADDRESS)) \
				 --cmosoff0=$(shell printf "%d" $(CONFIG_INTEL_CBNT_CMOS_OFFSET)) \
				 --cmosoff1=$(call int-add, $(CONFIG_INTEL_CBNT_CMOS_OFFSET) 1) \
				 --cut \
				 --out=$(obj)/bpm_cfg.json
endif

ifeq ($(CONFIG_INTEL_CBNT_BPM_ONLY_UNSIGNED),y)
build_complete:: $(obj)/bpm_unsigned.bin
	@printf "\n** WARNING **\n"
	@printf "Build generated an unsigned BPM image: build/bpm_unsigned.bin.\n"
	@printf "The resulting image will not work with CBnT.\n"
	@printf "After you have externally signed the image you can add it to the coreboot image:\n"
	@printf "$$ cbfstool build/coreboot.rom add -f bpm.bin -n boot_policy_manifest.bin -t raw -a 16\n"
	@printf "$$ ifittool -r COREBOOT -a -n boot_policy_manifest.bin -t 12 -s 12 -f build/coreboot.rom\n"
else
$(obj)/bpm.bin: $(obj)/bpm_unsigned.bin $(BG_PROV) $(call strip_quotes, $(CONFIG_INTEL_CBNT_BPM_PRIV_KEY_FILE))
	printf "    BG_PROV    signing real BPM\n"
	$(BG_PROV) bpm-sign $< $@ $(CONFIG_INTEL_CBNT_BPM_PRIV_KEY_FILE) ""

# Add BPM at the end of the build when all files have been added
files_added:: $(obj)/bpm.bin
	printf "    CBNT       Adding BPM\n"
	$(CBFSTOOL) $(obj)/coreboot.rom add -f $< -n boot_policy_manifest.bin -a 0x10 -t raw
	printf "    IFITTOOL   Adding BPM\n"
	$(IFITTOOL) -r COREBOOT -a -n boot_policy_manifest.bin -t 12 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $(obj)/coreboot.rom

endif # CONFIG_INTEL_CBNT_BPM_ONLY_UNSIGNED

else # CONFIG_INTEL_CBNT_GENERATE_BPM

ifneq ($(CONFIG_INTEL_CBNT_BOOT_POLICY_MANIFEST_BINARY),"")
cbfs-files-y += boot_policy_manifest.bin
boot_policy_manifest.bin-file := $(CONFIG_INTEL_CBNT_BOOT_POLICY_MANIFEST_BINARY)
boot_policy_manifest.bin-type := raw
boot_policy_manifest.bin-align := 0x10

$(call add_intermediate, add_bpm_fit, $(IFITTOOL) set_fit_ptr)
	$(IFITTOOL) -r COREBOOT -a -n boot_policy_manifest.bin -t 12 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<
endif
endif # CONFIG_INTEL_CBNT_GENERATE_BPM

ifeq ($(CONFIG_INTEL_CBNT_GENERATE_KM),y)
ifeq ($(CONFIG_INTEL_CBNT_BG_PROV_KM_USE_CFG_FILE),y)
$(obj)/km_unsigned.bin: $(obj)/km_pub.pem $(BG_PROV) $(CBNT_CFG)
	printf "    BG_PROV    creating unsigned KM using config file\n"
	$(BG_PROV) km-gen $@ $< --config=$(CBNT_CFG)
else
PK_HASH_ALG_SHA256:=11 # Hardcode as no other options are available for CBnT
$(obj)/km_unsigned.bin: $(obj)/km_pub.pem $(obj)/bpm_pub.pem $(BG_PROV)
	printf "    BG_PROV    creating unsigned KM\n"
	$(BG_PROV) km-gen $@ $< --revision=$(CONFIG_INTEL_CBNT_KM_REVISION) \
				--svn=$(CONFIG_INTEL_CBNT_KM_SVN) \
				--id=$(CONFIG_INTEL_CBNT_KM_ID) \
				--pkhashalg=$(PK_HASH_ALG_SHA256) \
				--bpmpubkey=$(obj)/bpm_pub.pem \
				--bpmhashalgo=$(PK_HASH_ALG_SHA256) \
				--out=$(obj)/km_cfg.json
endif

$(obj)/km.bin: $(obj)/km_unsigned.bin $(BG_PROV) $(call strip_quotes, $(CONFIG_INTEL_CBNT_KM_PRIV_KEY_FILE))
	printf "    BG_PROV    signing KM\n"
	$(BG_PROV) km-sign $< $@ $(CONFIG_INTEL_CBNT_KM_PRIV_KEY_FILE) ""

KM_FILE=$(obj)/km.bin
else
KM_FILE=$(CONFIG_INTEL_CBNT_KEY_MANIFEST_BINARY)
endif

ifneq ($(KM_FILE),"")
ifeq ($(CONFIG_INTEL_CBNT_KM_ONLY_UNSIGNED),y)
$(call add_intermediate, gen_unsigned_km, $(obj)/km_unsigned.bin)
	@printf "Generating unsgined KM\n"

build_complete::
	@printf "\n** WARNING **\n"
	@printf "Build generated an unsigned KM image: build/km_unsiged.bin.\n"
	@printf "The resulting image will not work with CBnT.\n"
	@printf "After you have externally signed the image you can add it to the coreboot image:\n"
	@printf "$$ cbfstool build/coreboot.rom add -f km.bin -n key_manifest.bin -t raw -a 16\n"
	@printf "$$ ifittool -r COREBOOT -a -n key_manifest.bin -t 11 -s 12 -f build/coreboot.rom\n"

else
cbfs-files-y += key_manifest.bin
key_manifest.bin-file := $(KM_FILE)
key_manifest.bin-type := raw
key_manifest.bin-align := 0x10

$(call add_intermediate, add_km_fit, $(IFITTOOL) set_fit_ptr)
	$(IFITTOOL) -r COREBOOT -a -n key_manifest.bin -t 11 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<
endif

endif # CONFIG_INTEL_CBNT_KM_ONLY_UNSIGNED
endif # CONFIG_INTEL_CBNT_SUPPORT
