#include "HeapManager.h"

#include <cstring>
#include "../DebuggingTool/ConsolePrint.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(_DEBUGACTIVITE)
void HeapManager::InitializeWith(size_t HeapSize, unsigned int numDescriptors, void * _pHeapMemeoy)
{
	this->_sizeHeap = HeapSize;
	this->_numDescriptors = numDescriptors;
	this->_pHeapMemory = _pHeapMemeoy;
	size_t base = reinterpret_cast<size_t>(_pHeapMemeoy);
	//this->_debug = reinterpret_cast<void*>(base + 1001976);
	//memset(_pHeapMemeoy, '\0', HeapSize);
	DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "The memory block Start at%p", _pHeapMemeoy);
	_current = _pHeapMemeoy;
	INFOBLCOK* infoblock = reinterpret_cast<INFOBLCOK*>(_current);
	memset(&(infoblock->start), HeapManager::fillguard, guardsize);
	memset(&(infoblock->end), HeapManager::fillguard, guardsize);
	infoblock->isusing = HeapManager::infoisnotusing;
	//Cause the last Infoblock which indicate the end of the memory only have start guard + isusing
	infoblock->size = HeapSize - sizeof(INFOBLCOK)*2 - guardsize ; // start with infroblock end with guards
	//To the start of user's memory
	_current = _movePointerForward(_current, sizeof(INFOBLCOK));
	memset(_current, HeapManager::fillinitialfilled, infoblock->size);
	//To the end infoblock
	_current = _movePointerForward(_current, infoblock->size);
	INFOBLCOK* endinfoblock = reinterpret_cast<INFOBLCOK*>(_current);
	memset(&(endinfoblock->start), HeapManager::fillguard, guardsize);
	memset(&(endinfoblock->end), HeapManager::fillguard, guardsize);
	endinfoblock->isusing = HeapManager::infoend;
	endinfoblock->size = 0;
	DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "%p", _current);
	DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "my info block have size %i", sizeof(INFOBLCOK));
	DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "size_t have size %i", sizeof(size_t));
	DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "char have size %i", sizeof(char));
}
#else
void HeapManager::InitializeWith(size_t HeapSize, unsigned int numDescriptors, void * _pHeapMemeoy)
{
	this->_sizeHeap = HeapSize;
	this->_numDescriptors = numDescriptors;
	this->_pHeapMemory = _pHeapMemeoy;
	//memset(_pHeapMemeoy, '\0', HeapSize);
	DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "The memory block Start at%p", _pHeapMemeoy);
	_current = _pHeapMemeoy;
	INFOBLCOK* infoblock = reinterpret_cast<INFOBLCOK*>(_current);
	infoblock->isusing = HeapManager::infoisnotusing;
	infoblock->size = HeapSize - sizeof(INFOBLCOK)*2; //no guard 1 byte for end only 2 bit actually used
	//To the user's data
	_current = _movePointerForward(_current, sizeof(INFOBLCOK));
	//Do not put initialfilled cause it take times
	//memset(_current, HeapManager::fillinitialfilled, infoblock->size);
	//To the end infoblock
	_current = _movePointerForward(_current, infoblock->size);
	INFOBLCOK* endinfoblock = reinterpret_cast<INFOBLCOK*>(_current);
	endinfoblock->isusing = HeapManager::infoend;
	endinfoblock->size = 0;
	DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "The memory block end at%p", endinfoblock);
	_end = reinterpret_cast<size_t>(endinfoblock) - reinterpret_cast<size_t>(_pHeapMemeoy);
}
#endif

HeapManager::~HeapManager()
{
	/*if (_pHeapMemory) {
		delete _pHeapMemory;
	}*/
}

