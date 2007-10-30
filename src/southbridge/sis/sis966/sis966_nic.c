/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <delay.h>
#include "sis966.h"


uint8_t	SiS_SiS191_init[6][3]={
{0x04, 0xFF, 0x07},					 
{0x2C, 0xFF, 0x39},					 
{0x2D, 0xFF, 0x10},					 
{0x2E, 0xFF, 0x91},					 
{0x2F, 0xFF, 0x01},
{0x00, 0x00, 0x00}					//End of table
};

#if 1
#define StatusReg       0x1    
#define SMI_READ	0x0
#define SMI_REQUEST	0x10
#define TRUE            1
#define FALSE           0

uint16_t MacAddr[3];


void writeApcByte(int addr, uint8_t value)
{
    outb(addr,0x78);
    outb(value,0x79); 
}
uint8_t readApcByte(int addr)
{
    uint8_t value;
    outb(addr,0x78);
    value=inb(0x79);
    return(value);
}

static void readApcMacAddr(void)
{
    uint8_t i;

// enable APC in south bridge sis966 D2F0

    outl(0x80001048,0xcf8);  
    outl((inl(0xcfc) & 0xfffffffd),0xcfc ); // enable IO78/79h for APC Index/Data

    printk_debug("MAC addr in APC = ");
    for(i = 0x9 ; i <=0xe ; i++) 
    {
        printk_debug("%2.2x",readApcByte(i));
    }
    printk_debug("\n");

    /* Set APC Reload */
    writeApcByte(0x7,readApcByte(0x7)&0xf7);
    writeApcByte(0x7,readApcByte(0x7)|0x0a);
          	
    /* disable APC in south bridge */
    outl(0x80001048,0xcf8); 
    outl(inl(0xcfc)&0xffffffbf,0xcfc);
}

static void set_apc(struct device *dev)
{
    uint32_t tmp;
    uint16_t addr;
    uint32_t idx;
    uint16_t i;
    uint8_t   bTmp;

    /* enable APC in south bridge sis966 D2F0 */
    outl(0x80001048,0xcf8);  
    outl((inl(0xcfc) & 0xfffffffd),0xcfc ); // enable IO78/79h for APC Index/Data
          
    for(i = 0 ; i <3; i++)
    {
       addr=0x9+2*i;
       writeApcByte(addr,(uint8_t)(MacAddr[i]&0xFF));
	writeApcByte(addr+1L,(uint8_t)((MacAddr[i]>>8)&0xFF));
        // printf("%x - ",readMacAddrByte(0x59+i));
    }

    /* Set APC Reload */
    writeApcByte(0x7,readApcByte(0x7)&0xf7);
    writeApcByte(0x7,readApcByte(0x7)|0x0a);
          	
    /* disable APC in south bridge */
    outl(0x80001048,0xcf8); 
    outl(inl(0xcfc)&0xffffffbf,0xcfc);

    // CFG reg0x73 bit=1, tell driver MAC Address load to APC 	
    bTmp = pci_read_config8(dev, 0x73);			
    bTmp|=0x1;
    pci_write_config8(dev, 0x73, bTmp);
}

//-----------------------------------------------------------------------------
// Procedure:   ReadEEprom
//
// Description: This routine serially reads one word out of the EEPROM.
//
// Arguments:
//      Reg - EEPROM word to read.
//
// Returns:
//      Contents of EEPROM word (Reg).
//-----------------------------------------------------------------------------
#define LoopNum 200
static  unsigned long ReadEEprom( struct device *dev,  uint32_t base,  uint32_t Reg)
{
    uint16_t 	data;
    uint32_t 	i;
    uint32_t 	ulValue;
    
    
    ulValue = (0x80 | (0x2 << 8) | (Reg << 10));  //BIT_7

    writel( ulValue,base+0x3c);
		
    mdelay(10);

    for(i=0 ; i <= LoopNum; i++)
    {
        ulValue=readl(base+0x3c);

        if(!(ulValue & 0x0080)) //BIT_7
            break;

        mdelay(100);
    }
	
    mdelay(50);

    if(i==LoopNum)   data=0x10000;
    else{
    	ulValue=readl(base+0x3c);
    	data = (uint16_t)((ulValue & 0xffff0000) >> 16);
    }			
    
    return data;
}

