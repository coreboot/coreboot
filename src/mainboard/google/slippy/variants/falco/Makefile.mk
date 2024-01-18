## SPDX-License-Identifier: GPL-2.0-only

# Order of names in SPD_SOURCES is important!
SPD_SOURCES  = Micron_4KTF25664HZ     # 4GB / CH0 + CH1 (RAM_ID=000)
SPD_SOURCES += Hynix_HMT425S6AFR6A    # 4GB / CH0 + CH1 (RAM_ID=001)
SPD_SOURCES += Elpida_EDJ4216EFBG     # 4GB / CH0 + CH1 (RAM_ID=010)
SPD_SOURCES += Micron_4KTF25664HZ     # 2GB / CH0 only  (RAM_ID=011)
SPD_SOURCES += Hynix_HMT425S6AFR6A    # 2GB / CH0 only  (RAM_ID=100)
SPD_SOURCES += Elpida_EDJ4216EFBG     # 2GB / CH0 only  (RAM_ID=101)
SPD_SOURCES += Samsung_M471B5674QH0   # 4GB / CH0 + CH1 (RAM_ID=110)
SPD_SOURCES += Samsung_M471B5674QH0   # 2GB / CH0 only  (RAM_ID=111)

LIB_SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/spd/$(f).spd.hex)
