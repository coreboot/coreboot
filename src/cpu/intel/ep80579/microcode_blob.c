/*
 * We support updating mirocode from CBFS, but do not have any microcode
 * updates for this CPU. This will generate a useles cpu_microcode_blob.bin in
 * CBFS, but this file can be later replaced without needing to recompile the
 * coreboot.rom image.
 */
unsigned microcode_updates_ep80579[] = {
	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};
