#include "scsi_low.h"

typedef struct sense_data {
	uchar code;
	
	uchar sense_key:4;
	uchar res1:4;

	uchar additional_code;
	uchar qualifier;

	uchar res2[3];

	uchar length;
} __attribute__ ((packed)) sense_data_t;

typedef struct fixed_sense_data {
	uchar code:7;
	uchar valid:1;

	uchar obs1;

	uchar sense_key:4;
	uchar res1:1;
	uchar ili:1;
	uchar eom:1;
	uchar mark:1;

	unsigned int  info;

	uchar add_len;
} __attribute__ ((packed)) fixed_sense_data_t;

typedef struct additional_fixed_data {
	unsigned int info;
	
	uchar code;
	uchar qualifier;
	uchar fru;

	uchar specific[3];
} __attribute__ ((packed)) additional_fixed_data_t;
	

void PrintSense(uchar *sense)
{
	int i;

	DPRINTF("sense data ");
	for(i=0;i<32; i++)
		DPRINTF(  ":%02x", sense[i]);
	DPRINTF( "\n\n");

	if( (sense[0] & 0x7f) >=0x72) {
		sense_data_t *sd = (sense_data_t *) sense;
		uchar *pos = sense+sizeof(sense_data_t);
		uchar remaining = sd->length;
		int dlen;
		
		DPRINTF( "code = %02x, key = %1x, additional = %02x, qual = %02x\n", sd->code, sd->sense_key, sd->additional_code, sd->qualifier);

		while(remaining) {
			DPRINTF("type = %02x", pos[0]);
			dlen = pos[1];
			pos+=2;
			remaining -=2;

			for(i=0; i<dlen; i++) 
				DPRINTF( ": %02x", pos[i]);

			DPRINTF("\n");
			pos+=i;
			remaining -=i;
		}

	} else {
		fixed_sense_data_t *fd = (fixed_sense_data_t *) sense;
		uchar remaining = fd->add_len;
		additional_fixed_data_t *afd;

		
		DPRINTF("code = %02x key = %1x\n", fd->code, fd->sense_key);
		if(fd->mark)
			DPRINTF("filemark ");

		if(fd->eom)
			DPRINTF(" End Of Media ");

		if(fd->ili)
			DPRINTF("Illegal instruction");

		DPRINTF(out,"\n");

		if(fd->valid)
			DPRINTF("(valid) ");

		DPRINTF("Info: %08x\n", ntohl(fd->info));

		afd = (additional_fixed_data_t *) (sense + 8);

		while(remaining) {
			DPRINTF( "command info = %08x\n", ntohl(afd->info));
			DPRINTF( "code = %02x, qual = %02x, fru = %02x\n", afd->code, afd->qualifier, afd->fru);
			DPRINTF( "sense key data = %02x:%02x:%02x\n\n", afd->specific[2], afd->specific[1], afd->specific[0]);

			afd++;
			remaining -= sizeof(additional_fixed_data_t);
		}
	}
		
}
	
typedef struct query_response {
	uchar type:5;
	uchar qualifier:3;

	uchar reserved1:7;
	uchar removable:1;

	uchar version;

	uchar ResponseDataFormat:4;	// should == 2
	uchar HiSup:1;			// report luns cmd supported
	uchar NormACA:1;
	uchar obsolete:1;
	uchar aerc:1;

	uchar AdditionalLength;		// length of vendor specific data (beyond 96 bytes)

	uchar reserved2:7;
	uchar sccs:1;			// have raid controller

	uchar addr16:1;			// 
	uchar obsolete2:2;
	uchar MChnger:1;		// media changer
	uchar MultiP:1;			// multi port
	uchar vs:1;			// ???
	uchar EncServ:1;		// enclosure service
	uchar BQue:1;			// basic command queueing

	uchar vs2:1;
	uchar CmdQue:1;			// full command queueing
	uchar obsolete4:1;
	uchar linked:1;
	uchar sync:1;
	uchar wbus16:1;			//
	uchar obsolete3:1;
	uchar RelAddr:1;		// treletive addressing

	char vendor[8];
	char product[16];
	char revision[4];
	char vendor_data[20];

	uchar ius:1;
	uchar qas:1;
	uchar clocking:2;		// 
	uchar reserved3:4;

	unsigned short version_desc[8];

	char reserved4[21];
} query_response_t;

