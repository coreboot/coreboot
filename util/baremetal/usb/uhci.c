/*******************************************************************************
 *
 *
 *	Copyright 2003 Steven James <pyro@linuxlabs.com> and
 *	LinuxLabs http://www.linuxlabs.com
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ******************************************************************************/

#ifdef __KERNEL__

#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/delay.h>

#endif


#include <asm/io.h>
#include "uhci.h"
#include "debug.h"
#include "pci_util.h"

#include <stddef.h>
#include <asm/errno.h>

int usec_offset=0;

int dump_count=0;

int wait_head( queue_head_t *head, int count)
{
	td_t *td;


	while(!head->depth.terminate) {
		td = MEM_ADDR(head->depth.link);
		if(!td->active)
			return(-1);	// queue failed

		if(count)
			if(! --count)
				return(0);	// still active
				
		udelay(500);	// give it some time
	}

	return(1);	// success
}

queue_head_t *free_qh;
queue_head_t _queue_heads[MAX_QUEUEHEAD];
queue_head_t *queue_heads = _queue_heads;

queue_head_t *new_queue_head(void)
{
	queue_head_t *qh;

	if(!free_qh)
		return(NULL);

	qh = free_qh;
	free_qh = MEM_ADDR(qh->bredth.link);

	memset(qh,0,sizeof(queue_head_t));
	qh->bredth.terminate = qh->depth.terminate=1;

	return(qh);
}

void free_queue_head( queue_head_t *qh)
{

	qh->bredth.link = LINK_ADDR(free_qh);
	if(!free_qh)
		qh->bredth.terminate=1;

	qh->depth.terminate=1;
	free_qh = qh;
}

void init_qh(void)
{
	int i;
#ifdef __KERNEL__
	dma_addr_t *dma_handle;

	queue_heads = pci_alloc_consistent(NULL, MAX_QUEUEHEAD * sizeof(queue_head_t), &dma_handle);
#endif

	for(i=0; i<MAX_QUEUEHEAD-1; i++) {
		memset(queue_heads+i, 0, sizeof(queue_head_t));
		queue_heads[i].bredth.link = LINK_ADDR( &queue_heads[i+1] );
		queue_heads[i].depth.terminate=1;
	}

	queue_heads[MAX_QUEUEHEAD-1].depth.terminate=1;
	queue_heads[MAX_QUEUEHEAD-1].bredth.terminate=1;

	free_qh = queue_heads;
}

td_t *free_td_list;
td_t _tds[MAX_TD];
td_t *tds = _tds;	// indirection added for kernel testing


void init_td(void)
{
	int i;

#ifdef __KERNEL__
	dma_addr_t dma_handle;

	tds = pci_alloc_consistent(NULL, MAX_TD * sizeof(td_t), &dma_handle);
#endif

	for(i=0; i<MAX_TD-1; i++) {
		memset(tds+i, 0, sizeof(td_t));
		tds[i].link.link = LINK_ADDR( &tds[i+1]);
	}

	memset( &tds[MAX_TD-1], 0, sizeof(td_t));
	tds[MAX_TD-1].link.terminate=1;

	free_td_list = tds;
}

td_t *new_td(void)
{
	td_t *td;

	if(!free_td_list)
		return(NULL);

//	DPRINTF("new_td: free_td = %p\n", free_td_list);
	td = free_td_list;

	free_td_list = MEM_ADDR( td->link.link);
//	DPRINTF("new_td: free_td_list = %p\n", free_td_list);

	memset(td, 0, sizeof(td_t));
	td->link.terminate=1;

//	DPRINTF("new_td: returning %p\n", td);
	return(td);
}

td_t *find_last_td(td_t *td)
{
	td_t *last;

	last = td;

	while(!last->link.terminate)
		last = MEM_ADDR(last->link.link);

	return(last);
}

void free_td( td_t *td)
{
	td_t *last_td;

	last_td = find_last_td(td);

	last_td->link.link = LINK_ADDR(free_td_list);
	if(!free_td_list) 
		last_td->link.terminate=1;
	else
		last_td->link.terminate=0;

	free_td_list = td;
	
}

link_pointer_t *queue_end( queue_head_t *queue)
{
	link_pointer_t *link;

	link = &(queue->depth);

	while(!link->terminate)
		link = MEM_ADDR(link->link);

	return(link);
}

void add_td( queue_head_t *head, td_t *td)
{
	link_pointer_t *link;

	link = queue_end(head);

	link->link = LINK_ADDR(td);
	link->terminate=0;
}

transaction_t transactions[MAX_TRANSACTIONS];
transaction_t *free_transactions;

void init_transactions(void)
{
	int i;

	memset(transactions, 0, sizeof(transactions));

	for(i=0; i<MAX_TRANSACTIONS-1; i++)
		transactions[i].next = &transactions[i+1];

	free_transactions = transactions;
}

void free_transaction( transaction_t *trans )
{
	transaction_t *my_current, *last;

	my_current = trans;
	
	while(my_current) {
		free_td( my_current->td_list );
		free_queue_head( my_current->qh );

		last = my_current;
		my_current = my_current->next;
	}

	last->next = free_transactions;
	free_transactions = trans;
}

transaction_t *new_transaction(td_t *td)
{
	transaction_t *trans = free_transactions;
	queue_head_t *qh;

	if(!trans) {
		DPRINTF("new_transaction( td = %p) failed!\n", td);
		return(NULL);
	}

	free_transactions = trans->next;

	memset(trans, 0, sizeof(transaction_t));

	if(td) {
		qh = new_queue_head();
		if(!qh) {
			free_transaction(trans);
			return(NULL);
		}

		trans->qh = qh;
		trans->td_list = td;
		qh->depth.link = LINK_ADDR(td);
		qh->depth.terminate = 0;
		qh->bredth.terminate=1;
	}

	return(trans);
}

