/************************************************************************
 *
 * bootselect.c
 *	A standalone rom image selector for LinuxBIOS
 *
 * Copyright 2002 Steven James <pyro@linuxlabs.com> and LinuxLabs
 * http://www.linuxlabs.com
 *
 * Licensed under the GNU General Public License (GPL) v2 or later
 *
 **********************************************************************/

#include <rom/read_bytes.h>
#include <string.h>
#include <stdlib.h>
#include <printk.h>
#include <serial_subr.h>


int choose_stream(struct stream *stream)
{
	int stream_count, i,j;
	tag_head *directory, *current;

	if(stream->init() < 0) {
		printk_err("could not init stream. This is bad!\n");
		return(-1);
	}

	if(!stream->init_tags)
		return(0);

	stream_count = stream->init_tags();

	if(!stream_count)
		return(0);

	current = directory = malloc(stream_count * sizeof(tag_head));

	printk_info("Stream count = %u blocks\n", stream_count);
	stream->get_tags(directory);

	j=0;
	for(i=stream_count; i>0;) {
		current->data[5 + current->block_count*16] = 0;
//		printk_info("TAG: block_count = %u, current = %08x\n", current->block_count, (unsigned int ) current);
		printk_info("STREAM: %u. %s\n", j++, current->data);
		i -= current->block_count;
		i--;
		current += current->block_count+1;
	}

	j=1;
	if(iskey()) {
		j = ttys0_rx_char();
		j=-1;

		while(j<0) {
			printk_info("Choose a stream: ");
			j = ttys0_rx_char();
			if(j == 0x1b || j == 'x')
				return(-1);

			if(j <0x30 || j> 0x39) {
				printk_info(" INVALID RESPONSE\n");
				j=-1;
			} else
				j-=0x30;
		}
	}


	stream->load_tag(j);

	return(0);
}
