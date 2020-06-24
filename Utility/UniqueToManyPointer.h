#pragma once
#pragma once
#include "../DebuggingTool/ConsolePrint.h"
#include <stdint.h>
namespace GStar {
	enum class PointerStatus
	{
		Valid,
		InValid
	};
#if defined(_WIN32)
	typedef uint32_t	ref_counter_t;
#else
	typedef uint64_t    ref_counter_t;
#endif
	template<class T>
	class RootPointer;
	template<class T>
	class SpectatorPointer {
		template<class U>
		friend class SpectatorPointer;
		template<class U>
		friend class RootPointer;
	private:
		T* m_ptr;
		ref_counter_t* m_Spectatorcount;
		PointerStatus* m_status;
		SpectatorPointer(T* ptr, ref_counter_t* reference_count, PointerStatus* i_status) :m_ptr(ptr), m_Spectatorcount(reference_count),m_status(i_status) {
			(*m_Spectatorcount)++;
		}
		inline void ReleaseReference() {
			if (--(*m_Spectatorcount) == 0) {
				if (*(m_status) == PointerStatus::InValid) {
					delete m_Spectatorcount;
					delete m_status;
				}
			}
		}
	public:
		inline bool isValid() {
			return (*m_status)==PointerStatus::Valid;
		}
		inline void Reset() {
			m_ptr = nullptr;
			m_Spectatorcount = nullptr;
			m_status = nullptr;
		}
		SpectatorPointer(SpectatorPointer&& i_other) :
			m_ptr(i_other.m_ptr),
			m_Spectatorcount(i_other.m_Spectatorcount),
			m_status(i_other.m_status)
		{
			ASSERT(i_other.m_ptr != nullptr, "try to move a nullptr");
			i_other.Reset();
		}
		template<class U>
		SpectatorPointer(SpectatorPointer<U>&& i_other) :
			m_ptr(i_other.m_ptr),
			m_Spectatorcount(i_other.m_Spectatorcount),
			m_status(i_other.m_status)
		{
			ASSERT(i_other.m_ptr != nullptr, "try to copy a nullptr");
			i_other.Reset();
		}
		inline SpectatorPointer& operator = (SpectatorPointer<T>&& i_other) {
			ASSERT(i_other.m_ptr != nullptr, "try to move a nullptr");
			if (m_ptr == i_other.m_ptr) {
				DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "SelfAssignment");
				return *this;
			}
			ReleaseReference();
			m_ptr = i_other.m_ptr;
			m_Spectatorcount = i_other.m_Spectatorcount;
			m_status = i_other.m_status;
			i_other.Reset();
			return *this;
		}
		template<class U>
		inline SpectatorPointer& operator = (SpectatorPointer<U>&& i_other) {
			ASSERT(i_other.m_ptr != nullptr, "try to move a nullptr");
			if (m_ptr == i_other.m_ptr) {
				DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "SelfAssignment");
				return *this;
			}
			ReleaseReference();
			m_ptr = i_other.m_ptr;
			m_Spectatorcount = i_other.m_Spectatorcount;
			i_other.Reset();
			return *this;
		}

		SpectatorPointer() :m_ptr(nullptr), m_Spectatorcount(nullptr),m_status(nullptr) {}
		SpectatorPointer(SpectatorPointer& i_other) :
			m_ptr(i_other.m_ptr),
			m_Spectatorcount(i_other.m_Spectatorcount),
			m_status(i_other.m_status)
		{
			ASSERT(i_other.m_ptr != nullptr, "try to copy a nullptr");
			(*m_Spectatorcount)++;
		}
		template<class U>
		SpectatorPointer(SpectatorPointer<U>& i_other) :
			m_ptr(i_other.m_ptr),
			m_Spectatorcount(i_other.m_Spectatorcount),
			m_status(i_other.m_status)
		{
			ASSERT(i_other.m_ptr != nullptr, "try to copy a nullptr");
			(*m_Spectatorcount)++;
		}
		inline SpectatorPointer& operator = (SpectatorPointer<T>& i_other) {
			ASSERT(i_other.m_ptr != nullptr, "try to copy a nullptr");
			if (m_ptr == i_other.m_ptr) {
				DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "SelfAssignment");
				return *this;
			}
			ReleaseReference();
			m_ptr = i_other.m_ptr;
			m_Spectatorcount = i_other.m_Spectatorcount;
			(*m_Spectatorcount)++;
			return *this;
		}
		template<class U>
		inline SpectatorPointer& operator = (SpectatorPointer<U>& i_other) {
			ASSERT(i_other.m_ptr != nullptr, "try to copy a nullptr");
			if (m_ptr == i_other.m_ptr) {
				DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "SelfAssignment");
				return *this;
			}
			ReleaseReference();
			m_ptr = i_other.m_ptr;
			m_Spectatorcount = i_other.m_Spectatorcount;
			(*m_Spectatorcount)++;
			return *this;
		}
		inline T* operator ->() {
			return m_ptr;
		}
		inline T& operator *() {
			ASSERT(m_ptr != nullptr, "RootPointer is null");
			return *m_ptr;
		}
		inline bool operator == (const SpectatorPointer<T>& i_other) const {
			return m_Spectatorcount == i_other.m_Spectatorcount;
		}
		template<class U>
		inline bool operator == (const SpectatorPointer<U>& i_other) const {
			return m_Spectatorcount == i_other.m_Spectatorcount;
		}
		inline bool operator ==(T* i_other_ptr) const {
			return m_ptr == i_other_ptr;
		}
		template<class U>
		inline bool operator ==(U* i_other_ptr) const {
			return m_ptr == i_other_ptr;
		}
		inline bool operator ==(std::nullptr_t nullp) const {
			return m_ptr == nullp;
		}

		inline bool operator != (const SpectatorPointer<T>& i_other) const {
			return m_Spectatorcount == i_other.m_Spectatorcount;
		}
		template<class U>
		inline bool operator != (const SpectatorPointer<U>& i_other) const {
			return m_Spectatorcount == i_other.m_Spectatorcount;
		}
		inline bool operator !=(T* i_other_ptr) const {
			return m_ptr == i_other_ptr;
		}
		template<class U>
		inline bool operator !=(U* i_other_ptr) const {
			return m_ptr == i_other_ptr;
		}
		inline bool operator !=(std::nullptr_t nullp) const {
			return m_ptr == nullp;
		}
		inline bool operator!() const {
			return !m_ptr;
		}
		inline operator bool() const {
			return m_ptr;
		}
	};
	template<class T>
	//m_prt could be nullptr due to nullinitialization and the copy assignment operator would swap datas
	//You can Intentionally release the resouce of this pointer by assign it to an dying block
	//Root Pointer won't be responsible for deleteing m_SpectatorCount  SpectatorPoniter is
	class RootPointer {
		template<class U>
		friend class SpectatorPointer;
		template<class U>
		friend class RootPointer;
	private:
		T* m_ptr;
		ref_counter_t* m_Spectatorcount;
		PointerStatus* m_Status;

		RootPointer(T* ptr):m_ptr(ptr),m_Spectatorcount(nullptr),m_Status(new PointerStatus(PointerStatus::Valid)){
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Pointer Constructor");
		}
		inline void Reset() {
			m_ptr = nullptr;
			m_Spectatorcount = nullptr;
			m_Status = nullptr;
		}
	public:
		/*inline T* GetUnderlingReference() {
			return m_ptr;
		}*/
		//PlaceHolder for future pointers
		RootPointer() :m_ptr(nullptr), m_Spectatorcount(nullptr),m_Status(nullptr) {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Default Constructor");
		}
		RootPointer(RootPointer<T>&& i_other):m_ptr(i_other.m_ptr),m_Spectatorcount(i_other.m_Spectatorcount),m_Status(i_other.m_Status) {
			i_other.Reset();
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Move Constructor");
		}
		//Notice this will only support automatic Upcasting, From children ptr to parent ones
		template<class U>
		RootPointer(RootPointer<U>&& i_other):m_ptr(static_cast<T*>(i_other.m_ptr)), m_Spectatorcount(i_other.m_Spectatorcount),m_Status(i_other.m_Status) {
			i_other.Reset();
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Move Constructor");
		}
		//Do not try to return a rvalue reference.
		//after the reference return and before you can move the content into another instance the content got erased
		template<class ... Types>
		static inline RootPointer make_rootpointer(Types ... args) {
			return RootPointer(new T(args...));
		}
		inline RootPointer& operator = (RootPointer<T>&& i_other) {
			Destory();
			m_ptr = i_other.m_ptr;
			m_Spectatorcount = i_other.m_Spectatorcount;
			m_Status = i_other.m_Status;
			i_other.Reset();
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Move Assignment ");
		}
		//Expand to other pointer in the inheritence Hirachy Swap the content
		template<class U>
		inline RootPointer & operator = (RootPointer<U> && i_other) {
			Destory();
			m_ptr = i_other.m_ptr;
			m_Spectatorcount = i_other.m_Spectatorcount;
			m_Status = i_other.m_Status;
			i_other.Reset();
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Move Assignment ");
		}
		RootPointer(RootPointer& i_other) = delete;
		template<class U>
		RootPointer(RootPointer<U>& i_other) = delete;
		RootPointer& operator = (RootPointer<T>& i_other) = delete;
		template<class U>
		RootPointer& operator = (RootPointer<U>& i_other) = delete;
		inline void Destory() {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "Delete");
			//the m_ptr can be nullptr by Constructed to or from or assign to or from a default RootPointer
			if (m_ptr) {
				delete m_ptr;
				(*m_Status) = PointerStatus::InValid;
				if ((m_Spectatorcount) && (*m_Spectatorcount == 0)) {
					delete m_Spectatorcount;
					delete m_Status;
				}
			}
			else {
				ASSERT( (!m_Status)||((*m_Status) == PointerStatus::InValid), "The pointer should not be a nullptr");
			}
		}
		~RootPointer() {
			Destory();
			Reset();
		}
		inline T* operator ->() {
			ASSERT(m_ptr != nullptr, "RootPointer is null");
			return m_ptr;
		}
		inline T& operator *() {
			ASSERT(m_ptr != nullptr, "RootPointer is null");
			return *m_ptr;
		}
		SpectatorPointer<T> make_SpecatorPointer() {
			if (!m_Spectatorcount) {
				m_Spectatorcount = new ref_counter_t(0);
			}
			return SpectatorPointer<T>(m_ptr,m_Spectatorcount,m_Status);
		}

		//Event though these compareson operators are here, there are very little reasons to use them
		inline bool operator == (const RootPointer<T>& i_other) const {
			return m_ptr == i_other.m_ptr;
		}
		template<class U>
		inline bool operator == (const RootPointer<U>& i_other) const {
			return m_ptr == i_other.m_ptr;
		}
		inline bool operator ==(T* i_other_ptr) const {
			return m_ptr == i_other_ptr;
		}
		template<class U>
		inline bool operator ==(U* i_other_ptr) const {
			return m_ptr == i_other_ptr;
		}
		inline bool operator ==(std::nullptr_t nullp) const {
			return m_ptr == nullp;
		}

		inline bool operator != (const RootPointer<T>& i_other) const {
			return m_ptr == i_other.m_ptr;
		}
		template<class U>
		inline bool operator != (const RootPointer<U>& i_other) const {
			return m_ptr == i_other.m_ptr;
		}
		inline bool operator !=(T* i_other_ptr) const {
			return m_ptr == i_other_ptr;
		}
		template<class U>
		inline bool operator !=(U* i_other_ptr) const {
			return m_ptr == i_other_ptr;
		}
		inline bool operator !=(std::nullptr_t nullp) const {
			return m_ptr == nullp;
		}
		inline bool operator!() const {
			return !m_ptr;
		}
		inline operator bool() const {
			return m_ptr;
		}
	};
}