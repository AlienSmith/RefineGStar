#pragma once
#include<string>
#include"../EventManager/Event.h"
#include"../DebuggingTool/ConsolePrint.h"
namespace GStar {
	const static std::string TimeToSleepEvent = "Time to Sleep";
	struct TimeEvent :public Event {
	public:
		float totaltime;
		TimeEvent(float input):totaltime(input){}
	};
	class SleepBug {
	public:
		SleepBug(int time):_time(time){}
		void Check(GStar::Event* timeEvent) {
			TimeEvent* tick = static_cast<TimeEvent*>(timeEvent);
			if (tick->totaltime > _time) {
				DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%d is sleeping ", _time);
			}
			else {
				DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "%d is still awake ", _time);
			}
		}
		int _time;
	};
	void TimeEventTest();
}