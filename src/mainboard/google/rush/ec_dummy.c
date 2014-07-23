/* Dummy CHROMEEC file to provide stub functions for vboot compilation */

#include <stdint.h>

int google_chromeec_vbnv_context(int is_read, uint8_t *data, int len);
int google_chromeec_vbnv_context(int is_read, uint8_t *data, int len)
{
	return 0;
}
