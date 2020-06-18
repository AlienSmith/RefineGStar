#pragma once
#include "Delegate.h"
#include <list>
#include <string>
#include "../DebuggingTool/ConsolePrint.h"
#include<memory>
namespace GStar {
	class EventUnite {
	public:
		static std::shared_ptr<EventUnite> Create(const std::string& name) {
			return std::make_shared<EventUnite>(std::hash<std::string>{}(name));
		}
		EventUnite(size_t id) :my_id(id) {}
		//Bind a free function
		template<void(*Function)(Event*)>
		void Bind(void) {
			Delegate temp;
			temp.m_stub.first = nullptr;
			temp.m_stub.second = &Delegate::FunctionStub<Function>;
			my_functionlist.push_back(temp);
		}
		//Bind a class method
		template <class C, void(C::*Function)(Event*)>
		void Bind(C* instance) {
			Delegate temp;
			temp.m_stub.first = instance;
			temp.m_stub.second = &Delegate::ClassMethodStub<C, Function>;
			my_functionlist.push_back(temp);
		}
		//Invokes the delegate
		void Invoke(Event* ARG0) {
			std::list<Delegate>::iterator it;
			for (it = my_functionlist.begin(); it != my_functionlist.end(); it++) {
				it->m_stub.second(it->m_stub.first, ARG0);
			}
			//use new to for an event, delete it when calling all functions.
			return;
		}
		inline size_t Getid() {
			return my_id;
		}
	private:
		//Pointer to function do not need to be deleted?
		std::list<Delegate> my_functionlist;
		size_t my_id;
	};
}