transaction_t *add_transaction( transaction_t *trans, td_t *td)
{
	transaction_t *t1;


	t1 = new_transaction(td);
	if(!t1)
		return(NULL);

	trans->next = t1;
	trans->qh->bredth.terminate=0;
	trans->qh->bredth.link = LINK_ADDR(t1->qh);
	trans->qh->bredth.queue=1;

	return(trans);
}

#define MAX_CONTROLLERS 4

link_pointer_t *frame_list[MAX_CONTROLLERS];
uchar fl_buffer[MAX_CONTROLLERS][8192];

//link_pointer_t frame_list[1024];

void init_framelist(uchar dev)
{
	int i;
#ifdef __KERNEL__
	dma_addr_t *dma_handle;

	frame_list[dev] = pci_alloc_consistent(NULL, 1024 * sizeof(link_pointer_t ), &dma_handle);
	memset(frame_list[dev], 0, 1024 * sizeof(link_pointer_t));
#else
	memset(fl_buffer[dev], 0, 4096);
	DPRINTF("raw frame_list is at %p\n", fl_buffer[dev]);
	frame_list[dev] = (link_pointer_t *) (((unsigned int)fl_buffer[dev] & ~0xfff) + 0x1000);
#endif

	DPRINTF("frame_list is at %p\n", frame_list[dev]);

	for(i=0;i<1024;i++)
		frame_list[dev][i].terminate=1;

}


int num_controllers=0;

unsigned int hc_base[MAX_CONTROLLERS];

#define USBCMD(x) hc_base[x]
#define USBSTS(x) (hc_base[x] + 0x02)
#define USBINTR(x) (hc_base[x] + 0x04)
#define FRNUM(x) ( hc_base[x] + 0x06)
#define FLBASE(x) ( hc_base[x] + 0x08)
#define SOFMOD(x) ( hc_base[x] + 0x0c)
#define PORTSC1(x) ( hc_base[x] + 0x10)
#define PORTSC2(x) ( hc_base[x] + 0x12)

#define USBCMDRUN 0x01
#define USBCMD_DEBUG 0x20

#define USBSTSHALTED 0x20

void hc_clear_stat()
{
	unsigned short value;

	value = inw(USBSTS(0));
	outw(value, USBSTS(0));
}
	
void clear_port_stat(unsigned short port)
{
	unsigned short value;

	value = inw(port);
	outw(value, port);
}

void port_suspend( unsigned short port)
{
	unsigned short value;

	value = inw(port);
	value |= 0x1000;
	outw( value, port);
dump_uhci(0x38c0);

}

void port_wakeup( unsigned short port)
{
	unsigned short value;

	value = inw(port);
	value &= ~0x1000;
	outw( value, port);

}

void port_resume( unsigned short port)
{
	unsigned short value;

	value = inw(port);
	value |= 0x40;
	outw(value, port);
	udelay(20000+usec_offset);
	value &= ~0x40;
	outw(value, port);

	do {
		value = inw(port);
	} while(value & 0x40);
}

void port_enable( unsigned short port)
{
	unsigned short value;

	value = inw(port);
	value |= 0x04;
	outw( value, port);

	do {
		value = inw(port);
	} while( !(value & 0x04) && (value & 0x01));

}

void port_disable( unsigned short port)
{
	unsigned short value;

	value = inw(port);
	value &= ~0x04;
	outw( value, port);
}

void port_reset(unsigned short port)
{
	unsigned short value;
	int i;

	value = inw(port);
	value |= 0x200;

	outw( value, port);

	for(i=0;i<5;i++)
		udelay(10000+usec_offset);

	value &= ~0x200;
	outw( value, port);

//	DPRINTF("Port %04x reset\n", port);
//	dump_uhci(0x38c0);
}

void port_reset_long(unsigned short port)
{
	unsigned short value;
	int i;

	value = inw(port);
	value |= 0x200;
	outw( value, port);

	for(i=0; i<20; i++)
		udelay(10000);

	value &= ~0x200;
	outw( value, port);

//	DPRINTF("Port %04x reset\n", port);
//	dump_uhci(0x38c0);
}

void hc_reset(uchar controller)
{
	DPRINTF("Resetting UHCI\n");
	outw(0x04, USBCMD(controller));
	udelay(20000);
	outw(0, USBCMD(controller));
}

int hc_stop(void)
{
	unsigned short tmp;

	tmp = inw(USBCMD(0));
	tmp &= ~USBCMDRUN;
	outw( tmp, USBCMD(0));

	while(! (inw(USBSTS(0)) & USBSTSHALTED) );
	outw( USBSTSHALTED, USBSTS(0));	// clear the status

	return(0);
}

int hc_start(uchar dev) {
	unsigned short tmp;

	DPRINTF("Starting UHCI\n");

	tmp = inw(USBCMD(dev));
	tmp |= USBCMDRUN;

//	tmp |= USBCMD_DEBUG;
	outw( tmp, USBCMD(dev));

	return(0);
}
	

