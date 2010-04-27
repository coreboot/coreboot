
#define RET_LABEL(label)        \
	jmp label##_done

#define CALL_LABEL(label)       \
	jmp label               ;\
label##_done:

#define CALLSP(func) \
	lea     0f, %esp        ; \
	jmp func                ; \
0:

#define RETSP \
	jmp *%esp


#include "console.inc"
#include "pci.inc"
#include "ramtest.inc"

jmp llshell_out

// (c) 2004 Bryan Chafy,  This program is released under the GPL

// LLShell, A low level interactive debug shell
// Designed to be an interactive shell that operates with zero
// system resources.  For example at initial boot.

// to use, jump to label "low_level_shell"
// set %esp to the return address for exiting


#define UART_BASEADDR $0x3f8
#define resultreg %esi
#define subroutinereg %edi
#define freqtime $2193  // 1.93 * freq
#define timertime $6000
.equ	sys_IOPL, 110

// .data
// .text

welcome:
 	.string "\r\n! Low Level Shell (LLShell)  (c)2004 Bryan Chafy \r\n\r\n"
prompt:
	.string "\r\n!> "
badcmd:
 	.string "bad command\r\n"
sorry:
 	.string "sorry, not yet implemented\r\n"
cmds:
       	.string "\r\nList of commands:\r\n \
\r\nbeep                    -- pc speaker beep \
\r\nrst (or RST)            -- reset \
\r\nout(b,w,l) <val> <port> -- raw out val at port \
\r\nin(b,w,l)  <port>       -- show raw port value \
\r\njmp  <address>          -- jmp to address (llshell addr is in eax) \
\r\ncall <address>          -- funcion call (assumes a working stack) \
\r\ncli                     -- clear interrupts \
\r\nsti                     -- enable interrupts \
\r\npush <value>            -- push value onto stack \
\r\npop                     -- pop from stack and display \
\r\nwm(b,w,l) <addr> <val>  -- write mem \
\r\ndm   <addr> <lines>     -- dump mem  \
\r\nmcp  <src> <dst> <size> -- mem copy  \
\r\nmpat <pat> <dst> <size> -- mem pattern \
\r\nmemt <begin> <end>      -- memory test \
\r\npcir(b,w,l) <loc>       -- pci read config \
\r\npciw(b,w,l) <loc> <val> -- pci write config  \
\r\ndl   <addr> <size>      -- memory download (display xor cheksum at completion) \
\r\ncram <addr> <size>      -- enable cache to be ram (experimental) \
\r\nbaud <val>              -- change baudrate (not yet implemented)  \
\r\nexit                    -- exit shell \
\r\nAll values in hex (0x prefixing ok)  \
\r\n"

cr:
	.string "\r\n"
spaces:
	.string "   "

// .globl _start
//ASSUME CS:@CODE, DS:@DATA

// _start:

// call ioperms

low_level_shell:

mov $preamble,subroutinereg
jmp beep
preamble:
mov $welcome,resultreg
mov $readcommand,subroutinereg
jmp displaystring

readcommand:
mov $prompt,resultreg
mov $rcmd,subroutinereg
jmp displaystring

rcmd:
mov $readcommand,subroutinereg
movl $0x0, resultreg

readchar:
mov  UART_BASEADDR+5,%dx
in   %dx, %al
and  $0x9f,%al
test $0x01,%al
jz   readchar
mov  UART_BASEADDR,%dx
in   %dx,%al             //char in al
xchg %al,%ah

send_char:
mov  UART_BASEADDR+5,%dx
us_wait:
in   %dx,%al
test $0x20,%al
jz us_wait
mov  UART_BASEADDR,%dx
xchg %al,%ah
out  %al,%dx            // output char

cmp  $0x0D,%al      //CR
jnz  eval_char
mov  $0x0A,%ah
jmp  send_char

eval_char:
cmp  $0x20,%al      //space
jz   cmdtable
cmp  $0x0A,%al      //CR
jz   cmdtable
cmp  $0x08,%al      //BS
jnz  additup
//subit:
shr  $0x8,resultreg
jmp  readchar
additup:
shl  $0x8,resultreg
and  $0xff,%eax
add  %eax,resultreg
jmp  readchar

