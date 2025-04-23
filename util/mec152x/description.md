Offline Microchip EC FW modification tool `C`

Generates the EC firmware pointer at flash offset 0 based on
the FMAP and the specified FMAP region name. The EC will read
this pointer to find the EC firmware in the host SPI flash.

This will overwrite the first 4 bytes of flash. The user must
be aware of potential conflicts such as with the Intel IFD.
