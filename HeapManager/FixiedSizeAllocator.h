#pragma once
#include"HeapManager.h"
#include"BitArray.h"
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

	static FixedSizeAllocator* Instance() { return instance; }
	static void Initialize(void * i_pHeapMemory, size_t i_sizeHeapMemory);
	static void Terminate() { if (instance) { delete instance; } }

	void* malloc(size_t i_size, size_t i_alignment);
	void free(void* i_ptr);
	bool AllCleared() {
		if (!(_16ByteArray.AreAllClear() && _32ByteArray.AreAllClear() && _32ByteArray.AreAllClear())) {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "static memory not all freed");
			return false;
		}
		return true;
	}
	~FixedSizeAllocator() {
		AllCleared();
		HeapManager::Instance().free(_16ByteBlock);
		HeapManager::Instance().free(_32ByteBlock);
		HeapManager::Instance().free(_96ByteBlock);
	}
private:
	static FixedSizeAllocator* instance;

	FixedSizeAllocator();

	bool Isin16ByteBlock(void* i_ptr, size_t& index) const;
	bool Isin32ByteBlock(void* i_ptr, size_t& index) const;
	bool Isin96ByteBlock(void* i_ptr, size_t& index) const;

	GStar::BitArray _16ByteArray;
	GStar::BitArray _32ByteArray;
	GStar::BitArray _96ByteArray;
	void* _16ByteBlock;
	void* _32ByteBlock;
	void* _96ByteBlock;

	int free_count;
	int new_count;
};