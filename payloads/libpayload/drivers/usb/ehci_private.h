/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2010 coresystems GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __EHCI_PRIVATE_H
#define __EHCI_PRIVATE_H

#include <pci.h>
#include <usb/usb.h>

#define USBBASE 0x10
#define FLADJ 0x61
#define FLADJ_framelength(x) (((x)-59488)/16)

typedef union {
	u32 val;
	volatile struct {
		unsigned long current_conn_status:1;
		unsigned long conn_status_change:1;
		unsigned long port_enable:1;
		unsigned long port_enable_change:1;
		unsigned long overcurrent:1;
		unsigned long overcurrent_change:1;
		unsigned long force_port_resume:1;
		unsigned long suspend:1;
		unsigned long port_reset:1;
		unsigned long:1;
		unsigned long line_status:2;
		unsigned long pp:1;
		unsigned long port_owner:1;
		unsigned long port_indicator_control:2;
		unsigned long port_test_control:4;
		unsigned long wake_on_connect_en:1;
		unsigned long wake_on_disconnect_en:1;
		unsigned long wake_on_overcurrent_en:1;
		unsigned long:9;
	} __attribute__ ((packed));
} __attribute__ ((packed)) portsc_t;

typedef struct {
	u8 caplength;
	u8 res1;
	u16 hciversion;
	union {
		u32 hcsparams;
		struct {
			unsigned long n_ports:4;
			unsigned long ppc:1;
			unsigned long:2;
			unsigned long port_routing_rules:1;
			unsigned long n_pcc:4;
			unsigned long n_cc:4;
			unsigned long p_indicator:1;
			unsigned long:3;
			unsigned long debug_port_number:4;
			unsigned long:8;
		} __attribute__ ((packed));
	};
	union {
		u32 hccparams;
		struct {
			unsigned long cap_64b_addr:1;
			unsigned long cap_prog_framelist_size:1;
			unsigned long cap_async_park:1;
			unsigned long:1;
			unsigned long isoc_sched_threshold:4;
			unsigned long eecp:8;
			unsigned long:16;
		} __attribute__ ((packed));
	};
	union {
		u64 hcsp_portroute;
		struct {
			unsigned long portroute0:4;
			unsigned long portroute1:4;
			unsigned long portroute2:4;
			unsigned long portroute3:4;
			unsigned long portroute4:4;
			unsigned long portroute5:4;
			unsigned long portroute6:4;
			unsigned long portroute7:4;
			unsigned long portroute8:4;
			unsigned long portroute9:4;
			unsigned long portroute10:4;
			unsigned long portroute11:4;
			unsigned long portroute12:4;
			unsigned long portroute13:4;
			unsigned long portroute14:4;
			unsigned long portroute15:4;
			unsigned long:4;
		} __attribute__ ((packed));
	};
} __attribute__ ((packed)) hc_cap_t;

typedef struct {
	union {
		u32 usbcmd;
		volatile struct {
			unsigned long rs:1;
			unsigned long hcreset:1;
			unsigned long frame_list_size:2;
			unsigned long periodic_sched_enable:1;
			unsigned long async_sched_enable:1;
			unsigned long irq_on_async_advance_doorbell:1;
			unsigned long light_hc_reset:1;
			unsigned long async_sched_park_mode_count:2;
			unsigned long:1;
			unsigned long async_sched_park_mode_enable:1;
			unsigned long:4;
			unsigned long irq_threshold_count:8;
			unsigned long:8;
		} __attribute__ ((packed));
	};
	union {
		u32 usbsts;
		struct {
			unsigned long usbint:1;
			unsigned long usberrint:1;
			unsigned long port_change_detect:1;
			unsigned long frame_list_rollover:1;
			unsigned long host_system_error:1;
			unsigned long irq_on_async_advance:1;
			unsigned long:6;
			unsigned long hchalted:1;
			unsigned long reclamation:1;
			unsigned long periodic_sched_status:1;
			unsigned long async_sched_status:1;
			unsigned long:16;
		} __attribute__ ((packed));
	};
	union {
		u32 usbintr;
		struct {
			unsigned long en_usb_irq:1;
			unsigned long en_usb_err_irq:1;
			unsigned long en_port_change_irq:1;
			unsigned long en_frame_list_rollover_irq:1;
			unsigned long en_host_system_error_irq:1;
			unsigned long en_irq_on_async_advance:1;
			unsigned long:26;
		} __attribute__ ((packed));
	};
	u32 frindex;
	u32 ctrldssegment;
	u32 periodiclistbase;
	u32 asynclistaddr;
	u8 res1[0x3f-0x1c];
	u32 configflag;
	portsc_t portsc[0];
} hc_op_t;

typedef struct {
	union {
		u32 next_qtd;
		struct {
			unsigned long terminate:1;
			unsigned long:4;
			unsigned long:27;
		} __attribute__ ((packed));
	};
	union {
		u32 alt_next_qtd;
		struct {
			unsigned long alt_terminate:1;
			unsigned long:4;
			unsigned long:27;
		} __attribute__ ((packed));
	};
	struct {
		union {
			volatile u8 status;
			struct {
				volatile unsigned long perr:1;
				volatile unsigned long splitxstate:1;
				volatile unsigned long missed_mframe:1;
				volatile unsigned long xact_err:1;
				volatile unsigned long babble:1;
				volatile unsigned long data_buf_err:1;
				volatile unsigned long halted:1;
				volatile unsigned long active:1;
			} __attribute__ ((packed));
		};
		unsigned long pid:2;
		volatile unsigned long cerr:2;
		volatile unsigned long c_page:3;
		unsigned long ioc:1;
		volatile unsigned long total_len:15;
		volatile unsigned long dt:1;
	} __attribute__ ((packed));
	union {
		u32 bufptr0;
		struct {
			volatile unsigned long cur_off:12;
			unsigned long:20;
		} __attribute__ ((packed));
	};
	u32 bufptrs[4];
	u32 bufptrs64[5];
} __attribute__ ((packed)) qtd_t;

typedef struct {
	union {
		u32 horiz_link_ptr;
		struct {
			unsigned long terminate:1;
			unsigned long type:2;
			unsigned long:1;
			unsigned long:28;
		} __attribute__ ((packed));
	};
	struct {
		unsigned long addr:7;
		unsigned long inactivate:1;
		unsigned long ep:4;
		unsigned long eps:2;
		unsigned long dtc:1;
		unsigned long reclaim_head:1;
		unsigned long max_packet_len:11;
		unsigned long non_hs_control_ep:1;
		unsigned long nak_cnt_reload:4;
	} __attribute__ ((packed));
	struct {
		unsigned long irq_sched_mask:8;
		unsigned long split_compl_mask:8;
		unsigned long hub_addr:7;
		unsigned long port_num:7;
		unsigned long pipe_multiplier:2;
	} __attribute__ ((packed));
	volatile u32 current_td_ptr;
	volatile qtd_t td;
} ehci_qh_t;

typedef struct ehci {
	hc_cap_t *capabilities;
	hc_op_t *operation;
} ehci_t;


#define EHCI_INST(controller) ((ehci_t*)((controller)->instance))

#endif
