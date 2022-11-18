/* SPDX-License-Identifier: GPL-2.0-only */

#pragma pack(push)
#include <AGESA.h>
#pragma pack(pop)

#include <amdlib.h>

#include <cbmem.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>

#include <acpi/acpi.h>
#include <console/console.h>
#include <string.h>

/* BIOS_HEAP_START_ADDRESS is only for cold boots. */
#define BIOS_HEAP_SIZE		0x30000
#define BIOS_HEAP_START_ADDRESS	0x010000000

#if CONFIG(HAVE_ACPI_RESUME) && (HIGH_MEMORY_SCRATCH < BIOS_HEAP_SIZE)
#error Increase HIGH_MEMORY_SCRATCH allocation
#endif

void *GetHeapBase(void)
{
	void *heap = (void *)BIOS_HEAP_START_ADDRESS;

	if (acpi_is_wakeup_s3()) {
		/* FIXME: For S3 resume path, buffer is in CBMEM
		 * with some arbitrary header. */
		heap = cbmem_find(CBMEM_ID_RESUME_SCRATCH);
		heap += 0x10;
	}

	return heap;
}

void EmptyHeap(void)
{
	void *base = GetHeapBase();
	memset(base, 0, BIOS_HEAP_SIZE);

	printk(BIOS_DEBUG, "Wiped HEAP at [%08x - %08x]\n",
		(unsigned int)(uintptr_t)base, (unsigned int)(uintptr_t)base + BIOS_HEAP_SIZE - 1);
}

#if defined(HEAP_CALLOUT_RUNTIME) && ENV_RAMSTAGE

#define AGESA_RUNTIME_SIZE 4096
static AGESA_STATUS alloc_cbmem(AGESA_BUFFER_PARAMS *AllocParams)
{
	static unsigned int used = 0;
	void *p = cbmem_find(CBMEM_ID_AGESA_RUNTIME);

	if ((AGESA_RUNTIME_SIZE - used) < AllocParams->BufferLength) {
		return AGESA_BOUNDS_CHK;
	}

	/* first time allocation */
	if (!p) {
		p = cbmem_add(CBMEM_ID_AGESA_RUNTIME, AGESA_RUNTIME_SIZE);
		if (!p)
			return AGESA_BOUNDS_CHK;
	}

	AllocParams->BufferPointer = p + used;
	used += AllocParams->BufferLength;
	return AGESA_SUCCESS;
}
#endif

typedef struct _BIOS_HEAP_MANAGER {
	UINT32 StartOfAllocatedNodes;
	UINT32 StartOfFreedNodes;
} BIOS_HEAP_MANAGER;

typedef struct _BIOS_BUFFER_NODE {
	UINT32 BufferHandle;
	UINT32 BufferSize;
	UINT32 NextNodeOffset;
} BIOS_BUFFER_NODE;

