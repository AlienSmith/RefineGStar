#include "HeapManagerProxy.h"
#include "FixiedSizeAllocator.h"
//HeapManager* HeapManagerProxy::CreateHeapManager(void* i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors)
//{
//	GStar::InitializeMemorySystem();
//}

bool HeapManagerProxy::Destroy()
{
	return true;
}

void * HeapManagerProxy::alloc(HeapManager * i_pManager, size_t i_size)
{
	//return i_pManager->FindFirstFit(i_size);
	return FixedSizeAllocator::Instance().malloc(i_size, 4);
}

void * HeapManagerProxy::alloc(HeapManager * i_pManager, size_t i_size, unsigned int i_alignment)
{
	//return i_pManager->FindFirstFit(i_size,i_alignment);
	return FixedSizeAllocator::Instance().malloc(i_size, i_alignment);
}

bool HeapManagerProxy::free(HeapManager * i_pManager, void * i_ptr)
{
	//return i_pManager->free(i_ptr);
	FixedSizeAllocator::Instance().free(i_ptr);
	return true;
}

void HeapManagerProxy::Collect(HeapManager * i_pManager)
{
	HeapManager::Instance().Collect();
}

bool HeapManagerProxy::Contains(const HeapManager * i_pManager, void * i_ptr)
{
	//return HeapManager::Instance().contains(i_ptr);
	return FixedSizeAllocator::Instance().contains(i_ptr);
}

bool HeapManagerProxy::IsAllocated(const HeapManager * i_pManager, void * i_ptr)
{
	//return HeapManager::Instance().IsAllocated(i_ptr);
	return FixedSizeAllocator::Instance().IsAllocated(i_ptr);
}

void HeapManagerProxy::ShowFreeBlocks(const HeapManager * i_pManager)
{
	HeapManager::Instance().ShowFreeBlocks();
}

void HeapManagerProxy::ShowOutstandingAllocations(const HeapManager * i_pManager)
{
	HeapManager::Instance().ShowOutstandingAllocations();
}
