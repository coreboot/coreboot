;*****************************************************
; $Id: loader.s,v 1.1 2002/11/25 02:07:53 rminnich Exp $
;*****************************************************
USE32
; code it is loaded into memory at 0x7C00
;*****************************************************
nop
nop
;*****************************************************
; A) setup GDT, so that we do not depend on program 
; that loaded us for GDT. 
; Ex: LinuxBIOS and EtherBOOT use different GDT's.

;-----------------------------------------------------
; 0)

cli

;-----------------------------------------------------
; I)

lgdt [0x7C00+protected_gdt]

;-----------------------------------------------------
; II) setup CS

jmp 0x08:0x7C00+newpgdt

newpgdt: nop

;-----------------------------------------------------
; III) setup all other segments

mov ax,  #0x10
mov ss,  ax
mov ds,  ax
mov es,  ax
mov fs,  ax
mov gs,  ax

;-----------------------------------------------------
; IV) 

; not now
;sti

;*****************************************************
nop
nop
;*****************************************************
; B) shadow - ON (enable/read/write)

mov eax, #0x80000070
mov dx,  #0x0cf8
out dx,  eax

mov eax, #0xFFFFFFFF
mov dx,  #0x0cfc
out dx,  eax

;*****************************************************
nop
nop
;*****************************************************
; C) copy -- boch bios

; counter - 64kb.     
mov ecx, #0x10000

; source - 0x8000  ( 0x7C00+0x400 = 0x8000 ) 
mov ax,  #0x10        ; src-segment - 2nd entry in GDT
mov ds,  ax
mov eax, #0x8000      ; src-offset  - 0x8000
mov esi, eax

; destination - 0xE0000
mov ax,  #0x10        ; dst-segment - 2nd entry in GDT
mov es,  ax     
mov eax, #0xF0000     ; dst-offset  - 0xF0000
mov edi, eax

; clear direction flag
cld

; the copy
rep
  movsb

;*****************************************************
nop
nop
;*****************************************************
; X) copy -- LinuxBIOS table into safe place.

	;; TODO.
	;; Q1 :	 what is the size of table.
	;; Q2 :	 where to copy?
		
;*****************************************************
nop
nop	
;*****************************************************
; E) shadow - OFF (write)

mov eax, #0x80000070
mov dx,  #0x0cf8
out dx,  eax

;mov eax, #0xFFFFFFFF
mov eax, #0x0000FFFF
mov dx,  #0x0cfc
out dx,  eax

;*****************************************************
nop
nop
;*****************************************************
; F) do a little prep work.

;-----------------------------------------------------
; I) disable cache

; if you disable cache, GRUB's GFX mode will be VERY slow.
; so DO NOT DISABLE

;mov eax, cr0
;or  eax, #0x60000000
;wbinvd
;mov cr0, eax
;wbinvd

;-----------------------------------------------------
; II) disable MTRR
; clear the "E" (0x800) and "FE" (0x400) flags in 
; IA32_MTRRdefType register (0x2FF)

;-----------------------

;mov ECX,#0x2FF

; select either of the two below 
; depending on if your compiler suports 
; {RD,WR}MSR or not
;rdmsr
; .byte 0x0F, 0x32

;xor edx, edx
; xor eax, eax
;and eax, #0xFFFFF3FF

; select either of the two below 
; depending on if your compiler suports 
; {RD,WR}MSR or not
;wrmsr
; .byte 0x0F, 0x30

;-----------------------
;; This is what PC BIOS is setting. -- P6STMT.
; add VIDEO BIOS cacheable!!!!
;-----------------------
; Fixed Range C0--C8
;mov ECX,#0x268
;mov EDX,#0x05050505 
;mov EAX,#0x05050505 
;wrmsr
;-----------------------
; Fixed Range C8--CF
;mov ECX,#0x269
;mov EDX,#0x0 
;mov EAX,#0x05050505 
;wrmsr
;-----------------------

;-----------------------------------------------------
; III) tell BOCHS' BIOS we want to boot from hdd.
; 0x00 - floppy
; 0x02 - hdd
; In future there will be 'fd failover'option in bochs.

mov  al, #0x3d ;; cmos_reg
out  0x70, al
mov  al, #0x02 ;; val (hdd)
out  0x71, al

;-----------------------------------------------------
; IV) tell BOCHS' BIOS length of our mem block @ 1mb.
;     This is for Int 15 / EAX=E820
;     119mb = 0x77 00 00 00 
;     (this is for 128mb of ram)
;     (FIXME: this value is currently hard coded)
;     (it should be being passed from LinuxBIOS )

; for WinFast  6300
; 07 70 = 0770
; 06 80 = 0770 - 00F0		<< ALT (for unpatched bochs)

; for P6STMT - 10kb less ram
; 077F - 10     = 07 6F 
; 07 6F - 00 F0 = 06 7F

mov  al, #0x35 ;; cmos_reg
out  0x70, al
mov  al, #0x06 ;; val 
out  0x71, al