typedef struct ReadBlockCMD {
	uchar cmd;

	uchar reladdr:1;
	uchar reserved:2;
	uchar fua:1;		// force unit access flush to media
	uchar dpo:1;		// direct page out, do not cache
	uchar reserved2:3;

	unsigned int block_address;
	uchar reserved3;
	
	unsigned short block_count;

	uchar control;
} __attribute__ ((packed)) ReadBlockCMD_t ;

int ll_read_block(devhandle sgd, char *buffer, int blocknum, int count)
{
	int ret;
	ReadBlockCMD_t rb;
	char sensedat[32];

	memset(&rb,0,sizeof(rb));
	rb.cmd = READ_10;
	rb.block_address = htonl(blocknum);
	rb.block_count = htons(count);
	
	ret = scsi_command( sgd, &rb, sizeof(rb), SG_DXFER_FROM_DEV,  buffer, count * 512, sensedat, sizeof(sensedat));
	return(ret);

}

int ll_write_block(devhandle sgd, char *buffer, int blocknum, int count)
{
	int ret;
	ReadBlockCMD_t rb;
	char sensedat[32];

	memset(&rb,0,sizeof(rb));
	rb.cmd = WRITE_10;
	rb.block_address = htonl(blocknum);
	rb.block_count = htons(count);
	
	ret = scsi_command( sgd, &rb, sizeof(rb), SG_DXFER_TO_DEV,  buffer, count * 512, sensedat, sizeof(sensedat));

	return(ret);
}

typedef struct ReadLongCMD {
	uchar cmd;

	uchar reladdr:1;
	uchar correct:1;
	uchar reserved:5;

	unsigned int block_address;
	uchar reserved3;
	
	unsigned short length;

	uchar control;
} __attribute__ ((packed)) ReadLongCMD_t ;

int ll_read_long(devhandle sgd, char *buffer, int blocknum, int size)
{
	int ret;
	ReadLongCMD_t rb;
	char sensedat[32];

	memset(&rb,0,sizeof(rb));
	rb.cmd = READ_LONG;
	rb.block_address = htonl(blocknum);
	rb.length = htons(size);
	
	ret = scsi_command( sgd, &rb, sizeof(rb), SG_DXFER_FROM_DEV,  buffer, size, sensedat, sizeof(sensedat));
	return(ret);
}

unsigned char ReadCapacityCMD[10] = { READ_CAPACITY, 0, 0,0,0,0, 0,0,0, 0};

struct ReadCapacityResponse {
	unsigned int block_address;
	unsigned int block_length;
};

int get_capacity(devhandle sgd, unsigned long *block_count, unsigned int *blk_len)
{
	int ret;
	struct ReadCapacityResponse response;
	char sensedat[32];

	ret = scsi_command(sgd, ReadCapacityCMD, sizeof(ReadCapacityCMD), SG_DXFER_FROM_DEV, &response, sizeof(response), sensedat, sizeof(sensedat) );
	if(ret<0)
		perror("get capacity: IOCTL");


	*block_count = ntohl(response.block_address) +1;
	*blk_len = ntohl(response.block_length);

	return(ret);
}

#define INQ_REP_LEN 96
unsigned char InquiryCMD[6] = { INQUIRY, 0, 0, 0, INQ_REP_LEN, 0};

int query(devhandle sgd, query_response_t *qr)
{
	int ret;
	char sensedat[32];

	ret = scsi_command(sgd, InquiryCMD, sizeof(InquiryCMD), SG_DXFER_FROM_DEV, qr, sizeof(query_response_t), sensedat, sizeof(sensedat) );

	if(ret<0)
		perror("query: IOCTL");

	return(ret);
}

typedef struct lun_list {
	unsigned int list_length;
	unsigned int reserved;
	unsigned long long lun[16];
} lun_list_t;

#define REPORT_LUNS 0xa0
unsigned char ReportLunsCMD[12] = { REPORT_LUNS, 0, 2, 0, 0, 0, 0, 0, 0, 128, 0, 0 };

int ReportLUNS(devhandle sgd, lun_list_t *list)
{
	int ret;
	char sensedat[32];

	memset (list, 0, sizeof(lun_list_t));
	ret = scsi_command(sgd, ReportLunsCMD, sizeof(ReportLunsCMD), SG_DXFER_FROM_DEV, list, sizeof(lun_list_t), sensedat, sizeof(sensedat) );

	if(ret<0)
		perror("Report Luns: IOCTL");

	list->list_length = ntohl(list->list_length);

	return(ret);
}