static int phy_read(uint32_t  base, unsigned phy_addr, unsigned phy_reg)
{
    uint32_t   ulValue;
    unsigned  loop = 0x100;
    uint32_t   Read_Cmd;
    uint16_t   usData;

    uint16_t   tmp;
	 

	   Read_Cmd = ((phy_reg << 11) |	
                       (phy_addr << 6) |
     		       SMI_READ |
     		       SMI_REQUEST);

           // SmiMgtInterface Reg is the SMI management interface register(offset 44h) of MAC
          writel( Read_Cmd,base+0x44);
          //outl( Read_Cmd,tmp+0x44);
           
           // Polling SMI_REQ bit to be deasserted indicated read command completed
           do
           {
	       // Wait 20 usec before checking status
    	       //StallAndWait(20);
		   mdelay(20);
    	       ulValue = readl(base+0x44);	    
    	       //ulValue = inl(tmp+0x44);	
           } while((ulValue & SMI_REQUEST) != 0);
            //printk_debug("base %x cmd %lx ret val %lx\n", tmp,Read_Cmd,ulValue);          
           usData=(ulValue>>16);

	

	return usData;

}

// Detect a valid PHY
// If there exist a valid PHY then return TRUE, else return FALSE
static int phy_detect(uint32_t base,uint16_t *PhyAddr) //BOOL PHY_Detect()
{
    int	              bFoundPhy = FALSE;
    uint32_t		Read_Cmd;
    uint16_t		usData;
    int		       PhyAddress = 0;
   
  
        // Scan all PHY address(0 ~ 31) to find a valid PHY
        for(PhyAddress = 0; PhyAddress < 32; PhyAddress++)
        {  
		usData=phy_read(base,PhyAddress,StatusReg);  // Status register is a PHY's register(offset 01h)      
           
           // Found a valid PHY 
     
           if((usData != 0x0) && (usData != 0xffff))
           {
               bFoundPhy = TRUE;
               break;
           }
        }
//        printk_debug(" PHY_Addr=%x\n",PhyAddress);

	//usData=phy_read(base,PhyAddress,0x0);
	//printk_debug("PHY=%x\n",usData);

	if(!bFoundPhy)
	{
	    printk_debug("PHY not found !!!! \n");
	   // DisableMac();
	}

       *PhyAddr=PhyAddress;
	
	return bFoundPhy;
}