int hc_init(uchar dev, uchar function)
{
	unsigned short cmd;

//	hc_base = pci_read_config_word(PCI_ADDR(0, 0x1d, function, 0x20));
	hc_base[num_controllers] = pci_read_config_word(PCI_ADDR(0, dev, function, 0x20));
	hc_base[num_controllers] &= ~1;

	DPRINTF("Found UHCI at %04x\n", hc_base[num_controllers]);
	hc_reset(num_controllers);

	// set master
	cmd = pci_read_config_word( PCI_ADDR(0, dev, function, 0x04));
	cmd |= 0x04;
	pci_write_config_word( PCI_ADDR(0, dev, function, 0x04), cmd);

	if( (unsigned int) frame_list[dev] != ( ( (unsigned int)frame_list[dev]) & ~0x7ff) ) {
		DPRINTF("UHCI: grave error, misaligned framelist (%p)\n", frame_list[dev]);
		return(-1);
	}

	DPRINTF("hc_init setting framelist to: %08x\n", (unsigned int) virt_to_bus( (frame_list[num_controllers]) ));
	outl( (unsigned int) virt_to_bus(frame_list[num_controllers]), FLBASE(num_controllers));
	outw( 0, FRNUM(num_controllers));
	outw( 0, USBINTR(num_controllers));	// no interrupts!

	outw(0x1000, PORTSC1(num_controllers));
	outw(0x1000, PORTSC2(num_controllers));

	hc_start(num_controllers);

	dump_uhci(hc_base[num_controllers]);

	num_controllers++;
	return(0);
}

queue_head_t *sched_queue[MAX_CONTROLLERS];
queue_head_t *term_qh[MAX_CONTROLLERS];
td_t *dummy_td[MAX_CONTROLLERS];
td_t *loop_td[MAX_CONTROLLERS];

void init_sched(uchar dev)
{
	int i;

	dummy_td[dev] = new_td();
	loop_td[dev] = new_td();
	term_qh[dev] = new_queue_head();

	sched_queue[dev] = new_queue_head();
	sched_queue[dev]->bredth.terminate=0;
	sched_queue[dev]->bredth.queue=1;
	sched_queue[dev]->bredth.link=LINK_ADDR(term_qh[dev]);
	sched_queue[dev]->depth.terminate=1;

	term_qh[dev]->bredth.terminate=1;
	term_qh[dev]->depth.link = LINK_ADDR(loop_td[dev]);
	term_qh[dev]->depth.terminate=0;

//	dummy_td->link.link = LINK_ADDR(sched_queue);
//	dummy_td->link.queue = 1;
//	dummy_td->link.depth=1;
//	dummy_td->link.terminate=0;
//	dummy_td->packet_type = IN_TOKEN;
//	dummy_td->max_transfer = 0x7;
//	dummy_td->isochronous=1;
//	dummy_td->active=1;
//	dummy_td->device_addr = 0x7f;
//	dummy_td->endpoint=0x01;
//	dummy_td->buffer = virt_to_bus(&dummy_td->data[2]);
//	dummy_td->retrys=3;

//dump_hex( (uchar *) dummy_td, sizeof(td_t), "dummy_td ");

	loop_td[dev]->link.link = LINK_ADDR(loop_td[dev]);
	loop_td[dev]->link.terminate=0;
	loop_td[dev]->link.queue=0;
	loop_td[dev]->packet_type = IN_TOKEN;
	loop_td[dev]->max_transfer=7;
	loop_td[dev]->retrys=0;
	loop_td[dev]->device_addr=0x7f;

	for(i=0; i< 1024; i++) {
		frame_list[dev][i].link = LINK_ADDR(sched_queue[dev]);
		frame_list[dev][i].queue=1;
		frame_list[dev][i].terminate=0;
//		frame_list[dev][i].terminate=1;
	}

	dump_link( frame_list[dev], "frame_list_link: ");
	DPRINTF("dummy_td = %p\n", dummy_td[dev]);

//	dump_frame_list("sched:");

}

void uhci_init(void)
{
	int i;

	init_td();
	init_qh();
	init_transactions();

	for(i=0;i<MAX_CONTROLLERS; i++) {
		init_framelist(i);
		init_sched(i);
	}

	hc_init( 0x1d, 0);
	hc_init( 0x1d, 1);
	hc_init( 0x1d, 2);
}

int poll_queue_head( queue_head_t *qh)
{
	td_t *td;
	int strikes=3;

	if(qh->depth.terminate)
		return(1);

	while(strikes--) {
		if(qh->depth.terminate)
			return(1);

		td = MEM_ADDR(qh->depth.link);

		if(td->active)
			return(0);

		udelay(1000);

//		if(!td->active)
//			return(1);
	}

	return(1);
}

int wait_queue_complete( queue_head_t *qh)
{
	int ret;
	int spins=1000;

	while( --spins && !(ret = poll_queue_head(qh))) {
		udelay(1500);
//		if(!(spins%30))
//			DPRINTF("wait_queue_complete: spin\n");
	}
//	DPRINTF("wait_queue_complete: returning %d\n", ret);

	if(!spins)
		return(-1);

	return(ret);
}

int next_usb_dev;
usbdev_t usb_device[MAX_USB_DEV];

void init_devices(void) 
{

	memset(usb_device,0,sizeof(usb_device));
	usb_device[0].max_packet[0] = 8;
	next_usb_dev=3;
}

#define BULK_DEPTH 1

