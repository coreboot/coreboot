void
final_superio_fixup()
{
        unsigned int devfn = SUPERIO_DEVFN;
        unsigned char enables;

	/* enable com ports, since we're using this built-in superio */
        // enable com1 and com2.
        enables = pcibios_read_config_byte(0, devfn, 0x6e, &enables);
        // 0x80 is enable com port b, 0x1 is to make it com2, 0x8 is enable com port a as com1
        enables = 0x80 | 0x1 | 0x8 ;
        pcibios_write_config_byte(0, devfn, 0x6e, enables);
        // note: this is also a redo of some port of assembly, but we want everything up.
        // set com1 to 115 kbaud
        // not clear how to do this yet.
        // forget it; done in assembly.

}

