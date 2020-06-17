#include "FixiedSizeAllocator.h"
#include <inttypes.h>
#include <malloc.h>
#include <Windows.h>
#include <stdio.h>
#include "HeapManager.h"
#include "../DebuggingTool/ConsolePrint.h"
FixedSizeAllocator* FixedSizeAllocator::instance = nullptr;
const size_t FixedSizeAllocator::ByteBlock16COUNT = 100000;
const size_t FixedSizeAllocator::ByteBlock32COUNT = 20000;
const size_t FixedSizeAllocator::ByteBlock96COUNT = 40000;
const size_t FixedSizeAllocator::Block16UP = 16;
const size_t FixedSizeAllocator::Block16Down = 0;
const size_t FixedSizeAllocator::Block32UP = 32;
const size_t FixedSizeAllocator::Block32Down = 17;
const size_t FixedSizeAllocator::Block96UP = 96;
const size_t FixedSizeAllocator::Block96Down = 33;

void FixedSizeAllocator::Initialize(void * i_pHeapMemory, size_t i_sizeHeapMemory)
{
	if (!FixedSizeAllocator::instance) {
		HeapManager::Instance().InitializeWith(i_sizeHeapMemory, (unsigned int)i_sizeHeapMemory, i_pHeapMemory);
		FixedSizeAllocator::instance = new FixedSizeAllocator();
	}
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
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring," is in the _16byte block but not a start point" );
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
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, " is in the _32byte block but not a start point");
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
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, " is in the _96 byte block but not a start point");
		}
	}
	return false;
}

void * FixedSizeAllocator::malloc(size_t i_size, size_t i_alignment)
{
	new_count++;
	// 1~15
	if (FixedSizeAllocator::Block16Down < i_size && i_size < FixedSizeAllocator::Block16UP) {
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
	// 18~31
	else if (FixedSizeAllocator::Block32Down < i_size && i_size < FixedSizeAllocator::Block32UP) {
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
	// 34~95
	else if (FixedSizeAllocator::Block96Down < i_size && i_size < FixedSizeAllocator::Block96UP) {
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
	//The block is too large or the corresponding address are all used
	return HeapManager::Instance().FindFirstFit(i_size, (unsigned int)i_alignment);
}

//void FixedSizeAllocator::free(void * i_ptr)
//{
//	free_count++;
//	size_t index;
//	if (Isin16ByteBlock(i_ptr,index)) {
//		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%p 16byte block free", i_ptr);
//#if defined(_DEBUG)
//		if (_16ByteArray.isBitFree(index)) {
//			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "attempt to realease memory that has already released");
//		}
//#endif
//		_16ByteArray.ClearBit(index);
//	}
//	else if (Isin32ByteBlock(i_ptr,index)) {
//		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%p 32byte block free", i_ptr);
//#if defined(_DEBUG)
//		if (_32ByteArray.isBitFree(index)) {
//			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "attempt to realease memory that has already released");
//		}
//#endif
//		_32ByteArray.ClearBit(index);
//	}
//	else if (Isin96ByteBlock(i_ptr,index)) {
//		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%p 96byte block free", i_ptr);
//#if defined(_DEBUG)
//		if (_96ByteArray.isBitFree(index)) {
//			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "attempt to realease memory that has already released");
//		}
//#endif
//		_96ByteArray.ClearBit(index);
//	}
//	else if (HeapManager::Instance().contains(i_ptr)) {
//		HeapManager::Instance().free(i_ptr);
//	}
//	else
//	{
//		DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "%p is not allocated by the memory system",i_ptr);
//	}
//}

FixedSizeAllocator::FixedSizeAllocator() :
	_16ByteArray(GStar::BitArray(FixedSizeAllocator::ByteBlock16COUNT, HeapManager::Instance().FindFirstFit((FixedSizeAllocator::ByteBlock16COUNT + 7) / 8), true)),
	_32ByteArray(GStar::BitArray(FixedSizeAllocator::ByteBlock32COUNT, HeapManager::Instance().FindFirstFit((FixedSizeAllocator::ByteBlock32COUNT + 7) / 8), true)),
	_96ByteArray(GStar::BitArray(FixedSizeAllocator::ByteBlock96COUNT, HeapManager::Instance().FindFirstFit((FixedSizeAllocator::ByteBlock96COUNT + 7) / 8), true)),
	_16ByteBlock(HeapManager::Instance().FindFirstFit(FixedSizeAllocator::ByteBlock16COUNT * 16)),
	_32ByteBlock(HeapManager::Instance().FindFirstFit(FixedSizeAllocator::ByteBlock32COUNT * 32)),
	_96ByteBlock(HeapManager::Instance().FindFirstFit(FixedSizeAllocator::ByteBlock96COUNT * 96)),
	free_count(0),
	new_count(0)
{
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