transaction_t *_bulk_transfer( uchar devnum, uchar ep, unsigned int len, uchar *data)
{
	uchar dt;
	transaction_t *trans;
	td_t *td, *cur, *last;
	int remaining = len;
	uchar *pos = data;
	int max;
	uchar type = OUT_TOKEN;
	int packet_length;


	if(ep & 0x80)
		type = IN_TOKEN;

	ep &= 0x7f;

	td = cur = last = NULL;
	dt = usb_device[devnum].toggle[ep];
	max = usb_device[devnum].max_packet[ep];

	while(remaining) {
		cur = new_td();
		cur->packet_type = type;
		cur->data_toggle = dt;
		cur->endpoint = ep&0x7f;
		cur->device_addr = devnum;
		cur->detect_short=1;
		cur->active=1;
		dt = dt^0x01;
		
		if(!td)
			td = cur;

		if(last) {
			last->link.terminate=0;
			last->link.link = LINK_ADDR(cur);
		}

		cur->buffer = (void *) virt_to_bus(pos);

		if(remaining>max)
			packet_length = max;
		else
			packet_length = remaining;
		
		cur->max_transfer=packet_length-1;
		cur->link.depth = BULK_DEPTH;

		remaining -= packet_length;
		pos+= packet_length;
		last = cur;
	}

//	if( packet_length == max) {	// if final packet wasn't short, add a zero packet
//		cur = new_td();
//		dt = dt^0x01;
//		cur->packet_type = type;
//		cur->max_transfer = 0x7ff;	// zero length code
//		last->link.terminate=0;
//		last->link.link = LINK_ADDR(cur);
//		
//	}

	cur->link.terminate=1;

	trans = new_transaction(td);
	usb_device[devnum].toggle[ep] = dt;

	return(trans);
}

#define DEPTH 0

transaction_t *ctrl_msg(uchar devnum, uchar request_type, uchar request, unsigned short wValue, unsigned short wIndex, unsigned short wLength, uchar *data)
{
	td_t *td;
	td_t *current_td;
	td_t *last_td;
	transaction_t *trans;

	ctrl_msg_t *message;

	unsigned char type;
	int remaining = wLength;
	uchar *pos = data;
	uchar dt=1;

//	DPRINTF("ctrl_msg( %02x, %02x, %02x, %04x, %04x, %04x, %p)\n", devnum, request_type, request, wValue, wIndex, wLength, data);
//	DPRINTF("%d bytes in payload\n", remaining);
//	DPRINTF("lowspeed = %u\n", usb_device[devnum].lowspeed);
	last_td = td = new_td();

	td->packet_type = SETUP_TOKEN;
	td->device_addr = devnum & 0x7f;
	td->max_transfer = 7;		// fixed for setup packets
	td->retrys = CTRL_RETRIES;
	td->active=1;
	td->data_toggle=0;
	td->link.depth=DEPTH;
	td->detect_short=0;
	td->interrupt=1;
	td->lowspeed = usb_device[devnum].lowspeed;

// steal 8 bytes from so-called software area to hole the control message itself
	td->buffer = (void *) virt_to_bus(&(td->data[2]));
	message = bus_to_virt( (unsigned int) td->buffer);

	message->bmRequestType = request_type;
	message->bRequest = request;
	message->wValue = wValue;
	message->wIndex = wIndex;
	message->wLength = wLength;
//dump_hex(td, sizeof(td_t), "ctrl_msg:");
	trans = new_transaction(td);

	if(!trans) {
		DPRINTF("ctrl_msg: couldn't allocate a transaction!\n");
		return(NULL);
	}

	if(request_type & CONTROL_DIR_MASK) 
		type = IN_TOKEN;
	else
		type = OUT_TOKEN;

	while(remaining >0)	{
		int length;

//		DPRINTF("ctrl_msg loop %d remaining, maxpacket = %u\n", remaining, usb_device[devnum].max_packet[0]);
		current_td = new_td();

		last_td->link.link = LINK_ADDR(current_td);
		last_td->link.terminate=0;
		last_td->link.queue=0;
		last_td->link.depth=DEPTH;
		

		current_td->device_addr = devnum & 0x7f;
		current_td->retrys = CTRL_RETRIES;
		current_td->active=1;
		current_td->data_toggle=dt;
		current_td->link.depth=DEPTH;
		current_td->lowspeed = usb_device[devnum].lowspeed;
		current_td->detect_short=1;

		dt = dt^0x01;

		current_td->packet_type = type;
//		if(type == IN_TOKEN)
//			current_td->detect_short=1;

		if(remaining >usb_device[devnum].max_packet[0])
			length = usb_device[devnum].max_packet[0];
		else
			length = remaining;

		current_td->max_transfer = length-1;
		current_td->buffer = (void *) virt_to_bus(pos);
		remaining -= length;
		pos += length;

		last_td = current_td;
	}

	current_td = new_td();

	current_td->device_addr =  devnum & 0x7f;
	current_td->retrys = CONTROL_STS_RETRIES;
	current_td->active=1;
	current_td->lowspeed = usb_device[devnum].lowspeed;

	if(type == IN_TOKEN)
		current_td->packet_type = OUT_TOKEN;
	else
		current_td->packet_type = IN_TOKEN;

	current_td->max_transfer=0x7ff;

	current_td->link.terminate=1;
	current_td->data_toggle=1;
	current_td->link.depth=DEPTH;

	
	last_td->link.link = LINK_ADDR(current_td);
	last_td->link.terminate=0;
	last_td->link.queue=0;
	last_td->link.depth=DEPTH;

	return(trans);
}
	

int schedule_transaction( uchar dev, transaction_t *trans)
{
	unsigned short value;

	if(!sched_queue[dev]->depth.terminate)
		return(-EBUSY);

	sched_queue[dev]->depth.link = LINK_ADDR(trans->qh);
	sched_queue[dev]->depth.terminate = 0;
	sched_queue[dev]->depth.queue=1;

	value = inw(hc_base[dev]);
	value |=1;
	outw( value, hc_base[dev]);
	
	return(0);
}

int wait_transaction( transaction_t *trans)
{
	queue_head_t *qh;

	qh = trans->qh;

	while(!qh->bredth.terminate) 
		qh = MEM_ADDR(qh->bredth.link);

	return( wait_queue_complete(qh));
}

