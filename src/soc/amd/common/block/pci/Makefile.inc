ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_PCI) += amd_pci_util.c

# FIXME: This gets added when CONFIG_SOC_AMD_COMMON is set, which is a bit unexpected.
bootblock-y += amd_pci_mmconf.c
verstage_x86-y += amd_pci_mmconf.c
romstage-y += amd_pci_mmconf.c
postcar-y += amd_pci_mmconf.c
ramstage-y += amd_pci_mmconf.c
