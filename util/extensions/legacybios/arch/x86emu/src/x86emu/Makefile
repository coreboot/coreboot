TARGETLIB = libx86emu.a
OBJS      = debug.o decode.o fpu.o ops.o ops2.o prim_ops.o sys.o
INCLUDES   = -I$(TOPDIR)/arch/x86emu/src/x86emu -I$(TOPDIR)/arch/x86emu/src/x86emu/x86emu -I$(TOPDIR)/arch/x86emu/include
CFLAGS := -D__DRIVER__ -DFORCE_POST -D_CEXPORT= -DNO_LONG_LONG -DDEBUG $(CFLAGS)

include $(TOPDIR)/Rules.make

$(TARGETLIB): $(OBJS)
	cd $(BUILDDIR) && ar rv $(TARGETLIB) $(OBJS)