void unlink_transaction( uchar dev, transaction_t *trans)
{
	sched_queue[dev]->depth.terminate=1;
	sched_queue[dev]->depth.link = 0;	// just in case
}

int bulk_transfer( uchar devnum, uchar ep, unsigned int len, uchar *data)
{
	transaction_t *trans;
	td_t *td;
	int data_len;
	uchar *buffer;
	int ret;

#ifdef __KERNEL__
	dma_addr_t dma_handle;
	buffer = pci_alloc_consistent(NULL, len , &dma_handle);

	if( !(ep & 0x80))
		memcpy(buffer, data, len);
#else
	buffer = data;
#endif
	

	trans = _bulk_transfer(devnum, ep, len, buffer);
//#ifdef DEBUG
//	dump_transaction(trans, "bulk_transfer:");
//#endif
	schedule_transaction( usb_device[devnum].controller, trans);
	ret = wait_transaction(trans);
	unlink_transaction( usb_device[devnum].controller, trans);

	if(ret<0) {
#ifdef DEBUG
		dump_uhci(hc_base[usb_device[devnum].controller] );
		dump_td(trans->td_list, "failed_bulk_transaction: ");
#endif
		free_transaction(trans);
		return(-1);
	}

	// a shortcut
	if(trans->qh->depth.terminate)
		data_len = len;
	else {
		data_len=0;
		td = trans->td_list;
		do {
			if(td->active)
				break;

			if(td->max_transfer == 0x7ff)
				break;

			data_len += td->actual +1;

			if(td->actual < td->max_transfer) // short packet also check for errors here
				break;

			if(!td->link.terminate)
				td = MEM_ADDR(td->link.link);
			else
				td=NULL;
		} while(td);
	}

//#ifdef DEBUG
//	dump_td(trans->td_list, "bulk_transfer_success:");
//#endif

	if(data_len < len) {
		DPRINTF("bulk_transfer( dev= %u, ep = %u, len = %u, buffer = %p) = %d:short transaction:\n", devnum, ep, len, data, data_len);
		dump_td(trans->td_list, "short_transaction:");
	}

	free_transaction(trans);
#ifdef __KERNEL__
	memcpy(data, buffer, data_len);
	pci_free_consistent(NULL, buffer, len, NULL);
#endif

//	DPRINTF("bulk_transfer returning %d\n", data_len);
	return(data_len);
}

int usb_control_msg( uchar devnum, uchar request_type, uchar request, unsigned short wValue, unsigned short wIndex, unsigned short wLength, void *data)
{
	transaction_t *trans, *cur;
	queue_head_t *qh;
	td_t *td;
	int data_len=0;
	int ret;
	uchar *buffer;
#ifdef __KERNEL__
	
	dma_addr_t dma_handle;
	
	buffer = pci_alloc_consistent(NULL, wLength, &dma_handle);
//	DPRINTF("usb_control_msg: buffer = %p\n", buffer);
	if( !(request_type & 0x80))
		memcpy(buffer, data, wLength);
#else
	buffer = data;
#endif

	trans = ctrl_msg(devnum, request_type, request, wValue, wIndex, wLength, buffer);
	if(!trans) {
		DPRINTF("usb_control_msg: ctrl_msg failed!\n");
#ifdef __KERNEL__
		pci_free_consistent(NULL, buffer, wLength, NULL);
#endif
		return(-1);
	}
	
	schedule_transaction( usb_device[devnum].controller, trans);
	ret = wait_transaction(trans);

	if(ret<0) {
#ifdef DEBUG
		dump_uhci(hc_base[usb_device[devnum].controller] );
		dump_td(trans->td_list, "failed_transaction: ");
#endif
		unlink_transaction( usb_device[devnum].controller, trans);
		free_transaction(trans);
#ifdef __KERNEL__
		pci_free_consistent(NULL, buffer, wLength, NULL);
#endif
		return(ret);
	}

//#ifdef DEBUG
//	dump_td(trans->td_list, "success: ");
//#endif

	unlink_transaction( usb_device[devnum].controller, trans);

	// now, see what happened
	qh = trans->qh;
	cur = trans;

	if(!qh->depth.terminate) {
//		handle setup error

		dump_uhci(hc_base);
		dump_td(trans->td_list, "qh->depth failed_transaction: ");

		free_transaction(trans);
#ifdef __KERNEL__
		pci_free_consistent(NULL, buffer, wLength, NULL);
#endif
		return(-1);
	}

	td = trans->td_list;

	do {
		if(td->packet_type != SETUP_TOKEN)
			data_len += td->actual;

		if(td->actual < td->max_transfer) // short packet also check for errors here
			break;

		if(!td->link.terminate)
			td = MEM_ADDR(td->link.link);
		else
			td=NULL;
	} while(td);

	// now check status phase
	if(! qh->depth.terminate) {
	}

	free_transaction(trans);

#ifdef __KERNEL__
	if( (request_type & 0x80))
		memcpy(data, buffer, wLength);
	pci_free_consistent(NULL, buffer, wLength, NULL);
#endif

	return(data_len);
}

inline int set_address(uchar address)
{
	int ret;

	ret = usb_control_msg(0, 0, SET_ADDRESS, address, 0, 0, NULL);

	return(ret);
}

inline int clear_stall(uchar device, uchar endpoint)
{
	int ret;

	ret = usb_control_msg(device, CONTROL_ENDPOINT, CLEAR_FEATURE, FEATURE_HALT, endpoint, 0, NULL);
	usb_device[device].toggle[endpoint]=0;

	return(ret);
}

inline int device_reset(uchar device) {
	usb_control_msg(device, 0x21, 0xff, 0, 0, 0, NULL);
}