void * HeapManager::FindFirstFit(size_t size)
{
	return FindFirstFit(size, 4);
}
//????????
#if defined(_DEBUGACTIVITE) && defined(_DEBUGADDRESS)
void* HeapManager::FindFirstFit(size_t size, unsigned int i_alignment)
{
	_current = _pHeapMemory;
	INFOBLCOK* current = reinterpret_cast<INFOBLCOK*>(_current);
	while (!(this->_Match(size, i_alignment))) {
		if (current->isusing != HeapManager::infoend && current->isusing != HeapManager::infoisusing && current->isusing != HeapManager::infoisnotusing) {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Waring, "Gonna Go Wrong  %p \n", _current);
		}
		if (current->isusing == HeapManager::infoend) {
			return nullptr;
		}
		else
		{
			_current = _TravelToNextDescriptor(current);
			current = reinterpret_cast<INFOBLCOK*>(_current);
		}
	}
	DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, " Heap Manager Allocated Block  %p \n", _current);
	unsigned long relative_address = RelativeAddress(_current);
	if (relative_address == _DEBUGADDRESS) {
		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Error, "Memory Debug point reached");
	}
	return _current;
}
#elif defined(_DEBUGACTIVITE) && !defined(_DEBUGADDRESS)
void * HeapManager::FindFirstFit(size_t size, unsigned int i_alignment)
{
	_current = _pHeapMemory;
	INFOBLCOK* current = reinterpret_cast<INFOBLCOK*>(_current);
	while (!(this->_Match(size, i_alignment))) {
		if (current->isusing != HeapManager::infoend && current->isusing != HeapManager::infoisusing && current->isusing != HeapManager::infoisnotusing) {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Waring, "Gonna Go Wrong  %p \n", _current);
		}
		if (current->isusing == HeapManager::infoend) {
			return nullptr;
		}
		else
		{
			_current = _TravelToNextDescriptor(current);
			current = reinterpret_cast<INFOBLCOK*>(_current);
		}
	}
	DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, " Heap Manager Allocated Block  %p \n", _current);
	/*if (_current == _debug) {
		DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, " error place allocated", _current);
	}*/
	return _current;
}
#elif defined(_DEBUGADDRESS)&& !defined(_DEBUGACTIVITE)
void* HeapManager::FindFirstFit(size_t size, unsigned int i_alignment)
{
	//printf("This is one search ==========================================================\n");
	num_alloc++;
	/*if (num_alloc == 1896) {
		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Waring, "Got it");
	}*/
	_current = _pHeapMemory;
	INFOBLCOK* current = reinterpret_cast<INFOBLCOK*>(_current);
	while (!(this->_Match(size, i_alignment))) {
		if (current->isusing == HeapManager::infoend) {
			return nullptr;
		}
		else
		{
			_current = _TravelToNextDescriptor(current);
			current = reinterpret_cast<INFOBLCOK*>(_current);
		}
	}
	//Successfully made the allocation Move _current pointer to user's location
	_current = _movePointerForward(_current, sizeof(INFOBLCOK));
	unsigned long relative_address = RelativeAddress(_current);
	if (relative_address == _DEBUGADDRESS) {
		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Error, "Memory Debug point reached, it is the %i allocation", num_alloc);
	}
	return _current;
}
#else
void * HeapManager::FindFirstFit(size_t size, unsigned int i_alignment)
{
	//printf("This is one search ==========================================================\n");
	num_alloc++;
	_current = _pHeapMemory;
	INFOBLCOK* current = reinterpret_cast<INFOBLCOK*>(_current);
	while (!(this->_Match(size, i_alignment))) {
		if (current->isusing == HeapManager::infoend) {
			return nullptr;
		}
		else
		{
			_current = _TravelToNextDescriptor(current);
			current = reinterpret_cast<INFOBLCOK*>(_current);
		}
	}
	//Successfully made the allocation Move _current pointer to user's location
	_current = _movePointerForward(_current, sizeof(INFOBLCOK));
	return _current;
}
#endif

#if defined(_DEBUGACTIVITE)
INFOBLCOK * HeapManager::_TravelToNextDescriptor(const INFOBLCOK* const i_ptr) const
{
	size_t difference = reinterpret_cast<size_t>(i_ptr);
	size_t base = reinterpret_cast<size_t>(_pHeapMemory);
	difference -= base;

	char* start = (char*)_movePointerForward(i_ptr, i_ptr->size + sizeof(INFOBLCOK));// lead to place before the users data
	while (*start == HeapManager::fillpadding) {
		start = (char*)_movePointerForward(start, 1);
	}
	if (*start != HeapManager::fillguard) {
		DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "Write on the Guard %p %p", i_ptr, start);
	}

	return (INFOBLCOK*)start;
}
#else
INFOBLCOK * HeapManager::_TravelToNextDescriptor(const INFOBLCOK* const i_ptr) const
{
	return (INFOBLCOK*)_movePointerForward(i_ptr, i_ptr->size + sizeof(INFOBLCOK));// lead to place after the users data
}
#endif
bool HeapManager::_Match(size_t size, unsigned int alignment)
{
	INFOBLCOK* current = reinterpret_cast<INFOBLCOK*>(_current);
	//Notice this size is not accurate in debug mode since the padding guard is not included
	if ((current->size > size + sizeof(INFOBLCOK)) && current->isusing == HeapManager::infoisnotusing) {
		return _TryCut(size, alignment);
	}
	else {
		return false;
	}

}

