Offline AMD Board specific USB Type-C PD FW modification tool `C`

Generates pointers at flash offset 16 for the EC firmware to consume
by reading the FMAP and the provided FMAP region name. The EC will
load the USB PD firmware into the corresponding chip where necessary.

This will overwrite parts of the flash without warning. The user must
be aware of potential conflicts such as with the Intel IFD.
