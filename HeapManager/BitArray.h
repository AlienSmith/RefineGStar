#pragma once
#define out
#include "HeapManager.h"
#include "stdint.h"
#include <stdio.h>
#include <string.h>
#include <intrin.h>
#include "../DebuggingTool/ConsolePrint.h"
#define Byte 8
//Comment
namespace GStar {
	class BitArray {
	public:
		BitArray(size_t i_numBit, void* i_pHead, bool i_startClear) :_numbits(i_numBit), _Phead(static_cast<uint8_t*>(i_pHead)) { i_startClear ? ClearAll() : SetAll(); }

		inline void ClearAll() { memset(reinterpret_cast<void*>(_Phead), 0, (_numbits + 7) / 8); }
		inline void SetAll() { memset(reinterpret_cast<void*>(_Phead), 255, (_numbits + 7) / 8); }

		bool AreAllClear() const;
		bool AreAllSet() const;

		inline bool isBitSet(size_t i_bitNumber) const;
		inline bool isBitFree(size_t i_bitNumber) const;

		inline void SetBit(size_t i_bitNumber);
		inline void ClearBit(size_t i_bitNumber);

		bool GetFirstClearBit(size_t& out o_bitNumber) const;
		bool GetFirstSetBit(size_t& out o_bitNumber) const;
		// this start with 1 for consistancy
		bool operator[](size_t i_bitNumber) const;
		~BitArray() {
			HeapManager::Instance().free(_Phead);
		}
		BitArray& operator = (const BitArray&) = delete;
	private:
		const size_t _numbits;
		uint8_t* const _Phead;
	};
#if defined(ENVIRONMENT64)
	inline bool BitArray::GetFirstClearBit(size_t &out o_bitNumber) const
	{
		int count = 0;
		uint64_t* current = reinterpret_cast<uint64_t*> (_Phead);
		uint64_t other = ~(*current);
		bool NoneZero;
		unsigned long index;
		NoneZero = _BitScanForward64(&index, other);
		while (!NoneZero && count < _numbits / (sizeof(*current)*Byte)) {
			current++;
			count++;
			other = ~(*current);
			NoneZero = _BitScanForward64(&index, other);
		}
		index += (sizeof(*current)*Byte)*count;
		if (index >= _numbits) {
			return false;
		}
		o_bitNumber = index;
		return NoneZero;
	}
	inline bool BitArray::GetFirstSetBit(size_t &out o_bitNumber) const
	{
		int count = 0;
		uint64_t* current = reinterpret_cast<uint64_t*> (_Phead);
		bool NoneZero;
		unsigned long index;
		NoneZero = _BitScanForward64(&index, *current);
		while (!NoneZero && count < _numbits / (sizeof(*current)*Byte)) {
			current++;
			count++;
			NoneZero = _BitScanForward64(&index, *current);
		}
		index += (sizeof(*current)*Byte)*count;
		if (index >= _numbits) {
			return false;
		}
		o_bitNumber = index;
		return NoneZero;
	}
	inline bool BitArray::AreAllClear() const
	{
		int count = 0;
		uint64_t* current = reinterpret_cast<uint64_t*> (_Phead);
		bool NoneZero;
		unsigned long index;
		NoneZero = _BitScanForward64(&index, *current);
		while (!NoneZero && count < _numbits / (sizeof(*current)*Byte)) {
			current++;
			count++;
			NoneZero = _BitScanForward64(&index, *current);
		}
		index += (sizeof(*current)*Byte)*count;
		if (index >= _numbits) {
			return true;
		}
		return !NoneZero;
	}
	inline bool BitArray::AreAllSet() const
	{
		int count = 0;
		uint64_t* current = reinterpret_cast<uint64_t*> (_Phead);
		uint64_t other = ~(*current);
		bool NoneZero;
		unsigned long index;
		NoneZero = _BitScanForward64(&index, other);
		while (!NoneZero && count < _numbits / (sizeof(*current)*Byte)) {
			current++;
			count++;
			other = ~(*current);
			NoneZero = _BitScanForward64(&index, other);
		}
		index += (sizeof(*current)*Byte)*count;
		if (index >= _numbits) {
			return true;
		}
		return !NoneZero;
	}
#else
	inline bool BitArray::GetFirstClearBit(size_t &out o_bitNumber) const
	{
		size_t count = 0;
		uint32_t* current = reinterpret_cast<uint32_t*> (_Phead);
		uint32_t other = ~(*current);
		bool NoneZero;
		unsigned long index;
		NoneZero = _BitScanForward(&index, other);
		while (!NoneZero && count < _numbits / (sizeof(*current)*Byte)) {
			current++;
			count++;
			other = ~(*current);
			NoneZero = _BitScanForward(&index, other);
		}
		index += (sizeof(*current)*Byte)*count;
		if (index >= _numbits) {
			return false;
		}
		o_bitNumber = index;
		return NoneZero;
	}
	inline bool BitArray::GetFirstSetBit(size_t &out o_bitNumber) const
	{
		size_t count = 0;
		uint32_t* current = reinterpret_cast<uint32_t*> (_Phead);
		bool NoneZero;
		unsigned long index;
		NoneZero = _BitScanForward(&index, *current);
		while (!NoneZero && count < _numbits/(sizeof(*current)*Byte)) {
			current++;
			count++;
			NoneZero = _BitScanForward(&index, *current);
		}
		index += (sizeof(*current)*Byte)*count;
		if (index >= _numbits) {
			return false;
		}
		o_bitNumber = index;
		return NoneZero;
	}
	inline bool BitArray::AreAllClear() const
	{
		size_t count = 0;
		uint32_t* current = reinterpret_cast<uint32_t*> (_Phead);
		bool NoneZero;
		unsigned long index;
		NoneZero = _BitScanForward(&index, *current);
		while (!NoneZero && count < _numbits / (sizeof(*current)*Byte)) {
			current++;
			count++;
			NoneZero = _BitScanForward(&index, *current);
		}
		index += (sizeof(*current)*Byte)*count;
		if (index >= _numbits) {
			return true;
		}
		return !NoneZero;
	}
	inline bool BitArray::AreAllSet() const
	{
		size_t count = 0;
		uint32_t* current = reinterpret_cast<uint32_t*> (_Phead);
		uint32_t other = ~(*current);
		bool NoneZero;
		unsigned long index;
		NoneZero = _BitScanForward(&index, other);
		while (!NoneZero && count < _numbits / (sizeof(*current)*Byte)) {
			current++;
			count++;
			other = ~(*current);
			NoneZero = _BitScanForward(&index, other);
		}
		index += (sizeof(*current)*Byte)*count;
		if (index >= _numbits) {
			return true;
		}
		return !NoneZero;
	}
#endif
	inline bool BitArray::isBitSet(size_t i_bitNumber) const
	{
		if (i_bitNumber >= _numbits) {
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "try to access memeory out side of array");
			return false;
		}
		bool result;
		uint8_t* current = reinterpret_cast<uint8_t*>(_Phead);
		size_t count = ((i_bitNumber) / 8);
		size_t mod = ((i_bitNumber) % 8);
		current += count;
		result = (*current >> mod) & 1U;
		return result;
	}
	inline bool BitArray::isBitFree(size_t i_bitNumber) const
	{
		if ( i_bitNumber >= _numbits) {
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "try to access memeory out side of array");
			return false;
		}
		bool result;
		uint8_t* current = reinterpret_cast<uint8_t*>(_Phead);
		size_t count = ((i_bitNumber) / 8);
		size_t mod = ((i_bitNumber) % 8);
		current += count;
		result = (*current >> mod) & 1U;
		return !result;
	}
	inline void BitArray::SetBit(size_t i_bitNumber) {
		if (i_bitNumber >= _numbits) {
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "try to access memeory out side of array");
		}
		uint8_t* current = reinterpret_cast<uint8_t*>(_Phead);
		size_t count = ((i_bitNumber) / 8);
		size_t mod = ((i_bitNumber) % 8);
		current += count;
		*current|= 1UL << mod;
	}
	inline void BitArray::ClearBit(size_t i_bitNumber)
	{
		if ( i_bitNumber >= _numbits) {
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "try to access memeory out side of array");
		}
		uint8_t* current = reinterpret_cast<uint8_t*>(_Phead);
		size_t count = ((i_bitNumber) / 8);
		size_t mod = ((i_bitNumber) % 8);
		current += count;
		*current &= ~(1UL << mod);
	}
	inline bool BitArray::operator[](size_t i_index) const
	{
		if ( i_index >= _numbits) {
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Waring, "try to access memeory out side of array");
		}
		bool result;
		uint8_t* current = reinterpret_cast<uint8_t*>(_Phead);
		size_t count = ((i_index) / 8);
		size_t mod = ((i_index) % 8);
		current += count;
		result = (*current >> mod) & 1U;
		return result;
	}
}