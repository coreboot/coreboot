
#include <AGESA.h>
#include <AMD.h>
#include <heapManager.h>

#include "debug_util.h"

static const char undefined[] = "undefined";

static const char *HeapStatusStr[] = {
	"DoNotExistYet", "LocalCache", "TempMem", "SystemMem", "DoNotExistAnymore","S3Resume"
};

/* This function has to match with enumeration of XXXX defined
 * inside heapManager.h header file.
 */
const char *heap_status_name(UINT8 HeapStatus)
{
	if ((HeapStatus < HEAP_DO_NOT_EXIST_YET) || (HeapStatus > HEAP_S3_RESUME))
		return undefined;

	int index = HeapStatus - HEAP_DO_NOT_EXIST_YET;
	return HeapStatusStr[index];
}
