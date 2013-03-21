/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <device/pci_def.h>
#include "i82830.h"

extern unsigned char *mbi;
extern u32 mbi_len;

// #define DEBUG_SMI_I82830

/* If YABEL is enabled and it's not running at 0x00000000, we have to add some
 * offset to all our mbi object memory accesses
 */
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL && !CONFIG_YABEL_DIRECTHW
#define OBJ_OFFSET CONFIG_YABEL_VIRTMEM_LOCATION
#else
#define OBJ_OFFSET 0x00000
#endif

/* I830M */
#define SMRAM		0x90
#define   D_OPEN	(1 << 6)
#define   D_CLS		(1 << 5)
#define   D_LCK		(1 << 4)
#define   G_SMRANE	(1 << 3)
#define   C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))


typedef struct {
	u32 mhid;
	u32 function;
	u32 retsts;
	u32 rfu;
} __attribute__((packed)) banner_id_t;

#define MSH_OK			0x0000
#define MSH_OK_RESTART		0x0001
#define MSH_FWH_ERR		0x00ff
#define MSH_IF_BAD_ID		0x0100
#define MSH_IF_BAD_FUNC		0x0101
#define MSH_IF_MBI_CORRUPT	0x0102
#define MSH_IF_BAD_HANDLE	0x0103
#define MSH_ALRDY_ATCHED	0x0104
#define MSH_NOT_ATCHED		0x0105
#define MSH_IF			0x0106
#define MSH_IF_INVADDR		0x0107
#define MSH_IF_UKN_TYPE		0x0108
#define MSH_IF_NOT_FOUND	0x0109
#define MSH_IF_NO_KEY		0x010a
#define MSH_IF_BUF_SIZE		0x010b
#define MSH_IF_NOT_PENDING	0x010c

#ifdef DEBUG_SMI_I82830
static void
dump(u8 * addr, u32 len)
{
	printk(BIOS_DEBUG, "\n%s(%p, %x):\n", __func__, addr, len);
	while (len) {
		unsigned int tmpCnt = len;
		unsigned char x;
		if (tmpCnt > 8)
			tmpCnt = 8;
		printk(BIOS_DEBUG, "\n%p: ", addr);
		// print hex
		while (tmpCnt--) {
			x = *addr++;
			printk(BIOS_DEBUG, "%02x ", x);
		}
		tmpCnt = len;
		if (tmpCnt > 8)
			tmpCnt = 8;
		len -= tmpCnt;
		//reset addr ptr to print ascii
		addr = addr - tmpCnt;
		// print ascii
		while (tmpCnt--) {
			x = *addr++;
			if ((x < 32) || (x >= 127)) {
				//non-printable char
				x = '.';
			}
			printk(BIOS_DEBUG, "%c", x);
		}
	}
	printk(BIOS_DEBUG, "\n");
}
#endif

typedef struct {
	banner_id_t banner;
	u16 versionmajor;
	u16 versionminor;
	u32 smicombuffersize;
} __attribute__((packed)) version_t;

typedef struct {
	u16 header_id;
	u16 attributes;
	u16 size;
	u8  name_len;
	u8 reserved;
	u32 type;
	u32 header_ext;
	u8 name[0];
} __attribute__((packed)) mbi_header_t;

typedef struct {
	banner_id_t banner;
	u64 handle;
	u32 objnum;
	mbi_header_t header;
} __attribute__((packed)) obj_header_t;

typedef struct {
	banner_id_t banner;
	u64 handle;
	u32 objnum;
	u32 start;
	u32 numbytes;
	u32 buflen;
	u32 buffer;
} __attribute__((packed)) get_object_t;

