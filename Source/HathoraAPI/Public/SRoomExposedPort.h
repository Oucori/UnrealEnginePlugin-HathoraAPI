#pragma once

#include "SRoomExposedPort.generated.h"

USTRUCT(BlueprintType)
struct FSRoomExposedPort
{
	GENERATED_BODY()
    	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString transportType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 port;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString host;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;
};
