/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <amdblocks/agesawrapper.h>
#include <amdlib.h>
#include <arch/acpi.h>
#include <amdblocks/BiosCallOuts.h>
#include <cbmem.h>
#include <string.h>

static void *agesa_heap_base(void)
{
	return cbmem_add(CBMEM_ID_RESUME_SCRATCH, BIOS_HEAP_SIZE);
}

static void EmptyHeap(int unused)
{
	void *BiosManagerPtr = agesa_heap_base();
	memset(BiosManagerPtr, 0, BIOS_HEAP_SIZE);
}

#if IS_ENABLED(CONFIG_LATE_CBMEM_INIT)
#error "Only EARLY_CBMEM_INIT is supported."
#endif
ROMSTAGE_CBMEM_INIT_HOOK(EmptyHeap)

AGESA_STATUS agesa_AllocateBuffer (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	UINT32              AvailableHeapSize;
	UINT8               *BiosHeapBaseAddr;
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
	BIOS_HEAP_MANAGER  *BiosHeapBasePtr;
	AGESA_BUFFER_PARAMS *AllocParams;

	AllocParams = ((AGESA_BUFFER_PARAMS *)ConfigPtr);
	AllocParams->BufferPointer = NULL;

	AvailableHeapSize = BIOS_HEAP_SIZE - sizeof(BIOS_HEAP_MANAGER);
	BiosHeapBaseAddr = agesa_heap_base();
	BiosHeapBasePtr = (BIOS_HEAP_MANAGER *)BiosHeapBaseAddr;

	if (BiosHeapBasePtr->StartOfAllocatedNodes == 0) {
		/* First allocation */
		CurrNodeOffset = sizeof(BIOS_HEAP_MANAGER);
		CurrNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
						+ CurrNodeOffset);
		CurrNodePtr->BufferHandle = AllocParams->BufferHandle;
		CurrNodePtr->BufferSize = AllocParams->BufferLength;
		CurrNodePtr->NextNodeOffset = 0;
		AllocParams->BufferPointer = (UINT8 *)CurrNodePtr
						+ sizeof(BIOS_BUFFER_NODE);

		/* Update the remaining free space */
		FreedNodeOffset = CurrNodeOffset + CurrNodePtr->BufferSize
						+ sizeof(BIOS_BUFFER_NODE);
		FreedNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
						+ FreedNodeOffset);
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
		CurrNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
						+ CurrNodeOffset);

		while (CurrNodeOffset != 0) {
			CurrNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
						+ CurrNodeOffset);
			if (CurrNodePtr->BufferHandle ==
						AllocParams->BufferHandle) {
				return AGESA_BOUNDS_CHK;
			}
			CurrNodeOffset = CurrNodePtr->NextNodeOffset;
			/* If BufferHandle has not been allocated on the heap,
			 * CurrNodePtr here points to the end of the allocated
			 * nodes list.
			 */
		}
		/* Find the node that best fits the requested buffer size */
		FreedNodeOffset = BiosHeapBasePtr->StartOfFreedNodes;
		PrevNodeOffset = FreedNodeOffset;
		BestFitNodeOffset = 0;
		BestFitPrevNodeOffset = 0;
		while (FreedNodeOffset != 0) { /* todo: simplify this */
			FreedNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
						+ FreedNodeOffset);
			if (FreedNodePtr->BufferSize >=
						(AllocParams->BufferLength +
						sizeof(BIOS_BUFFER_NODE))) {
				if (BestFitNodeOffset == 0) {
					/*
					 * First node that fits the requested
					 * buffer size
					 */
					BestFitNodeOffset = FreedNodeOffset;
					BestFitPrevNodeOffset = PrevNodeOffset;
				} else {
					/*
					 * Find out whether current node is a
					 * betterfit than the previous nodes
					 */
					BestFitNodePtr = (BIOS_BUFFER_NODE *)
							 (BiosHeapBaseAddr +
							  BestFitNodeOffset);
					if (BestFitNodePtr->BufferSize >
						FreedNodePtr->BufferSize) {

						BestFitNodeOffset =
							FreedNodeOffset;
						BestFitPrevNodeOffset =
							PrevNodeOffset;
					}
				}
			}
			PrevNodeOffset = FreedNodeOffset;
			FreedNodeOffset = FreedNodePtr->NextNodeOffset;
		} /* end of while loop */

		if (BestFitNodeOffset == 0) {
			/*
			 * If we could not find a node that fits the requested
			 * buffer size, return AGESA_BOUNDS_CHK.
			 */
			return AGESA_BOUNDS_CHK;
		}

		BestFitNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
					+ BestFitNodeOffset);
		BestFitPrevNodePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr +
							BestFitPrevNodeOffset);

		/*
		 * If BestFitNode is larger than the requested buffer,
		 * fragment the node further
		 */
		if (BestFitNodePtr->BufferSize >
		    (AllocParams->BufferLength + sizeof(BIOS_BUFFER_NODE))) {
			NextFreeOffset = BestFitNodeOffset +
					 AllocParams->BufferLength +
					 sizeof(BIOS_BUFFER_NODE);
			NextFreePtr = (BIOS_BUFFER_NODE *) (BiosHeapBaseAddr +
				       NextFreeOffset);
			NextFreePtr->BufferSize = BestFitNodePtr->BufferSize -
						 (AllocParams->BufferLength +
						  sizeof(BIOS_BUFFER_NODE));
			NextFreePtr->NextNodeOffset =
					BestFitNodePtr->NextNodeOffset;
		} else {
			/*
			 * Otherwise, next free node is
			 * NextNodeOffset of BestFitNode
			 */
			NextFreeOffset = BestFitNodePtr->NextNodeOffset;
		}

		/*
		 * If BestFitNode is the first buffer in the list, then
		 * update StartOfFreedNodes to reflect new free node.
		 */
		if (BestFitNodeOffset == BiosHeapBasePtr->StartOfFreedNodes)
			BiosHeapBasePtr->StartOfFreedNodes = NextFreeOffset;
		else
			BestFitPrevNodePtr->NextNodeOffset = NextFreeOffset;

		/* Add BestFitNode to the list of Allocated nodes */
		CurrNodePtr->NextNodeOffset = BestFitNodeOffset;
		BestFitNodePtr->BufferSize = AllocParams->BufferLength;
		BestFitNodePtr->BufferHandle = AllocParams->BufferHandle;
		BestFitNodePtr->NextNodeOffset = 0;

		/* Remove BestFitNode from list of Freed nodes */
		AllocParams->BufferPointer = (UINT8 *)BestFitNodePtr +
					     sizeof(BIOS_BUFFER_NODE);
	}

	return AGESA_SUCCESS;
}

