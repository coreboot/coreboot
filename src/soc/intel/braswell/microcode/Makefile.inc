# Add CPU uCode source to list of files to build.
cpu_microcode-y += microcode_blob.c

# This section overrides the default build process for the microcode to place
# it at a known location in the CBFS.  This only needs to be enabled if FSP is
# being used.
# Define the correct offset for the file in CBFS
fsp_ucode_cbfs_base = $(CONFIG_CPU_MICROCODE_CBFS_LOC)

# Override the location that was supplied by the core code.
add-cpu-microcode-to-cbfs = \
	$(CBFSTOOL) $(1) add -n $(cpu_ucode_cbfs_name) -f $(cpu_ucode_cbfs_file) -t 0x53 -b $(fsp_ucode_cbfs_base)