///////////////////////////////////////////////////////////////////////////////////////
//
//	String Descriptors
//
//////////////////////////////////////////////////////////////////////////////////////

#define STRING_DESCRIPTOR 0x0300

int get_string( uchar addr, uchar string, int len, uchar *buffer)
{
	int ret;
	int i,j;
	int real_len;
	ushort lang;

	if(!string) {
		strcpy(buffer, "unknown");
		return(0);
	}

	ret = usb_control_msg(addr, 0x80, GET_DESCRIPTOR, STRING_DESCRIPTOR | string, 0, 4, buffer);
	real_len = buffer[0];
	if(real_len>len)
		real_len = len;

	lang = buffer[2] | buffer[3]<<8;
	ret = usb_control_msg(addr, 0x80, GET_DESCRIPTOR, STRING_DESCRIPTOR | string, lang, real_len, buffer);

	// de-unicode it!
	for(i=0, j=2; j<real_len; i++, j+=2)
		buffer[i] = buffer[j];

	buffer[i]=0;
	real_len/=2;

	return(real_len);
}


///////////////////////////////////////////////////////////////////////////////////////
//
// HUB functions. This will be moved to it's own module soonishly
//
///////////////////////////////////////////////////////////////////////////////////////

typedef struct port_charge {
	ushort c_port_connection:1;
	ushort c_port_enable:1;
	ushort c_port_suspend:1;
	ushort c_port_over_current:1;
	ushort c_port_reset:1;
	ushort reserved:11;
} port_change_t;

typedef struct port_status {
	ushort port_connection:1;
	ushort port_enable:1;
	ushort port_suspend:1;
	ushort port_over_current:1;
	ushort port_reset:1;
	ushort reserved:3;
	ushort port_power:1;
	ushort port_lowspeed:1;
	ushort port_highspeed:1;
	ushort port_test:1;
	ushort port_indicator:1;
} __attribute__ ((packed)) portstatus_t;


typedef struct portstat {
	portstatus_t  stat;
	port_change_t change;
} __attribute__ ((packed)) portstat_t;

int hub_port_reset( uchar addr, uchar port)
{
	int ret;
	int tries=100;
	portstat_t status;

	ret = usb_control_msg(addr, 0x23, SET_FEATURE, PORT_RESET, port, 0, NULL);	// reset port

	while(tries--) {
		udelay(10000);
		ret = usb_control_msg(addr, 0xa3, GET_STATUS, 0x0, port, 4, &status);
		if(!status.change.c_port_reset)
			continue;

		ret = usb_control_msg(addr, 0x23, CLEAR_FEATURE, C_PORT_RESET, port, 0, NULL);	// clear status
		return(0);
	}

	DPRINTF("hub_port_reset(%u, %u) failed,\n", addr, port);
	dump_hex(&status, 4, "status=");

	return(-1);
}

int hub_port_resume( uchar addr, uchar port)
{
	int ret;
	int tries=100;
	portstat_t status;

	ret = usb_control_msg(addr, 0x23, CLEAR_FEATURE, PORT_SUSPEND, port, 0, NULL);	// reset port

	while(tries--) {
		udelay(10000);
		ret = usb_control_msg(addr, 0xa3, GET_STATUS, 0x0, port, 4, &status);
		if(!status.change.c_port_suspend)
			continue;

		ret = usb_control_msg(addr, 0x23, CLEAR_FEATURE, C_PORT_SUSPEND, port, 0, NULL);	// clear status
		return(0);
	}

	return(-1);
}

int poll_hub(uchar addr)
{
	int i;
	int ret;
	uchar devaddr=0;
	hub_descriptor_t *desc;
	portstat_t status;

	DPRINTF("Poll hub (%u)\n", addr);
	desc = usb_device[addr].private;

	for(i=1; i<= desc->bNbrPorts; i++) {
		ret = usb_control_msg(addr, 0xa3, GET_STATUS, 0x0, i, 4, &status);
//		DPRINTF("Get status for port %u returns: %d\n", i, ret);
//		dump_hex(&status, 4, "status=");	

		if(status.change.c_port_connection) {
			ret = usb_control_msg(addr, 0x23, CLEAR_FEATURE, C_PORT_CONNECTION, i, 0, NULL);	// clear status

			if(status.stat.port_connection) {
				udelay(desc->bPwrOn2PwrGood * 20000);

				hub_port_resume(addr, i);

				ret = hub_port_reset(addr,i);
				udelay(10);
				ret = usb_control_msg(addr, 0x23, SET_FEATURE, PORT_ENABLE, i, 0, NULL);	// enable port

//		ret = usb_control_msg(addr, 0xa3, GET_STATUS, 0x0, i, 4, &status);
//		DPRINTF("*****Get status again  for port %u returns: %d\n", i, ret);
//		dump_hex(&status, 4, "status=");	

				devaddr = configure_device(i, usb_device[addr].controller, status.stat.port_lowspeed);

				// configure
			} else {
				ret = usb_control_msg(addr, 0x23, SET_FEATURE, PORT_SUSPEND, i, 0, NULL);	// suspend port
				ret = usb_control_msg(addr, 0x23, CLEAR_FEATURE, PORT_ENABLE, i, 0, NULL);	// disable port
				DPRINTF("Hub %u, Port %04x disconnected\n", addr, i);
				// deconfigure
			}
		}
	}
	return(devaddr);

}

