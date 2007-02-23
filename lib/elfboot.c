#include <console/console.h>
//#include <part/fallback_boot.h>
#include <elf.h>
#include <elf_boot.h>
#include <linuxbios_tables.h>
#include <ip_checksum.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct segment {
	struct segment *next;
	struct segment *prev;
	struct segment *phdr_next;
	struct segment *phdr_prev;
	unsigned long s_addr;
	unsigned long s_memsz;
	unsigned long s_offset;
	unsigned long s_filesz;
};

struct verify_callback {
	struct verify_callback *next;
	int (*callback)(struct verify_callback *vcb, 
		Elf_ehdr *ehdr, Elf_phdr *phdr, struct segment *head);
	unsigned long desc_offset;
	unsigned long desc_addr;
};

struct ip_checksum_vcb {
	struct verify_callback data;
	unsigned short ip_checksum;
};

/* we're trying to keep out of the way of elf segments, without requiring the
  * bounce buffer. This may fail, but it's worth the effort. 
  */
static unsigned char *arena;
int arenasize;

static void
setupmalloc(void *s, int size)
{
	arena = s;
	arenasize = size;
}

static void *localmalloc(int nbytes)
{
	unsigned char *ret;
	if (nbytes > arenasize)
		return NULL;

	arenasize -= nbytes;
	ret = arena;
	arena += nbytes;
	return ret;
}


/* streams are a nice way to abstract the pointer/size-based nature of the 
  * memory away. The main good part is that we have a way to fail out if the 
  * elfboot code is running off the end of the array for some reason. So we won't
  * rip it out just yet. 
  */
static unsigned char *streambase = NULL;
static int streamsize = -1;

int stream_init(void){
	return 0;
}

void stream_fini(void){
}

int stream_skip(int bytes){
	streamsize -= bytes;
	if (streamsize < 0)
		return -1;
	streambase += bytes;

	return bytes;
}

int stream_read(void *dest, int bytes) {
printk(BIOS_INFO, "read %d bytes %d left\n", bytes, streamsize);
	if (streamsize < bytes)
		return -1;

	memcpy(dest, streambase, bytes);
	return stream_skip(bytes);
}

int verify_ip_checksum(
	struct verify_callback *vcb, 
	Elf_ehdr *ehdr, Elf_phdr *phdr, struct segment *head)
{
	struct ip_checksum_vcb *cb;
	struct segment *ptr;
	unsigned long bytes;
	unsigned long checksum;
	unsigned char buff[2], *n_desc;
	cb = (struct ip_checksum_vcb *)vcb;
	/* zero the checksum so it's value won't
	 * get in the way of verifying the checksum.
	 */
	n_desc = 0;
	if (vcb->desc_addr) {
		n_desc = (unsigned char *)(vcb->desc_addr);
		memcpy(buff, n_desc, 2);
		memset(n_desc, 0, 2);
	}
	bytes = 0;
	checksum = compute_ip_checksum(ehdr, sizeof(*ehdr));
	bytes += sizeof(*ehdr);
	checksum = add_ip_checksums(bytes, checksum, 
		compute_ip_checksum(phdr, ehdr->e_phnum*sizeof(*phdr)));
	bytes += ehdr->e_phnum*sizeof(*phdr);
	for(ptr = head->phdr_next; ptr != head; ptr = ptr->phdr_next) {
		checksum = add_ip_checksums(bytes, checksum,
			compute_ip_checksum((void *)ptr->s_addr, ptr->s_memsz));
		bytes += ptr->s_memsz;
	}
	if (n_desc != 0) {
		memcpy(n_desc, buff, 2);
	}
	if (checksum != cb->ip_checksum) {
		printk(BIOS_ERR, "Image checksum: %04x != computed checksum: %04x\n",
			cb->ip_checksum, checksum);
	}
	return checksum == cb->ip_checksum;
}