static void mbi_call(u8 subf, banner_id_t *banner_id)
{
#ifdef DEBUG_SMI_I82830
	printk(BIOS_DEBUG, "MBI\n");
	printk(BIOS_DEBUG, "|- sub function %x\n", subf);
	printk(BIOS_DEBUG, "|- banner id @ %x\n", (u32)banner_id);
	printk(BIOS_DEBUG, "|  |- mhid %x\n", banner_id->mhid);
	printk(BIOS_DEBUG, "|  |- function %x\n", banner_id->function);
	printk(BIOS_DEBUG, "|  |- return status %x\n", banner_id->retsts);
	printk(BIOS_DEBUG, "|  |- rfu %x\n", banner_id->rfu);
#endif

	switch(banner_id->function) {
	case 0x0001: {
		version_t *version;
		printk(BIOS_DEBUG, "|- MBI_QueryInterface\n");
		version = (version_t *)banner_id;
		version->banner.retsts = MSH_OK;
		version->versionmajor=1;
		version->versionminor=3;
		version->smicombuffersize=0x1000;
		break;
	}
	case 0x0002:
		printk(BIOS_DEBUG, "|- MBI_Attach\n");
		printk(BIOS_DEBUG, "|  |- Not Implemented!\n");
		break;
	case 0x0003:
		printk(BIOS_DEBUG, "|- MBI_Detach\n");
		printk(BIOS_DEBUG, "|  |- Not Implemented!\n");
		break;
	case 0x0201: {
		obj_header_t *obj_header = (obj_header_t *)banner_id;
		mbi_header_t *mbi_header = NULL;
		printk(BIOS_DEBUG, "|- MBI_GetObjectHeader\n");
		printk(BIOS_DEBUG, "|  |- objnum = %d\n", obj_header->objnum);

		int i, count=0;
		obj_header->banner.retsts = MSH_IF_NOT_FOUND;

		for (i=0; i<mbi_len;) {
			int len;

			if (!(mbi[i] == 0xf0 && mbi [i+1] == 0xf6)) {
				i+=16;
				continue;
			}

			mbi_header = (mbi_header_t *)&mbi[i];
			len = ALIGN((mbi_header->size * 16) + sizeof(mbi_header) + ALIGN(mbi_header->name_len, 16), 16);

			if (obj_header->objnum == count) {
#ifdef DEBUG_SMI_I82830
				if (mbi_header->name_len == 0xff) {
					printk(BIOS_DEBUG, "|  |- corrupt.\n");
					break;
				}
#endif
				int headerlen = ALIGN(sizeof(mbi_header) + ALIGN(mbi_header->name_len, 16), 16);
#ifdef DEBUG_SMI_I82830
				printk(BIOS_DEBUG, "|  |- headerlen = %d\n", headerlen);
#endif
				memcpy(&obj_header->header, mbi_header, headerlen);
				obj_header->banner.retsts = MSH_OK;
				printk(BIOS_DEBUG, "|     |- MBI module '");
				int j;
				for (j=0; j < mbi_header->name_len && mbi_header->name[j]; j++)
					printk(BIOS_DEBUG, "%c",  mbi_header->name[j]);
				printk(BIOS_DEBUG, "' found.\n");
#ifdef DEBUG_SMI_I82830
				dump((u8 *)banner_id, sizeof(obj_header_t) + ALIGN(mbi_header->name_len, 16));
#endif
				break;
			}
			i += len;
			count++;
		}
		if (obj_header->banner.retsts == MSH_IF_NOT_FOUND)
			printk(BIOS_DEBUG, "|     |- MBI object #%d not found.\n", obj_header->objnum);
		break;
	}
	case 0x0203: {
		get_object_t *getobj = (get_object_t *)banner_id;
		mbi_header_t *mbi_header = NULL;
		printk(BIOS_DEBUG, "|- MBI_GetObject\n");
#ifdef DEBUG_SMI_I82830
		printk(BIOS_DEBUG, "|  |- handle = %016Lx\n", getobj->handle);
#endif
		printk(BIOS_DEBUG, "|  |- objnum = %d\n", getobj->objnum);
		printk(BIOS_DEBUG, "|  |- start = %x\n", getobj->start);
		printk(BIOS_DEBUG, "|  |- numbytes = %x\n", getobj->numbytes);
		printk(BIOS_DEBUG, "|  |- buflen = %x\n", getobj->buflen);
		printk(BIOS_DEBUG, "|  |- buffer = %x\n", getobj->buffer);

		int i, count=0;
		getobj->banner.retsts = MSH_IF_NOT_FOUND;

		for (i=0; i< mbi_len;) {
			int headerlen, objectlen;

			if (!(mbi[i] == 0xf0 && mbi [i+1] == 0xf6)) {
				i+=16;
				continue;
			}

			mbi_header = (mbi_header_t *)&mbi[i];
			headerlen = ALIGN(sizeof(mbi_header) + ALIGN(mbi_header->name_len, 16), 16);
			objectlen = ALIGN((mbi_header->size * 16), 16);

			if (getobj->objnum == count) {
				printk(BIOS_DEBUG, "|  |- len = %x\n", headerlen + objectlen);

				memcpy((void *)(getobj->buffer + OBJ_OFFSET),
						((char *)mbi_header) + headerlen, (objectlen > getobj->buflen) ? getobj->buflen : objectlen);

				getobj->banner.retsts = MSH_OK;
#ifdef DEBUG_SMI_I82830
				dump((u8 *)banner_id, sizeof(*getobj));
				dump((u8 *)getobj->buffer + OBJ_OFFSET, objectlen);
#endif
				break;
			}
			i += (headerlen + objectlen);
			count++;
		}
		if (getobj->banner.retsts == MSH_IF_NOT_FOUND)
			printk(BIOS_DEBUG, "MBI module %d not found.\n", getobj->objnum);
		break;
	}
	default:
		printk(BIOS_DEBUG, "|- function %x\n", banner_id->function);
		printk(BIOS_DEBUG, "|  |- Unknown Function!\n");
		break;
	}
	printk(BIOS_DEBUG, "\n");
	//dump(banner_id, 0x20);
}