int usb_hub_init( uchar addr) 
{
	int i;
	int ret;
	hub_descriptor_t *desc;

	desc = malloc(sizeof(hub_descriptor_t));

	memset(desc, 0 , sizeof(hub_descriptor_t));

	DPRINTF("hub init (%u)\n", addr);

	ret = usb_control_msg(addr, 0xa0, GET_DESCRIPTOR, 0x2900, 0, 8, desc);
	ret = usb_control_msg(addr, 0xa0, GET_DESCRIPTOR, 0x2900, 0, desc->bLength, desc);

	usb_device[addr].private = desc;

	for(i=1; i<=desc->bNbrPorts; i++)
		ret = usb_control_msg(addr, 0x23, SET_FEATURE, PORT_POWER, i, 0, NULL);	// power port


	// register hub to be polled

	devpoll[num_polls] = poll_hub;
	parm[num_polls++] = addr;

	return(0);
}

typedef struct partition_entry {
	uchar boot_flag;

	uchar chs[7];

	unsigned int lba_start;
	unsigned int lba_len;
} __attribute__ ((packed)) partition_entry_t;

typedef struct partition {
	char loader[446];
	partition_entry_t entry[4];
	char sig[2];
} __attribute__ ((packed)) partition_t;

// will set up whatever device is answering at address 0.
int configure_device(ushort port, uchar controller, unsigned int lowspeed)
{
	device_descriptor_t *desc;
	config_descriptor_t *conf;
	interface_descriptor_t *iface;
	endpoint_descriptor_t *epd;
	int ret;
	int i;
	int addr = next_usb_dev++;
	int address_retry=5;
	uchar buffer[512];
	uchar string[512];

	desc = (device_descriptor_t *) buffer;

	memset( &usb_device[addr], 0, sizeof(usbdev_t));

	DPRINTF("New USB device, setting address %u\n", addr);
	if(lowspeed) {
		usb_device[addr].lowspeed = usb_device[0].lowspeed = 1;
		DPRINTF("LOWSPEED\n");
	} else
		usb_device[addr].lowspeed = usb_device[0].lowspeed = 0;

	usb_device[0].port = usb_device[addr].port = port;
	usb_device[0].controller = usb_device[addr].controller = controller;


//	hc_clear_stat();
//	dump_uhci(hc_base[controller]);

	ret = set_address(addr);
	if(ret<0) {
		DPRINTF("configure_device: set_address failed!\n");
//		dump_uhci(hc_base[controller]);
	}

	if(ret<0) {
		DPRINTF("configure_device: can't set address! bailing!\n");
		next_usb_dev--;
		return(-1);
	}
		
	usb_device[addr].max_packet[0] = 8;
	

	DPRINTF("Fetching device descriptor length\n");
	ret = usb_control_msg(addr, 0x80, GET_DESCRIPTOR, 0x100, 0, 8, desc);

	usb_device[addr].max_packet[0] = desc->max_packet;

	DPRINTF("Fetching device descriptor\n");
	ret = usb_control_msg(addr, 0x80, GET_DESCRIPTOR, 0x100, 0, desc->bLength, desc);
	if(ret < desc->bLength)
		return(-1);

	DPRINTF("Fetching config descriptor length\n");
	conf = (config_descriptor_t *) (buffer + sizeof(device_descriptor_t));

	ret = usb_control_msg(addr, 0x80, GET_DESCRIPTOR, 0x200, 0, 8, conf);

	DPRINTF("Fetching config descriptor\n");
	ret = usb_control_msg(addr, 0x80, GET_DESCRIPTOR, 0x200, 0, conf->wTotalLength, conf);
	if(ret < conf->wTotalLength)
		return(-1);

	iface = (interface_descriptor_t *) (buffer + sizeof(device_descriptor_t) + conf->bLength);
	epd = (endpoint_descriptor_t *) (buffer + conf->bLength + iface->bLength + sizeof(device_descriptor_t));

	DPRINTF("device:\n");
	dump_device_descriptor( desc, "");
	DPRINTF("config:\n");
	dump_config_descriptor( conf, "");

	DPRINTF("Selecting Configuration number %u:\n", conf->bConfigurationValue);
	ret = usb_control_msg(addr, 0, SET_CONFIGURATION, conf->bConfigurationValue, 0, 0, NULL);
	
	for(i=0; i<iface->bNumEndpoints;i++) {
		if(!epd[i].bEndpointAddress) {
			usb_device[addr].max_packet[ 1 ] = epd[i].wMaxPacketSize & 0x3ff;
		} else {
			usb_device[addr].max_packet[ epd[i].bEndpointAddress & 0x7f ] = epd[i].wMaxPacketSize & 0x3ff;
		}

		if( (epd[i].bmAttributes & 0x03) == 0x01)
			usb_device[addr].interrupt = epd[i].bEndpointAddress;

		if( (epd[i].bmAttributes & 0x03) == 0x02) {
			DPRINTF("clear stall on ep=%u\n", epd[i].bEndpointAddress);
			clear_stall(addr, epd[i].bEndpointAddress);	// to reset data toggle

			if(epd[i].bEndpointAddress & 0x80)
				usb_device[addr].bulk_in = epd[i].bEndpointAddress; 
			else
				usb_device[addr].bulk_out = epd[i].bEndpointAddress; 
		}

	}

	// determine device class
	if(desc->Class) {
		usb_device[addr].class = desc->Class;
		usb_device[addr].subclass = desc->SubClass;
		usb_device[addr].protocol = desc->protocol;
	} else {
		usb_device[addr].class = iface->bInterfaceClass;
		usb_device[addr].subclass = iface->bInterfaceSubClass;
		usb_device[addr].protocol = iface->bInterfaceProtocol;
	}

	printk_info("%02x:%02x:%02x\n", usb_device[addr].class, usb_device[addr].subclass, usb_device[addr].protocol);

	get_string(addr, desc->iManufacturor, sizeof(string), string);
	printk_info("Manufacturor: %s\n", string);

	get_string(addr, desc->iProduct, sizeof(string), string);
	printk_info("Product: %s\n", string);

	get_string(addr, desc->iSerial, sizeof(string), string);
	printk_info("Serial: %s\n", string);

	switch( usb_device[addr].class) {
		case 0x09:	// hub
			usb_hub_init(addr);
			break;

		default:
			break;

	}
		

// a test!
	if(usb_device[addr].class == 0x08 && usb_device[addr].subclass == 0x06 && usb_device[addr].protocol == 0x50) {
		uchar sense_data[32];
		unsigned long block_count=0;
		unsigned int block_len=0;
		partition_t part;

		DPRINTF("Mass storage, bulk only SCSI transparent\n");

		DPRINTF("Requesting initial sense data\n");
		request_sense( addr, sense_data, 32);
		PrintSense(sense_data, 32);

		get_capacity(addr, &block_count, &block_len);
		DPRINTF("%u %u byte blocks\n", block_count, block_len);

		ret = ll_read_block(addr, &part, 0, 1);

		DPRINTF("ll_read_block returns %d\n", ret);

		for(i=0; i<4; i++) 
			DPRINTF("%u: boot=%02x, start=%08x length=%08x\n",i,  part.entry[i].boot_flag, part.entry[i].lba_start, part.entry[i].lba_len);
	}

	DPRINTF("DEVICE CONFIGURED\n");
//	dump_uhci(hc_base[controller]);
	if(port>8)
		clear_port_stat(port);
//	dump_uhci(hc_base[controller]);
	return(addr);
}

