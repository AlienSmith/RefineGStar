#include <Windows.h>
#include <stdio.h>
#include "Assert.h"
#include <algorithm>
#include <vector>
#include"../HeapManager/MemorySystem.h"
#include "HeapManager_UnitTest.h"
#include "../HeapManager/HeapManagerProxy.h"
#include "../HeapManager/HeapManager.h"
#include "../DebuggingTool/ConsolePrint.h"
//#define SUPPORTS_ALIGNMENT
#define SUPPORTS_SHOWFREEBLOCKS
#define SUPPORTS_SHOWOUTSTANDINGALLOCATIONS

bool HeapManager_UnitTest()
{
	using namespace HeapManagerProxy;

	const size_t 		sizeHeap = 1024 * 1024 * 10;
	const unsigned int 	numDescriptors = 2048;

	// Allocate memory for my test heap.
	void* pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
	ASSERT(pHeapMemory, "pHeapMemory");

	// Create a heap manager for my test heap.
	HeapManager::Instance().InitializeWith(sizeHeap, numDescriptors, pHeapMemory);
	HeapManager* pHeapManager = &HeapManager::Instance();
	ASSERT(pHeapMemory, "pHeapManager");

	if (pHeapManager == nullptr)
		return false;
	//int* i_ptr = new int();
	//return true;
//
//#ifdef TEST_SINGLE_LARGE_ALLOCATION
//		// This is a test I wrote to check to see if using the whole block if it was almost consumed by 
//		// an allocation worked. Also helped test my ShowFreeBlocks() and ShowOutstandingAllocations().
//		{
//#ifdef SUPPORTS_SHOWFREEBLOCKS
//			ShowFreeBlocks(pHeapManager);
//#endif // SUPPORTS_SHOWFREEBLOCKS
//
//			size_t largestBeforeAlloc = GetLargestFreeBlock(pHeapManager);
//			void* pPtr = alloc(pHeapManager, largestBeforeAlloc - HeapManager::s_MinumumToLeave);
//
//			if (pPtr)
//			{
//#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
//				printf("After large allocation:\n");
//#ifdef SUPPORTS_SHOWFREEBLOCKS
//				ShowFreeBlocks(pHeapManager);
//#endif // SUPPORTS_SHOWFREEBLOCKS
//#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
//				ShowOutstandingAllocations(pHeapManager);
//#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
//				printf("\n");
//#endif
//
//				size_t largestAfterAlloc = GetLargestFreeBlock(pHeapManager);
//				bool success = Contains(pHeapManager, pPtr) && IsAllocated(pHeapManager, pPtr);
//				assert(success);
//
//				success = free(pHeapManager, pPtr);
//				assert(success);
//
//				Collect(pHeapManager);
//
//#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
//				printf("After freeing allocation and garbage collection:\n");
//#ifdef SUPPORTS_SHOWFREEBLOCKS
//				ShowFreeBlocks(pHeapManager);
//#endif // SUPPORTS_SHOWFREEBLOCKS
//#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
//				ShowOutstandingAllocations(pHeapManager);
//#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
//				printf("\n");
//#endif
//
//				size_t largestAfterCollect = GetLargestFreeBlock(pHeapManager);
//			}
//		}
//#endif
	int i = 0;
	{
		i++;
		printf("%d", i);
		std::vector<void*> AllocatedAddresses;
		//AllocatedAddresses.reserve(200000);
		long	numAllocs = 0;
		long	numFrees = 0;
		long	numCollects = 0;
		// allocate memory of random sizes up to 1024 bytes from the heap manager
		// until it runs out of memory
		do
		{
			const size_t		maxTestAllocationSize = 1024;

			size_t			sizeAlloc = 1 + (rand() & (maxTestAllocationSize - 1));

#ifdef SUPPORTS_ALIGNMENT
			// pick an alignment
			const unsigned int	alignments[] = { 4, 8, 16, 32, 64 };

			const unsigned int	index = rand() % (sizeof(alignments) / sizeof(alignments[0]));

			const unsigned int	alignment = alignments[index];
			void* pPtr = alloc(pHeapManager, sizeAlloc, alignment);

			// check that the returned address has the requested alignment
			ASSERT((reinterpret_cast<uintptr_t>(pPtr) & (alignment - 1)) == 0, "Alignment Wrong");
#else
			void* pPtr = alloc(pHeapManager, sizeAlloc);
#endif // SUPPORT_ALIGNMENT

			// if allocation failed see if garbage collecting will create a large enough block
			if (pPtr == nullptr)
			{
				Collect(pHeapManager);

#ifdef SUPPORTS_ALIGNMENT
				pPtr = alloc(pHeapManager, sizeAlloc, alignment);
#else
				pPtr = alloc(pHeapManager, sizeAlloc);
#endif // SUPPORT_ALIGNMENT

				// if not we're done. go on to cleanup phase of test
				if (pPtr == nullptr)
					break;
			}

			AllocatedAddresses.push_back(pPtr);
			numAllocs++;

			// randomly free and/or garbage collect during allocation phase
			const unsigned int freeAboutEvery = 10;
			const unsigned int garbageCollectAboutEvery = 40;

			if (!AllocatedAddresses.empty() && ((rand() % freeAboutEvery) == 0))
			{
				pPtr = AllocatedAddresses.back();
				AllocatedAddresses.pop_back();

				bool success = Contains(pHeapManager, pPtr) && IsAllocated(pHeapManager, pPtr);
				ASSERT(success, "Contain");

				success = free(pHeapManager, pPtr);
				ASSERT(success, "Free");

				numFrees++;
			}

			if ((rand() % garbageCollectAboutEvery) == 0)
			{
				Collect(pHeapManager);

				numCollects++;
			}

		} while (1);

#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
		printf("After exhausting allocations:\n");
#ifdef SUPPORTS_SHOWFREEBLOCKS
		ShowFreeBlocks(pHeapManager);
#endif // SUPPORTS_SHOWFREEBLOCKS
#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
		ShowOutstandingAllocations(pHeapManager);
#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
		printf("\n");
#endif

		// now free those blocks in a random order
		if (!AllocatedAddresses.empty())
		{
			// randomize the addresses
			//std::random_shuffle(AllocatedAddresses.begin(), AllocatedAddresses.end());

			// return them back to the heap manager
			while (!AllocatedAddresses.empty())
			{
				void* pPtr = AllocatedAddresses.back();
				AllocatedAddresses.pop_back();

				bool success = Contains(pHeapManager, pPtr) && IsAllocated(pHeapManager, pPtr);
				ASSERT(success, "Contain 2");

				success = free(pHeapManager, pPtr);
				ASSERT(success, "Free 2");
			}

#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
			printf("After freeing allocations:\n");
#ifdef SUPPORTS_SHOWFREEBLOCKS
			ShowFreeBlocks(pHeapManager);
#endif // SUPPORTS_SHOWFREEBLOCKS

#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
			ShowOutstandingAllocations(pHeapManager);
#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
			printf("\n");
#endif

			// do garbage collection
			Collect(pHeapManager);
			// our heap should be one single block, all the memory it started with

#if defined(SUPPORTS_SHOWFREEBLOCKS) || defined(SUPPORTS_SHOWOUTSTANDINGALLOCATIONS)
			printf("After garbage collection:\n");
#ifdef SUPPORTS_SHOWFREEBLOCKS
			ShowFreeBlocks(pHeapManager);
#endif // SUPPORTS_SHOWFREEBLOCKS

#ifdef SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
			ShowOutstandingAllocations(pHeapManager);
#endif // SUPPORTS_SHOWOUTSTANDINGALLOCATIONS
			printf("\n");
#endif

			// do a large test allocation to see if garbage collection worked
			void* pPtr = alloc(pHeapManager, sizeHeap / 2);
			ASSERT(pPtr, "alloc 1");

			if (pPtr)
			{
				bool success = Contains(pHeapManager, pPtr) && IsAllocated(pHeapManager, pPtr);
				ASSERT(success, "Contain 0");

				success = free(pHeapManager, pPtr);
				ASSERT(success, "Free 1");

			}
		}
	}
	ASSERT(Destroy(), "Memory Allocated Not deleted");
	HeapFree(GetProcessHeap(), 0, pHeapMemory);

	// we succeeded
	return true;
}
