// 2026 sabaka-chabaka

#include "CarHealthTools.h"

bool UCarHealthTools::CanEngineStart(float CurrentRPM, float MinRunningRPM)
{
	float StartChance = FMath::Clamp(GetMassEngineHealth() / 100.0f, 0.3f, 0.95f);
        
	if (FMath::FRand() < StartChance && CurrentRPM > MinRunningRPM)
	{
		return true;
	}
	
	return false;
}

float UCarHealthTools::GetMassEngineHealth()
{
	return 50.0f;	
}