#if defined(_DEBUGACTIVITE)
bool HeapManager::_TryCut(size_t size, unsigned int alignment)
{
	INFOBLCOK* current = reinterpret_cast<INFOBLCOK*>(_current);
	INFOBLCOK* endinfo = _TravelToNextDescriptor(current);
	size_t realsize = difference(current, endinfo);
	realsize -= sizeof(INFOBLCOK);
	size_t end = reinterpret_cast<size_t>(endinfo);
	end -= size;
	size_t padding = end % alignment;
	//If the real size of the block can not put the size + alignment + padding
	if (realsize < size + padding + sizeof(INFOBLCOK)) {
		return false;
	}
	/*This code could help with defragmentation
	it is not inlcuded in debug mode cause If the use
	filled place before the allocated pointer it won't report error

	else if (realsize < (size + padding + sizeof(INFOBLCOK)) * 2) {
		end -= padding;
		_current = reinterpret_cast<void*>(end);
		void* temppointer = _movePointerForward(_current, size);
		memset(temppointer, HeapManager::fillpadding, padding);
		memset(_current, HeapManager::fillinitialfilled, size);
		current->size = realsize - padding;
		_current = current;
	}*/
	else
	{
		end -= padding;
		_current = reinterpret_cast<void*>(end);
		void* temppointer = _movePointerForward(_current, size);
		memset(temppointer, HeapManager::fillpadding, padding);
		memset(_current, HeapManager::fillinitialfilled, size);
		_current = _movePointerBackward(_current, sizeof(INFOBLCOK));
		_addinfoblock(size);
		size_t output = current->size;
		current->size = realsize - (size + padding + sizeof(INFOBLCOK));
		if (current->size > output) {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Waring, "Wrong Size At %p is used to have length %u", current, output);
		}
	}
	return true;
}
void HeapManager::_addinfoblock(size_t size)
{
	INFOBLCOK* infoblock = reinterpret_cast<INFOBLCOK*>(_current);
#if defined(ENVIRONMENT64)
	memset(&(infoblock->start), HeapManager::fillguard, 8);
	memset(&(infoblock->end), HeapManager::fillguard, 8);
#elif defined(ENVIRONMENT32)
	memset(&(infoblock->start), HeapManager::fillguard, 4);
	memset(&(infoblock->end), HeapManager::fillguard, 4);
#endif
	infoblock->isusing = HeapManager::infoisusing;
	infoblock->size = size; // start with infroblock end with guards
	_current = _movePointerForward(_current, sizeof(INFOBLCOK));
	memset(_current, HeapManager::fillinitialfilled, infoblock->size);
}
#else
bool HeapManager::_TryCut(size_t size, unsigned int alignment)
{
	INFOBLCOK* current = reinterpret_cast<INFOBLCOK*>(_current);
	INFOBLCOK* endinfo = _TravelToNextDescriptor(current);
	size_t realsize = current->size;
	size_t end = reinterpret_cast<size_t>(endinfo);
	// end at position end - size
	end -= size;
	size_t padding = end % alignment;
	//If the real size of the block can not put the size + alignment + padding
	if (realsize < size + padding + sizeof(INFOBLCOK)) {
		return false;
	}
	//Overwrite the descriptor if the padding is correct otherwise return false
	else if ((realsize < (size + padding + sizeof(INFOBLCOK)) * 2)&& ((reinterpret_cast<size_t>(current) + sizeof(INFOBLCOK)) % alignment == 0)) {
		current->isusing = HeapManager::infoisusing;
		return true;
	}
	//Cut the space
	else
	{
		//end at end - size - padding start of user's data?
		end -= padding;
		_current = reinterpret_cast<void*>(end);
		//To heigher address
		//InfoBlock + User data + Padding
		void* temppointer = _movePointerForward(_current, size);
		//memset(temppointer, HeapManager::fillpadding, padding);
		//memset(_current, HeapManager::fillinitialfilled, size);
		_current = _movePointerBackward(_current, sizeof(INFOBLCOK));
		//this function will move _current forward to user's location
		_addinfoblock(size+padding); // realsize
		current->size = realsize - (size + padding + sizeof(INFOBLCOK));
	}
	return true;
}
void HeapManager::_addinfoblock(size_t size)
{
	DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "The new block start at%p", _current);
	INFOBLCOK* infoblock = reinterpret_cast<INFOBLCOK*>(_current);
	infoblock->isusing = HeapManager::infoisusing;
	infoblock->size = size; // start with infroblock end with guards
}
#endif