static AGESA_STATUS agesa_AllocateBuffer(BIOS_HEAP_MANAGER *BiosHeapBasePtr,
	AGESA_BUFFER_PARAMS *AllocParams)
{
	UINT32              AvailableHeapSize;
	UINT8               *BiosHeapBaseAddr = (void *)BiosHeapBasePtr;
	UINT32              CurrNodeOffset;
	UINT32              PrevNodeOffset;
	UINT32              FreedNodeOffset;
	UINT32              BestFitNodeOffset;
	UINT32              BestFitPrevNodeOffset;
	UINT32              NextFreeOffset;
	BIOS_BUFFER_NODE   *CurrNodePtr;
	BIOS_BUFFER_NODE   *FreedNodePtr;
	BIOS_BUFFER_NODE   *BestFitNodePtr;
	BIOS_BUFFER_NODE   *BestFitPrevNodePtr;
	BIOS_BUFFER_NODE   *NextFreePtr;

	AllocParams->BufferPointer = NULL;
	AvailableHeapSize = BIOS_HEAP_SIZE - sizeof(BIOS_HEAP_MANAGER);

	if (BiosHeapBasePtr->StartOfAllocatedNodes == 0) {
		/* First allocation */
		CurrNodeOffset = sizeof(BIOS_HEAP_MANAGER);
		CurrNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + CurrNodeOffset);
		CurrNodePtr->BufferHandle = AllocParams->BufferHandle;
		CurrNodePtr->BufferSize = AllocParams->BufferLength;
		CurrNodePtr->NextNodeOffset = 0;
		AllocParams->BufferPointer = (UINT8 *)CurrNodePtr + sizeof(BIOS_BUFFER_NODE);

		/* Update the remaining free space */
		FreedNodeOffset = CurrNodeOffset + CurrNodePtr->BufferSize + sizeof(BIOS_BUFFER_NODE);
		FreedNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + FreedNodeOffset);
		FreedNodePtr->BufferSize = AvailableHeapSize
					- (FreedNodeOffset - CurrNodeOffset)
					- sizeof(BIOS_BUFFER_NODE);
		FreedNodePtr->NextNodeOffset = 0;

		/* Update the offsets for Allocated and Freed nodes */
		BiosHeapBasePtr->StartOfAllocatedNodes = CurrNodeOffset;
		BiosHeapBasePtr->StartOfFreedNodes = FreedNodeOffset;
	} else {
		/* Find out whether BufferHandle has been allocated on the heap.
		 * If it has, return AGESA_BOUNDS_CHK.
		 */
		CurrNodeOffset = BiosHeapBasePtr->StartOfAllocatedNodes;
		CurrNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + CurrNodeOffset);

		while (CurrNodeOffset != 0) {
			CurrNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + CurrNodeOffset);
			if (CurrNodePtr->BufferHandle == AllocParams->BufferHandle) {
				return AGESA_BOUNDS_CHK;
			}
			CurrNodeOffset = CurrNodePtr->NextNodeOffset;
			/* If BufferHandle has not been allocated on the heap, CurrNodePtr here points
			 * to the end of the allocated nodes list.
			 */
		}
		/* Find the node that best fits the requested buffer size */
		FreedNodeOffset = BiosHeapBasePtr->StartOfFreedNodes;
		PrevNodeOffset = FreedNodeOffset;
		BestFitNodeOffset = 0;
		BestFitPrevNodeOffset = 0;
		while (FreedNodeOffset != 0) {
			FreedNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + FreedNodeOffset);
			if (FreedNodePtr->BufferSize >= (AllocParams->BufferLength + sizeof(BIOS_BUFFER_NODE))) {
				if (BestFitNodeOffset == 0) {
					/* First node that fits the requested buffer size */
					BestFitNodeOffset = FreedNodeOffset;
					BestFitPrevNodeOffset = PrevNodeOffset;
				} else {
					/* Find out whether current node is a better fit than the previous nodes */
					BestFitNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + BestFitNodeOffset);
					if (BestFitNodePtr->BufferSize > FreedNodePtr->BufferSize) {
						BestFitNodeOffset = FreedNodeOffset;
						BestFitPrevNodeOffset = PrevNodeOffset;
					}
				}
			}
			PrevNodeOffset = FreedNodeOffset;
			FreedNodeOffset = FreedNodePtr->NextNodeOffset;
		} /* end of while loop */

		if (BestFitNodeOffset == 0) {
			/* If we could not find a node that fits the requested buffer
			 * size, return AGESA_BOUNDS_CHK.
			 */
			return AGESA_BOUNDS_CHK;
		} else {
			BestFitNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + BestFitNodeOffset);
			BestFitPrevNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + BestFitPrevNodeOffset);

			/* If BestFitNode is larger than the requested buffer, fragment the node further */
			if (BestFitNodePtr->BufferSize > (AllocParams->BufferLength + sizeof(BIOS_BUFFER_NODE))) {
				NextFreeOffset = BestFitNodeOffset + AllocParams->BufferLength + sizeof(BIOS_BUFFER_NODE);

				NextFreePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + NextFreeOffset);
				NextFreePtr->BufferSize = BestFitNodePtr->BufferSize - (AllocParams->BufferLength + sizeof(BIOS_BUFFER_NODE));
				NextFreePtr->NextNodeOffset = BestFitNodePtr->NextNodeOffset;
			} else {
				/* Otherwise, next free node is NextNodeOffset of BestFitNode */
				NextFreeOffset = BestFitNodePtr->NextNodeOffset;
			}

			/* If BestFitNode is the first buffer in the list, then update
			 * StartOfFreedNodes to reflect the new free node.
			 */
			if (BestFitNodeOffset == BiosHeapBasePtr->StartOfFreedNodes) {
				BiosHeapBasePtr->StartOfFreedNodes = NextFreeOffset;
			} else {
				BestFitPrevNodePtr->NextNodeOffset = NextFreeOffset;
			}

			/* Add BestFitNode to the list of Allocated nodes */
			CurrNodePtr->NextNodeOffset = BestFitNodeOffset;
			BestFitNodePtr->BufferSize = AllocParams->BufferLength;
			BestFitNodePtr->BufferHandle = AllocParams->BufferHandle;
			BestFitNodePtr->NextNodeOffset = 0;

			/* Remove BestFitNode from list of Freed nodes */
			AllocParams->BufferPointer = (UINT8 *)BestFitNodePtr + sizeof(BIOS_BUFFER_NODE);
		}
	}

	return AGESA_SUCCESS;
}