static void nic_init(struct device *dev)
{
	uint32_t dword, old;
	uint32_t mac_h, mac_l;
	int eeprom_valid = 0;
	int val;
	uint16_t  PhyAddr;
	struct southbridge_sis_sis966_config *conf;

	static uint32_t nic_index = 0;

	uint32_t base;
	struct resource *res;
       uint32_t reg;
	
	
printk_debug("SIS NIC init-------->\r\n");


//-------------- enable NIC (SiS19x) -------------------------
{
        uint8_t  temp8;
        int i=0;
        while(SiS_SiS191_init[i][0] != 0)
	{				temp8 = pci_read_config8(dev, SiS_SiS191_init[i][0]);
					temp8 &= SiS_SiS191_init[i][1];
					temp8 |= SiS_SiS191_init[i][2];				
					pci_write_config8(dev, SiS_SiS191_init[i][0], temp8);
					i++;
	};
}
//-----------------------------------------------------------




{
unsigned long  i;
unsigned long ulValue;

#if 0
for(i=0;i<0xFF;i+=4)
{    if((i%16)==0)
    {print_debug("\r\n");print_debug_hex8(i);print_debug("  ");}
    print_debug_hex32(pci_read_config32(dev,i));
    print_debug("  ");    
}
print_debug("\r\n");
#endif
	res = find_resource(dev, 0x10);

	if(!res) return;

	base = res->base;
printk_debug("NIC base address %lx\n",base);
	if(!(val=phy_detect(base,&PhyAddr)))  
	  {
	      printk_debug("PHY detect fail !!!!\r\n"); 
		return;		  
	   }	

#if 0
//------------ show op registers ----------------------
{
//device_t dev;
int i;
//dev = pci_locate_device(PCI_ID(0x1039, 0x5513), 0); 
printk_debug("NIC OP Registers \n");
for(i=0;i<0xFF;i+=4)
{    if((i%16)==0)
    {print_debug("\r\n");print_debug_hex8(i);print_debug("  ");}
    print_debug_hex32(readl(base+i));
    print_debug("  ");    
}

}

//----------------------------------------------------
#endif

    ulValue=readl(base + 0x38L);   //  check EEPROM existing
   
    if((ulValue & 0x0002))
    {       
         
     //	read MAC address from EEPROM at first
	
          //	if that is valid we will use that
			
			printk_debug("EEPROM contents %x \n",ReadEEprom( dev,  base,  0LL));
			for(i=0;i<3;i++) {
				//status = smbus_read_byte(dev_eeprom, i);
				ulValue=ReadEEprom( dev,  base,  i+3L);
				if (ulValue ==0x10000) break;  // error 
				
				MacAddr[i] =ulValue & 0xFFFF;	
				
			}
		 
    }else{
             // read MAC address from firmware
		 printk_debug("EEPROM invalid!!\nReg 0x38h=%.8lx \n",ulValue);
		 MacAddr[0]=readw(0xffffffc0); // mac address store at here
		 MacAddr[1]=readw(0xffffffc2);
		 MacAddr[2]=readw(0xffffffc4);
    }	


#if 0
//	read MAC address from EEPROM at first
printk_debug("MAC address in firmware trap \n");
 for( i=0;i<3;i++)
 	printk_debug(" %4x\n",MacAddr[i]);
 printk_debug("\n");      
#endif

set_apc(dev);
 	
readApcMacAddr();

#if 0
{
//device_t dev;
int i;
//dev = pci_locate_device(PCI_ID(0x1039, 0x5513), 0); 
printk_debug("NIC PCI config \n");
for(i=0;i<0xFF;i+=4)
{    if((i%16)==0)
    {print_debug("\r\n");print_debug_hex8(i);print_debug("  ");}
    print_debug_hex32(pci_read_config32(dev,i));
    print_debug("  ");    
}

}
#endif

}

printk_debug("nic_init<--------\r\n");
return;

#define RegStationMgtInf	0x44
#define PHY_RGMII	0x10000000

	writel(PHY_RGMII, base + RegStationMgtInf);
	conf = dev->chip_info;

	if(conf->mac_eeprom_smbus != 0) {
//	read MAC address from EEPROM at first

		struct device *dev_eeprom;
		dev_eeprom = dev_find_slot_on_smbus(conf->mac_eeprom_smbus, conf->mac_eeprom_addr);

		if(dev_eeprom) {
		//	if that is valid we will use that
			unsigned char dat[6];
			int status;
			int i;
			for(i=0;i<6;i++) {
				status = smbus_read_byte(dev_eeprom, i);
				if(status < 0) break;
				dat[i] = status & 0xff;
			}
			if(status >= 0) {
				mac_l = 0;
				for(i=3;i>=0;i--) {
					mac_l <<= 8;
					mac_l += dat[i];
				}
				if(mac_l != 0xffffffff) {
					mac_l += nic_index;
					mac_h = 0;
					for(i=5;i>=4;i--) {
						mac_h <<= 8;
						mac_h += dat[i];
					}
					eeprom_valid = 1;
				}
			}
		}
	}
//	if that is invalid we will read that from romstrap
	if(!eeprom_valid) {
		unsigned long mac_pos;
		mac_pos = 0xffffffd0; // refer to romstrap.inc and romstrap.lds
		mac_l = readl(mac_pos) + nic_index; // overflow?
		mac_h = readl(mac_pos + 4);

	}
#if 1
//	set that into NIC MMIO
#define NvRegMacAddrA	0xA8
#define NvRegMacAddrB	0xAC
	writel(mac_l, base + NvRegMacAddrA);
	writel(mac_h, base + NvRegMacAddrB);
#else
//	set that into NIC
	pci_write_config32(dev, 0xa8, mac_l);
	pci_write_config32(dev, 0xac, mac_h);
#endif

	nic_index++;

#if CONFIG_PCI_ROM_RUN == 1
	pci_dev_init(dev);// it will init option rom
#endif

}



#else // orginal code

tatic int phy_read(uint8_t *base, unsigned phy_addr, unsigned phy_reg)
{
	uint32_t dword;
	unsigned loop = 0x100;
	writel(0x8000, base+0x190); //Clear MDIO lock bit
	mdelay(1);
	dword = readl(base+0x190);
	if(dword & (1<<15)) return -1;

	writel(1, base+0x180);
	writel((phy_addr<<5) | (phy_reg),base + 0x190);
	do{
		dword = readl(base + 0x190);
		if(--loop==0) return -4;
	} while ((dword & (1<<15)) );

	dword = readl(base + 0x180);
	if(dword & 1) return -3;

	dword = readl(base + 0x194);

	return dword;

}

