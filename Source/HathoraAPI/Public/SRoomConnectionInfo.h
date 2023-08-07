#pragma once

#include "SRoomExposedPort.h"
#include "SRoomConnectionInfo.generated.h"

USTRUCT(BlueprintType)
struct FSRoomConnectionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSRoomExposedPort> additionalExposedPorts;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSRoomExposedPort exposedPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString roomId;
	
};
