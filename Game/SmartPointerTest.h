#pragma once
#include"../DebuggingTool/ConsolePrint.h"
namespace GStar {
	class Shape {
	protected:
		int m_size = 10;
	public:
		virtual void PrintType() {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "I am a shape");
		}
		Shape() {}
	};
	class Squre : public Shape {
		int angles = 90;
	public:
		virtual void PrintType() override {
			DEBUG_PRINT(GStar::LOGPlatform::Console, GStar::LOGType::Log, "I am a squre");
		}
		Squre():Shape(){}
	};
	void SmartPointerTestEnterance();
}