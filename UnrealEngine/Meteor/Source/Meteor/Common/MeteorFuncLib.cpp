#include "MeteorFuncLib.h"

#include "Engine/Engine.h"


UMeteorFuncLib::UMeteorFuncLib(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UMeteorSingleton* UMeteorFuncLib::GetMeteorSingleton()
{
	UMeteorSingleton* MeteorSingleton = Cast<UMeteorSingleton>(GEngine->GameSingleton);
	if (MeteorSingleton && MeteorSingleton->IsValidLowLevelFast(false))
	{
		return MeteorSingleton;
	}
	return nullptr;
}
