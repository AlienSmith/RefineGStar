#include"../DebuggingTool/ConsolePrint.h"
#include"../Time/GSTime.h"
int main() {
	GStar::GSTime::Instance().Initialize();
	for (int i = 0; i < 10; i++) {
		GStar::GSTime::Instance().Tick();
		DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "The memory block Start at %f", GStar::GSTime::Instance().GetTotalTime());
		DEBUG_PRINT(GStar::LOGPlatform::Output, GStar::LOGType::Log, "The memory block Start at %f", GStar::GSTime::Instance().GetdeltaTime());
	}
	GStar::GSTime::Terminate();
	return 0;
}