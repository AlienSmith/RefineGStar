#include "../DebuggingTool/ConsolePrint.h"
#include "FixiedSizeAllocator.h"
#include <inttypes.h>
#include <malloc.h>
#include <stdio.h>
#include "HeapManager.h"
//Why Assert must be included before ConsolePrint?
//need at least 6080000 bytes + 2 InfoBlocks + Bitarray 
const size_t FixedSizeAllocator::ByteBlock16COUNT = 10000;
const size_t FixedSizeAllocator::ByteBlock32COUNT = 2000;
const size_t FixedSizeAllocator::ByteBlock96COUNT = 4000;
const size_t FixedSizeAllocator::Block16UP = 16;
const size_t FixedSizeAllocator::Block16Down = 1;
const size_t FixedSizeAllocator::Block32UP = 32;
const size_t FixedSizeAllocator::Block32Down = 17;
const size_t FixedSizeAllocator::Block96UP = 96;
const size_t FixedSizeAllocator::Block96Down = 33;
#if defined(_WIN32)
const size_t FixedSizeAllocator::Padding = 4;
#else
const size_t FixedSizeAllocator::Padding = 8;
#endif
void FixedSizeAllocator::Initialize(void * i_pHeapMemory, size_t i_sizeHeapMemory)
{
	ASSERT(i_sizeHeapMemory > 6080000, "The heap is not bigger enough to include the fixsize allocator");
	_pheapmemory = i_pHeapMemory;
	_16ByteArray.Initialize(FixedSizeAllocator::ByteBlock16COUNT, HeapManager::Instance().FindFirstFit((FixedSizeAllocator::ByteBlock16COUNT + 7) / 8,FixedSizeAllocator::Padding), true);
	_32ByteArray.Initialize(FixedSizeAllocator::ByteBlock32COUNT, HeapManager::Instance().FindFirstFit((FixedSizeAllocator::ByteBlock32COUNT + 7) / 8, FixedSizeAllocator::Padding), true);
	_96ByteArray.Initialize(FixedSizeAllocator::ByteBlock96COUNT, HeapManager::Instance().FindFirstFit((FixedSizeAllocator::ByteBlock96COUNT + 7) / 8, FixedSizeAllocator::Padding), true);
	_16ByteBlock = HeapManager::Instance().FindFirstFit(FixedSizeAllocator::ByteBlock16COUNT * FixedSizeAllocator::Block16UP);
	_32ByteBlock = HeapManager::Instance().FindFirstFit(FixedSizeAllocator::ByteBlock32COUNT * FixedSizeAllocator::Block32UP);
	_96ByteBlock = HeapManager::Instance().FindFirstFit(FixedSizeAllocator::ByteBlock96COUNT * FixedSizeAllocator::Block96UP);
	_isAlive = true;
}
bool FixedSizeAllocator::Destory()
{
	bool result = AllCleared();
	_isAlive = false;
	free_count = 0;
	new_count = 0;
	_pheapmemory = nullptr;
	HeapManager::Instance().free(_96ByteBlock);
	_96ByteBlock = nullptr;
	HeapManager::Instance().free(_32ByteBlock);
	_32ByteBlock = nullptr;
	HeapManager::Instance().free(_16ByteBlock);
	_16ByteBlock = nullptr;
	_96ByteArray.Destory();
	_32ByteArray.Destory();
	_16ByteArray.Destory();
	return result;
}

bool FixedSizeAllocator::Isin16ByteBlock(void * i_ptr, size_t& index)const
{
	size_t temp = reinterpret_cast<size_t>(i_ptr);
	size_t base = reinterpret_cast<size_t>(_16ByteBlock);
	size_t difference = temp - base;
	if ((0 <= (difference / Block16UP)) && ((difference / Block16UP) < ByteBlock16COUNT)) {
		if (difference % Block16UP == 0) {
			index = difference / Block16UP;
			return true;
		}
		else
		{
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log," is in the _16byte block but not a start point" );
			size_t debug_base = reinterpret_cast<size_t>(_pheapmemory);
			size_t reltaive_distance = temp - debug_base;
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, " Relative position is %lu", reltaive_distance);
		}
	}
	return false;
}

bool FixedSizeAllocator::Isin32ByteBlock(void * i_ptr, size_t& index)const
{
	size_t temp = reinterpret_cast<size_t>(i_ptr);
	size_t base = reinterpret_cast<size_t>(_32ByteBlock);
	size_t difference = temp - base;
	if ((0 <= (difference / Block32UP)) && ((difference / Block32UP) < ByteBlock32COUNT)) {
		if (difference % Block32UP == 0) {
			index = difference / Block32UP;
			return true;
		}
		else
		{
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, " is in the _32byte block but not a start point");
			size_t debug_base = reinterpret_cast<size_t>(_pheapmemory);
			size_t reltaive_distance = temp - debug_base;
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, " Relative position is %lu", reltaive_distance);
		}
	}
	return false;
}

bool FixedSizeAllocator::Isin96ByteBlock(void * i_ptr,size_t& index) const
{
	size_t temp = reinterpret_cast<size_t>(i_ptr);
	size_t base = reinterpret_cast<size_t>(_96ByteBlock);
	size_t difference = temp - base;
	if ((0 <= (difference / Block96UP)) && ((difference / Block96UP) < ByteBlock96COUNT)) {
		if (difference % Block96UP == 0) {
			index = difference / Block96UP;
			return true;
		}
		else
		{
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, " is in the _96 byte block but not a start point");
			size_t debug_base = reinterpret_cast<size_t>(_pheapmemory);
			size_t reltaive_distance = temp - debug_base;
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, " Relative position is %lu", reltaive_distance);
		}
	}
	return false;
}