cmdtable:
mov resultreg,%eax
cmp $0,%eax
jz  readcommand
cmp $0x74657374,%eax
jz  dotest
cmp $0x68656c70,%eax
jz  dohelp
cmp $0x0000003f,%eax
jz  dohelp
cmp $0x6f757462,%eax
jz  dooutb
cmp $0x6f757477,%eax
jz  dooutw
cmp $0x6f75746c,%eax
jz  dooutd
cmp $0x00696e62,%eax
jz doinb
cmp $0x00696e77,%eax
jz doinw
cmp $0x00696e6c,%eax
jz doind
cmp $0x63697262,%eax
jz pcirb
cmp $0x63697277,%eax
jz pcirw
cmp $0x6369726c,%eax
jz pcirl
cmp $0x63697762,%eax
jz pciwb
cmp $0x63697777,%eax
jz pciww
cmp $0x6369776c,%eax
jz pciwl
cmp $0x00776d62,%eax
jz  wmemb
cmp $0x00776d77,%eax
jz  wmemw
cmp $0x00776d6c,%eax
jz  wmeml
cmp $0x0000646d,%eax
jz  dodmem
cmp $0x6d656d74,%eax
jz  memt                    // mem test
cmp $0x00727374,%eax
jz  rst                     // reset
cmp $0x00525354,%eax
jz  RST
cmp $0x62656570,%eax
jz  beep
cmp $0x0000646c,%eax
jz  dodl                    // download to mem <loc> <size>
cmp $0x006a6d70,%eax
jz  jmpto                   // jump to location (eax holds return addr)
cmp $0x62617564,%eax
jz  baud                    // change baudrate
cmp $0x00696e74,%eax
jz  doint                   // trigger an interrupt
cmp $0x63616c6c,%eax
jz  callto                  // call assumes memory
cmp $0x70757368,%eax
jz  dopush                  // assumes mem
cmp $0x00706f70,%eax
jz  dopop                   // assumes mem
cmp $0x6372616d,%eax
jz  cram                    // cache ram trick <location> <size>
cmp $0x006d6370,%eax
jz  mcp                     // mem copy <src> <dst> <size>
cmp $0x6d706174,%eax
jz  dopattern
cmp $0x00636c69,%eax
jz docli
cmp $0x00737469,%eax
jz dosti
cmp $0x65786974,%eax
jz  doexit
mov $badcmd,resultreg
mov $readcommand,subroutinereg
jmp displaystring


readnibbles:
movl $0x0, resultreg
readit:
mov  UART_BASEADDR+5,%dx
in   %dx, %al
and  $0x9f,%al
test $0x1,%al
jz   readit
mov  UART_BASEADDR,%dx
in   %dx,%al
xchg %al,%ah

sendchar:
mov  UART_BASEADDR+5,%dx
us_waitit:
in   %dx,%al
test $0x20,%al
jz   us_waitit
mov  UART_BASEADDR,%dx
xchg %al,%ah
out  %al,%dx            // output char

cmp  $0x78,%al
jz   readit
cmp  $0x0D,%al      //CR
jnz  evalchar
mov  $0x0A,%ah
jmp  sendchar

evalchar:
cmp  $0x20,%al        //space
jz   gosub
cmp  $0x0A,%al      //CR
jz   gosub
cmp  $0x08,%al      //BS
jnz  processchar
//subit:
shr  $0x04,resultreg
jmp  readit
processchar:
cmp  $0x3A,%al
jl   subnum
cmp  $0x47,%al
jl   subcaps
//sublc:
sub  $0x57,%al
jmp  additupn
subcaps:
sub  $0x37,%al
jmp  additupn
subnum:
sub  $0x30,%al
additupn:
shl  $0x04,resultreg
and  $0xf,%eax
add  %eax,resultreg
jmp  readit

gosub:
jmp  *subroutinereg

//intersubcall
// eax,edx,esi,edi

// ebx,ecx,ebp,esp(?)
// ds,es,fs,gs

dotest:
mov $ramtest,resultreg
mov $test1a,subroutinereg
jmp displayhex
test1a:
mov $welcome,resultreg
mov $readcommand,subroutinereg
jmp displayhex

dodmem:

movl $dmem1a, subroutinereg
jmp  readnibbles
dmem1a:
mov  resultreg,%ebx    // address
movl $dmem1b, subroutinereg
jmp  readnibbles
dmem1b:
mov  resultreg,%ecx    // length

dmemloop:
mov %ebx,resultreg
mov $daddr1,subroutinereg
jmp displayhex
daddr1:
mov $spaces,resultreg
mov $startshowm,subroutinereg
jmp displaystring