static AGESA_STATUS agesa_DeallocateBuffer(BIOS_HEAP_MANAGER *BiosHeapBasePtr,
	AGESA_BUFFER_PARAMS *AllocParams)
{
	UINT8               *BiosHeapBaseAddr = (void *)BiosHeapBasePtr;
	UINT32              AllocNodeOffset;
	UINT32              PrevNodeOffset;
	UINT32              NextNodeOffset;
	UINT32              FreedNodeOffset;
	UINT32              EndNodeOffset;
	BIOS_BUFFER_NODE   *AllocNodePtr;
	BIOS_BUFFER_NODE   *PrevNodePtr;
	BIOS_BUFFER_NODE   *FreedNodePtr;
	BIOS_BUFFER_NODE   *NextNodePtr;

	/* Find target node to deallocate in list of allocated nodes.
	 * Return AGESA_BOUNDS_CHK if the BufferHandle is not found.
	 */
	AllocNodeOffset = BiosHeapBasePtr->StartOfAllocatedNodes;
	AllocNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + AllocNodeOffset);
	PrevNodeOffset = AllocNodeOffset;

	while (AllocNodePtr->BufferHandle !=  AllocParams->BufferHandle) {
		if (AllocNodePtr->NextNodeOffset == 0) {
			return AGESA_BOUNDS_CHK;
		}
		PrevNodeOffset = AllocNodeOffset;
		AllocNodeOffset = AllocNodePtr->NextNodeOffset;
		AllocNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + AllocNodeOffset);
	}

	/* Remove target node from list of allocated nodes */
	PrevNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + PrevNodeOffset);
	PrevNodePtr->NextNodeOffset = AllocNodePtr->NextNodeOffset;

	/* Zero out the buffer, and clear the BufferHandle */
	LibAmdMemFill((UINT8 *)AllocNodePtr + sizeof(BIOS_BUFFER_NODE), 0, AllocNodePtr->BufferSize, &(AllocParams->StdHeader));
	AllocNodePtr->BufferHandle = 0;

	/* Add deallocated node in order to the list of freed nodes */
	FreedNodeOffset = BiosHeapBasePtr->StartOfFreedNodes;
	FreedNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + FreedNodeOffset);

	EndNodeOffset = AllocNodeOffset + AllocNodePtr->BufferSize +
						sizeof(BIOS_BUFFER_NODE);

	if (AllocNodeOffset < FreedNodeOffset) {
		/* Add to the start of the freed list */
		if (EndNodeOffset == FreedNodeOffset) {
			/* If the freed node is adjacent to the first node in the list, concatenate both nodes */
			AllocNodePtr->BufferSize += FreedNodePtr->BufferSize +
						sizeof(BIOS_BUFFER_NODE);
			AllocNodePtr->NextNodeOffset = FreedNodePtr->NextNodeOffset;

			/* Zero out the FreedNode header */
			memset((UINT8 *)FreedNodePtr, 0,
						sizeof(BIOS_BUFFER_NODE));
		} else {
			/* Otherwise, add freed node to the start of the list
			 * Update NextNodeOffset and BufferSize to include the
			 * size of BIOS_BUFFER_NODE.
			 */
			AllocNodePtr->NextNodeOffset = FreedNodeOffset;
		}
		/* Update StartOfFreedNodes to the new first node */
		BiosHeapBasePtr->StartOfFreedNodes = AllocNodeOffset;
	} else {
		/* Traverse list of freed nodes to find where the deallocated node
		 * should be placed.
		 */
		NextNodeOffset = FreedNodeOffset;
		NextNodePtr = FreedNodePtr;
		while (AllocNodeOffset > NextNodeOffset) {
			PrevNodeOffset = NextNodeOffset;
			if (NextNodePtr->NextNodeOffset == 0) {
				break;
			}
			NextNodeOffset = NextNodePtr->NextNodeOffset;
			NextNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + NextNodeOffset);
		}

		/* If deallocated node is adjacent to the next node,
		 * concatenate both nodes.
		 */
		if (NextNodeOffset == EndNodeOffset) {
			NextNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + NextNodeOffset);
			AllocNodePtr->BufferSize += NextNodePtr->BufferSize +
						sizeof(BIOS_BUFFER_NODE);
			AllocNodePtr->NextNodeOffset = NextNodePtr->NextNodeOffset;

			/* Zero out the NextNode header */
			memset((UINT8 *)NextNodePtr, 0,
						sizeof(BIOS_BUFFER_NODE));
		} else {
			/*AllocNodePtr->NextNodeOffset = FreedNodePtr->NextNodeOffset; */
			AllocNodePtr->NextNodeOffset = NextNodeOffset;
		}
		/* If deallocated node is adjacent to the previous node,
		 * concatenate both nodes.
		 */
		PrevNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + PrevNodeOffset);
		EndNodeOffset = PrevNodeOffset + PrevNodePtr->BufferSize +
						sizeof(BIOS_BUFFER_NODE);
		if (AllocNodeOffset == EndNodeOffset) {
			PrevNodePtr->NextNodeOffset = AllocNodePtr->NextNodeOffset;
			PrevNodePtr->BufferSize += AllocNodePtr->BufferSize +
						sizeof(BIOS_BUFFER_NODE);

			/* Zero out the AllocNode header */
			memset((UINT8 *)AllocNodePtr, 0,
						sizeof(BIOS_BUFFER_NODE));
		} else {
			PrevNodePtr->NextNodeOffset = AllocNodeOffset;
		}
	}
	return AGESA_SUCCESS;
}

