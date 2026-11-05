// UnderwaterSim.cpp
#include "UnderwaterSim.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogUnderwaterSim);

void FUnderwaterSimModule::StartupModule()
{
    UE_LOG(LogUnderwaterSim, Log, TEXT("UnderwaterSim module started"));
}

void FUnderwaterSimModule::ShutdownModule()
{
    UE_LOG(LogUnderwaterSim, Log, TEXT("UnderwaterSim module shutdown"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(FUnderwaterSimModule, UnderwaterSim, "UnderwaterSim");

