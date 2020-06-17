/*The Memeory system must be initialized first and destoried last*/
/*Allocated from high address to low address Check and Collect from Low Addresses to High Addresses*/
//Using Singleton
//use _DEBUGADDRESS to break at the allocations of address returned by AreBlockFrees(Under Debug mode)
//#define _DEBUGADDRESS 15392
// the size in info block decides it can manage at most 1GB memory at release mod 2^30 byte = 1 GB
#pragma once
#include <stdio.h>
#define GENERALHEAPSIZE 2.56e+8
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif
#if defined(_DEBUG) &&!defined(DISABLE_DEBUG_HEAPMANAGER) && defined(ENVIRONMENT32)
#define _DEBUGACTIVITE
struct INFOBLCOK { char start[4]; size_t isusing :2; size_t size:30; char end[4]; };

#elif defined(_DEBUG) && !defined(DISABLE_DEBUG_HEAPMANAGER) && defined(ENVIRONMENT64)
#define _DEBUGACTIVITE
struct INFOBLCOK { char start[8]; size_t isusing : 2; size_t size : 62; char end[8]; };

#elif defined(ENVIRONMENT32)
struct INFOBLCOK { size_t isusing : 2; size_t size : 30; };

#elif defined(ENVIRONMENT64)
struct INFOBLCOK { size_t isusing : 2; size_t size : 30; };

#endif


class HeapManager {
public:
	//static void* GeneralHeap;
	static HeapManager& Instance() {
		static HeapManager instance;
		return instance;
	}
	HeapManager() :_pHeapMemory(nullptr) {}
	void InitializeWith(size_t HeapSize, unsigned int numDescriptors, void * _pHeapMemeoy);
	void SetPointerTo(void * _pHeapMemeoy);
	//Move to Lower Adresses
	static void* _movePointerForward(const void* const _pointer, size_t number);
	//Move to Higher Addresses
	static void* _movePointerBackward(const void* const _pointer, size_t number);
	static size_t difference(void* one, void* two);
	// these statics decides wheter a block is being used by client. e suggests end of the heap;
	static const size_t infoisusing = 1;
	static const size_t infoisnotusing = 3;
	static const size_t infoend = 2;
#if defined(ENVIRONMENT64) && defined(_DEBUG)
	static const int guardsize = 8;
#elif defined(ENVIRONMENT32) 
	static const int guardsize = 4;
#endif
	static const char fillguard = 'g';
	static const char fillfreed = 'f';
	static const char fillinitialfilled = 'i';
	static const char fillpadding = 'p';
	//HeapManager(size_t HeapSize, unsigned int numDescriptors, void* _pHeapMemeoy);
	HeapManager(const HeapManager& other) = delete;
	HeapManager& operator == (const HeapManager& other) = delete;
	~HeapManager();
	void* FindFirstFit(size_t size);
	void* FindFirstFit(size_t size, unsigned int i_alignment);
	bool free(void* i_ptr);
	//This function will jump three blocks backward to see if they are descriptor
	bool contains(void* ipr) const;
	bool IsAllocated(void* ipr) const;
	void Collect();
	void ShowFreeBlocks() const;
	void ShowOutstandingAllocations() const;
	bool AreBlocksFree() const;
	//Temp Function
private:
	bool _tryFastBackCollect();// Require the _current pointer set to the descriptor return blocksize + INFOSIZE -1 for false
	void _deletHead();// Require the _current pointer set to the descriptor return blocksize + INFOSIZE
	
	size_t _sizeHeap;
	unsigned int _numDescriptors;
	void* _pHeapMemory;
	void* _current;

	size_t _end;

	void* _debug;
	size_t num_alloc = 0;
	size_t num_free = 1;


	bool _Match(size_t size, unsigned int alignment);
	bool _TryCut(size_t size, unsigned int alignment);
	//this function will move _current forward to user's location
	void _addinfoblock(size_t size);
	// take the input pointer return the pointer to the next descriptor this will also check if write on the padding value under debug
	INFOBLCOK* _TravelToNextDescriptor(const INFOBLCOK* const i_ptr) const;
#if defined(_DEBUG) && !defined(DISABLE_DEBUG_HEAPMANAGER)
	void _FreeCheck(void* ipr); // Do notic this function will not report if the user write on the padding area check if write overrange and on to the filled guard writing on 
								//TODO add a descriptro as block + using + size for pervious + this padding size + size for next + block for debug and use using + size for pervious +size for next for release
#endif
	unsigned long RelativeAddress(void* ptr) const;
};
/*void* operator new(size_t i_size);
void operator delete(void* i_ptr);*/