static struct verify_callback *process_elf_notes(
	unsigned char *header, 
	unsigned long offset, unsigned long length)
{
	struct verify_callback *cb_chain;
	unsigned char *note, *end;
	char *program, *version;

	cb_chain = 0;
	note = header + offset;
	end = note + length;
	program = version = 0;
	while(note < end) {
		Elf_Nhdr *hdr;
		unsigned char *n_name, *n_desc, *next;
		hdr = (Elf_Nhdr *)note;
		n_name = note + sizeof(*hdr);
		n_desc = n_name + ((hdr->n_namesz + 3) & ~3);
		next = n_desc + ((hdr->n_descsz + 3) & ~3);
		if (next > end) {
			break;
		}
		if ((hdr->n_namesz == sizeof(ELF_NOTE_BOOT)) && 
			(memcmp(n_name, ELF_NOTE_BOOT, sizeof(ELF_NOTE_BOOT)) == 0)) {
			switch(hdr->n_type) {
			case EIN_PROGRAM_NAME:
				if (n_desc[hdr->n_descsz -1] == 0) {
					program = (char *) n_desc;
				}
				break;
			case EIN_PROGRAM_VERSION:
				if (n_desc[hdr->n_descsz -1] == 0) {
					version = (char *) n_desc;
				}
				break;
			case EIN_PROGRAM_CHECKSUM:
			{
				struct ip_checksum_vcb *cb;
				cb = localmalloc(sizeof(*cb));
				cb->ip_checksum = *((uint16_t *)n_desc);
				cb->data.callback = verify_ip_checksum;
				cb->data.next = cb_chain;
				cb->data.desc_offset = n_desc - header;
				cb_chain = &cb->data;
				break;
			}
			}
		}
		printk(BIOS_SPEW, "n_type: %08x n_name(%d): %-*.*s n_desc(%d): %-*.*s\n", 
			hdr->n_type,
			hdr->n_namesz, hdr->n_namesz, hdr->n_namesz, n_name,
			hdr->n_descsz,hdr->n_descsz, hdr->n_descsz, n_desc);
		note = next;
	}
	if (program && version) {
		printk(BIOS_INFO, "Loading %s version: %s\n",
			program, version);
	}
	return cb_chain;
}

static int valid_area(struct lb_memory *mem, 
	unsigned long start, unsigned long len)
{
	/* Check through all of the memory segments and ensure
	 * the segment that was passed in is completely contained
	 * in RAM.
	 */
	int i;
	unsigned long end = start + len;
	unsigned long mem_entries = (mem->size - sizeof(*mem))/sizeof(mem->map[0]);

	/* Walk through the table of valid memory ranges and see if I
	 * have a match.
	 */
	for(i = 0; i < mem_entries; i++) {
		uint64_t mstart, mend;
		uint32_t mtype;
		mtype = mem->map[i].type;
		mstart = unpack_lb64(mem->map[i].start);
		mend = mstart + unpack_lb64(mem->map[i].size);
		if ((mtype == LB_MEM_RAM) && (start < mend) && (end > mstart)) {
			break;
		}
	}
	if (i == mem_entries) {
		printk(BIOS_ERR, "No matching ram area found for range:\n");
		printk(BIOS_ERR, "  [0x%016lx, 0x%016lx)\n", start, end);
		printk(BIOS_ERR, "Ram areas\n");
		for(i = 0; i < mem_entries; i++) {
			uint64_t mstart, mend;
			uint32_t mtype;
			mtype = mem->map[i].type;
			mstart = unpack_lb64(mem->map[i].start);
			mend = mstart + unpack_lb64(mem->map[i].size);
			printk(BIOS_ERR, "  [0x%016lx, 0x%016lx) %s\n",
				(unsigned long)mstart, 
				(unsigned long)mend, 
				(mtype == LB_MEM_RAM)?"RAM":"Reserved");
			
		}
		return 0;
	}
	return 1;
}

static int build_elf_segment_list(
	struct segment *head, struct lb_memory *mem,
	Elf_phdr *phdr, int headers)
{
	struct segment *ptr;
	int i;
	memset(head, 0, sizeof(*head));
	head->phdr_next = head->phdr_prev = head;
	head->next = head->prev = head;
	for(i = 0; i < headers; i++) {
		struct segment *new;
		/* Ignore data that I don't need to handle */
		if (phdr[i].p_type != PT_LOAD) {
			printk(BIOS_DEBUG, "Dropping non PT_LOAD segment\n");
			continue;
		}
		if (phdr[i].p_memsz == 0) {
			printk(BIOS_DEBUG, "Dropping empty segment\n");
			continue;
		}
		new = localmalloc(sizeof(*new));
		new->s_addr = phdr[i].p_paddr;
		new->s_memsz = phdr[i].p_memsz;
		new->s_offset = phdr[i].p_offset;
		new->s_filesz = phdr[i].p_filesz;
		printk(BIOS_DEBUG, "New segment addr 0x%lx size 0x%lx offset 0x%lx filesize 0x%lx\n",
			new->s_addr, new->s_memsz, new->s_offset, new->s_filesz);
		/* Clean up the values */
		if (new->s_filesz > new->s_memsz)  {
			new->s_filesz = new->s_memsz;
		}
		printk(BIOS_DEBUG, "(cleaned up) New segment addr 0x%lx size 0x%lx offset 0x%lx filesize 0x%lx\n",
			new->s_addr, new->s_memsz, new->s_offset, new->s_filesz);
		for(ptr = head->next; ptr != head; ptr = ptr->next) {
			if (new->s_offset < ptr->s_offset)
				break;
		}
		/* Order by stream offset */
		new->next = ptr;
		new->prev = ptr->prev;
		ptr->prev->next = new;
		ptr->prev = new;
		/* Order by original program header order */
		new->phdr_next = head;
		new->phdr_prev = head->phdr_prev;
		head->phdr_prev->phdr_next  = new;
		head->phdr_prev = new;

		/* Verify the memory addresses in the segment are valid */
		if (!valid_area(mem, new->s_addr, new->s_memsz)) 
			goto out;
	}
	return 1;
 out:
	return 0;
}

