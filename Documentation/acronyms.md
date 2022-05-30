# Firmware and Computer Acronyms, Initialisms and Definitions

** Note that this document even more of a work in progress than most **
** of the coreboot documentation **

## _0-9

* _XXX - An underscore followed by 3 uppercase letters will typically be
an ACPI specified method. Look in the [ACPI
Spec](https://uefi.org/specifications) for details, or run the tool
`acpihelp _XXX`
* 2FA - [**Two-factor Authentication**](https://en.wikipedia.org/wiki/Multi-factor_authentication)
* 4G - In coreboot, this typically refers to the 4 gibibyte boundary of 32-bit addressable memory space.
* 5G - Telecommunication: [**Fifth-Generation Cellular Network**](https://en.wikipedia.org/wiki/5G)

## A
* ABI - [**Application Binary Interface**](https://en.wikipedia.org/wiki/Application_binary_interface)
* ABL - AMD: AGESA BootLoader (or AMD BootLoader) - The portion of the AMD processor
  initialization that happens from the PSP.  Significantly, Memory
  Initialization.
* AC - Electricity: [**Alternating Current**](https://en.wikipedia.org/wiki/Alternating_current)
* Ack - Acknowledgment
* ACM – [**Authenticated Code Module**](https://doc.coreboot.org/security/intel/acm.html)
* ACP - [**Average CPU power**](https://en.wikipedia.org/wiki/Thermal_design_power)
* ACPI - The [**Advanced Configuration and Power
  Interface**](http://en.wikipedia.org/wiki/Advanced_Configuration_and_Power_Interface)
  is an industry standard for letting the OS control power management.
  * [http://www.acpi.info/](http://www.acpi.info/)
  * [http://kernelslacker.livejournal.com/88243.html](http://kernelslacker.livejournal.com/88243.html)
* ADC - [**Analog-to-Digital Converter**](https://en.wikipedia.org/wiki/Analog-to-digital_converter)
* ADL - Intel: [**Alder Lake**](https://en.wikichip.org/wiki/intel/microarchitectures/alder_lake)
* AES - [**Advanced Encryption Standard**](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard)
* AGESA - [**AMD Generic Encapsulated Software Architecture**](https://en.wikipedia.org/wiki/AGESA_
* AGP - The [**Accelerated Graphics
  Port**](http://en.wikipedia.org/wiki/Accelerated_Graphics_Port) is an
  older (1997-2004) point-to-point bus for video cards to communicate
  with the processor.
* AHCI - The [**Advanced Host Controller
  Interface**](http://en.wikipedia.org/wiki/Advanced_Host_Controller_Interface)
  is a standard register set for communicating with a SATA controller.
  * [http://www.intel.com/technology/serialata/ahci.htm](http://www.intel.com/technology/serialata/ahci.htm)
  * [http://download.intel.com/technology/serialata/pdf/rev1_3.pdf](http://download.intel.com/technology/serialata/pdf/rev1_3.pdf)
* AIC - Add-in Card
* AIO - Computer formfactor: [**All In One**](https://en.wikipedia.org/wiki/Desktop_computer#All-in-one)
* ALIB - AMD: ACPI-ASL Library
* ALS - [**Ambient Light Sensor**](https://en.wikipedia.org/wiki/Ambient_light_sensor)
* ALU - [**Arithmetic Logic Unit**](https://en.wikipedia.org/wiki/Arithmetic_logic_unit)
* AMD64 - Another name for [**x86-64**](https://en.wikipedia.org/wiki/X86-64)
* AMPL - AMD: [**Advanced Platform Management Link**](https://web.archive.org/web/20220509053546/https://developer.amd.com/wordpress/media/2012/10/419181.pdf) - Also referred to as
  SBI: Sideband Interface
* AMT - Intel: [**Active Management Technology**](https://en.wikipedia.org/wiki/Intel_Active_Management_Technology)
* ANSI - [**American National Standards Institute**](American_National_Standards_Institute)
* AOAC - AMD: Always On, Always Connected
* AP - Application processor - The main processor on the board (as
  opposed to the embedded controller or other processors that may be on
  the system), any cores in processor chip that isn’t the BSP - Boot
  Strap Processor.
* APCB - AMD: AMD PSP Customization Block
* API - [**Application Programming Interface**](https://en.wikipedia.org/wiki/API)
* APIC - [**Advanced Programmable Interrupt
  Controller**](http://en.wikipedia.org/wiki/Advanced_Programmable_Interrupt_Controller)
  this is an advanced version of a PIC that can handle interrupts from
  and for multiple CPUs. Modern systems usually have several APICs:
  Local APICs (LAPIC) are CPU-bound, IO-APICs are bridge-bound.
  * [http://osdev.berlios.de/pic.html](http://osdev.berlios.de/pic.html)
* APL - Intel: [**Apollo Lake**](https://en.wikichip.org/wiki/intel/cores/apollo_lake)
* APM - [**Advanced Power Management**](https://en.wikipedia.org/wiki/Advanced_Power_Management) - The standard for power management
  before ACPI (Yes, they’re both advanced).  APM was managed entirely by
  the firmware and the operating system had no control or even awareness
  of the power management.
* APOB - AMD: [**AGESA PSP Output Buffer**](https://doc.coreboot.org/soc/amd/family17h.html#additional-definitions)
* APU - AMD: [**Accelerated Processing Unit**](https://en.wikipedia.org/wiki/AMD_Accelerated_Processing_Unit)
* ARC - HDMI: [**Audio Return Channel**](https://en.wikipedia.org/wiki/HDMI#ARC)
* ARM - [**Advanced RISC Machines**](https://en.wikipedia.org/wiki/Arm_%28company%29) - Originally Acorn RISC Machine.  This
  may refer to either the company or the instruction set.
* ARP - Networking: [**Address Resolution Protocol**](https://en.wikipedia.org/wiki/Address_Resolution_Protocol)
* ASCII - [**American Standard Code for Information Interchange**](https://en.wikipedia.org/wiki/ASCII)
* ASEG - The A_0000h-B_FFFFh memory segment - this area was typically
  hidden by the Video BIOS
* ASF - [**Alert Standard Format**](https://en.wikipedia.org/wiki/Alert_Standard_Format)
* ASL - [**ACPI Source Language**](https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/19_ASL_Reference/ACPI_Source_Language_Reference.html)
* ASLR - Address Space Layout Randomization
* ASP - AMD: AMD Security Processor (Formerly the PSP - Platform
  Security Processor)
* ASPM - PCI: [**Active State Power
  Management**](https://en.wikipedia.org/wiki/Active_State_Power_Management)
* ATA - [**Advanced Technology Attachment**](https://en.wikipedia.org/wiki/Parallel_ATA)
* ATAPI - [**ATA Packet Interface**](https://en.wikipedia.org/wiki/Parallel_ATA#ATAPI)
* ATX - [**Advanced Technology eXtended**](https://en.wikipedia.org/wiki/ATX)
* AVX -  [**Advanced Vector Extensions**](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions)


## B

* BAR - [**Base Address Register**](http://en.wikipedia.org/wiki/Base_Address_Register) This generally refers to one of the
  base address registers in the PCI config space of a PCI device
* Baud - [**Baud**](https://en.wikipedia.org/wiki/Baud) - Not an acronym - Symbol rate unit of symbols per second, named
  after Émile Baudot
* BBS - [**BIOS boot specification**](https://en.wikipedia.org/wiki/Option_ROM#BIOS_Boot_Specification)
* BCD - [**Binary-Coded Decimal**](https://en.wikipedia.org/wiki/Binary-coded_decimal)
* BCT - Intel: [**Binary Configuration Tool**](https://github.com/intel/BCT)
* BDA - [**BIOS Data Area**](http://www.bioscentral.com/misc/bda.htm) This refers to the memory area of 0x40:0000 which is where the original PC-BIOS stored its data tables.
* BDF - [**BUS, Device, Function**](https://en.wikipedia.org/wiki/PCI_configuration_space#Technical_information) - A way of referencing a PCI Device
  function address.
* BDS - UEFI: [**Boot-Device Select**](https://en.wikipedia.org/wiki/Unified_Extensible_Firmware_Interface#BDS_%E2%80%93_Boot_Device_Select)
* BDW - Intel: [**Broadwell**](https://en.wikichip.org/wiki/intel/microarchitectures/broadwell_%28client%29)
* BERT - ACPI: [**Boot Error Record Table**](https://uefi.org/specs/ACPI/6.4/18_ACPI_Platform_Error_Interfaces/error-source-discovery.html)
* BGA - [**Ball Grid Array**](https://en.wikipedia.org/wiki/Ball_grid_array)
* BGP - Networking: [**Border Gateway Protocol**](https://en.wikipedia.org/wiki/Border_Gateway_Protocol)
* Big Real mode - Real mode running in a way that allows it to access
  the entire 4GiB of the 32-bit address space. Also known as flat mode
  or [**Unreal mode**](https://en.wikipedia.org/wiki/Unreal_mode).
* BIOS - [**Basic Input/Output
  System**](http://en.wikipedia.org/wiki/BIOS)
* BIST - The [**Built-in Self Test**](https://en.wikipedia.org/wiki/Built-in_self-test) is a test run by the processor on
  itself when it is first started. Usually, any nonzero value indicates
  that the selftest failed.
* Bit-banging - [**Bit-banging**](https://en.wikipedia.org/wiki/Bit_banging) - A term for the method of emulating a more complex
  protocol by using GPIOs.
* BKDG - AMD: [**Bios & Kernel Developers' guide**](https://en.wikichip.org/wiki/amd/List_of_AMD_publications) (Replaced by the PPR -
  Processor Programming Reference)
* BLOB - [**Binary Large OBject**](https://en.wikipedia.org/wiki/Binary_large_object) - Originally a collection of binary files
  stored as a single object, this was co-opted by the open source
  communities to mean any proprietary binary file that is not available
  as source code.
* BMC - [**Baseboard Management Controller**](https://en.wikipedia.org/wiki/Intelligent_Platform_Management_Interface#Baseboard_management_controller)
* BMP - [**Bitmap**](https://en.wikipedia.org/wiki/BMP_file_format)
* BOM - [**Bill of Materials**](https://en.wikipedia.org/wiki/Bill_of_materials)
* BPDT - Boot Partition Description Table
* bps - Bits Per Second
* BS - coreboot: Boot State - coreboot's ramstage sequence are made up
  of boot states.  Each of these states can be hooked to run functions
  before the stat, during the state, or after the state is complete.
* BSF - Intel: [**Boot Specification File**](https://www.intel.com/content/dam/develop/external/us/en/documents/boot-setting-1-0-820293.pdf)
* BSP - BootStrap Processor - The initialization core of the main
  system processor.  This is the processor core that starts the boot
  process.
* BSS - [**Block Starting Symbol**](https://en.wikipedia.org/wiki/.bss)
* BT - [**Bluetooth**](https://en.wikipedia.org/wiki/Bluetooth)
* Bus - Initially a term for a number of connectors wired together in
  parallel, this is now used as a term for any hardware communication
  method.
* BWG - Intel: BIOS Writers Guide


## C
* C-states: ACPI Processor Idle states.
  [**C-States**](https://en.wikichip.org/wiki/acpi/c-states) C0-Cx: Each
  higher number saves more power, but takes longer to return to a fully
  running processor.
* C0 - ACPI Defined Processor Idle state: Active - CPU is running
* C1 - ACPI Defined Processor Idle state: Halt - Nothing currently
  running, but can start running again immediately
* C2 - ACPI Defined Processor Idle state: Stop-clock - core clocks off
* C3 - ACPI Defined Processor Idle state: Sleep - L1 & L2 caches may be
  saved to Last Level Cache (LLC), core powered down.
* C4+ - Processor Specific idle states
* CAR - [**Cache As RAM**](https://web.archive.org/web/20140818050214/https://www.coreboot.org/data/yhlu/cache_as_ram_lb_09142006.pdf)
* CBFS - coreboot filesystem
* CBMEM - coreboot Memory
* CBI - Google: [**CrOS Board Information**](https://chromium.googlesource.com/chromiumos/docs/+/HEAD/design_docs/cros_board_info.md)
* CDN - [**Content Delivery Network**](https://en.wikipedia.org/wiki/Content_delivery_network)
* CEM - PCIe: [**Card ElectroMechanical**](https://members.pcisig.com/wg/PCI-SIG/document/folder/839) specification
* CFL - [**Coffee Lake**](https://en.wikichip.org/wiki/intel/microarchitectures/coffee_lake)
* CID - [**Coverity ID**](https://en.wikipedia.org/wiki/Coverity)
* CIM - [**Common Information Model**](https://www.dmtf.org/standards/cim)
* CISC - [**Complex Instruction Set Computer**](https://en.wikipedia.org/wiki/Complex_instruction_set_computer)
* CL - Change List - A git patch in gerrit
* CLK - Clock - Used when there isn't enough room for 2 additional
  characters - similar to RST, for people who hate vowels.
* CML - Intel: [**Comet Lake**](https://en.wikichip.org/wiki/intel/microarchitectures/comet_lake)
* CMOS - [**Complementary Metal Oxide
  Semiconductor**](https://en.wikipedia.org/wiki/Nonvolatile_BIOS_memory)
  - This is a method of making ICs (Integrated Circuits). For BIOS, it’s
  generally used to describe a section of NVRAM (Non-volatile RAM), in
  this case a section battery-backed memory in the RTC (Real Time Clock)
  that is typically used to store BIOS settings.
  *[http://en.wikipedia.org/wiki/Nonvolatile_BIOS_memory](http://en.wikipedia.org/wiki/Nonvolatile_BIOS_memory)
* CNL - Intel: [**Cannon Lake**](https://en.wikichip.org/wiki/intel/microarchitectures/cannon_lake) (formerly Skymont)
* CNVi - Intel: [**Connectivity Integration**](https://en.wikipedia.org/wiki/CNVi)
* CPL - x86: Current Privilege Level - Privilege levels range from 0-3; lower numbers are more privileged.
* CPLD - [**Complex Programmable Logic Device**](https://en.wikipedia.org/wiki/Complex_programmable_logic_device)
* CPS - Characters Per Second
* CPU - [**Central Processing
  Unit**](http://en.wikipedia.org/wiki/Central_processing_unit)
* CPUID - x86: [**CPU Identification**](https://en.wikipedia.org/wiki/CPUID) opcode
* Cr50 - Google: The first generation Google Security Chip (GSC) used on
  Chrome OS devices.
* CRB - Customer Reference Board
* CRLF - Carriage Return, Line Feed - \\r\\n - The standard window EOL
  (End-of-Line) marker.
* crt0 - [**C Run Time 0**](http://en.wikipedia.org/wiki/Crt0)
* crt0s - crt0 Source code
* CRT - [**Cathode Ray Tube**](https://en.wikipedia.org/wiki/Cathode-ray_tube)
* CSE - Intel: Converged Security Engine
* CSI - MIPI: [**Camera Serial
  Interface**](https://en.wikipedia.org/wiki/Camera_Serial_Interface)
* CSME - Intel: Converged Security and Management Engine
* CVE - [**Common Vulnerabilities and Exposures**](https://en.wikipedia.org/wiki/Common_Vulnerabilities_and_Exposures)
* CZN - AMD: Cezanne - CPU Family 19h, Model 50h


## D

* D-States - [**ACPI Device power
  states**](https://en.wikipedia.org/wiki/Advanced_Configuration_and_Power_Interface#Device_states)
  D0-D3 - These are device specific power states, with each higher
  number requiring less power, and typically taking a longer time to get
  back to D0, fully running.
* D0 - ACPI Device power state: Active - Device fully on and running
* D1 - ACPI Device power state: Lower power than D0
* D2 - ACPI Device power state: Lower power than D1
* D3 Hot - ACPI Device power state: Device is in a low power state, but
  still has power.
* D3 Cold - ACPI Device power state: Power is completely removed from
  the device.
* DASH - [**Desktop and mobile Architecture for System Hardware**](Desktop_and_mobile_Architecture_for_System_Hardware)
* DB - DaughterBoard
* DC - Electricity: Direct Current
* DCP - Digital Content Protection
* DCR - **Decode Control Register** This is a way of identifying the
  hardware in question. This is generally paired with a Vendor ID (VID)
* DDC - [**Display Data Channel**](https://en.wikipedia.org/wiki/Display_Data_Channel)
* DDI -  Intel: Digital Display Interface
* DDR - [**Double Data Rate**](https://en.wikipedia.org/wiki/Double_data_rate)
* DHCP - [**Dynamic Host Configuration Protocol**](https://en.wikipedia.org/wiki/Dynamic_Host_Configuration_Protocol)
* DID - Device Identifier
* DIMM - [**Dual Inline Memory Module**](https://en.wikipedia.org/wiki/DIMM)
* DIP - [**Dual inline package**](https://en.wikipedia.org/wiki/Dual_in-line_package)
* DMA - [**Direct Memory
  Access**](http://en.wikipedia.org/wiki/Direct_memory_access) Allows
  certain hardware subsystems within a computer to access system memory
  for reading and/or writing independently of the main CPU. Examples of
  systems that use DMA: Hard Disk Controller, Disk Drive Controller,
  Graphics Card, Sound Card. DMA is an essential feature of all modern
  computers, as it allows devices of different speeds to communicate
  without subjecting the CPU to a massive interrupt load.
* DMI - [**Desktop Management Interface**}(Desktop_Management_Interface)
* DMIC - Digital Microphone
* DMTF - [**Distributed Management Task Force**](https://en.wikipedia.org/wiki/Distributed_Management_Task_Force)
* DMZ - Demilitarized Zone
* DNS - [**Domain Name Service**](https://en.wikipedia.org/wiki/Domain_Name_System)
* DNV - Intel: [**Denverton**](https://en.wikichip.org/wiki/intel/cores/denverton)
* DOS - Disk Operating System
* DP - DisplayPort
* DPTF - Intel: Dynamic Power and Thermal Framework
* DRAM - Memory: [**Dynamic Random Access Memory**](https://en.wikipedia.org/wiki/Dynamic_random-access_memory)
* DRTM - Dynamic Root of Trust for Measurement
* DQ - Memory: Data I/O signals.  On a D-flipflop, used for SRAM, the
  data-in pin is generally referred to as D, and the data-out pin is Q,
  thus the IO Data signal lines are referred to as DQ lines.
* DQS - Memory: Data Q Strobe - Data valid signal for DDR memory.
* DRM - [**Digital Rights Management**](https://en.wikipedia.org/wiki/Digital_rights_management)
* DRQ - DMA Request
* DRTU - Intel: Diagnostics and Regulatory Testing Utility
* DSDT - The [**Differentiated System Descriptor
  Table**](http://acpi.sourceforge.net/dsdt/index.php), is generated by
  BIOS and necessary for ACPI. Implementation of ACPI in coreboot needs
  to be done in a "cleanroom" development process and **MAY NOT BE
  COPIED** from an existing firmware to avoid legal issues.
* DSC - [**Digital Signal Controller**](https://en.wikipedia.org/wiki/Digital_signal_controller)
* DSL - [**Digital subscriber line**](https://en.wikipedia.org/wiki/Digital_subscriber_line)
* DSP - [**Digital Signal Processor**](https://en.wikipedia.org/wiki/Digital_signal_processor)
* DTB - U-Boot: Device Tree Binary
* dTPM - Discrete Trusted Platform Module
* DTS - U-Boot: Device Tree Source
* DVFS - ARM: Dynamic Voltage and Frequency Scaling
* DVI - [**Digital Video Interface**](https://en.wikipedia.org/wiki/Digital_Visual_Interface)
* DVT - Production Timeline: Design Validation Test
* DW - DesignWare
* DXE - UEFI:  [**Driver Execution Environment**](https://en.wikipedia.org/wiki/Unified_Extensible_Firmware_Interface#DXE_%E2%80%93_Driver_Execution_Environment_
* DXIO - AMD: Distributed CrossBar I/O


## E

* EBDA - Extended BIOS Data Area
* ECC - [**Error Correction Code**](https://en.wikipedia.org/wiki/Error_correction_code) - Typically used to refer to a type of
  memory that can detect and correct memory errors.
* EDID - [**Extended Display Identification Data**](https://en.wikipedia.org/wiki/Extended_Display_Identification_Data)
* EDK2 - EFI Development Kit 2
* EDO - Memory: [**Extended Data
  Out**](https://en.wikipedia.org/wiki/Dynamic_random-access_memory#Extended_data_out_DRAM)
  - A DRAM standard introduced in 1994 that improved upon, but was
  backwards compatible with FPM (Fast Page Mode) memory.
* EDP - [**Embedded DisplayPort**](DisplayPort)
* EDS - Intel: External Design Specification
* EEPROM - [**Electrically Erasable Programmable ROM**](https://en.wikipedia.org/wiki/EEPROM) (common mistake:
  electrical erasable programmable ROM).
* EFI - [**Extensible Firmware Interface**](https://en.wikipedia.org/wiki/Unified_Extensible_Firmware_Interface)
* EHCI - [**Enhanced Host Controller Interface**](https://en.wikipedia.org/wiki/Host_controller_interface_%28USB%2C_Firewire%29#EHCI) - USB 2.0
* EHL - Intel: [**Elkhart Lake**](https://en.wikichip.org/wiki/intel/cores/elkhart_lake)
* EIDE - Enhanced Integrated Drive Electronics
* EMI - [**ElectroMagnetic
  Interference**](https://en.wikipedia.org/wiki/Electromagnetic_interference)
* eMMC - [**embedded MultiMedia
  Card**](https://en.wikipedia.org/wiki/MultiMediaCard#eMMC)
* EOP - End of POST
* EOL - End of Life
* EPP - Intel: Energy-Performance Preference
* EPROM - Erasable Programmable Read-Only Memory
* ESD - Electrostatic discharge
* eSPI - Enhanced System Peripheral Interface
* EVT - Production Timeline: Engineering Validation Test


## F

* FADT - ACPI Table: Fixed ACPI Description Table
* FAE - Field Application Engineer
* FAT - File Allocation Table
* FCH - AMD: Firmware Control Hub
* FCS - Production Timeline: First Customer Shipment
* FDD - Floppy Disk Drive
* FFS - UEFI: Firmware File System
* FIFO - First In, First Out
* FIT - Intel: Firmware Interface Table
* FIT - Flattened-Image Tree
* FIVR - Intel: Fully Integrated Voltage Regulators
* Flashing - Flashing means the writing of flash memory. The BIOS on
  modern mainboards is stored in a NOR flash EEPROM chip.
* Flat mode - Real mode running in a way that allows it to access the
  entire 4GiB of the 32-bit address space.  Also known as Unreal mode or
  Big Real mode
* FMAP - coreboot: [**Flash map**](https://doc.coreboot.org/lib/flashmap.html)
* FPDT - ACPI: Firmware Performance Data Table
* FPGA - [**Field-Programmable Gate Array**](https://en.wikipedia.org/wiki/Field-programmable_gate_array)
* Framebuffer - The
  [**framebuffer**](http://en.wikipedia.org/wiki/Framebuffer) is a part
  of RAM in a computer which is allocated to hold the graphics
  information for one frame or picture. This information typically
  consists of color values for every pixel on the screen. A framebuffer
  is either:
  * Off-screen, meaning that writes to the framebuffer don't appear on
    the visible screen.
  * On-screen, meaning that the framebuffer is directly coupled to the
    visible display.
* FPM - Memory: [**Fast Page Mode**](https://en.wikipedia.org/wiki/Dynamic_random-access_memory#Page_mode_DRAM) - A DRAM standard introduced in 1990.
* FPU - [**Floating-Point Unit**](https://en.wikipedia.org/wiki/Floating-point_unit)
* FSB - [**Front-Side Bus**](https://en.wikipedia.org/wiki/Front-side_bus)
* FSP - Intel: Firmware Support Package
* FTP - Network Protocol: [**File Transfer Protocol**](https://en.wikipedia.org/wiki/File_Transfer_Protocol)
* FTPM - Firmware TPM


## G

* G0 - ACPI Global Power State: System is running
* G0-G3 - ACPI Global Power States
* G1 - ACPI Global Power State: System is suspended
* G2 - ACPI Global Power State: Soft power-off.  The mainboard is off,
  but can be woken up electronically, by a button, wake-on-lan, a
  keypress, or some other method.
* G3 - ACPI Global Power State: Mechanical Off. There is no power going
  to the system except for a small battery to keep the CMOS contents,
  Real Time Clock, and maybe a few other registers running.
* GART - AMD: [**Graphics Address Remapping Table**](https://en.wikipedia.org/wiki/Graphics_address_remapping_table)
* GATT - Graphics Aperture Translation Table
* GLK - Intel: [**Gemini Lake**](https://en.wikichip.org/wiki/intel/cores/gemini_lake)
* GMA - Intel: [**Graphics Media
  Accelerator**](https://en.wikipedia.org/wiki/Intel_GMA)
* GNB - Graphics NorthBridge
* GNVS - Global Non-Volatile Storage
* GPD - PCH GPIO in Deep Sleep well (D5 power)
* GPI - GPIOs: GPIO Input
* GPIO - [**General Purpose Input/Output**](https://en.wikipedia.org/wiki/General-purpose_Input/Output) (Pin)
* GPMR - Intel: General Purpose Memory Range
* GPO - GPIOs: GPIO Output
* GPP - AMD: General Purpose (PCI/PCIe) port
* GPP - Intel: PCH GPIO in Primary Well (S0 power only)
* GPS - Nvidia: GPU Performance Scale
* GPT - UEFI: [**GUID Partition Table**](https://en.wikipedia.org/wiki/GUID_Partition_Table)
* GPU - [**Graphics Processing Unit**](https://en.wikipedia.org/wiki/Graphics_processing_unit)
* GSoC - [**Google Summer of Code**](https://en.wikipedia.org/wiki/Google_Summer_of_Code)
* GSC - Google Security Chip - Typically Cr50/Ti50, though could also refer to the titan chips
* GUID - UEFI: [**Globally Unique IDentifier**](https://en.wikipedia.org/wiki/Universally_unique_identifier)


## H

* HDA - [**High Definition Audio**](https://en.wikipedia.org/wiki/Intel_High_Definition_Audio)
* HDCP - [**High-bandwidth Digital Content Protection**](https://en.wikipedia.org/wiki/High-bandwidth_Digital_Content_Protection)
* HDD - Hard Disk Drive
* HDMI - [**High-Definition Multimedia Interface**](https://en.wikipedia.org/wiki/HDMI)
* HDR - [**High Dynamic Range**](https://en.wikipedia.org/wiki/High_dynamic_range)
* HECI - Intel: [**Host Embedded Controller Interface**](https://en.wikipedia.org/wiki/Host_Embedded_Controller_Interface) (Replaced by MEI)
* HID - [**Human Interface
  Device**](https://en.wikipedia.org/wiki/Human_interface_device)
* HOB - UEFI: Hand-Off Block
* HPET - [**High Precision Event Timer**](https://en.wikipedia.org/wiki/High_Precision_Event_Timer)
* HSTI - Hardware Security Test Interface
* HSW - Intel: Haswell
* Hybrid S3 - System Power State:  This is where the operating system
  saves the contents of RAM out to the Hard drive, as if preparing to go
  to S4, but then goes into suspend to RAM.  This allows the system to
  resume quickly from S3 if the system stays powered, and resume from
  the disk if power is lost.
* Hypertransport - AMD: The
  [**Hypertransport**](http://en.wikipedia.org/wiki/Hypertransport) bus
  is an older (2001-2017) high-speed electrical interconnection protocol
  specification between CPU, Memory, and (occasionally) peripheral
  devices.  This was originally called the Lightning Data Transport
  (LDT), which could be seen reflected in various register names.
  Hypertransport was replaced by AMD's Infinity Fabric (IF) on AMD's Zen
  processors.


## I

* I2C - **Inter-Integrated Circuit** is a bidirectional 2-wire bus for
  communication generally between different ICs on a circuit board.
  * [https://www.esacademy.com/en/library/technical-articles-and-documents/miscellaneous/i2c-bus.html](https://www.esacademy.com/en/library/technical-articles-and-documents/miscellaneous/i2c-bus.html)
* I2S - [**Inter-IC Sound**](https://en.wikipedia.org/wiki/I%C2%B2S)
* I3C - [**I3c**](https://en.wikipedia.org/wiki/I3C_%28bus%29) is not an
  acronym - The follower to I2C (Inter-Integrated Circuit)
  - Also known as SenseWire
* IA - Intel Architecture
* IA-64 - Intel Itanium 64-bit architecture
* IBB – Initial Boot Block
* IBV - Independent BIOS Vendor
* IC - Integrated Circuit
* ICL - Intel: Ice Lake
* IDE - Software: Integrated Development Environment
* IDE - Integrated Drive Electronics - A type of hard drive - Used
  interchangeable with ATA, though IDE describes the drive, and ATA
  describes the interface.  Generally replaced by SATA (Though again,
  SATA describes the interface, not actually the drive)
* IDSEL/AD - Initialization Device SELect/Address and Data. Each PCI
  slot has a signal called IDSEL. It is used to differentiate between
  the different slots.
* IF - AMD: [**Infinity
  Fabric**](https://en.wikipedia.org/wiki/HyperTransport#Infinity_Fabric)
  is a superset of AMD's earlier Hypertransport interconnect.
* IMC - AMD: Integrated micro-controller - An 8051 microcontroller built
  into some AMD FCHs (Fusion Controller Hubs) and Southbridge chips.
  This never worked well for anything beyond fan control and caused
  numerous issues by reading from the BIOS flash chip, preventing other
  devices from communicating with the flash chip at runtime.
* IMC - Integrated Memory Controller - This is a less usual use of the
  IMC acronym, but seems to be growing somewhat.
* IO or I/O - Input/Output
* IoC - Security: Indicator of Compromise
* IOC - Intel: I/O Cache
* IOE - Intel: I/O Expander
* IOM - Intel: I/O Manager
* IOMMU - [**I/O Memory Management Unit**](https://en.wikipedia.org/wiki/Input%E2%80%93output_memory_management_unit)
* IOMUX - AMD: The I/O Mux block controls how each GPIO is configured.
* IOSF - Intel: Intel On-chip System Fabric
* IP - Intellectual Property
* IP - Internet Protocol
* IPC - Inter-Processor Communication/Inter-Process Communication
* IPI - Inter Processor Interrupt
* IPMI - Intelligent Platform Management Interface
* IRQ - Interrupt Request
* ISA - Instruction set architecture
* ISA (bus) - Industry standard architecture - Replaced generally by PCI
  (Peripheral Control Interface)
* ISDN - Integrated Services Digital Network
* ISH - AMD PSP: Image Slot Header
* ISH - Intel: Integrated Sensor Hub - A microcontroller built into the
  processor to help offload data processing from various sensors on a
  mainboard.
* ISP - Internet Service Provider
* IVHD - ACPI: I/O Virtualization Hardware Definition
* IVMD - ACPI: I/O Virtualization Memory Definition
* IVRS - I/O Virtualization Reporting Structure


## J

* JEDEC - Joint Electron Device Engineering Council
* JSL - Intel: Jasper Lake
* JTAG - The [**Joint Test Action
  Group**](https://en.wikipedia.org/wiki/JTAG) created a standard for
  communicating between chips  to verify and test ICs and PCB designs.
  The standard was named after the group, and has become a standard
  method of accessing special debug functions on a chip allowing for
  hardware-level debug of both the hardware and software.


## K

* KBL - Intel: Kaby Lake
* KVM - Keyboard Video Mouse


## L
* L0s - ASPM Power State: Turn off power for one direction of the PCIe
  serial link.
* L1-Cache - The fastest but smallest memory cache on a processor.
  Frequently split into Instruction and Data caches (I-Cache / D-Cache,
  also occasionally abbreviated as i$ and d$)
* L1 - ASPM Power State: The L1 power state shuts the PCIe link off
  completely until triggered to resume by the CLKREQ# signal.
* L2-Cache - The second level of memory cache on a processor, this is a
  larger cache than L1, but takes longer to access.  Typically checked
  only after data has not been found in the L1-cache.
* L3-Cache - The Third, and typically final memory cache level on a
  processor.  The L3 cache is typically quite a bit larger than the L1 &
  L2 caches, but again takes longer to access, though it's still much
  faster than reading memory.  The L3 cache is frequently shared between
  multiple cores on a modern CPU.
* LAN - Local Area Network
* LAPIC - Local APIC
* LBA - Logical Block Address
* LCD - Liquid Crystal Display
* LCAP - PCIe:Link Capabilities
* LED - Light Emitting Diode
* LF - Line Feed - The standard Unix EOL (End-of-Line) marker.
* LGTM - Looks Good To Me
* LLC - Last Level Cache
* LLVM - Initially stood for Low Level Virtual Machine, but now is just
  the name of the project, as it has expanded past its original goal.
* LP5 - LPDDR5
* LPDDR5 - [**Low-Power DDR 5 SDRAM**](https://en.wikipedia.org/wiki/LPDDR)
* LPC - The [**Low Pin
  count**](http://www.intel.com/design/chipsets/industry/lpc.htm) bus
  was a replacement for the ISA bus, created by serializing a number of
  parallel signals to get rid of those connections.
* LPT - Line Print Terminal, Local Print Terminal, or Line Printer. -
  The Parallel Port
* LRU - Least Recently Used - a rule used in operating systems that
  utilises a paging system. LRU selects a page to be paged out if it has
  been used less recently than any other page. This may be applied to a
  cache system as well.
* LSB - Least Significant Bit
* LTE - Telecommunication: [**Long-Term
  Evolution**](https://en.wikipedia.org/wiki/LTE_%28telecommunication%29)
* LVDS - Low-Voltage Differential Signaling


## M

* M.2 - An interface specification for small peripheral cards.
* MAC Address - Media Access Control Address
* MBR - Master Boot Record
* MCA - [**Machine Check Architecture**](https://en.wikipedia.org/wiki/Machine_Check_Architecture)
* MCR - Machine Check Registers
* MCU - Memory Control Unit
* MCU - [**MicroController
  Unit**](https://en.wikipedia.org/wiki/Microcontroller)
* MDFIO - Intel: Multi-Die Fabric IO
* ME - Intel: Management Engine
* MEI - Intel: ME Interface (Previously known as HECI)
* Memory training - the process of finding the best speeds, voltages,
  and delays for system memory.
* MHU: ARM: Message Handling Unit
* MIPI: The [**Mobile Industry Processor
  Interface**](https://en.wikipedia.org/wiki/MIPI_Alliance) Alliance has
  developed a number of different specifications for mobile devices.
  The Camera Serial Interface (CSI) is a widely used interface that has
  made its way into laptops.
* MIPS - Millions of Instructions per Second
* MIPS (processor) - Microprocessor without Interlocked Pipelined
  Stages.
* MKBP - Matrix Keyboard Protocol
* MMC - [**MultiMedia
  Card**](https://en.wikipedia.org/wiki/MultiMediaCard)
* MMIO - [**Memory Mapped I/O**](http://en.wikipedia.org/wiki/MMIO)
  allows peripherals' memory or registers to be accessed directly
  through the memory bus.  When the memory bus size was very small, this
  was initially done by hiding any memory at that address, effectively
  wasting that memory.  In modern systems, that memory is typically
  moved to the end of the physical memory space, freeing a 'hole' to map
  devices into.
* MMU - Memory Management Unit
* MMX - Officially, not an acronym, trademarked by Intel. Unofficially,
  Matrix Math eXtension.
* MODEM - Modulator-Demodulator
* Modern Standby - Microsoft's name for the S0iX states
* MOP - Macro-Operation
* MOS - Metal-Oxide-Silicon
* MP - Production Timeline: Mass Production
* MPU - Memory Protection Unit
* MPTable - The Intel [**MultiProcessor
  specification**](https://en.wikipedia.org/wiki/MultiProcessor_Specification)
  is a hardware compatibility guide for machine hardware designers and
  OS software writers to produce SMP-capable machines and OSes in a
  vendor-independent manner. Version 1.1 of the spec was released in
  1994, and the 1.4 version was released in 1995.  This has been
  generally been
  https://en.wikipedia.org/wiki/MultiProcessor_Specification by the ACPI
  tables.
* MRC - Intel: Memory Reference Code
* MSB - Most Significant Bit
* MSI - Message Signaled Interrupt
* MSR - Machine-Specific Register
* MT/s - MegaTransfers per second
* MTL - Intel: Meteor Lake
* MTL - ARM: MHU Transport Layer
* MTRR - [**Memory Type and Range
  Register**](http://en.wikipedia.org/wiki/MTRR)


## N

* Nack - Negative Acknowledgement
* NBCI - Nvidia: NoteBook Common Interface
* NC - GPIOs: No Connect
* NDA - Non-Disclosure Agreement.
* NF - GPIOs: Native Function - GPIOs frequently have multiple different
  functions, one of which is defined as the default, or Native function.
* NFC - [**Near Field
  Communication**](https://en.wikipedia.org/wiki/Near-field_communication)
* NGFF - [**Next Generation Form
  Factor**](https://en.wikipedia.org/wiki/M.2) - The original name for
  M.2
* NHLT - ACPI Table - Non-HDA Link Table
* NIC - Network Interface Card
* NMI - Non-maskable interrupt
* Nonce - Cryptography: [**Number used once**](https://en.wikipedia.org/wiki/Cryptographic_nonce)
* NOP - No Operation
* NTFS - New Technology File System
* NVME - Non-Volatile Memory Express - An SSD interface that allows
  access to the flash memory through a PCIe bus.
* NVPCF - Nvidia Platform and Control Framework
* NX - No Execute


## O

* ODH - GPIOs: Open Drain High - High is driven to the reference voltage, low is a high-impedance state
* ODL - GPIOs: Open Drain Low - Low is driven to ground, High is a high-impedance state.
* ODM - Original Design Manufacturer
* OEM - Original Equipment Manufacturer
* OHCI - [**Open Host Controller
  Interface**](https://en.wikipedia.org/wiki/Host_Controller_Interface_%28USB%29)
  - non-proprietary USB Host controller for USB 1.1 (May also refer to
  the open host controller for IEEE 1394, but this is less common).
* OOBE - Out Of the Box Experience
* OPP - ARM: Operating Performance Points
* OS - Operating System
* OTA - Over the Air
* OTP - One Time Programmable


## P

* PAE - physical address extension
* PAL - Programmable Array Logic
* PAM - Intel: Programmable Attribute Map - This is the legacy BIOS
  region from 0xC_0000 to 0xF_FFFF
* PAT - [**Page Attribute
  Table**](https://en.wikipedia.org/wiki/Page_attribute_table) This can
  be used independently or in combination with MTRR to setup memory type
  access ranges. Allows more finely-grained control than MTRR.
* PAT - Intel: [**Performance Acceleration
  Technology**](https://en.wikipedia.org/wiki/Performance_acceleration_technology)
* PATA - Parallel Advanced Technology Attachment - A renaming of ATA
  after SATA became the standard.
* PAVP - [**Intel: Protected Audio-Video
  Path**](https://en.wikipedia.org/wiki/Intel_GMA#Protected_Audio_Video_Path)
* PC - Personal Computer
* PC AT - Personal Computer Advanced Technology
* PC100 - An SDRAM specification for a 100MHz memory bus.
* PCB - Printed Circuit Board
* PCD - UEFI: Platform Configuration Database
* PCH - Intel: [**Platform Controller Hub**](https://en.wikipedia.org/wiki/Platform_Controller_Hub)
* PCI - [**Peripheral Control
  Interconnect**](http://en.wikipedia.org/wiki/Peripheral_Component_Interconnect)
  - Replaced generally by PCIe (PCI Express)
* PCI Configuration Space - The [**PCI Config
  space**](http://en.wikipedia.org/wiki/PCI_Configuration_Space) is an
  [address space](https://en.wikipedia.org/wiki/Address_space) for all
  PCI devices.  Originally, this address space was accessed through an
  index/data pair by writing the address that you wanted to read/write
  into the I/O address 0xCF8, then reading or writing I/O Address 0xCFC.
  This has been updated to an MMIO method which increases each PCI
  function's configuration space from 256 bytes to 4K.
* PCIe - [**PCI Express**](http://en.wikipedia.org/wiki/Pci_express)
* PCMCIA: Personal Computer Memory Card International Association
* PCO - AMD: Picasso
* PCR: TPM: Platform Configuration Register
* PD - GPIOs: Pull-Down - Setting the pin high drives it to the reference voltage.  Setting it low drives it to ground through a resistor.
* PD - Power Delivery - This is a specification for communicating power
  needs and availability between two devices, typically over USB type C.
* PEG - PCIe Graphics - A (typically) x16 PCIe slot connected to the CPU
  for higher graphics bandwidth and lower latency.
* PEI - UEFI: Pre-EFI Initialization
* PEIM - UEFI: PEI Module
* PEP - Intel: Power Engine Plug-in
* PHY - [**PHYsical layer**](http://en.wikipedia.org/wiki/PHY) - The
  hardware that implements the send/receive functionality of a
  communication protocol.
* PI - Platform Initialization
* PIC - [**Programmable Interrupt
  Controller**](https://en.wikipedia.org/wiki/Programmable_interrupt_controller)
* PII - [**Personally Identifiable
  Information**](https://en.wikipedia.org/wiki/Personal_data)
* PIO - [**Programmed
  I/O**](https://en.wikipedia.org/wiki/Programmed_input%E2%80%93output)
* PIR - PCI Interrupt Router
* PIR Table - The [**PCI Interrupt Routing
  Table**](https://web.archive.org/web/20080206072638/http://www.microsoft.com/whdc/archive/pciirq.mspx)
  was a Microsoft specification that allowed windows to determine how
  each PCI slot was wired to the interrupt router.
* PIRQ - PCI IRQ
* PIT - Generally refers to the 8253/8254 [**Programmable Interval
  Timer](https://en.wikipedia.org/wiki/Programmable_interval_timer).
* PLCC - [**Plastic leaded chip
  carrier**](http://en.wikipedia.org/wiki/Plastic_leaded_chip_carrier)
* PLL - [**Phase-Locked
  Loop**](https://en.wikipedia.org/wiki/Phase-locked_loop)
* PM - Platform Management
* PM - Power Management
* PMC Intel: Power Management Controller
* PMIC - Power Management IC (Pronounced "P-mick")
* PMIO - Port-Mapped I/O
* PMU - Power Management Unit
* PNP - Plug aNd Play
* PoP - Point-of-Presence
* POR - Plan of Record
* POR - Power On Reset
* Port80 - The [**I/O port
  0x80**](https://en.wikipedia.org/wiki/Power-on_self-test#Progress_and_error_reporting)
  is the address for BIOS writes to update diagnostic information during
  the boot process.
* POST - [**Power-On Self
  Test**](https://en.wikipedia.org/wiki/Power-on_self-test)
* POTS - [**Plain Old Telephone
  Service**](https://en.wikipedia.org/wiki/Plain_old_telephone_service)
* PPI - UEFI: PEIM-to-PEIM Interface
* PPR: Processor Programming Reference
* PPT - AMD: Package Power Tracking
* PROM: Programmable Read Only Memory
* Proto - Production Timeline: The first initial production to test key
  concepts.
* PSE - Page Size Extention
* PSP - AMD: Platform Security Processor
* PSPP - AMD: PCIE Speed Power Policy
* PU - GPIOs: Pull-Up - Setting the pin low drives it to ground.  Setting it high drives it to the reference voltage through a resistor.
* PVT - Production Timeline: (Production Validation Test
* PWM - Pulse Width Modulation
* PXE - Pre-boot Execution Environment


## Q

* QOS - Quality of Service


## R

* RAID - redundant array of inexpensive disks - as opposed to SLED -
  single large expensive disk.
* RAM - Random Access Memory
* RAMID - Boards that have soldered-down memory (no DIMMs) can have
  various different sizes, speeds, and brands of memory chips attached.
  Because there is no SPD, (for cost savings) the memory needs to be
  identified in a different manner.  The simplest of these is done using
  a set of 3 or 4 GPIOs to allow 8 to 16 different memory chips to be
  used.
* RAPL - Running Average Power Limit
* RCS - [**Revision control
  system**](https://en.wikipedia.org/wiki/Revision_Control_System)
* Real mode - The original 20-bit addressing mode of the 8086 & 8088
  computers, allowing the system to access 1MiB of memory through a
  Segment:Offset index pair.  In 2022, this is still the mode that
  x86-64 processors are in at the reset vector!
* RDMA - [**Remote Direct Memory
  Access**](http://en.wikipedia.org/wiki/Remote_Direct_Memory_Access) is
  a concept whereby two or more computers communicate via DMA directly
  from main memory of one system to the main memory of another.
* RFC - Request for Comment
* RFI - [**Radio-Frequency
  Interference**](https://en.wikipedia.org/wiki/Electromagnetic_interference)
* RGB - Red, Green, Blue
* RISC - Reduced Instruction Set Computer
* RMA - Return Merchandise Authorization
* RO - Read Only
* ROM - Read Only Memory
* RoT - Root of Trust
* RPL - Intel: [**Raptor Lake**](https://en.wikipedia.org/wiki/Raptor_Lake)
* RRG - AMD (ATI): Register Reference Guide
* RSDP - Root System Description Pointer
* RTC - Real Time Clock
* RTFM - Read the Fucking Manual
* RTOS - Real-Time Operating System
* RVP - Intel: Reference Validation Platform
* RW - Read / Write
* RX - Receive


## S

* S-states - ACPI System Power States: [**Sleep states**](https://uefi.org/specs/ACPI/6.4/16_Waking_and_Sleeping/sleeping-states.html)
* S0 - ACPI System Power State: Fully running
* S0 - S5 - ACPI System power states level 0 - 5, with each higher
  numbered power state being (theoretically) lower power than the
  previous, and (again theoretically) taking longer to get back to a
  fully running system than the previous.
* S1 - ACPI System Power State: Standby - This isn’t use much anymore,
  but it used to put the Processor into a powered, but idle state, power
  down any drives, and turn off the display. This would wake up almost
  instantly because no processor context was lost in this state.
* S2 - ACPI System Power State: Lower power than S1, Higher power than
  S3, I don’t know that this state was ever well defined by any group.
* S3 - ACPI System Power State: Suspend to RAM - A low-power state where
  the processor context is copied to the system Memory, then the
  processor and all peripherals are powered off.  On wake, or resume,
  the system starts to boot normally, then switches to restore the
  memory registers to the previous settings, restore the processor
  context from memory, and jump back to the operating system to pick up
  where it left off.
* S4 - ACPI System Power State: Suspend to Disk.  The processor context
  and all the contents of memory are copied to the hard drive. This is
  typically fully handled by the operating system, so resume is a normal
  boot through all of the firmware, then the OS restore the original
  contents of memory.  Any critical processor state is restored.
* S5 - ACPI System Power State: System is “completely powered off”, but
  still has power going to the board.
* SAR - The [**Specific Absorption
  Rate**](https://en.wikipedia.org/wiki/Specific_absorption_rate) is the
  measurement for the amount of Radio Frequency (RF) energy absorbed by
  the body in units of Watts per Kilogram. This may be built into
  coreboot as a table.
* SAS - Serial Attached SCSI - A serialized version of SCSI used mostly
  for high performance hard drives and tape drives.
* SATA - Serial Advanced Technology Attachment
* SB - South Bridge
* SB-RMI - AMD: Sideband Remote Management Interface
* SB-TSI - SideBand Temperature Sensor Interface
* SBA - SideBand Addressing
* SBI - SideBand Interface
* SBOM - Software Bill of Materials
* SCI - System Control Interrupt
* SCP - ARM: System Control Processor
* SCP - Network Protocol: Secure Copy
* SCSI - Small Computer System Interface - A high-bandwidth
  communication interface for peripherals.  This is a very old interface
  that has seen numerous updates and is still used today, primarily in
  SAS (Serial Attached SCSI).  The initial version is now often referred
  to as Parallel SCSI.
* SD - [**Secure Digital**](https://en.wikipedia.org/wiki/SD_card) card
* SDRAM - Synchronous DRAM
* SDLE: AMD: Stardust Dynamic Load Emulator
* SEEP - Serial EEPROM (Electrically Erasable Programmable Read-Only
  Memory)
* SEV - AMD: Secure Encrypted Virtualization
* Shadow RAM - RAM which content is copied from ROM residing at the same
  address for speedup purposes.
* Shim - A small piece of code whose only purpose is to act as an
  interface to load another piece of code.
* SIMD - Single Instruction, Multiple Data
* SIMM - Single Inline Memory Module
* SIPI - Startup Inter Processor Interrupt
* SIO - [**Super I/O**](https://en.wikipedia.org/wiki/Super_I/O)
* SKL - Intel: SkyLake
* SKU - Stock Keeping Unit
* SMART: [**Self-Monitoring Analysis And Reporting
  Technology**](https://en.wikipedia.org/wiki/S.M.A.R.T.)
* SMBIOS - [**System Management
  BIOS**](https://en.wikipedia.org/wiki/System_Management_BIOS)
* SMBus -  [**System Management
  Bus**](https://en.wikipedia.org/wiki/System_Management_Bus)
  * [http://www.smbus.org/](http://www.smbus.org/
* SMI - System management interrupt
* SMM - [**System management
  mode**](https://en.wikipedia.org/wiki/System_Management_Mode)
* SMN - AMD: System Management Network
* SMRAM - System Management RAM
* SMT - Simultaneous Multithreading
* SMT - Surface Mount
* SMT - Symmetric Multithreading
* SNP - AMD: Secure Nested Paging
* SMU - AMD: System Management Unit
* SO-DIMM: Small Outline Dual In-Line Memory Module
* SoC - System on a Chip
* SOIC - [**Small-Outline Integrated
  Circuit**](http://en.wikipedia.org/wiki/Small-outline_integrated_circuit)
* SPD - [**Serial Presence
  Detect**](https://en.wikipedia.org/wiki/Serial_presence_detect)
* SPI - [**Serial Peripheral
  Interface**](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)
* SPL - AMD: Security Patch Level
* SPMI - MIPI: System Power Management Interface
* SRAM - Static Random Access Memory
* SSD - Solid State Drive
* SSDT - Secondary System Descriptor Table - ACPI table
* SSE - Streaming SIMD Extensions
* SSH - Network Protocol: Secure Shell
* SSI - **Server System Infrastructure**
* SSI-CEB - Physical board format: [**SSI Compact Electronics
  Bay**](https://en.wikipedia.org/wiki/SSI_CEB)
* SSI-EEB - Physical board format: [**SSI Enterprise Electronics
  Bay**](https://en.wikipedia.org/wiki/SSI_CEB) is a wider version of
  ATX with different standoff placement.
* SSI-MEB - Physical board format: [**SSI Midrange Electronics
  Bay**](https://en.wikipedia.org/wiki/SSI_CEB)
* SSI-TEB - Physical board format: [**SSI Thin Electronics
  Bay**](https://en.wikipedia.org/wiki/SSI_CEB)
* STAPM - AMD: Skin Temperature Aware Power Management
* SuperIO - The [**Super I/O**](https://en.wikipedia.org/wiki/Super_I/O)
  (SIO) device provides a system with any of a number of different
  peripherals.  Most common are: A PS/2 Keyboard and mouse port, LPT
  Ports, UARTS, Watchdog Timers, Floppy drive Controllers, GPIOs, or any
  of a number of various other devices.
* SVI2/3 - Serial VID (Voltage Identification) Interface 2.0 / 3.0


## T

* TBT - Thunderbolt
* TBT - Intel: Turbo Boost Technology
* TCC - Intel: Thermal Control Circuit
* TCP - Transmission Control Protocol
* TCPC - Type C Port Controller
* TCSS - Intel: Type C SubSystem
* TDMA - Time-Division Multiple Access
* TDP - [**Thermal Design
  Power**](https://en.wikipedia.org/wiki/Thermal_design_power)
* TEE - Trusted Execution Environment
* TFTP - Network Protocol: Trivial File Transfer Protocol
* TGL - Intel: Tigerlake
* THC - Touch Host Controller
* Ti50 - Google: The next generation GSC (Google Security chip) on
  Chrome OS devices after Cr50
* TLA - Techtronics Logic Analyzer
* TLA - Three Letter Acronym
* TLB - [**Translation Lookside
  Buffer**](https://en.wikipedia.org/wiki/Translation_lookaside_buffer)
* TOCTOU - Time-Of-Check to Time-Of-Use
* TOLUM - Top of Low Usable Memory
* ToM - Top of Memory
* TPM - Trusted Platform Module
* TS - TimeStamp -
* TSC - [**Time Stamp
  Counter**](https://en.wikipedia.org/wiki/Time_Stamp_Counter)
* TSEG - TOM (Top of Memory) Segment
* TWAIN - Technology without an interesting name.
* TX - Transmit
* TXE - Intel: Trusted eXecution Engine


## U

* UART - Universal asynchronous receiver-transmitter
* UC - UnCacheable. Memory type setting in MTRR/PAT.
* uCode - [**Microcode**](https://en.wikipedia.org/wiki/Microcode)
* UDK - UEFI: UEFI Development Kit
* UDP - User Datagram Protocol
* UEFI - Unified Extensible Firmware Interface
* UFS - Universal Flash storage
* UHCI - USB: [**Universal Host Controller
  Interface**](https://en.wikipedia.org/wiki/Host_controller_interface_%28USB%2C_Firewire%29%23UHCI)
  - Intel proprietary USB 1.x Host controller
* Unreal mode - Real mode running in a way that allows it to access the
  entire 4GiB of the 32-bit address space - Also known as Big real mode
  or Flat mode.
* UMA - Unified Memory Architecture
* UMI - AMD: [**Unified Media
  Interface**](https://en.wikipedia.org/wiki/Unified_Media_Interface)
* UPD - Updatable Product Data
* UPS - Uninterruptible Power Supply
* USART - Universal Synchronous/Asynchronous Receiver/Transmitter
* USB - Universal Serial Bus


## V

* VBIOS - Video BIOS
* VBNV - Vboot Non-Volatile storage
* VBT - [**Video BIOS
  Table**](https://01.org/linuxgraphics/gfx-docs/drm/ch04s02.html#id-1.4.3.4.16)
* VESA - Video Electronics Standards Association
* VGA: Video Graphics Array
* VID: Vendor Identifier
* VID: AMD: Voltage Identifier
* VLB - VESA Local Bus
* VOIP - Voice over IP
* Voodoo mode - a silly name for Big Real mode.
* VPD - Vital Product Data
* VPN - Virtual Private Network
* VR - Voltage Regulator
* VRAM - Video Random Access Memory
* VRM - Voltage Regulator Module
* VT-d - Intel: Virtualization Technology for Directed I/O


## W

* WAN - [**Wide Area Network**](https://en.wikipedia.org/wiki/Wide_area_network)
* WB - Cache Policy: [**Write-Back**](https://en.wikipedia.org/wiki/Cache_%28computing%29)
* WC - Cache Policy: [**Write-Combining**](https://en.wikipedia.org/wiki/Cache_%28computing%29)
* WCAM - World-facing Camera - A camera on a device that is not intended
  to be used as a webcam, but instead to film scenes away from the user.
  For clamshell devices, his may be on the keyboard panel for devices
  devices that open 360 degrees, or on the outside of the cover.  For
  tablets, it's on the the side away from the screen.
* WDT - [**WatchDog Timer**](https://en.wikipedia.org/wiki/Watchdog_timer)
* WLAN - Wireless LAN (Local Area Network)
* WP - Cache policy: [**Write-Protected**](https://en.wikipedia.org/wiki/Cache_%28computing%29)
* WO - Write-only
* WOL - [**Wake-on-LAN**](https://en.wikipedia.org/wiki/Wake-on-LAN)
* WT - Cache Policy: [**Write Through**](https://en.wikipedia.org/wiki/Cache_%28computing%29)


## X

* x64 - Another name for [**x86-64**](https://en.wikipedia.org/wiki/X86-64) or AMD64.
* x86 - [**x86**](https://en.wikipedia.org/wiki/X86) Originally referred to any device compatible with the 8088/8086
  architectures, this now typically means compatibility with the 80386
  32-bit instruction set (also referred to as IA-32)
* x86-64 - The 64-bit extension to the x86 architecture.  Also known as
  [**AMD64**](https://en.wikipedia.org/wiki/X86-64) as it was developed by AMD.  Long-mode refers to when the
  processor is running in the 64-bit mode.
* XBAR - AMD: Abbreviation for crossbar, their command packet switch
  which determines what data goes where within the processor or SoC
* XHCI - USB: [**Extensible Host Controller Interface**](https://en.wikipedia.org/wiki/Extensible_Host_Controller_Interface) - USB Host controller
  supporting 1.x, 2.0, and 3.x devices.



## Y


## Z

* ZIF - Zero Insertion Force


## References:
* [AMD Glossary of terms](https://www.amd.com/system/files/documents/glossary-of-terms-20220505-for-web.pdf)