mov  al, #0x34 ;; cmos_reg
out  0x70, al
mov  al, #0x7F ;; val 
out  0x71, al

mov  al, #0x31 ;; cmos_reg
out  0x70, al
mov  al, #0x00 ;; val 
out  0x71, al

mov  al, #0x30 ;; cmos_reg
out  0x70, al
mov  al, #0x00 ;; val 
out  0x71, al

;-----------------------------------------------------
; V) tell BOCHS' BIOS we want to have LBA translation.
; 0x00 - NONE
; 0x01 - LBA    <<<<
; 0x02 - LARGE
; 0x03 - R-CHS
; In future there will be 'fd failover'option in bochs.

mov  al, #0x39 ;; cmos_reg
out  0x70, al
mov  al, #0x01 ;; val (LBA)
out  0x71, al

;*****************************************************
nop
nop
;*****************************************************
; G) the switch -- protected to real mode

; IASDM, Vol 3
; (8-14) 8.8.2 Switching Back to Real-Address Mode

;=====================================================
; 1) disable interrupts

cli

;=====================================================
nop
;=====================================================
; 2) paging

;not enabled, so not applicable.

;=====================================================
; 3) setup CS segment limit (64kb)
; I)

lgdt [0x7C00+new_gdt]

;-----------------------------------------------------
; II)

jmp 0x08:0x7C00+new64lim

new64lim: nop

;=====================================================
nop
;=====================================================
; 4) setup all other segments

mov ax,  #0x10
mov ss,  ax
mov ds,  ax
mov es,  ax
mov fs,  ax
mov gs,  ax

;=====================================================
nop
;=====================================================
; 5) LIDT
; I)

; set up Real Mode IDT table (0...3FF)

; for BOCH's BIOS the address 0xF000:0xFF53 
; cantains value 0xCF which is IRET opcode.

; counter 
mov cx,  #0xFF ;1024 bytes(255 interrupts)(4*255=0x3FF)

; destination - 0x00000 = ES:EDI
mov ax,  #0x10        ; dst-segment - 2nd entry in GDT
mov es,  ax
mov eax, #0x00000     ; dst-offset  - 0x00000
mov edi, eax

; data to store -- 0xF000:FF53
mov eax, #0xF000FF53

; clear direction flag
cld

; the store 
rep
  stosd

;-----------------------------------------------------
; II)
; load interrupt descriptor table

lidt [0x7C00+new_idt]

;=====================================================
nop
nop
;=====================================================
; 6) clear the PE flag in CR0 register.
; I)

; switch to 16 bit segments
mov ax,  #0x20
mov ss,  ax
mov ds,  ax
mov es,  ax
mov fs,  ax
mov gs,  ax

;-----------------------------------------------------
; II)

; switch to 16 bit CS

jmp 0x018:0x7C00+new16bit

USE16

new16bit: nop

;-----------------------------------------------------
; III)
; the switch

;xor eax, eax

mov eax, cr0            
and eax, #0xFFFFFFFE
mov cr0, eax            ;switch to RM

;=====================================================
nop
nop
;=====================================================
; 7) far jump -- (to real mode address)

jmp 0x0:0x7C00+realcs

realcs: nop

;=====================================================
; 8) set all segment registers to 0's

mov ax,  #0x0
mov ss,  ax
mov ds,  ax
mov es,  ax
mov fs,  ax
mov gs,  ax

;=====================================================
; 9) re-enable interrupts

sti

;*****************************************************
nop
nop
;*****************************************************
; G) jump to BIOS.

jmp 0xFFFF:0x0000
;jmp 0xF000:0xFFF0

;*****************************************************
;*****************************************************
nop
nop
nop
nop
;*****************************************************
;*****************************************************

USE32

new_idt:
dw 0x03ff ;; limit 15:00
dw 0x0000 ;; base  15:00
dw 0x0000 ;; base  23:16

new_gdt:
dw 0x0028                     ;; limit 15:00
dw 0x7C00+new_gdt_table       ;; base  15:00
dw 0x0000                     ;; base  23:16

protected_gdt:
dw 0x0018                     ;; limit 15:00
dw 0x7C00+pmode_gdt_table     ;; base  15:00
dw 0x0000                     ;; base  23:16

;-----------------------------------------------------

new_gdt_table:
;//  1 2 3 4 
;//0
dd 0x00000000
dd 0x00000000

;//8
dd 0x0000ffff
dd 0x00409E00

;//10
dd 0x0000ffff
dd 0x00409200

;//18
dd 0x0000ffff
dd 0x00009a00

;//20
dd 0x0000ffff
dd 0x00009200

;-------------------------

pmode_gdt_table:
;//  1 2 3 4 
;//0
dd 0x00000000
dd 0x00000000

;//8
dd 0x0000ffff
dd 0x00CF9E00

;//10
dd 0x0000ffff
dd 0x00CF9200

;*****************************************************
;*****************************************************
; the file size must be 1024 bytes.


.org 0x400-1
; dd 0xdeadbeef			
db 0x0

;*****************************************************
