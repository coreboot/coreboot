#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>

#include <string.h>

#ifndef ZKERNEL_START
#define ZKERNEL_START 0xfff00000
#endif

#ifndef ZKERNEL_MASK
#define ZKERNEL_MASK 0x0000ffff
#endif

#ifndef _ROMTOP
#define _ROMTOP 0xfffffff0
#endif

/* The inbuf copy option has been killed... */

#define K64 (64 * 1024)

static unsigned char *zkernel_start = (unsigned char *)ZKERNEL_START;
static unsigned long zkernel_mask = ZKERNEL_MASK;

static unsigned char *nvram;
static int block_count;
static int block_offset;

static unsigned int max_block = (_ROMTOP - ZKERNEL_START)/K64;


static int init_bytes(void);

int rom_seek(unsigned long int where)
{
	block_count = where / K64;
	block_offset = where - (block_count * K64);
	nvram = zkernel_start + (K64 * block_count);

	return(where);
}

static void fini_bytes(void)
{
	return;
}

static byte_offset_t rom_read_bytes(int cp, void *vdest, byte_offset_t count)
{
	unsigned char *dest = vdest;
	byte_offset_t bytes = 0;
	while (bytes < count) {
		int length;
		if (block_offset == K64) {
			block_offset = 0;
			block_count++;
			nvram+= K64;
		}
		if (block_count > max_block) {
			printk_emerg( "%6d:%s() - overflowed source buffer. max_block = %u\n",
				__LINE__, __FUNCTION__, max_block);
			return bytes;
		}
		length = K64 - block_offset;
		if (length > (count - bytes)) {
			length = count - bytes;
		}
		if (cp) {
			memcpy(dest, nvram + block_offset, length);
		}
		dest += length;
		block_offset += length;
		bytes += length;
	}
	return bytes;
}

static byte_offset_t skip_bytes(byte_offset_t count)
{
	return rom_read_bytes(0, 0, count);
}

static byte_offset_t read_bytes(void *vdest, byte_offset_t count)
{
	return rom_read_bytes(1, vdest, count);
}

static int tag_count=0;
static int tag_blocks=0;
unsigned long tags[32];

void rom_load_tag(int tag) 
{
	tag_head current;
	int i;

	printk_info("Loading tag %u @ %08x\n", tag, tags[tag]);

	if(tag < tag_count) {
		rom_seek( tags[tag] );
		read_bytes(&current, sizeof(tag_head) );

		printk_info("Tag contents: ");
		for(i=0; i<16; i++)
			printk_info("%02x ", ( (char *) &current)[i] );

		printk_info("\n");

		printk_info("TAG block count = %u\n", current.block_count);

		skip_bytes( current.block_count * sizeof(tag_head) );
	}

}

int strncmp(char *subject, char *target, int count)
{
	int i;

	for(i=0; i<count; i++)
		if(subject[i] != target[i])
			break;

	if ( i == count)
		return(0);

	return(subject[i] - target[i]);

}

void rom_get_tags(void *buffer)
{
	int i;
	tag_head *ptr = buffer;
	int count;

	for(i=0; i<tag_count; i++) {
		rom_seek(tags[i]);
		read_bytes(ptr, sizeof(tag_head));
//		printk_info("copying tag at %08x, with block_count=%u to %08x\n", tags[i], ptr->block_count, (int) ptr);
		count = ptr->block_count;
		ptr++;
		if(count) {
			read_bytes(ptr, sizeof(tag_head) * count);
			ptr += count;
		}
	}
}


int search_tags(void)
{
	tag_head current;


	rom_seek(0);
	tag_count=0;
	tag_blocks=0;

	printk_info("Searching for %u byte tags\n", sizeof(tag_head) );

	if(!tag_count) {
		while( read_bytes(&current, sizeof(tag_head)) == sizeof(tag_head) ) {
			if(strncmp(current.signature, "$TAG$", 5))
				continue;

			// we found a tag, now look at it
			tag_blocks++;
			tags[tag_count++] = K64 * block_count + block_offset - sizeof(tag_head);

			printk_info("Found tag at %08x block_count = %u\n", tags[tag_count-1], current.block_count);

			if(current.block_count >0) {
				skip_bytes( current.block_count * sizeof(tag_head) );
				tag_blocks += current.block_count;
			}

		}
	}

	if(tag_count) {
		rom_load_tag(0);
	} else 
		rom_seek(0);

	return(tag_blocks);
}

static int init_bytes(void)
{
	int i;

	block_count = 0;
	block_offset = 0;
	nvram = zkernel_start;

	printk_debug("%6d:%s() - zkernel_start:0x%08x  "
		"zkernel_mask:0x%08x\n",
		__LINE__, __FUNCTION__,
		zkernel_start, zkernel_mask);

	i = search_tags();

	printk_info("init_bytes found %u tags\n", i);

	return 0;
}

static struct stream rom_stream __stream = {
	.init = init_bytes,
	.init_tags = search_tags,
	.get_tags = rom_get_tags,
	.load_tag = rom_load_tag,
	.read = read_bytes,
	.skip = skip_bytes,
	.fini = fini_bytes,
};