startshowm:
mov (%ebx),resultreg
mov $showm1,subroutinereg
jmp displayhexlinear
showm1:
add $0x04,%ebx
mov (%ebx),resultreg
mov $showm2,subroutinereg
jmp displayhexlinear
showm2:
add $0x04,%ebx
mov (%ebx),resultreg
mov $showm3,subroutinereg
jmp displayhexlinear
showm3:
add $0x04,%ebx
mov (%ebx),resultreg
mov $showa0,subroutinereg
jmp displayhexlinear

showa0:
sub $0xC,%ebx
mov (%ebx),resultreg
mov $showa1,subroutinereg
jmp displayasciilinear
showa1:
add $0x04,%ebx
mov (%ebx),resultreg
mov $showa2,subroutinereg
jmp displayasciilinear
showa2:
add $0x04,%ebx
mov (%ebx),resultreg
mov $showa3,subroutinereg
jmp displayasciilinear
showa3:
add $0x04,%ebx
mov (%ebx),resultreg
mov $doneshow,subroutinereg
jmp displayasciilinear
doneshow:
mov $cr,resultreg
mov $doneshow1,subroutinereg
jmp displaystring
doneshow1:
dec %cx
cmp $0x0,%cx
jz  exitdmem
add $0x04,%ebx
jmp dmemloop
exitdmem:
jmp readcommand

dooutb:
// out val,port
movl $outb1a, subroutinereg
jmp  readnibbles
outb1a:
mov  resultreg,%ebx
movl $outb1b, subroutinereg
jmp  readnibbles
outb1b:
mov  resultreg,%edx
mov  %ebx,%eax
out  %al,%dx
jmp  readcommand

dooutw:
// out val,port
movl $outw1a, subroutinereg
jmp  readnibbles
outw1a:
mov  resultreg,%ebx
movl $outw1b, subroutinereg
jmp  readnibbles
outw1b:
mov  resultreg,%edx
mov  %ebx,%eax
out  %ax,%dx
jmp  readcommand

dooutd:
// out val,port
movl $outd1a, subroutinereg
jmp  readnibbles
outd1a:
mov  resultreg,%ebx
movl $outd1b, subroutinereg
jmp  readnibbles
outd1b:
mov  resultreg,%edx
mov  %ebx,%eax
out  %eax,%dx
jmp  readcommand

wmemb:
movl $wmemba, subroutinereg
jmp  readnibbles
wmemba:
mov  resultreg,%ebx
movl $wmembb, subroutinereg
jmp  readnibbles
wmembb:
mov  resultreg,%eax
mov  %al,(%ebx)
jmp  readcommand

wmemw:
movl $wmemwa, subroutinereg
jmp  readnibbles
wmemwa:
mov  resultreg,%ebx
movl $wmemwb, subroutinereg
jmp  readnibbles
wmemwb:
mov  resultreg,%eax
mov  %ax,(%ebx)
jmp  readcommand

wmeml:
movl $wmemla, subroutinereg
jmp  readnibbles
wmemla:
mov  resultreg,%ebx
movl $wmemlb, subroutinereg
jmp  readnibbles
wmemlb:
mov  resultreg,%eax
mov  %eax,(%ebx)
jmp  readcommand

doinb:
// in port
movl $inb1a, subroutinereg
jmp  readnibbles
inb1a:
mov  resultreg,%edx
mov  $0x0,%eax
in   %dx,%al
mov  %eax,resultreg
mov  $readcommand,subroutinereg
jmp  displayhex

doinw:
// in port
movl $inw1a, subroutinereg
jmp  readnibbles
inw1a:
mov  resultreg,%edx
mov  $0x0,%eax
in   %dx,%ax
mov  %eax,resultreg
mov  $readcommand,subroutinereg
jmp  displayhex

doind:
// in port
movl $ind1a, subroutinereg
jmp  readnibbles
ind1a:
mov  resultreg,%edx
in   %dx,%eax
mov  %eax,resultreg
mov  $readcommand,subroutinereg
jmp  displayhex

jmpto:
movl $jmp1a, subroutinereg
jmp  readnibbles
jmp1a:
mov  $readcommand,%eax
jmp  *resultreg

callto:
movl $call1a, subroutinereg
jmp  readnibbles
call1a:
mov  $readcommand,%eax
call *resultreg
jmp  readcommand

dopush:
movl $push1a, subroutinereg
jmp  readnibbles
push1a:
mov  resultreg,%eax
push %eax
jmp  readcommand

doint:
movl $int1a, subroutinereg
jmp  readnibbles
int1a:
mov  resultreg,%eax
// need to lookup int table?
// int  %eax
jmp  readcommand

