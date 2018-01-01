#include "MeteorSingletonLibrary.h"
#include "Engine/Engine.h"

UMeteorSingletonLibrary::UMeteorSingletonLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UMeteorSingleton* UMeteorSingletonLibrary::GetMeteorSingleton()
{
	UMeteorSingleton* MeteorSingleton = Cast<UMeteorSingleton>(GEngine->GameSingleton);
	if (MeteorSingleton && MeteorSingleton->IsValidLowLevelFast(false))
	{
		return MeteorSingleton;
	}
	return nullptr;
}