int poll_root_hub(unsigned short port, uchar controller)
{
	ushort value;
	int addr=0;
	int i;
	static int do_over=0;

	value = inw(port);

	if(value & 0x02 || do_over == port) {
		do_over=0;
		if(value & 0x01 ) {	// if port connected
			DPRINTF("Connection on port %04x\n", port);

			outw(value, port);
			for(i=0; i<40; i++) {
				udelay(10000+usec_offset);
				value = inw(port);
				if(value & 0x02) {
					outw(value, port);
					i=0;
					DPRINTF("BOUNCE!\n");
				}
			}

			port_wakeup(port);
//			DPRINTF("Wakup %04x\n", port);

			port_reset(port);
			udelay(10);
			port_enable(port);

			if(!value & 0x01) {
				DPRINTF("Device went away!\n");
				return(-1);
			}

			addr = configure_device( port, controller, value & 0x100);

			if(addr<0) {
				port_disable(port);
				udelay(20000);
//				port_reset(port);
				port_reset_long(port);
				port_suspend(port);
				do_over=port;
				hc_clear_stat();
dump_uhci(0x38c0);
			}
		} else {
			port_suspend(port);
			port_disable(port);
			DPRINTF("Port %04x disconnected\n", port);
			// wave hands, deconfigure devices on this port!
		}
	}
			
	
	return(addr);
}

int num_polls=0;
int (*devpoll[MAX_POLLDEV])(uchar);
uchar parm[MAX_POLLDEV];

int poll_usb()
{
	int addr;
	int found=0;
	int i;

	for(i=0; i<num_controllers; i++) {
		// if addr >0, should probably see what was attached!
		addr = poll_root_hub(PORTSC1(0), i);
		if(addr && !found)
			found=addr;

		addr = poll_root_hub(PORTSC2(0), i);
		if(addr && !found)
			found=addr;
	}

	// now poll registered drivers (such as the hub driver
	for(i=0;i<num_polls; i++) {
		addr = devpoll[i](parm[i]);
		if(addr && !found)
			found=addr;
	}

	return(found);	
}

// if kernel is defined, build as a kernel module.
// this is only useful for testing
// this code is good for firmware, but a lousy
// kernel driver!

#ifdef __KERNEL__

#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/timer.h>


unsigned long uhci_poll_interval = 50;
unsigned short int_stat=0;

void uhci_set_timer(unsigned long interval);

void uhci_kernel_poll( unsigned long data)
{
	unsigned short value;

	poll_usb();
	uhci_set_timer(uhci_poll_interval);

}

struct timer_list uhci_timer;

static void uhci_interrupt(int irq, void *__uhci, struct pt_regs *regs)
{

	int_stat = inw(0x38c2);
	outw( int_stat, 0x38c2);
	printk("<1>UHCI interrupt, status = %04x\n", int_stat);
	

}

void uhci_set_timer(unsigned long interval)
{
	init_timer(&uhci_timer);
	uhci_timer.function = uhci_kernel_poll;
	uhci_timer.expires = jiffies + interval;
	
	add_timer( &uhci_timer );
}

int __init uhci_module_init(void)
{
#ifdef TEST
	unsigned int value;

	printk("<1>UHCI DUMP:\n");
	dump_uhci(0x38c0);
	value = inl(0x38c0 + 0x08);

	value = bus_to_virt(value);
	value &= ~0x7ff;
	dump_frame_list(value, "");
	return(0);
#endif

	request_irq(16, uhci_interrupt, SA_SHIRQ, "usb-uhci", uhci_interrupt);

	printk("<1>UHCI TEST INIT\n");
	init_devices();
	uhci_init();
	uhci_set_timer( uhci_poll_interval);
	
	return(0);

}

void __exit uhci_module_exit(void)
{

	outw( 0x00, 0x38c4);

	port_suspend(PORTSC1(0));
	port_suspend(PORTSC2(0));
	hc_reset(0);
	free_irq(16, uhci_interrupt);
	del_timer( &uhci_timer);

}

module_init(uhci_module_init);
module_exit(uhci_module_exit);



#endif
