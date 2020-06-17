#include "HeapManagerProxy.h"
#include "HeapManager.h"
//HeapManager * HeapManagerProxy::CreateHeapManager(void * i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors)
//{
//	return new HeapManager(i_sizeMemory, i_numDescriptors, i_pMemory);
//}

bool HeapManagerProxy::Destroy()
{
	HeapManager::Instance().AreBlocksFree();
	HeapManager::Instance().Destory();
	return true;
}

void * HeapManagerProxy::alloc(HeapManager * i_pManager, size_t i_size)
{
	return i_pManager->FindFirstFit(i_size);
}

void * HeapManagerProxy::alloc(HeapManager * i_pManager, size_t i_size, unsigned int i_alignment)
{
	return i_pManager->FindFirstFit(i_size,i_alignment);
}

bool HeapManagerProxy::free(HeapManager * i_pManager, void * i_ptr)
{
	return i_pManager->free(i_ptr);
}

void HeapManagerProxy::Collect(HeapManager * i_pManager)
{
	i_pManager->Collect();
}

bool HeapManagerProxy::Contains(const HeapManager * i_pManager, void * i_ptr)
{
	return i_pManager->contains(i_ptr);
}

bool HeapManagerProxy::IsAllocated(const HeapManager * i_pManager, void * i_ptr)
{
	return i_pManager->IsAllocated(i_ptr);
}

void HeapManagerProxy::ShowFreeBlocks(const HeapManager * i_pManager)
{
	i_pManager->ShowFreeBlocks();
}

void HeapManagerProxy::ShowOutstandingAllocations(const HeapManager * i_pManager)
{
	i_pManager->ShowOutstandingAllocations();
}
