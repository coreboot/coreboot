# Apollolake
## SPI flash layout

![][apl_flash_layout]

With Apollolake Intel invented another flash layout for x86 firmware called IFWI (Intel FirmWare Image).

Usually on x86 platforms the bootblock is stored at the end of the bios region
and the Intel ME / TXE has its own IFD region. On Apollolake both have been
moved into the IFWI region, which is a subregion of "BIOS", since it allows to
store multiple firmware components.

The IFWI region can be manipulated by `ifwitool`.

[apl_flash_layout]: flash_layout.svg