void HeapManager::Collect()
{
	INFOBLCOK*  d_ptr = (INFOBLCOK*)_pHeapMemory;
	//the start of user info
	void*  i_ptr = _movePointerForward(d_ptr, sizeof(INFOBLCOK));
	while (contains(i_ptr)) {
		if (d_ptr->isusing == HeapManager::infoisnotusing) {
			_current = _TravelToNextDescriptor(d_ptr);
			if (((INFOBLCOK*)_current)->isusing == HeapManager::infoend) {
				return;
			}
			void* next = _TravelToNextDescriptor((INFOBLCOK*)_current);
			if (_tryFastBackCollect()) {
				d_ptr->size = difference(i_ptr, next);
			}
			else {
				d_ptr = (INFOBLCOK*)next;
				i_ptr = _movePointerForward(d_ptr, sizeof(INFOBLCOK));
			}

		}
		else {
			d_ptr = _TravelToNextDescriptor(d_ptr);
			i_ptr = _movePointerForward(d_ptr, sizeof(INFOBLCOK));
		}

	}
}
void HeapManager::ShowFreeBlocks() const
{
	INFOBLCOK* current = (INFOBLCOK*)_pHeapMemory;
	int count = 0;
	while (current->isusing != HeapManager::infoend) {
		if (current->isusing == HeapManager::infoisnotusing) {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Free Block %i has size %u \n", count, current->size);
			count++;
		}
		current = _TravelToNextDescriptor(current);
	}
	return;
}
void HeapManager::ShowOutstandingAllocations() const
{
	INFOBLCOK* current = (INFOBLCOK*)_pHeapMemory;
	int count = 0;
	while (current->isusing != HeapManager::infoend) {
		if (current->isusing == HeapManager::infoisusing) {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "OUtStanding Block %i has size %u \n", count, current->size);
			count++;
		}
		current = _TravelToNextDescriptor(current);
	}
	return;
}
bool HeapManager::AreBlocksFree() const
{
	INFOBLCOK* current = (INFOBLCOK*)_pHeapMemory;
	void* result = nullptr;
	while (current->isusing != HeapManager::infoend) {
		if (current->isusing == HeapManager::infoisusing) {
			result = _movePointerForward(current, sizeof(INFOBLCOK));
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Block %p not freed at relative position %lu", result, RelativeAddress(result));
		}
		current = _TravelToNextDescriptor(current);
	}
	if (result) {
		return false;
	}
	return true;
}
#if defined(_DEBUGACTIVITE)
void HeapManager::_FreeCheck(void* ipr)
{
	INFOBLCOK* temp = (INFOBLCOK*)_movePointerBackward(ipr, sizeof(INFOBLCOK));
	INFOBLCOK* tempnext = (INFOBLCOK*)_TravelToNextDescriptor(temp);
	for (int i = 0; i < 4; i++) {
		if (temp->start[i] != HeapManager::fillguard || temp->end[i] != HeapManager::fillguard || tempnext->start[i] != HeapManager::fillguard) {
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "Write on the Fill Guard %p", ipr);
		}
	}
}
#endif
unsigned long HeapManager::RelativeAddress(void* ptr) const
{
	unsigned long ulbase = reinterpret_cast<unsigned long>(_pHeapMemory);
	unsigned long target = reinterpret_cast<unsigned long>(ptr);
	return target - ulbase;
}
bool HeapManager::free(void * i_ptr)
{
	num_free++;
	bool result = false;
	if (IsAllocated(i_ptr)) {
		DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Heap manager Free Block %p \n", _current);
		_current = _movePointerBackward(i_ptr, sizeof(INFOBLCOK));
		INFOBLCOK* temp = (INFOBLCOK*)_current;
		temp->isusing = HeapManager::infoisnotusing;
		result = true;
#if defined(_DEBUGACTIVITE)
		_FreeCheck(i_ptr);
#endif
		_current = _TravelToNextDescriptor(temp);// move to the next dicriptor
		if (((INFOBLCOK*)_current)->isusing != HeapManager::infoend) {
			void* next = _TravelToNextDescriptor((INFOBLCOK*)_current);
			if (_tryFastBackCollect()) {
				temp->size = difference(i_ptr, next);;
			}
		}

	}

	return result;
}
bool HeapManager::_tryFastBackCollect()
{
	INFOBLCOK* temp = (INFOBLCOK*)_current;
	if (temp->isusing == HeapManager::infoisnotusing) {
		_deletHead();
		return true;
	}
	return false;
}

