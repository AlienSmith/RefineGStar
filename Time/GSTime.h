#pragma once
#include <Windows.h>
#include <GLFW\glfw3.h>
//use GLFW for non windows application
//This feels like the delta time for the last frame instead of this one
namespace GStar {
	class GSTime {
	public:
		static GSTime& Instance() {
			static GSTime instance;
			return instance;
		}
		void Destory() {

		}
		GSTime() :_lasttime(0.0f) {}
		//Call this function after the GLFW window got setup and in every update()
		inline float GetdeltaTime() const {
			return (float)_deltatime;
		}
		inline float GetTotalTime() const {
			return (float)_totaltime;
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
		double _deltatime =0;
		double _lasttime =0;
		double _totaltime =0;
#ifdef _WIN32
		double _base_time=0;
		__int64 _frequency=0;
#endif
	};
}