static int phy_detect(uint8_t *base)
{
	uint32_t dword;
	int i;
	int val;
	unsigned id;
	dword = readl(base+0x188);
	dword &= ~(1<<20);
	writel(dword, base+0x188);

	phy_read(base, 0, 1);

	for(i=1; i<=32; i++) {
		int phyaddr = i & 0x1f;
		val = phy_read(base, phyaddr, 1);
		if(val<0) continue;
		if((val & 0xffff) == 0xfffff) continue;
		if((val & 0xffff) == 0) continue;
		if(!(val & 1)) {
			break; // Ethernet PHY
		}
		val = phy_read(base, phyaddr, 3);
		if (val < 0 || val == 0xffff) continue;
		id = val & 0xfc00;
		val = phy_read(base, phyaddr, 2);
		if (val < 0 || val == 0xffff) continue;
		id |= ((val & 0xffff)<<16);
		printk_debug("SIS966 MAC PHY ID 0x%08x PHY ADDR %d\n", id, i);
//		if((id == 0xe0180000) || (id==0x0032cc00))
			break;
	}

	if(i>32) {
		printk_debug("SIS966 MAC PHY not found\n");
	}

}
static void nic_init(struct device *dev)
{
	uint32_t dword, old;
	uint32_t mac_h, mac_l;
	int eeprom_valid = 0;
	struct southbridge_sis_sis966_config *conf;

	static uint32_t nic_index = 0;

	uint8_t *base;
	struct resource *res;
	
	res = find_resource(dev, 0x10);

	if(!res) return;

	base = res->base;

	phy_detect(base);

#define NvRegPhyInterface	0xC0
#define PHY_RGMII	0x10000000

	writel(PHY_RGMII, base + NvRegPhyInterface);

	conf = dev->chip_info;

	if(conf->mac_eeprom_smbus != 0) {
//	read MAC address from EEPROM at first
		struct device *dev_eeprom;
		dev_eeprom = dev_find_slot_on_smbus(conf->mac_eeprom_smbus, conf->mac_eeprom_addr);

		if(dev_eeprom) {
		//	if that is valid we will use that
			unsigned char dat[6];
			int status;
			int i;
			for(i=0;i<6;i++) {
				status = smbus_read_byte(dev_eeprom, i);
				if(status < 0) break;
				dat[i] = status & 0xff;
			}
			if(status >= 0) {
				mac_l = 0;
				for(i=3;i>=0;i--) {
					mac_l <<= 8;
					mac_l += dat[i];
				}
				if(mac_l != 0xffffffff) {
					mac_l += nic_index;
					mac_h = 0;
					for(i=5;i>=4;i--) {
						mac_h <<= 8;
						mac_h += dat[i];
					}
					eeprom_valid = 1;
				}
			}
		}
	}
//	if that is invalid we will read that from romstrap
	if(!eeprom_valid) {
		unsigned long mac_pos;
		mac_pos = 0xffffffd0; // refer to romstrap.inc and romstrap.lds
		mac_l = readl(mac_pos) + nic_index; // overflow?
		mac_h = readl(mac_pos + 4);

	}
#if 1
//	set that into NIC MMIO
#define NvRegMacAddrA	0xA8
#define NvRegMacAddrB	0xAC
	writel(mac_l, base + NvRegMacAddrA);
	writel(mac_h, base + NvRegMacAddrB);
#else
//	set that into NIC
	pci_write_config32(dev, 0xa8, mac_l);
	pci_write_config32(dev, 0xac, mac_h);
#endif

	nic_index++;

#if CONFIG_PCI_ROM_RUN == 1
	pci_dev_init(dev);// it will init option rom
#endif

}

#endif
static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem	= lpci_set_subsystem,
};

static struct device_operations nic_ops  = {
	.read_resources	= pci_dev_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init		= nic_init,
	.scan_bus	= 0,
//	.enable		= sis966_enable,
	.ops_pci	= &lops_pci,
};
static const struct pci_driver nic_driver __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_NIC1,
};
static const struct pci_driver nic_bridge_driver __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_SIS,
	.device	= PCI_DEVICE_ID_SIS_SIS966_NIC_BRIDGE,
};
