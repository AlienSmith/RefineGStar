#pragma once
#include <Windows.h>
#include <GLFW\glfw3.h>
#include <memory>
//use GLFW for non windows application
//This feels like the delta time for the last frame instead of this one
namespace GStar {
	class GSTime {
	public:
		static GSTime& Instance() {
			if (!instance) {
				instance = new GSTime();
			}
			return *instance;
		}
		static void Terminate() {
			if (instance) {
				delete instance;
			}
		}
		//Call this function after the GLFW window got setup and in every update()
		inline double GetdeltaTime() const {
			return _deltatime;
		}
		inline double GetTotalTime() const {
			return _totaltime;
		}
#ifdef _WIN32
		inline void Initialize() {
			QueryPerformanceFrequency((LARGE_INTEGER*)&_frequency);
			__int64 count;
			QueryPerformanceCounter((LARGE_INTEGER*)&count);
			_base_time = (count* 1.0 / _frequency);
			_totaltime = 0.0;
		}
		inline void Tick() {
			_lasttime = _totaltime;
			__int64 count;
			QueryPerformanceCounter((LARGE_INTEGER*)&count);
			_totaltime = (count*1.0 / _frequency) - _base_time;
			_deltatime = _totaltime - _lasttime;
		}
#else
		inline void Initialize() {
			_totaltime = 0.0;
		}
		inline void Tick() {
			_lasttime = _totaltime;
			_totaltime = glfwGetTime();
			_deltatime = _totaltime - _lasttime;
		}
#endif
	private:
		GSTime() :_lasttime(0.0f) {}
		static GSTime* instance;
		double _deltatime;
		double _lasttime;
		double _totaltime;
#ifdef _WIN32
		double _base_time;
		__int64 _frequency;
#endif
	};
}