static int load_elf_segments(
	struct segment *head, unsigned char *header, unsigned long header_size)
{
	unsigned long offset;
	struct segment *ptr;
	
	offset = 0;
	for(ptr = head->next; ptr != head; ptr = ptr->next) {
		unsigned long start_offset;
		unsigned long skip_bytes, read_bytes;
		unsigned char *dest, *middle, *end;
		unsigned long result;
		printk(BIOS_DEBUG, "Loading Segment: addr: 0x%016lx memsz: 0x%016lx filesz: 0x%016lx\n",
			ptr->s_addr, ptr->s_memsz, ptr->s_filesz);
		
		/* Compute the boundaries of the segment */
		dest = (unsigned char *)(ptr->s_addr);
		end = dest + ptr->s_memsz;
		middle = dest + ptr->s_filesz;
		start_offset = ptr->s_offset;
		/* Ignore s_offset if I have a pure bss segment */
		if (ptr->s_filesz == 0) {
			start_offset = offset;
		}
		
		printk(BIOS_SPEW, "[ 0x%016lx, %016lx, 0x%016lx) <- %016lx\n",
			(unsigned long)dest,
			(unsigned long)middle,
			(unsigned long)end,
			(unsigned long)start_offset);
		
		/* Skip intial buffer unused bytes */
		if (offset < header_size) {
			if (start_offset < header_size) {
				offset = start_offset;
			} else {
				offset = header_size;
			}
		}
		
		/* Skip the unused bytes */
		skip_bytes = start_offset - offset;
		if (skip_bytes && 
			((result = stream_skip(skip_bytes)) != skip_bytes)) {
			printk(BIOS_ERR, "ERROR: Skip of %ld bytes skipped %ld bytes\n",
				skip_bytes, result);
			goto out;
		}
		offset = start_offset;
		
		/* Copy data from the initial buffer */
		if (offset < header_size) {
			size_t len;
			if ((ptr->s_filesz + start_offset) > header_size) {
				len = header_size - start_offset;
			}
			else {
				len = ptr->s_filesz;
			}
			memcpy(dest, &header[start_offset], len);
			dest += len;
		}
		
		/* Read the segment into memory */
		read_bytes = middle - dest;
		if (read_bytes && 
			((result = stream_read(dest, read_bytes)) != read_bytes)) {
			printk(BIOS_ERR, "ERROR: Read of %ld bytes read %ld bytes...\n",
				read_bytes, result);
			goto out;
		}
		offset += ptr->s_filesz;
		
		/* Zero the extra bytes between middle & end */
		if (middle < end) {
			printk(BIOS_DEBUG, "Clearing Segment: addr: 0x%016lx memsz: 0x%016lx\n",
				(unsigned long)middle, end - middle);
			
			/* Zero the extra bytes */
			memset(middle, 0, end - middle);
		}
	}
	return 1;
 out:
	return 0;
}

static int verify_loaded_image(
	struct verify_callback *vcb,
	Elf_ehdr *ehdr, Elf_phdr *phdr,
	struct segment *head
	)
{
	struct segment *ptr;
	int ok;
	ok = 1;
	for(; ok && vcb ; vcb = vcb->next) {
		/* Find where the note is loaded */
		/* The whole note must be loaded intact
		 * so an address of 0 for the descriptor is impossible
		 */
		vcb->desc_addr = 0; 
		for(ptr = head->next; ptr != head; ptr = ptr->next) {
			unsigned long desc_addr;
			desc_addr = ptr->s_addr + vcb->desc_offset - ptr->s_offset;
			if ((desc_addr >= ptr->s_addr) &&
				(desc_addr < (ptr->s_addr + ptr->s_filesz))) {
				vcb->desc_addr = desc_addr;
			}
		}
		ok = vcb->callback(vcb, ehdr, phdr, head);
	}
	return ok;
}