void HeapManager::_deletHead()
{
	memset(_current, HeapManager::fillinitialfilled, sizeof(INFOBLCOK));
}
bool HeapManager::contains(void * ipr) const
{
	bool result = true;
	void* temp_ptr = _movePointerBackward(ipr, sizeof(INFOBLCOK));
	INFOBLCOK* temp = reinterpret_cast<INFOBLCOK*>(temp_ptr);
	//Sable is wrong
	if (temp->isusing != HeapManager::infoisusing &&temp->isusing != HeapManager::infoisnotusing) {
		return false;
	}
	//Out of range
	if (reinterpret_cast<size_t>(ipr) - reinterpret_cast<rsize_t>(_pHeapMemory) > _sizeHeap) {
		return false;
	}
	return result;
}

bool HeapManager::IsAllocated(void * ipr) const
{
	if (contains(ipr)) {
		void* temp_ptr = _movePointerBackward(ipr, sizeof(INFOBLCOK));
		INFOBLCOK* temp = reinterpret_cast<INFOBLCOK*>(temp_ptr);
		if (temp->isusing == HeapManager::infoisusing) {
			return true;
		}
	}
	unsigned long relative_address = RelativeAddress(ipr);
	DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Waring, "You should not get this %p addresses %lu", ipr, relative_address);
	return false;
}




void HeapManager::SetPointerTo(void * _pHeapMemeoy)
{
	this->_pHeapMemory = _pHeapMemeoy;
}

void* HeapManager::_movePointerForward(const void * const _pointer, size_t number)
{
	size_t address = reinterpret_cast<size_t>(_pointer);
	address += number;
	return reinterpret_cast<void*>(address);
}
void* HeapManager::_movePointerBackward(const void * const _pointer, size_t number)
{
	size_t address = reinterpret_cast<size_t>(_pointer);
	address -= number;
	return reinterpret_cast<void*>(address);
}
size_t HeapManager::difference(void * one, void * two)
{
	size_t address1 = reinterpret_cast<size_t>(one);
	size_t address2 = reinterpret_cast<size_t>(two);
	if (address1 > address2) {
		return address1 - address2;
	}
	else
	{
		return address2 - address1;
	}
}


//#if defined(_DEBUG)
void* operator new(size_t i_size)
{
	/*if (!HeapManager::GeneralHeap) {
		HeapManager::GeneralHeap = malloc(GENERALHEAPSIZE);
		HeapManager::Instance().InitializeWith(GENERALHEAPSIZE, GENERALHEAPSIZE, HeapManager::GeneralHeap);
	}*/
	//HeapManager::Instance().SetPointerTo(HeapManager::GeneralHeap);
	return HeapManager::Instance().FindFirstFit(i_size);
}
//#else
//void* operator new(size_t i_size)
//{
//	/*if (!HeapManager::GeneralHeap) {
//		HeapManager::GeneralHeap = malloc(GENERALHEAPSIZE);
//		HeapManager::Instance().InitializeWith(GENERALHEAPSIZE, GENERALHEAPSIZE, HeapManager::GeneralHeap);
//	}*/
//	//HeapManager::Instance().SetPointerTo(HeapManager::GeneralHeap);
//	return HeapManager::Instance().FindFirstFit(i_size);
//}
//#endif // _DEBUG && _HEAPADDRESS


void operator delete(void * i_ptr)
{
	if ((rand() % 1000) == 0) {
		//HeapManager::Instance().SetPointerTo(HeapManager::GeneralHeap);
		HeapManager::Instance().free(i_ptr);
		HeapManager::Instance().Collect();
	}
	else {
		HeapManager::Instance().free(i_ptr);
	}
}