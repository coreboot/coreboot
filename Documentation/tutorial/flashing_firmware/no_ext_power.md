# Flashing firmware externally supplying no power

On some mainboards the flash IC's Vcc pin is connected to the internal
power-rail, powering the entire board if the flash IC is powered externally.
Likely it powers other chips which access the flash IC, preventing the external
programmer from reading/writing the chip. It also violates the components'
power sequence, bringing the ICs into an undefined state.

![][flash_ic_no_diode]

Please have a look at the mainboard specific documentation for details.

On those boards it's recommended to use a programmer without supplying power
externally.

The key to read and write the flash IC is to put the machine into *S3* sleep-
state or *S5* sleep-state *maybe* with Wake-On-LAN enabled.
Another option that sometimes works is to keep the device in reset. This method requires
knowledge of the board schematics and might require hardware modifications.
Use a multimeter to make sure the flash IC is powered in those sleep states.

[flash_ic_no_diode]: flash_ic_no_diode.svg
