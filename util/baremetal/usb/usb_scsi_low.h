#ifndef _USB_SCSI_LOW_H
#define _USB_SCSI_LOW_H

#define SG_DXFER_FROM_DEV -3
#define SG_DXFER_TO_DEV -2

int scsi_command( unsigned char device, unsigned char *cmd, int cmd_len, int direction, unsigned char *data, int data_len, char *sense_data, int sense_len);
int request_sense( unsigned char device, char *sense_data, int len);

#endif
