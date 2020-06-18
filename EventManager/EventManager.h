#pragma once
#include "EventUnite.h"
#include <string>
#include <memory>
namespace GStar {
	class EventManager {
		//All the EventUnites die with the manager;
	public:
		static EventManager& Instance() {
			static EventManager instance;
			return instance;
		}
		void Initalize() {
			my_eventlist = new std::list<std::shared_ptr<EventUnite>>();
		}
		void Destory() {
			delete my_eventlist;
			my_eventlist = nullptr;
		}
		std::shared_ptr<EventUnite> RegisterEvent(const std::string& string) {
			std::shared_ptr<EventUnite> temp = EventUnite::Create(string);
			my_eventlist->push_back(temp);
			return temp;
		}
		bool GetEventUnite(const std::string& string,std::shared_ptr<EventUnite>& event) {
			size_t tempid = std::hash<std::string>{}(string);
			std::list<std::shared_ptr<EventUnite>>::iterator it;
			for (it = my_eventlist->begin(); it != my_eventlist->end(); it++) {
				if ((*it)->Getid() == tempid) {
					event = *it;
					return true;
				}
			}
			DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Error, "Try to access eventunit %s has not been registered",string);
			return false;
		}
		~EventManager() {
		}
		static void Terminate() {
			if (instance) {
				delete instance;
			}
		}
	private:
		EventManager():my_eventlist(nullptr) {}
		EventManager(const EventManager& other) = delete;
		EventManager& operator = (const EventManager& other) = delete;
		static EventManager* instance;
		//I have no Idea if std list would dynamically allocate some memory when calling their constructor hence it is a pointer here
		std::list<std::shared_ptr<EventUnite>>* my_eventlist;
	};
}