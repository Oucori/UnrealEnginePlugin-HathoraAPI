#pragma once
#include "EHathoraLobbyVisibility.h"

#include "SHathoraLobbyInfo.generated.h"

USTRUCT(BlueprintType)
struct FSHathoraLobbyInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString createdAt;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString createdBy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EHathoraLobbyVisibility> visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString region;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString roomId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString appId;
};
