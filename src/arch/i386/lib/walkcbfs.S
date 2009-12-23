#define CBFS_HEADER_PTR 0xfffffffc

#define CBFS_HEADER_MAGIC 0
#define CBFS_HEADER_VERSION (CBFS_HEADER_MAGIC + 4)
#define CBFS_HEADER_ROMSIZE (CBFS_HEADER_VERSION + 4)
#define CBFS_HEADER_BOOTBLOCKSIZE (CBFS_HEADER_ROMSIZE + 4)
#define CBFS_HEADER_ALIGN (CBFS_HEADER_BOOTBLOCKSIZE + 4)
#define CBFS_HEADER_OFFSET (CBFS_HEADER_ALIGN + 4)

#define CBFS_FILE_MAGIC 0
#define CBFS_FILE_LEN (CBFS_FILE_MAGIC + 8)
#define CBFS_FILE_TYPE (CBFS_FILE_LEN + 4)
#define CBFS_FILE_CHECKSUM (CBFS_FILE_TYPE + 4)
#define CBFS_FILE_OFFSET (CBFS_FILE_CHECKSUM + 4)

#define CBFS_FILE_STRUCTSIZE (CBFS_FILE_OFFSET + 4)

#define CBFS_STAGE_COMPRESSION 0
#define CBFS_STAGE_ENTRY (CBFS_STAGE_COMPRESSION + 4)
#define CBFS_STAGE_LOAD (CBFS_STAGE_ENTRY + 8)
#define CBFS_STAGE_LEN (CBFS_STAGE_LOAD + 8)
#define CBFS_STAGE_MEMLEN (CBFS_STAGE_LEN + 4)

/*
  input %esi: filename
  input %esp: return address (not pointer to return address!)
  output %eax: entry point
  clobbers %ebx, %ecx, %edx, %edi, %ebp
*/
walkcbfs:
	mov %esi, %ebp /* stash away filename pointer */
	mov $0, %edx
1:
	cmpb $0, (%edx,%esi)
	jz 2f
	add $1, %edx
	jmp 1b
2:
	add $1, %edx
	mov CBFS_HEADER_PTR, %eax
	mov CBFS_HEADER_ROMSIZE(%eax), %ecx
	bswap %ecx
	mov $0, %ebx
	sub %ecx, %ebx
	mov CBFS_HEADER_OFFSET(%eax), %ecx
	bswap %ecx
	add %ecx, %ebx
	mov CBFS_HEADER_ALIGN(%eax), %eax
	bswap %eax
	sub $1, %eax

walker:
	mov %ebp, %esi
	mov %ebx, %edi
	add $CBFS_FILE_STRUCTSIZE, %edi /* edi = address of first byte after struct cbfs_file */
	mov %edx, %ecx
	repe cmpsb
	# zero flag set if strings are equal
	jnz tryharder

	# we found it!
	mov CBFS_FILE_OFFSET(%ebx), %eax
	bswap %eax
	add %ebx, %eax
	add $CBFS_STAGE_ENTRY, %eax /* eax = ((cbfs_stage* (cbfs_file* ebx)->offset)->entry) */
	mov 0(%eax), %eax
	jmp *%esp

tryharder:
	mov CBFS_FILE_OFFSET(%ebx), %ecx
	bswap %ecx
	add %ebx, %ecx
	mov CBFS_FILE_LEN(%ebx), %edi
	bswap %edi
	add %edi, %ecx
	add %eax, %ecx
	mov %eax, %edi
	not %edi
	and %edi, %ecx
	mov %ecx, %ebx

	/* look if we should exit */
	mov CBFS_HEADER_PTR, %esi
	mov CBFS_HEADER_ROMSIZE(%esi), %ecx
	bswap %ecx
	not %ecx
	add $1, %ecx

	cmp %ebx, %ecx
	/* if we're still inside the ROM area, jump back */
	jbe walker

	mov $0, %eax
	jmp *%esp