doenter:
//setup stack frame


dopop:
movl $readcommand, subroutinereg
pop  resultreg
jmp  displayhex

docli:
cli
jmp  readcommand

dosti:
sti
jmp  readcommand


displaystring:
// resultreg= pointer to string terminated by \0
dsloop:
movb  (resultreg),%ah
cmp  $0x0, %ah
jz   displaystringexit
mov  UART_BASEADDR+5,%dx
us_waits:
in   %dx,%al
test $0x20,%al
jz us_waits
mov  UART_BASEADDR,%dx
xchg %al,%ah
out  %al,%dx            // output char
inc  resultreg
jmp  dsloop
displaystringexit:
jmp  *subroutinereg

displayhexlinear:
mov  resultreg,%eax
xchg %al,%ah
rol  $0x10,%eax
xchg %al,%ah
mov  %eax,resultreg
displayhex:
rol  $0x10,%ecx
mov  $0x8,%cx
dhloop:
cmp  $0xf,%cl
je   exitdisplayhex
rol  $0x04,resultreg
movl resultreg,%eax
and  $0xf,%al
cmp  $0xa,%al
jl   addnum
//addcaps
add $0x37,%al
jmp  outcharhex
addnum:
add  $0x30,%al
outcharhex:
xchg %al,%ah
mov  UART_BASEADDR+5,%dx
us_waith:
in   %dx,%al
test $0x20,%al
jz us_waith
mov  UART_BASEADDR,%dx
xchg %al,%ah
out  %al,%dx            // output char
dec  %cx
cmp  $0x0,%cx
jne  dhloop
mov  $0x20,%al
mov  $0x10,%cl
jmp  outcharhex
exitdisplayhex:
rol  $0x10,%ecx
jmp  *subroutinereg

displayasciilinear:
mov  resultreg,%eax
xchg %al,%ah
rol  $0x10,%eax
xchg %al,%ah
mov  %eax,resultreg
displayascii:
rol  $0x10,%ecx
mov  $0x4,%cx
daloop:
rol  $0x08,resultreg
movl resultreg,%eax
cmp  $0x7e,%al
jg   unprintable
cmp  $0x20,%al
jl   unprintable
jmp  outcharascii
unprintable:
mov  $0x2e,%al          // dot
outcharascii:
xchg %al,%ah
mov  UART_BASEADDR+5,%dx
us_waita:
in   %dx,%al
test $0x20,%al
jz us_waita
mov  UART_BASEADDR,%dx
xchg %al,%ah
out  %al,%dx            // output char
dec  %cx
cmp  $0x0,%cx
jne  daloop
rol  $0x10,%ecx
jmp  *subroutinereg

rst:
cli
movb $0x0fe,%al
out  %al,$0x64
hlt

RST:
cli
lidt %cs:0x03fff
int  $0x3
hlt


beep:
mov  timertime,%eax
rol  $0x10,%eax
mov  $0xb6,%al
out  %al,$0x43
mov  freqtime,%ax
out  %al,$0x42
xchg %al,%ah
out  %al,$0x42

in   $0x61,%al
or   $0x03,%al
out  %al,$0x61

//timer here
timer:
in   $0x42,%al
// xchg %al,%ah
in   $0x42,%al
// xchg %al,%ah
cmp  $0x0,%al
jnz  timer
rol  $0x10,%eax
dec  %ax
cmp  $0x0,%ax;
rol  $0x10,%eax
jnz  timer
// timer

in   $0x61,%al
and  $0xfc,%al
out  %al,$0x61
jmp  *subroutinereg

dohelp:
mov $cmds,resultreg
mov $readcommand,subroutinereg
jmp displaystring

memt:
movl $memt1, subroutinereg
jmp  readnibbles
memt1:
mov  resultreg,%ecx
movl $memt2, subroutinereg
jmp  readnibbles
memt2:
mov  resultreg,%ebx
xchg %ecx,%eax
mov  $readcommand,%esp   // internal to linux bios
jmp ramtest

pcirb:
movl $pcirb1, subroutinereg
jmp  readnibbles
pcirb1:
mov  resultreg,%eax
PCI_READ_CONFIG_BYTE
and  $0xff,%eax
mov  %eax,resultreg
mov  $readcommand,subroutinereg
jmp  displayhex

pcirw:
movl $pcirw1, subroutinereg
jmp  readnibbles
pcirw1:
mov  resultreg,%eax
PCI_READ_CONFIG_WORD
and  $0xffff,%eax
mov  %eax,resultreg
mov  $readcommand,subroutinereg
jmp  displayhex

