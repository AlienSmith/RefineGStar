#include "MemorySystem.h"
#include "FixiedSizeAllocator.h"
//This is windows specific
#include <Windows.h>
void* generalHeap = nullptr;
bool InitializeMemorySystem()
{ 
	generalHeap = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)GENERALHEAPSIZE);
	FixedSizeAllocator::Initialize(generalHeap, (size_t)GENERALHEAPSIZE);
	return true;
}

void Collect()
{
	HeapManager::Instance().Collect();
}

void DestroyMemorySystem()
{
	FixedSizeAllocator::Terminate();
	HeapManager::Instance().AreBlocksFree();
	HeapManager::Instance().Destory();
	HeapFree(GetProcessHeap(), 0, generalHeap);
}