static AGESA_STATUS agesa_LocateBuffer(BIOS_HEAP_MANAGER *BiosHeapBasePtr,
	AGESA_BUFFER_PARAMS *AllocParams)
{
	UINT32              AllocNodeOffset;
	UINT8               *BiosHeapBaseAddr = (void *)BiosHeapBasePtr;
	BIOS_BUFFER_NODE   *AllocNodePtr;

	AllocNodeOffset = BiosHeapBasePtr->StartOfAllocatedNodes;
	AllocNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + AllocNodeOffset);

	while (AllocParams->BufferHandle != AllocNodePtr->BufferHandle) {
		if (AllocNodePtr->NextNodeOffset == 0) {
			AllocParams->BufferPointer = NULL;
			AllocParams->BufferLength = 0;
			return AGESA_BOUNDS_CHK;
		} else {
			AllocNodeOffset = AllocNodePtr->NextNodeOffset;
			AllocNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + AllocNodeOffset);
		}
	}

	AllocParams->BufferPointer = (UINT8 *)((UINT8 *)AllocNodePtr + sizeof(BIOS_BUFFER_NODE));
	AllocParams->BufferLength = AllocNodePtr->BufferSize;

	return AGESA_SUCCESS;
}

AGESA_STATUS HeapManagerCallout(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	AGESA_BUFFER_PARAMS *AllocParams = ConfigPtr;

#if defined(HEAP_CALLOUT_RUNTIME) && ENV_RAMSTAGE
	if (Func == AGESA_ALLOCATE_BUFFER && Data == HEAP_CALLOUT_RUNTIME)
		return alloc_cbmem(AllocParams);
#endif

	/* Must not call GetHeapBase() in AGESA_UNSUPPORTED path. */
	if (Func == AGESA_LOCATE_BUFFER)
		return agesa_LocateBuffer(GetHeapBase(), AllocParams);
	else if (Func == AGESA_ALLOCATE_BUFFER)
		return agesa_AllocateBuffer(GetHeapBase(), AllocParams);
	else if (Func == AGESA_DEALLOCATE_BUFFER)
		return agesa_DeallocateBuffer(GetHeapBase(), AllocParams);

	return AGESA_UNSUPPORTED;
}
