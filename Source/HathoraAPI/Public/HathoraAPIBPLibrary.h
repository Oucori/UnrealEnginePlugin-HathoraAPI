// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Http.h"
#include "SHathoraLobbyInfo.h"
#include "SRoomConnectionInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HathoraAPIBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/

// Delegate with three parameters with dummy response array

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FHathoraLobbyListCallback, const TArray<FSHathoraLobbyInfo>&, HathoraLobbyInfos, int32, HttpStatus, bool, Success);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FHathoraLobbyCreateCallback, const FSHathoraLobbyInfo, HathoraLobbyInfos, int32, HttpStatus, bool, Success);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FHathoraAuthCallback, const FString, Token, int32, HttpStatus, bool, Success);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FHathoraRoomInfoCallback, const FSRoomConnectionInfo, RoomConnectionInfo, int32, HttpStatus, bool, Success);

UCLASS()
class UHathoraAPIBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
public:
	/**
	 * Lobbies
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "List Active Public Lobbies", Keywords = "Lists all Lobbies that are currently Active."), Category = "HathoraAPI|Lobbies")
	static void HathoraAPIListActivePublicLobbies(FHathoraLobbyListCallback Callback);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create Lobby", Keywords = "Creates a Lobby."), Category = "HathoraAPI|Lobbies")
	static void HathoraAPICreateLobby(FHathoraLobbyCreateCallback Callback, FString token);

	/**
	 * Configuration
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get App Id", Keywords = "HathoraAPI Project App Id."), Category = "HathoraAPI|Configuration")
	static FString HathoraAPIGetAppId();

	/**
	 * Rooms
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Room Connection Info", Keywords = "Returns Room Connection Info."), Category = "HathoraAPI|Rooms")
	static void HathoraAPIGetRoomConnectionInfo(FHathoraRoomInfoCallback Callback, FString RoomId);

	/**
	 * Authentication
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Login with Nickname", Keywords = "Login with Nickname."), Category = "HathoraAPI|Authentication")
	static void HathoraAPILoginWithNickname(FHathoraAuthCallback Callback, FString Nickname);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Login Anonymous", Keywords = "Login Anonymous."), Category = "HathoraAPI|Authentication")
	static void HathoraAPILoginAnonymous(FHathoraAuthCallback Callback);

private:
	static const FString API_URL;

	static void OnLobbyListInfoCallBackResponse(FHathoraLobbyListCallback Callback, FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	static void OnLobbyCreatedResponse(FHathoraLobbyCreateCallback Callback, FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	static void OnLoginResponse(FHathoraAuthCallback, FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	static void OnRoomInfoResponse(FHathoraRoomInfoCallback, FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