int elfload(struct lb_memory *mem,
	unsigned char *header, unsigned long header_size)
{
	Elf_ehdr *ehdr;
	Elf_phdr *phdr;
	void *entry;
	void (*v)(void);
	struct segment head;
	struct verify_callback *cb_chain;

	ehdr = (Elf_ehdr *)header;
	entry = (void *)(ehdr->e_entry);
	phdr = (Elf_phdr *)(&header[ehdr->e_phoff]);

	/* Digest elf note information... */
	cb_chain = 0;
	if ((phdr[0].p_type == PT_NOTE) && 
		((phdr[0].p_offset + phdr[0].p_filesz) < header_size)) {
		cb_chain = process_elf_notes(header,
			phdr[0].p_offset, phdr[0].p_filesz);
	}

	/* Preprocess the elf segments */
	if (!build_elf_segment_list(&head, mem, phdr, ehdr->e_phnum))
		goto out;

	/* Load the segments */
	if (!load_elf_segments(&head, header, header_size))
		goto out;

	printk(BIOS_SPEW, "Loaded segments\n");
	/* Verify the loaded image */
	if (!verify_loaded_image(cb_chain, ehdr, phdr, &head)) 
		goto out;

	printk(BIOS_SPEW, "verified segments\n");
	/* Shutdown the stream device */
	stream_fini();
	
	printk(BIOS_SPEW, "closed down stream\n");
	/* Reset to booting from this image as late as possible */
	/* what the hell is boot_successful? */
	//boot_successful();

	printk(BIOS_DEBUG, "Jumping to boot code at 0x%x\n", entry);
	post_code(0xfe);

	/* Jump to kernel */
	/* just call it as a function. If it wants to return, it will. */
	v = entry;
	v();
	return 1;

 out:
	return 0;
}

int elfboot(struct lb_memory *mem)
{
	Elf_ehdr *ehdr;
	unsigned char header[ELF_HEAD_SIZE];
	int header_offset;
	int i, result;
	/* for stupid allocator which won't run into trouble with segments */
	char alloc[256];

	setupmalloc(alloc, sizeof(alloc));
	result = 0;
	printk(BIOS_INFO, "\n");
	printk(BIOS_INFO, "Elfboot\n");
	post_code(0xf8);

	if (stream_init() < 0) {
		printk(BIOS_ERR, "Could not initialize driver...\n");
		goto out;
	}

	/* Read in the initial ELF_HEAD_SIZE bytes */
	if (stream_read(header, ELF_HEAD_SIZE) != ELF_HEAD_SIZE) {
		printk(BIOS_ERR, "Header read failed...\n");
		goto out;
	}
	/* Scan for an elf header */
	header_offset = -1;
	for(i = 0; i < ELF_HEAD_SIZE - (sizeof(Elf_ehdr) + sizeof(Elf_phdr)); i+=16) {
		ehdr = (Elf_ehdr *)(&header[i]);
		if (memcmp(ehdr->e_ident, ELFMAG, 4) != 0) {
			printk(BIOS_SPEW, "NO header at %d\n", i);
			continue;
		}
		printk(BIOS_DEBUG, "Found ELF candidate at offset %d\n", i);
		/* Sanity check the elf header */
		if ((ehdr->e_type == ET_EXEC) &&
			elf_check_arch(ehdr) &&
			(ehdr->e_ident[EI_VERSION] == EV_CURRENT) &&
			(ehdr->e_version == EV_CURRENT) &&
			(ehdr->e_ehsize == sizeof(Elf_ehdr)) &&
			(ehdr->e_phentsize = sizeof(Elf_phdr)) &&
			(ehdr->e_phoff < (ELF_HEAD_SIZE - i)) &&
			((ehdr->e_phoff + (ehdr->e_phentsize * ehdr->e_phnum)) <= 
				(ELF_HEAD_SIZE - i))) {
			header_offset = i;
			break;
		}
		ehdr = 0;
	}
	printk(BIOS_SPEW, "header_offset is %d\n", header_offset);
	if (header_offset == -1) {
		goto out;
	}

	printk(BIOS_SPEW, "Try to load at offset 0x%x\n", header_offset);
	result = elfload(mem, 
		header + header_offset , ELF_HEAD_SIZE - header_offset);
 out:
	if (!result) {
		/* Shutdown the stream device */
		stream_fini();

		printk(BIOS_ERR, "Cannot Load ELF Image\n");

		post_code(0xff);
	}
	return 0;

}

int elfboot_mem(struct lb_memory *mem, void *where, int size)
{
	streambase = where;
	streamsize = size;
printk(BIOS_INFO, "base %p, size %d\n", streambase, streamsize);
	return elfboot(mem);
}	