AGESA_STATUS agesa_DeallocateBuffer (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{

	UINT8               *BiosHeapBaseAddr;
	UINT32              AllocNodeOffset;
	UINT32              PrevNodeOffset;
	UINT32              NextNodeOffset;
	UINT32              FreedNodeOffset;
	UINT32              EndNodeOffset;
	BIOS_BUFFER_NODE   *AllocNodePtr;
	BIOS_BUFFER_NODE   *PrevNodePtr;
	BIOS_BUFFER_NODE   *FreedNodePtr;
	BIOS_BUFFER_NODE   *NextNodePtr;
	BIOS_HEAP_MANAGER  *BiosHeapBasePtr;
	AGESA_BUFFER_PARAMS *AllocParams;

	AllocParams = (AGESA_BUFFER_PARAMS *)ConfigPtr;

	BiosHeapBaseAddr = agesa_heap_base();
	BiosHeapBasePtr = (BIOS_HEAP_MANAGER *)BiosHeapBaseAddr;

	/* Find target node to deallocate in list of allocated nodes.
	 * Return AGESA_BOUNDS_CHK if the BufferHandle is not found.
	 */
	AllocNodeOffset = BiosHeapBasePtr->StartOfAllocatedNodes;
	AllocNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + AllocNodeOffset);
	PrevNodeOffset = AllocNodeOffset;

	while (AllocNodePtr->BufferHandle != AllocParams->BufferHandle) {
		if (AllocNodePtr->NextNodeOffset == 0)
			return AGESA_BOUNDS_CHK;
		PrevNodeOffset = AllocNodeOffset;
		AllocNodeOffset = AllocNodePtr->NextNodeOffset;
		AllocNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
						+ AllocNodeOffset);
	}

	/* Remove target node from list of allocated nodes */
	PrevNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + PrevNodeOffset);
	PrevNodePtr->NextNodeOffset = AllocNodePtr->NextNodeOffset;

	/* Zero out the buffer, and clear the BufferHandle */
	memset((UINT8 *)AllocNodePtr + sizeof(BIOS_BUFFER_NODE), 0,
		AllocNodePtr->BufferSize);
	AllocNodePtr->BufferHandle = 0;

	/* Add deallocated node in order to the list of freed nodes */
	FreedNodeOffset = BiosHeapBasePtr->StartOfFreedNodes;
	FreedNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + FreedNodeOffset);

	EndNodeOffset = AllocNodeOffset + AllocNodePtr->BufferSize +
						sizeof(BIOS_BUFFER_NODE);

	if (AllocNodeOffset < FreedNodeOffset) {
		/* Add to the start of the freed list */
		if (EndNodeOffset == FreedNodeOffset) {
			/* If the freed node is adjacent to the first node in
			 * the list, concatenate both nodes
			 */
			AllocNodePtr->BufferSize += FreedNodePtr->BufferSize +
						sizeof(BIOS_BUFFER_NODE);
			AllocNodePtr->NextNodeOffset =
						FreedNodePtr->NextNodeOffset;

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
		/* Traverse list of freed nodes to find where the deallocated
		 * node should be placed.
		 */
		NextNodeOffset = FreedNodeOffset;
		NextNodePtr = FreedNodePtr;
		while (AllocNodeOffset > NextNodeOffset) {
			PrevNodeOffset = NextNodeOffset;
			if (NextNodePtr->NextNodeOffset == 0)
				break;
			NextNodeOffset = NextNodePtr->NextNodeOffset;
			NextNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
						+ NextNodeOffset);
		}

		/* If deallocated node is adjacent to the next node,
		 * concatenate both nodes.
		 */
		if (NextNodeOffset == EndNodeOffset) {
			NextNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
						+ NextNodeOffset);
			AllocNodePtr->BufferSize += NextNodePtr->BufferSize +
						sizeof(BIOS_BUFFER_NODE);
			AllocNodePtr->NextNodeOffset =
						NextNodePtr->NextNodeOffset;

			/* Zero out the NextNode header */
			memset((UINT8 *)NextNodePtr, 0,
						sizeof(BIOS_BUFFER_NODE));
		} else {
			/*AllocNodePtr->NextNodeOffset =
			 * 			FreedNodePtr->NextNodeOffset; */
			AllocNodePtr->NextNodeOffset = NextNodeOffset;
		}
		/*
		 * If deallocated node is adjacent to the previous node,
		 * concatenate both nodes.
		 */
		PrevNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr
						+ PrevNodeOffset);
		EndNodeOffset = PrevNodeOffset + PrevNodePtr->BufferSize +
						sizeof(BIOS_BUFFER_NODE);

		if (AllocNodeOffset == EndNodeOffset) {
			PrevNodePtr->NextNodeOffset =
						AllocNodePtr->NextNodeOffset;
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

AGESA_STATUS agesa_LocateBuffer (UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	UINT32              AllocNodeOffset;
	UINT8               *BiosHeapBaseAddr;
	BIOS_BUFFER_NODE    *AllocNodePtr;
	BIOS_HEAP_MANAGER   *BiosHeapBasePtr;
	AGESA_BUFFER_PARAMS *AllocParams;

	AllocParams = (AGESA_BUFFER_PARAMS *)ConfigPtr;

	BiosHeapBaseAddr = agesa_heap_base();
	BiosHeapBasePtr = (BIOS_HEAP_MANAGER *)BiosHeapBaseAddr;

	AllocNodeOffset = BiosHeapBasePtr->StartOfAllocatedNodes;
	AllocNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr + AllocNodeOffset);

	while (AllocParams->BufferHandle != AllocNodePtr->BufferHandle) {
		if (AllocNodePtr->NextNodeOffset == 0) {
			AllocParams->BufferPointer = NULL;
			AllocParams->BufferLength = 0;
			return AGESA_BOUNDS_CHK;
		}
		AllocNodeOffset = AllocNodePtr->NextNodeOffset;
		AllocNodePtr = (BIOS_BUFFER_NODE *)(BiosHeapBaseAddr +
						    AllocNodeOffset);
	}

	AllocParams->BufferPointer = (UINT8 *)((UINT8 *)AllocNodePtr
						+ sizeof(BIOS_BUFFER_NODE));
	AllocParams->BufferLength = AllocNodePtr->BufferSize;

	return AGESA_SUCCESS;

}
