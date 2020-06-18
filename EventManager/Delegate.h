#pragma once
#include <utility>
#include "Event.h"
namespace GStar {
	//The base Class For All Events
	class Delegate {
	public:
		typedef void* InstancePtr;
		typedef void(*InternalFunction)(InstancePtr, Event*);
		typedef std::pair<InstancePtr, InternalFunction> Stub;
		template <void (*Function)(Event*)>
		static void FunctionStub(InstancePtr, Event* ATG0) {
			return (Function)(ATG0);
		}
		template <class C, void(C::*Function)(Event*)>
		static void ClassMethodStub(InstancePtr instance, Event* ARG0) {
			return (static_cast<C*>(instance)->*Function)(ARG0);
		}
		Stub m_stub;
		Delegate(void) :m_stub(nullptr, nullptr) {}
	};
}