typedef struct command_descriptor {
	uchar opcode;
	uchar reserved;
	unsigned short service_action;
	uchar reserved2;

	uchar action_valid:1;
	uchar reserved3:7;

	unsigned short cdb_len;
} __attribute__ ((packed)) command_descriptor_t;

typedef struct report_opcodes_result {
	unsigned long length;

	command_descriptor_t command[256];
} __attribute__ ((packed)) report_opcode_result_t;


#define REPORT_OPCODES 0xa3

typedef struct report_opcodes_cmd {
	uchar cmd;
	uchar reserved[5];
	unsigned int reply_len;
	uchar reserved2;
	uchar control;
} __attribute__ ((packed)) ReportOpcodesCMD_t;

//ReportOpcodesCMD_t ReportOpcodesCMD = { cmd : REPORT_OPCODES, reply_len: htonl(sizeof(report_opcode_result_t)) };

int ReportOpCodes(devhandle sgd, report_opcode_result_t *list)
{
	int ret;
	char sensedat[32];
	ReportOpcodesCMD_t ReportOpcodesCMD;

	memset (list, 0, sizeof(report_opcode_result_t));
	ReportOpcodesCMD.cmd = REPORT_OPCODES;
	ReportOpcodesCMD.reply_len = htonl( sizeof(report_opcode_result_t));

	ret = scsi_command(sgd, &ReportOpcodesCMD, sizeof(ReportOpcodesCMD_t), SG_DXFER_FROM_DEV, list, sizeof(report_opcode_result_t), sensedat, sizeof(sensedat) );

	if(ret<0)
		perror("Report Luns: IOCTL");

	list->length = ntohl(list->length);

	return(ret);
}


#define READ_ATTRIBUTE 0x8c
#define VOLUME_LIST 2
#define PARTITION_LIST 3

typedef struct read_attribute_cmd {
	uchar cmd;
	
	uchar action:5;
	uchar res:3;

	uchar restricted[3];

	uchar volume;
	uchar res2;
	uchar partition;

	ushort attribute;
	unsigned int reply_len;
	uchar res3;
	uchar control;
} __attribute__ ((packed)) ReadAttributeCMD_t;

int CheckVolumes(devhandle sgd)
{
	int ret;
	uchar reply[4];
	uchar sensedat[32];
	ReadAttributeCMD_t cmd;

	memset(&cmd,0,sizeof(cmd));

	cmd.cmd=READ_ATTRIBUTE;
	cmd.action = VOLUME_LIST;
	cmd.reply_len = htonl(4);

	ret = scsi_command(sgd, &cmd, sizeof(cmd), SG_DXFER_FROM_DEV, reply, sizeof(reply), sensedat, sizeof(sensedat) );
	if(ret<0) {
		perror("Report Volumes: IOCTL");
		return(-1);
	}

	if(! reply[0] && !reply[1])
		return(0);

	return(reply[3]);
}

int CheckPartitions(devhandle sgd)
{
	int ret;
	uchar reply[4];
	uchar sensedat[32];
	ReadAttributeCMD_t cmd;

	memset(&cmd,0,sizeof(cmd));

	cmd.cmd=READ_ATTRIBUTE;
	cmd.action = PARTITION_LIST;
	cmd.reply_len = htonl(4);

	ret = scsi_command(sgd, &cmd, sizeof(cmd), SG_DXFER_FROM_DEV, reply, sizeof(reply), sensedat, sizeof(sensedat) );
	if(ret<0) {
		perror("Report PARTITIONVolumes: IOCTL");
		return(-1);
	}

	if(! reply[0] && !reply[1])
		return(0);

	return(reply[3]);
}

int UnitReady(devhandle sgd)
{
	uchar cmd[6];
	uchar sensedat[32];
	int ret;

	memset(cmd,0,sizeof(cmd));
	
	ret = scsi_command(sgd, &cmd, sizeof(cmd), SG_DXFER_FROM_DEV, NULL, 0, sensedat, sizeof(sensedat) );
	if(ret<0) {
		perror("UnitReady :");
		return(0);
	}

	return(1);
}
	