#define SMI_IFC_SUCCESS		    1
#define SMI_IFC_FAILURE_GENERIC     0
#define SMI_IFC_FAILURE_INVALID     2
#define SMI_IFC_FAILURE_CRITICAL    4
#define SMI_IFC_FAILURE_NONCRITICAL 6

#define PC10 	0x10
#define PC11	0x11
#define PC12	0x12
#define PC13	0x13

static void smi_interface_call(void)
{
	u32 mmio = pci_read_config32(PCI_DEV(0, 0x02, 0), 0x14);
	// mmio &= 0xfff80000;
	// printk(BIOS_DEBUG, "mmio=%x\n", mmio);
	u16 swsmi = pci_read_config16(PCI_DEV(0, 0x02, 0), 0xe0);

	if (!(swsmi & 1))
		return;

	swsmi &= ~(1 << 0); // clear SMI toggle

	switch ((swsmi>>1) & 0xf) {
	case 0:
		printk(BIOS_DEBUG, "Interface Function Presence Test.\n");
		swsmi = 0;
		swsmi &= ~(7 << 5); // Exit: Result
		swsmi |= (SMI_IFC_SUCCESS << 5);
		swsmi &= 0xff;
		swsmi |= (PC13 << 8);
		pci_write_config16(PCI_DEV(0, 0x02, 0), 0xe0, swsmi);
		// write magic
		write32(mmio + 0x71428, 0x494e5443);
		return;
	case 4:
		printk(BIOS_DEBUG, "Get BIOS Data.\n");
		printk(BIOS_DEBUG, "swsmi=%04x\n", swsmi);
		break;
	case 5:
		printk(BIOS_DEBUG, "Call MBI Functions.\n");
		mbi_call(swsmi >> 8, (banner_id_t *)((read32(mmio + 0x71428) & 0x000fffff) + OBJ_OFFSET) );
		// swsmi = 0x0000;
		swsmi &= ~(7 << 5); // Exit: Result
		swsmi |= (SMI_IFC_SUCCESS << 5);
		pci_write_config16(PCI_DEV(0, 0x02, 0), 0xe0, swsmi);
		return;
	case 6:
		printk(BIOS_DEBUG, "System BIOS Callbacks.\n");
		printk(BIOS_DEBUG, "swsmi=%04x\n", swsmi);
		break;
	default:
		printk(BIOS_DEBUG, "Unknown SMI interface call %04x\n", swsmi);
		break;
	}

	swsmi &= ~(7 << 5); // Exit: Result
	swsmi |= (SMI_IFC_FAILURE_CRITICAL << 7);
	pci_write_config16(PCI_DEV(0, 0x02, 0), 0xe0, swsmi);
}

/**
 * @brief read and clear ERRSTS
 * @return ERRSTS register
 */
static u16 reset_err_status(void)
{
	u16 reg16;

	reg16 = pci_read_config16(PCI_DEV(0, 0x00, 0), ERRSTS);
	/* set status bits are cleared by writing 1 to them */
	pci_write_config16(PCI_DEV(0, 0x00, 0), ERRSTS, reg16);

	return reg16;
}

static void dump_err_status(u32 errsts)
{
	printk(BIOS_DEBUG, "ERRSTS: ");
	if (errsts & (1 << 12)) printk(BIOS_DEBUG, "MBI ");
	if (errsts & (1 <<  9)) printk(BIOS_DEBUG, "LCKF ");
	if (errsts & (1 <<  8)) printk(BIOS_DEBUG, "DTF ");
	if (errsts & (1 <<  5)) printk(BIOS_DEBUG, "UNSC ");
	if (errsts & (1 <<  4)) printk(BIOS_DEBUG, "OOGF ");
	if (errsts & (1 <<  3)) printk(BIOS_DEBUG, "IAAF ");
	if (errsts & (1 <<  2)) printk(BIOS_DEBUG, "ITTEF ");
	printk(BIOS_DEBUG, "\n");
}

void northbridge_smi_handler(unsigned int node, smm_state_save_area_t *state_save)
{
	u16 errsts;

	/* We need to clear the SMI status registers, or we won't see what's
	 * happening in the following calls.
	 */
	errsts = reset_err_status();
	if (errsts & (1 << 12)) {
		smi_interface_call();
	} else {
		if (errsts)
			dump_err_status(errsts);
	}

}
