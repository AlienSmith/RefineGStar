#include <Windows.h>
#include "MemorySystem.h"
#include "FixiedSizeAllocator.h"
void* generalHeap = nullptr;
namespace GStar {
	bool InitializeMemorySystem()
	{
		generalHeap = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)GENERALHEAPSIZE);
		HeapManager::Instance().InitializeWith(GENERALHEAPSIZE, (unsigned int)GENERALHEAPSIZE, generalHeap);
		FixedSizeAllocator::Instance().Initialize(generalHeap, (size_t)GENERALHEAPSIZE);
		return true;
	}

	void Collect()
	{
		HeapManager::Instance().Collect();
	}

	void DestroyMemorySystem()
	{
		FixedSizeAllocator::Instance().Destory();
		HeapManager::Instance().Destory();
		HeapFree(GetProcessHeap(), 0, generalHeap);
	}
}
////Comment out these to not use the memory system
//void * operator new(size_t i_size)
//{
//	return FixedSizeAllocator::Instance().malloc(i_size,4);
//}
//
//void operator delete(void * i_ptr)
//{
//	FixedSizeAllocator::Instance().free(i_ptr);
//	return;
//}
//
//void * operator new[](size_t i_size)
//{
//	return FixedSizeAllocator::Instance().malloc(i_size, 4);
//}
//
//void operator delete [](void * i_ptr)
//{
//	FixedSizeAllocator::Instance().free(i_ptr);
//	return;
//}


