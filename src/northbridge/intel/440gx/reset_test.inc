       /* If I have already booted once skip a bunch of initialization */

#if 0
       /* To see if I have already booted I check to see if memory has
        * been enabled.  In this case by reading the memory size register.
        * This gets set part way through the memory initialization but
        * it should be fine for this purpose.
        */
       movl    $0x67, %eax
       PCI_READ_CONFIG_BYTE
       testb   %al, %al
       jnz     __cpu_reset
#endif
#if 1
       /* To see if I have already booted I check to see if memory has
        * been enabled.  In this case by seeing if memory refresh
        * hass ben enabled.
        */
       movl    $0x57, %eax
       PCI_READ_CONFIG_BYTE
       testb   $0x7, %al
       jnz     __cpu_reset
#endif
