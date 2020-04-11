# HP Laptops with KBC1126 Embedded Controller

This document is about HP EliteBook series laptops up to Ivy Bridge era
which use SMSC KBC1126 as embedded controller.

SMSC KBC1126 (and older similar chips like KBC1098) has been used in
HP EliteBooks for many generations.  BIOS and EC firmware share an SPI
flash chip in these laptops, so we need to put firmware blobs for the
EC to the coreboot image.

## EC firmware extraction and coreboot building

The following document takes EliteBook 2760p as an example.

First, you need to extract the blobs needed by EC firmware using util/kbc1126.
You can extract them from your backup firmware image, or firmware update
provided by HP with [unar] as follows:

```bash
wget https://ftp.hp.com/pub/softpaq/sp79501-80000/sp79710.exe
unar sp79710.exe
${COREBOOT_DIR}/util/kbc1126/kbc1126_ec_dump sp79710/Rompaq/68SOU.BIN
mv 68SOU.BIN.fw1 ${COREBOOT_DIR}/2760p-fw1.bin
mv 68SOU.BIN.fw2 ${COREBOOT_DIR}/2760p-fw2.bin
```

When you config coreboot, select:

```text
Chipset --->
  [*] Add firmware images for KBC1126 EC
      (2760p-fw1.bin) KBC1126 firmware #1 path and filename
      (2760p-fw2.bin) KBC1126 filename #2 path and filename
```

## Porting guide for HP laptops with KBC1126

To port coreboot to an HP laptop with KBC1126, you need to do the
following:

- select Kconfig option `EC_HP_KBC1126`
- select Kconfig option `SUPERIO_SMSC_LPC47N217` if there is LPC47n217
  Super I/O, usually in EliteBook 8000 series, which can be used for
  debugging via serial port
- initialize EC and Super I/O in romstage
- add EC and Super I/O support to devicetree.cb

To get the related values for EC in devicetree.cb, you need to extract the EFI
module EcThermalInit from the vendor UEFI firmware with [UEFITool]. Usually,
`ec_data_port`, `ec_cmd_port` and `ec_ctrl_reg` has the following values:

- For EliteBook xx60 series: 0x60, 0x64, 0xca
- For EliteBook xx70 series: 0x62, 0x66, 0x81

You can use [radare2] and the following [r2pipe] Python script to find
these values from the EcThermalInit EFI module:

```python
#!/usr/bin/env python

# install radare2 and use `pip3 install --user r2pipe` to install r2pipe

import r2pipe
import sys

if len(sys.argv) < 2:
    fn = "ecthermalinit.efi"
else:
    fn = sys.argv[1]

r2 = r2pipe.open(fn)
r2.cmd("aa")
entryf = r2.cmdj("pdfj")

for insn in entryf["ops"]:
    if "lea r8" in insn["opcode"]:
        _callback = insn["ptr"]
        break

r2.cmd("af @ {}".format(_callback))
callbackf_insns = r2.cmdj("pdfj @ {}".format(_callback))["ops"]

def find_port(addr):
    ops = r2.cmdj("pdfj @ {}".format(addr))["ops"]
    for insn in ops:
        if "lea r8d" in insn["opcode"]:
            return insn["ptr"]

ctrl_reg_found = False

for i in range(0, len(callbackf_insns)):
    if not ctrl_reg_found and "mov cl" in callbackf_insns[i]["opcode"]:
        ctrl_reg_found = True
        ctrl_reg = callbackf_insns[i]["ptr"]
        print("ec_ctrl_reg = 0x%02x" % ctrl_reg)
        cmd_port = find_port(callbackf_insns[i+1]["jump"])
        data_port = find_port(callbackf_insns[i+3]["jump"])
        print("ec_cmd_port = 0x%02x\nec_data_port = 0x%02x" % (cmd_port, data_port))

    if "mov bl" in callbackf_insns[i]["opcode"]:
        ctrl_value = callbackf_insns[i]["ptr"]
        print("ec_fan_ctrl_value = 0x%02x" % ctrl_value)
```


[unar]: https://theunarchiver.com/command-line
[UEFITool]: https://github.com/LongSoft/UEFITool
[radare2]: https://radare.org/
[r2pipe]: https://github.com/radare/radare2-r2pipe
