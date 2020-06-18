#include "TimeToSleepEvent.h"
#include"../HeapManager/MemorySystem.h"
#include"../EventManager/EventManager.h"
#include"../Time/GSTime.h"
#include<thread>
#include <chrono>
void GStar::TimeEventTest()
{
	GStar::InitializeMemorySystem();
	{
		GStar::EventManager::Instance().Initalize();
		GStar::GSTime::Instance().Initialize();
		SleepBug one(2);
		SleepBug two(4);
		SleepBug three(6);
		std::shared_ptr<EventUnite> sleep = GStar::EventManager::Instance().RegisterEvent(TimeToSleepEvent);
		sleep->Bind<SleepBug, &SleepBug::Check>(&one);
		sleep->Bind<SleepBug, &SleepBug::Check>(&two);
		sleep->Bind<SleepBug, &SleepBug::Check>(&three);
		while (GSTime::Instance().GetTotalTime() < 8) {
			GSTime::Instance().Tick();
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::shared_ptr<EventUnite> temp;
			ASSERT(EventManager::Instance().GetEventUnite(TimeToSleepEvent, temp),"Try to obtain a unregistered event");
			TimeEvent tevent(GSTime::Instance().GetTotalTime());
			temp->Invoke(&tevent);
		}
		GSTime::Instance().Destory();
		EventManager::Instance().Destory();
		
	}
	GStar::DestroyMemorySystem();
}
