
#pragma once  
#include "MeteorSingleton.h"  
#include "Kismet/BlueprintFunctionLibrary.h"

#include "MeteorFuncLib.generated.h"

UCLASS()
class UMeteorFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "Meteor Singleton")
	static UMeteorSingleton* GetMeteorSingleton();

};