void * FixedSizeAllocator::malloc(size_t i_size, size_t i_alignment)
{
	ASSERT(_isAlive, "Try to use dead instance");
	new_count++;
	// 1~16
	if (FixedSizeAllocator::Block16Down < i_size && i_size <= FixedSizeAllocator::Block16UP) {
		bool temp;
		size_t index;
		temp = _16ByteArray.GetFirstClearBit(index);
		if (temp) {
			uint8_t* head = reinterpret_cast<uint8_t*>(_16ByteBlock);
			head += index * Block16UP;
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%p 16byte block allocation", head);
			_16ByteArray.SetBit(index);
			return head;
		}
	}
	// 17~32
	else if (FixedSizeAllocator::Block32Down <= i_size && i_size <= FixedSizeAllocator::Block32UP) {
		bool temp;
		size_t index;
		temp = _32ByteArray.GetFirstClearBit(index);
		if (temp) {
			uint8_t* head = reinterpret_cast<uint8_t*>(_32ByteBlock);
			head += index * Block32UP;
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%p 32byte block allocation", head);
			_32ByteArray.SetBit(index);
			return head;
		}
	}
	// 33~96
	else if (FixedSizeAllocator::Block96Down <= i_size && i_size <= FixedSizeAllocator::Block96UP) {
		bool temp;
		size_t index;
		temp = _96ByteArray.GetFirstClearBit(index);
		if (temp) {
			uint8_t* head = reinterpret_cast<uint8_t*>(_96ByteBlock);
			head += index * Block96UP;
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%p 96byte block allocation", head);
			_96ByteArray.SetBit(index);
			return head;
		}
	}
	else {
		return HeapManager::Instance().FindFirstFit(i_size, (unsigned int)i_alignment);
	}
	/*if (reinterpret_cast<size_t>(result) - reinterpret_cast<size_t>(_pheapmemory) == 8166964) {
		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Waring, "got it");
	}*/
	//The block is too large or the corresponding address are all used
	return nullptr;
}

void FixedSizeAllocator::free(void * i_ptr)
{
	ASSERT(_isAlive, "Try to use dead instance");
	free_count++;
	size_t index;
	if (Isin16ByteBlock(i_ptr,index)) {
		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%p 16byte block free", i_ptr);
#if defined(_DEBUG)
		if (_16ByteArray.isBitFree(index)) {
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "attempt to realease memory that has already released");
		}
#endif
		_16ByteArray.ClearBit(index);
	}
	else if (Isin32ByteBlock(i_ptr,index)) {
		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%p 32byte block free", i_ptr);
#if defined(_DEBUG)
		if (_32ByteArray.isBitFree(index)) {
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "attempt to realease memory that has already released");
		}
#endif
		_32ByteArray.ClearBit(index);
	}
	else if (Isin96ByteBlock(i_ptr,index)) {
		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%p 96byte block free", i_ptr);
#if defined(_DEBUG)
		if (_96ByteArray.isBitFree(index)) {
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "attempt to realease memory that has already released");
		}
#endif
		_96ByteArray.ClearBit(index);
	}
	else if (HeapManager::Instance().contains(i_ptr)) {
		HeapManager::Instance().free(i_ptr);
	}
	else
	{
		DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "%p is not allocated by the memory system",i_ptr);
	}
}

bool FixedSizeAllocator::IsAllocated(void* iptr) const
{
	size_t index;
	if(Isin16ByteBlock(iptr, index)){
		if (_16ByteArray.isBitSet(index)) {
			return true;
		}
	}
	else if (Isin32ByteBlock(iptr, index)) {
		if (_32ByteArray.isBitSet(index)) {
			return true;
		}
	}
	else if (Isin96ByteBlock(iptr,index)){
		if (_96ByteArray.isBitSet(index)) {
			return true;
		}
	}
	else if(HeapManager::Instance().IsAllocated(iptr)) {
		return true;
	}
	else {
		return false;
	}
	
	return true;
}

bool FixedSizeAllocator::contains(void* iptr) const
{
	size_t index;
	if (Isin16ByteBlock(iptr, index)) {}
	else if (Isin32ByteBlock(iptr, index)) {}
	else if (Isin96ByteBlock(iptr, index)) {}
	else if (HeapManager::Instance().contains(iptr)) {}
	else {
		return false;
	}

	return true;
}

/*void * __cdecl malloc(size_t i_size)
{
	return FixedSizeAllocator::Instance()->malloc(i_size, 4);
}

void __cdecl free(void * i_ptr)
{
	FixedSizeAllocator::Instance()->free(i_ptr);
	return;
}*/

//void * operator new(size_t i_size)
//{
//	if (!FixedSizeAllocator::Instance()) {
//		printf("new HeappAlloc%zu\n", i_size);
//		return HeapManager::Instance().FindFirstFit(i_size);
//	}
//	return FixedSizeAllocator::Instance()->malloc(i_size,4);
//}
//
//void operator delete(void * i_ptr)
//{
//	FixedSizeAllocator::Instance()->free(i_ptr);
//	return;
//}
//
//void * operator new[](size_t i_size)
//{
//	return FixedSizeAllocator::Instance()->malloc(i_size, 4);
//}
//
//void operator delete [](void * i_ptr)
//{
//	FixedSizeAllocator::Instance()->free(i_ptr);
//	return;
//}

