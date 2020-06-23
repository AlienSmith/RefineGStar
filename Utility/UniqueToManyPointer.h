#pragma once
#pragma once
#include "../DebuggingTool/ConsolePrint.h"
#include <stdint.h>
namespace GStar {
#if defined(_DEBUG)
	enum class PointerStatus
	{
		Valid,
		InValid
	};
#endif
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
		SpectatorPointer(T* ptr, ref_counter_t* reference_count) :m_ptr(ptr), m_Spectatorcount(reference_count) {
			reference_count++;
		}
		inline void ReleaseReference() {
			ASSERT(m_Spectatorcount != nullptr, "try to release a nullptr");
			//should this one be here since
			if (--(*m_Spectatorcount) == 0) {
				if (!m_ptr) {
					delete m_Spectatorcount;
				}
			}
		}
		inline bool isValid() {
			return m_ptr;
		}
	public:
		SpectatorPointer() :m_ptr(nullptr), m_Spectatorcount(new ref_counter_t(1)) {}
		SpectatorPointer(const SpectatorPointer& i_other) :
			m_ptr(i_other.m_ptr),
			m_Spectatorcount(i_other.m_Spectatorcount)
		{
			ASSERT(i_other.m_ptr != nullptr, "try to copy a nullptr");
			(*m_Spectatorcount)++;
		}
		template<class U>
		SpectatorPointer(const SpectatorPointer<U>& i_other) :
			m_ptr(i_other.m_ptr),
			m_Spectatorcount(i_other.m_Spectatorcount)
		{
			ASSERT(i_other.m_ptr != nullptr, "try to copy a nullptr");
			(*m_Spectatorcount)++;
		}
		inline SpectatorPointer& operator = (const SpectatorPointer<T>& i_other) {
			ASSERT(i_other.m_ptr != nullptr, "try to copy a nullptr");
			if (m_ptr == i_other.m_ptr) {
				DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "SelfAssignment");
				return *this;
			}
			ReleaseReference();
			m_ptr = i_other.m_ptr;
			m_RefCount = i_other.m_RefCount;
			m_Spectatorcount = i_other.m_Spectatorcount;
			(*m_Spectatorcount)++;
			return *this;
		}
		template<class U>
		inline SpectatorPointer& operator = (const SpectatorPointer<U>& i_other) {
			ASSERT(i_other.m_ptr != nullptr, "try to copy a nullptr");
			if (m_ptr == i_other.m_ptr) {
				DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "SelfAssignment");
				return *this;
			}
			ReleaseReference();
			m_ptr = i_other.m_ptr;
			m_RefCount = i_other.m_RefCount;
			m_Spectatorcount = i_other.m_Spectatorcount;
			(*m_Spectatorcount)++;
			return *this;
		}
		inline T* operator ->() {
			if (isValid()) {
				return m_ptr;
			}
		}
		inline T& operator *() {
			if (isValid) {
				ASSERT(m_ptr != nullptr, "RootPointer is null");
				return *m_ptr;
			}
		}
	};
	template<class T>
	//m_prt could be nullptr due to nullinitialization and the copy assignment operator would swap datas
	//You can Intentionally release the resouce of this pointer by assign it to an dying block or just call ReleaseReference
	//Root Pointer won't be responsible for deleteing m_SpectatorCount  SpectatorPoniter is
	class RootPointer {
		template<class U>
		friend class SpectatorPointer;
		template<class U>
		friend class RootPointer;
	private:
		T* m_ptr;
		ref_counter_t* m_Spectatorcount;
#if defined(_DEBUG)
		SpectatorPointer m_Status;
#endif
		inline void ReleaseReference() {
			ASSERT(m_ptr != nullptr, "try to release a nullptr");
			delete m_ptr;
			m_ptr = nullptr;
#if defined(_DEBUG)
			m_Status = PointerStatus::InValid;
#endif
		}
		RootPointer(T* ptr):m_ptr(ptr),m_Spectatorcount(nullptr){
#if defined(_DEBUG)
			m_Status = PointerStatus::Valid;
#endif
		}
	public:
		/*inline T* GetUnderlingReference() {
			return m_ptr;
		}*/
		//PlaceHolder for future pointers
		RootPointer() :m_ptr(nullptr), m_Spectatorcount(nullptr) {
#if defined(_DEBUG)
			m_Status = PointerStatus::InValid;
#endif
		}
		RootPointer(RootPointer<T>&& i_other):m_ptr(i_other.m_ptr),m_Spectatorcount(i_other.m_Spectatorcount) {
			i_other.m_ptr = nullptr;
			i_other.m_Spectatorcount = nullptr;
#if defined(_DEBUG)
			m_Status = i_other.m_Status;
			i_other.m_Spectatorcount = PointerStatus::InValid;
#endif
		}
		//Notice this will only support automatic Upcasting, From children ptr to parent ones
		template<class U>
		RootPointer(RootPointer<U>&& i_other):m_ptr(static_cast<T*>(i_other.m_ptr)), m_Spectatorcount(i_other.m_Spectatorcount) {
			i_other.m_ptr = nullptr;
			i_other.m_Spectatorcount = nullptr;
#if defined(_DEBUG)
			m_Status = i_other.m_Status;
			i_other.m_Spectatorcount = PointerStatus::InValid;
#endif
		}
		template<typename ... Types>
		inline RootPointer&& make_rootpointer(Types ... args) {
			return RootPointer(new T(args));
		}
		//Swap the content
		inline RootPointer&& operator = (RootPointer<T>&& i_other) {
			T* temp = m_ptr;
			ref_counter_t temp_count = m_Spectatorcount;
			m_ptr = i_other.m_ptr;
			m_Spectatorcount = i_other.m_Spectatorcount;
			i_other.m_ptr = temp;
			i_other.m_ptr = temp_count;
#if defined(_DEBUG)
			m_Status = m_Status;
			i_other.m_Spectatorcount = PointerStatus::InValid;
#endif
		}
		//Expand to other pointer in the inheritence Hirachy Swap the content
		template<class U>
		inline RootPointer && operator = (RootPointer<U> && i_other) {
			U* temp = static_cast<U*>(m_ptr);
			ref_counter_t temp_count = m_Spectatorcount;
			m_ptr = static_cast<T*>(i_other.m_ptr);
			m_Spectatorcount = i_other.m_Spectatorcount;
			i_other.m_ptr = temp;
			i_other.m_ptr = temp_count;
#if defined(_DEBUG)
			m_Status = m_Status;
			i_other.m_Spectatorcount = PointerStatus::InValid;
#endif
		}
		RootPointer(RootPointer& i_other) = delete;
		template<class U>
		RootPointer(RootPointer<U>& i_other) = delete;
		RootPointer& operator = (RootPointer<T>& i_other) = delete;
		template<class U>
		RootPointer& operator = (RootPointer<U>& i_other) = delete;
		~RootPointer() {
			//the m_ptr can be nullptr by Constructed to or from or assign to or from a default RootPointer
			if (m_ptr) {
#if defined(_DEBUG)
				ASSERT(m_Status == PointerStatus::InValid, "The pointer should not be a nullptr");
#endif
				delete m_ptr;
			}
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
			return SpectatorPointer<T>(m_ptr,m_Spectatorcount);
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