obj-y += clog2.o
obj-y += uart8250.o
obj-y += memset.o
obj-y += memcpy.o
obj-y += memcmp.o
obj-y += memmove.o
obj-y += malloc.o
obj-y += delay.o
obj-y += fallback_boot.o
obj-y += compute_ip_checksum.o
obj-y += version.o
obj-y += cbfs.o
obj-y += lzma.o
#obj-y += lzmadecode.o
obj-y += gcc.o

initobj-y += uart8250.o
initobj-y += memset.o
initobj-y += memcpy.o
initobj-y += memcmp.o
initobj-y += cbfs.o
initobj-y += lzma.o
#initobj-y += lzmadecode.o

obj-$(CONFIG_USBDEBUG_DIRECT) += usbdebug_direct.o
obj-$(CONFIG_COMPRESSED_PAYLOAD_LZMA) += lzma.o

ifdef POST_EVALUATION
$(obj)/lib/version.o :: $(obj)/build.h
endif
