#-------------------------------------------------
# $Id$
#-------------------------------------------------

ELF064=elf/elf-header-065kb.payload
ELF113=elf/elf-header-113kb.payload
ELF129=elf/elf-header-129kb.payload

LOADER=loader.o

PIRQ_W=
PIRQ_M=pirq/pirq-MS7308E.bin
PIRQ_P=pirq/pirq-P6STMT.bin
PIRQ_T=pirq/pirq-ThinkPad.T23.bin

BOCHS_R=bochs
BOCHS_B=bochs/bios
BOCHS_C=bochs/bochs-cvs
BOCHS_X=bochs/bochs-cvs/bochs/bios

BIOS_R=${BOCHS_R}/rombios.bin
BIOS_B=${BOCHS_B}/rombios.bin
BIOS_X=${BOCHS_X}/rombios.bin

VIDEO64W=video/video.bios-WINFAST64kb.bin
VIDEO64M=
VIDEO64P=video/video.bios-P6STMT-64kb.bin

PAYLOAD=payload

BOCHSCVS=-d:pserver:anonymous@cvs.bochs.sourceforge.net:/cvsroot/bochs

#-------------------------------------------------

all: clean build #install

#-------------------------------------------------

build: loader.o bios
	cat ${ELF129} ${LOADER} ${PIRQ_M} ${BIOS_B} ${VIDEO64P} > ${PAYLOAD}

#-------------------------------------------------

loader.o: loader.s
	as86 loader.s -b ${LOADER}

#-------------------------------------------------

bios:
	( cd ${BOCHS_B} ; make )
	sync

#-------------------------------------------------

#64kb -- sive of Video BIOS
#
#C=`echo 64*1024 | bc`
C=65536

#0xC0000 -- start of Video BIOS in ram.
#S=`echo -e "ibase=16 \n C0000" | bc`
#
# ELF header offset -- 0x1000 == 4096
#
# 786432+4096 = 790528
S=790528

videobios:
	 dd if=/proc/kcore of=./video.bios.bin bs=1 count=${C} skip=${S}

# 0xF000+0x1000
S2=987136

pcbios: 
	 dd if=/proc/kcore of=./pc.bios.bin bs=1 count=${C} skip=${S2}

pirq: pcbios
	#grep -a "\$PIR" -B0 -A127 pc.bios.bin > pirq.bin
	grep -a "\$PIR" -B0 -A0 pc.bios.bin > pirq.bin

#now you have to hand edit the pirq.bin file

#-------------------------------------------------

bochslogin:
	mkdir -p ${BOCHS_C}
	echo "When prompted for a password, simply press the Enter key."
	( cd ${BOCHS_C} ; cvs ${BOCHSCVS} login )

bochsco:
	mkdir -p ${BOCHS_C}
	( cd ${BOCHS_C} ; cvs ${BOCHSCVS} co bochs/bios )

bochsstat:
	( cd ${BOCHS_C} ; cvs ${BOCHSCVS} status bochs/bios/rombios.c )

bochsupdate:
	( cd ${BOCHS_C} ; cvs ${BOCHSCVS} update bochs/bios/rombios.c )


#-------------------------------------------------

install:
	cp -abf ${PAYLOAD} /tftpboot/kernel
	sync

#-------------------------------------------------

clean:
	rm -rf *.o ${PAYLOAD}
	(cd ${BOCHS_B} ; make clean ) 

distclean: clean
	rm -rf ${BOCHS_C}

#-------------------------------------------------

