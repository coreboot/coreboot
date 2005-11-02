#ifndef _ppc970lib_h_
#define _ppc970lib_h_

/*----------------------------------------------------------------------------+
| Time base structure.
+----------------------------------------------------------------------------*/
typedef struct tb {
   unsigned long   			tb_all;	
} tb_t;

/*----------------------------------------------------------------------------+
| 970FX specific ppc prototypes.
+----------------------------------------------------------------------------*/
void ppcMfvscr(
					void );

void ppcMtvscr(
     					void );

int ppcMfvr(
    unsigned int			reg_num,
    unsigned long			*data_msb,
    unsigned long			*data_lsb );

int ppcMtvr(
    unsigned int			reg_num,
    unsigned long			data_msb,
    unsigned long			data_lsb );

void ppcLvxl(
     unsigned int			reg_num,
     void				*addr );

void ppcStvx(
     unsigned int			reg_num,
     void				*addr );

unsigned long ppcMflr(
					void );

unsigned char inbyte(
	      unsigned long		addr );

void outbyte(
     unsigned long			addr,
     unsigned int			data );

unsigned short inhalf(
	       unsigned long		addr );

void outhalf(
     unsigned long			addr,
     unsigned int			data );

unsigned short inhalf_brx(
	       unsigned long		addr );

void outhalf_brx(
     unsigned long			addr,
     unsigned int			data );

unsigned long inword(
	      unsigned long		addr );

void outword(
     unsigned long			addr,
     unsigned long			data );

unsigned int inint(
	     unsigned long		addr );

void outint(
     unsigned long			addr,
     unsigned int			data );

unsigned int inint_brx(
	     unsigned long		addr );

void outint_brx(
     unsigned long			addr,
     unsigned int			data );

void ppcDflush(
  					void );

void ppcDcbz_area(
     unsigned long			addr,
     unsigned long			len );

unsigned long ppcTlbsync(
					void );

unsigned long ppcTlbie(
              unsigned long 		vaddr,
	      int			large_page );

void ppcTlbiel(
     unsigned long 			vaddr );

void ppcSlbie(
     unsigned long 		 	rb );

void ppcSlbia(
					void );

void ppcSlbmte(
     unsigned long			rs,
     unsigned long			rb );

unsigned long ppcSlbmfev(
     	      int			index );

unsigned long ppcSlbmfee(
     	      int			index );

void ppcAbend(
					void );

unsigned long ppcAndMsr(
              unsigned long 		value );

unsigned int ppcCntlzw(
             unsigned int 		value );

unsigned int ppcCntlzd(
             unsigned long 		value );

void ppcDcbf(
     void 				*addr );

void ppcDcbst(
     void 				*addr );

void ppcDcbz(
     void 				*addr );

void ppcHalt(
     					void );

void ppcIcbi(
     void 				*addr );

void ppcIsync(
     					void );

unsigned long ppcMfgpr1(
					void );

unsigned long ppcMfgpr2(
					void );

void ppcMtmsr(
     unsigned long 			msr_value );

unsigned long ppcMfmsr(
					void );

unsigned long ppcOrMsr(
              unsigned long 		value );

void ppcSync(
     					void );

void ppcLwsync(
     					void );

void ppcPtesync(
     					void );

void ppcEieio(
     					void );

void ppcTestandset(
     unsigned long			addr,
     unsigned long			value );

unsigned long ppcMfscom(
              unsigned int 		scom_num );

void ppcMtscom(
     unsigned int 			scom_num,
     unsigned long 			scom_data );

/*----------------------------------------------------------------------------+
| 970FX SPR's.
+----------------------------------------------------------------------------*/
void ppcMthid0(
     unsigned long 			data );

void ppcMthid1(
     unsigned long 			data );

void ppcMthid4(
     unsigned long 			data );

void ppcMthid5(
     unsigned long 			data );

void ppcMftb(
     tb_t 				*clock_data );

void ppcMttb(
     tb_t 				*clock_data );

void ppcMtspr_any(
     unsigned int                       spr_num,
     unsigned long                      value );

unsigned long ppcMfspr_any(
              unsigned int              spr_num );

/*----------------------------------------------------------------------------+
| Additional functions required by debug connection.
+----------------------------------------------------------------------------*/
int ppcCachelinesize(
                                        void );

unsigned long ppcProcid(
                                        void );

void ppcMtmmucr(
     unsigned long                      data );

void ppcMttlb1(
     unsigned long                      index,
     unsigned long                      value );

void ppcMttlb2(
     unsigned long                      index,
     unsigned long                      value );

void ppcMttlb3(
     unsigned long                      index,
     unsigned long                      value );

unsigned long ppcMftlb1(
              unsigned long             index );

unsigned long ppcMftlb2(
              unsigned long             index );

unsigned long ppcMftlb3(
              unsigned long             index );

unsigned long ppcMfmmucr(
                                        void );

unsigned long ppcMfdcr_any(
              unsigned long             dcr_num );

unsigned long ppcMfspr_any_name(
              char                      *name,
              unsigned long	 	*value_msb );

void ppcMtdcr_any(
     unsigned long                      dcr_num,
     unsigned long                      value );

void ppcMtspr_any_name(
     char                               *name,
     unsigned long                      value_lsb,
     unsigned long			value_msb );

int ppcIstrap(
                                        void );

unsigned long p_ptegg(
	      int			lp,
	      unsigned long		ea,
	      unsigned long		sdr1,
	      unsigned long		vsid );

unsigned long s_ptegg(
	      int			lp,
	      unsigned long		ea,
	      unsigned long		sdr1,
	      unsigned long		vsid );

#endif /* _ppc970lib_h_ */
