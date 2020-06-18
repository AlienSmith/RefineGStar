#pragma once
#include"HeapManager.h"
#include"BitArray.h"
//All array blocks use default allocator hence 4 byte aligned
class FixedSizeAllocator {
public:
	const static size_t ByteBlock16COUNT;
	const static size_t ByteBlock32COUNT;
	const static size_t ByteBlock96COUNT;
	const static size_t Block16Down;
	const static size_t Block16UP;
	const static size_t Block32Down;
	const static size_t Block32UP;
	const static size_t Block96Down;
	const static size_t Block96UP;
	const static size_t Padding;

	static FixedSizeAllocator& Instance() { 
		static FixedSizeAllocator instance;
		return instance; 
	}
	void Initialize(void * i_pHeapMemory, size_t i_sizeHeapMemory);
	bool Destory();
	void Terminate() {
		AllCleared();
		HeapManager::Instance().free(_16ByteBlock);
		HeapManager::Instance().free(_32ByteBlock);
		HeapManager::Instance().free(_96ByteBlock);
	}

	void* malloc(size_t i_size, size_t i_alignment);
	void free(void* i_ptr);
	bool AllCleared() {
		if (!(_16ByteArray.AreAllClear() && _32ByteArray.AreAllClear() && _96ByteArray.AreAllClear())) {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "static memory not all freed");
			return false;
		}
		return true;
	}
	bool contains(void* iptr) const;
	bool IsAllocated(void* iptr) const;
	//This singleton looks wired, Singleton should be initialized at Initialize() and freeed at Destory() for resources like ptr.
private:

	FixedSizeAllocator() :
		_16ByteArray(),
		_32ByteArray(),
		_96ByteArray(),
		_16ByteBlock(nullptr),
		_32ByteBlock(nullptr),
		_96ByteBlock(nullptr),
		_pheapmemory(nullptr),
		free_count(0),
		new_count(0),
		_isAlive(false){}
	~FixedSizeAllocator() {
	}
	bool Isin16ByteBlock(void* i_ptr, size_t& index) const;
	bool Isin32ByteBlock(void* i_ptr, size_t& index) const;
	bool Isin96ByteBlock(void* i_ptr, size_t& index) const;

	GStar::BitArray _16ByteArray;
	GStar::BitArray _32ByteArray;
	GStar::BitArray _96ByteArray;
	void* _16ByteBlock;
	void* _32ByteBlock;
	void* _96ByteBlock;

	void* _pheapmemory;

	int free_count;
	int new_count;
	bool _isAlive;
};