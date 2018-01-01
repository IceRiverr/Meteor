
#pragma once  
#include "MeteorSingleton.h"  
#include "MeteorSingletonLibrary.generated.h"  

UCLASS()
class UMeteorSingletonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "Meteor Singleton")
	static UMeteorSingleton* GetMeteorSingleton();

};