pcirl:
movl $pcirl1, subroutinereg
jmp  readnibbles
pcirl1:
mov  resultreg,%eax
PCI_READ_CONFIG_DWORD
mov  %eax,resultreg
mov  $readcommand,subroutinereg
jmp  displayhex




pciwb:
movl $pciwb1, subroutinereg
jmp  readnibbles
pciwb1:
mov  resultreg,%ebx
movl $pciwb2, subroutinereg
jmp  readnibbles
pciwb2:
mov  resultreg,%edx
mov  %ebx,%eax
PCI_WRITE_CONFIG_BYTE
jmp  readcommand

pciww:
movl $pciww1, subroutinereg
jmp  readnibbles
pciww1:
mov  resultreg,%ebx
movl $pciww2, subroutinereg
jmp  readnibbles
pciww2:
mov  resultreg,%ecx
mov  %ebx,%eax
PCI_WRITE_CONFIG_WORD
jmp  readcommand

pciwl:
movl $pciwl1, subroutinereg
jmp  readnibbles
pciwl1:
mov  resultreg,%ebx
movl $pciwl2, subroutinereg
jmp  readnibbles
pciwl2:
mov  resultreg,%ecx
mov  %ebx,%eax
PCI_WRITE_CONFIG_DWORD
jmp  readcommand

cram:
//likely not working.  Just testing for now
movl $cram1, subroutinereg
jmp  readnibbles
cram1:
mov resultreg,%ebx
movl $cram1, subroutinereg
jmp  readnibbles
cram2:
mov resultreg,%ecx
// enable it
mov %cr0,%eax
and $0x9fffffff,%eax  // also try 0x0fff, 0x2ff(write back)...
mov %eax,%cr0
//wbinvd ??
cacheloop:
mov (%ebx),%eax
inc %ebx
loop cacheloop
// disable it
mov %cr0,%eax
or  $0x60000000,%eax
mov %eax,%cr0
//wbinvd ??

dodl:
movl $dl1, subroutinereg
jmp  readnibbles
dl1:
mov  resultreg,%ebx
movl $dl2, subroutinereg
jmp  readnibbles
dl2:
mov  resultreg,%ecx
mov  resultreg,subroutinereg
mov  %ebx,resultreg
dlloop:
mov  UART_BASEADDR+5,%dx
in   %dx, %al
and  $0x9f,%al
test $0x01,%al
jz   dlloop
mov  UART_BASEADDR,%dx
in   %dx,%al
mov  %al,(%ebx)
inc  %ebx
loop dlloop
csum:
mov subroutinereg,%ecx
shr $0x02,%ecx
mov resultreg,%ebx
mov $0x0,%eax
csumloop:
rol $0x03,%eax
mov (%ebx),%dl
xor  %dl,%al
inc  %ebx
loop csumloop
mov $readcommand,subroutinereg
mov %eax,resultreg
jmp displayhex

baud:
mov $sorry,resultreg
mov $readcommand,subroutinereg
jmp displaystring

mcp:
movl $mcp1, subroutinereg
jmp  readnibbles
mcp1:
mov  resultreg,%ebx
movl $mcp2, subroutinereg
jmp  readnibbles
mcp2:
mov  resultreg,%ecx
movl $mcp3, subroutinereg
jmp  readnibbles
mcp3:
mov  resultreg,%eax
xchg %ecx,%eax
mcploop:
mov  (%ebx),%dl
mov  %dl,(%eax)
inc  %ebx
inc  %eax
loop mcploop
jmp  readcommand

dopattern:
movl $pat1, subroutinereg
jmp  readnibbles
pat1:
mov  resultreg,%ebx
movl $pat2, subroutinereg
jmp  readnibbles
pat2:
mov  resultreg,%ecx
movl $pat3, subroutinereg
jmp  readnibbles
pat3:
mov  resultreg,%eax
xchg %ecx,%eax
patloop:
rol $0x08,%ebx
mov %bl,(%eax)
inc %eax
loop patloop
jmp readcommand


doexit:
         // LB specific:
RETSP    // if there's no stack yet, caller must set %esp manually
// RET_LABEL(low_level_shell)


//Linux OS Specific
ioperms:
movl	$sys_IOPL, %eax		# system-call ID-number
movl	$3, %ebx		# new value for IO0PL
int	$0x80			# enter the kernel
ret

llshell_out:
