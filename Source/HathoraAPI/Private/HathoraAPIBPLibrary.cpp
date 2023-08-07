// Copyright Epic Games, Inc. All Rights Reserved.

#include "HathoraAPIBPLibrary.h"
#include "Json.h"
#include "HathoraAPI.h"
#include "JsonObjectConverter.h"

const FString UHathoraAPIBPLibrary::API_URL = "https://api.hathora.dev";

UHathoraAPIBPLibrary::UHathoraAPIBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FString UHathoraAPIBPLibrary::HathoraAPIGetAppId()
{
	FString AppId = "";

	UE_LOG(LogTemp, Error, TEXT("Starting to Read Configuration"))
	
	GConfig->GetString(TEXT("/Script/HathoraAPI.Settings"), TEXT("AppId"), AppId, GEngineIni);

	if(AppId.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("HathoraAPI: AppId is Empty"))
		return "";
	}
	
	return AppId;
}

void UHathoraAPIBPLibrary::HathoraAPILoginWithNickname(FHathoraAuthCallback Callback, FString Nickname)
{
	TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
	RequestObj->SetStringField("nickname", Nickname);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestObj, Writer);
	
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		UHathoraAPIBPLibrary::OnLoginResponse(Callback, Request, Response, bWasSuccessful);
	});
	
	Request->SetURL(UHathoraAPIBPLibrary::API_URL + "/auth/v1/" + UHathoraAPIBPLibrary::HathoraAPIGetAppId() + "/login/nickname");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(RequestBody);
	Request->SetVerb("POST");
	Request->ProcessRequest();
}

void UHathoraAPIBPLibrary::HathoraAPILoginAnonymous(FHathoraAuthCallback Callback)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		UHathoraAPIBPLibrary::OnLoginResponse(Callback, Request, Response, bWasSuccessful);
	});
	
	Request->SetURL(UHathoraAPIBPLibrary::API_URL + "/auth/v1/" + UHathoraAPIBPLibrary::HathoraAPIGetAppId() + "/login/anonymous");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetVerb("POST");
	Request->ProcessRequest();
}

void UHathoraAPIBPLibrary::HathoraAPIListActivePublicLobbies(FHathoraLobbyListCallback Callback)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		UHathoraAPIBPLibrary::OnLobbyListInfoCallBackResponse(Callback, Request, Response, bWasSuccessful);
	});
	Request->SetURL(UHathoraAPIBPLibrary::API_URL + "/lobby/v2/" + UHathoraAPIBPLibrary::HathoraAPIGetAppId() + "/list/public");
	UE_LOG(	LogTemp, Error, TEXT("HathoraAPI: URL: %s"), *Request->GetURL());
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void UHathoraAPIBPLibrary::OnLobbyListInfoCallBackResponse(FHathoraLobbyListCallback Callback, FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TSharedPtr<FJsonValue> callbackJsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	TArray<FSHathoraLobbyInfo> LobbyInfoArray = TArray<FSHathoraLobbyInfo>();

	FSHathoraLobbyInfo LobbyInfo = FSHathoraLobbyInfo();

	if(!FJsonSerializer::Deserialize(Reader, callbackJsonObject) || !callbackJsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize response JSON"));
		// ReSharper disable once CppExpressionWithoutSideEffects
		Callback.ExecuteIfBound(LobbyInfoArray, Response->GetResponseCode(), false);
		return;
	}
	
	for (const TSharedPtr<FJsonValue> JsonValue : callbackJsonObject->AsArray())
	{
		LobbyInfo = FSHathoraLobbyInfo();

		FString OutputString = "";
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(JsonValue->AsObject().ToSharedRef(), Writer);

		LobbyInfo.region = JsonValue->AsObject()->GetStringField("region");
		LobbyInfo.createdAt = JsonValue->AsObject()->GetStringField("createdAt");
		LobbyInfo.createdBy = JsonValue->AsObject()->GetStringField("createdBy");
		LobbyInfo.roomId = JsonValue->AsObject()->GetStringField("roomId");
		LobbyInfo.appId = JsonValue->AsObject()->GetStringField("appId");
		LobbyInfo.visibility = EHathoraLobbyVisibility::PUBLIC;
		
		LobbyInfoArray.Push(LobbyInfo);
	}
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	Callback.ExecuteIfBound(LobbyInfoArray, Response->GetResponseCode(), bWasSuccessful);
}

void UHathoraAPIBPLibrary::HathoraAPICreateLobby(FHathoraLobbyCreateCallback Callback, FString token)
{
	TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
	RequestObj->SetStringField("visibility", "public");
	RequestObj->SetStringField("region", "Frankfurt");
	RequestObj->SetStringField("initialConfig", "{ \"maxPlayers\": 10 }");

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestObj, Writer);
	
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		UHathoraAPIBPLibrary::OnLobbyCreatedResponse(Callback, Request, Response, bWasSuccessful);
	});
	Request->SetURL(UHathoraAPIBPLibrary::API_URL + "/lobby/v2/" + UHathoraAPIBPLibrary::HathoraAPIGetAppId() + "/create");
	Request->SetHeader("Content-Type", "application/json");
	Request->AppendToHeader("Authorization", token);
	Request->SetContentAsString(RequestBody);
	Request->SetVerb("POST");
	Request->ProcessRequest();
}

void UHathoraAPIBPLibrary::OnLobbyCreatedResponse(FHathoraLobbyCreateCallback Callback, FHttpRequestPtr Request,
	FHttpResponsePtr Response, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Error, TEXT("HathoraAPI: Response: %s"), *Response->GetContentAsString());
	
	TSharedPtr<FJsonValue> callbackJsonObject;
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	TArray<FSHathoraLobbyInfo> LobbyInfoArray = TArray<FSHathoraLobbyInfo>();

	FSHathoraLobbyInfo LobbyInfo = FSHathoraLobbyInfo();

	if(!FJsonSerializer::Deserialize(Reader, callbackJsonObject) || !callbackJsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize response JSON"));
		// ReSharper disable once CppExpressionWithoutSideEffects
		Callback.ExecuteIfBound(LobbyInfo, Response->GetResponseCode(), false);
		return;
	}
	
	LobbyInfo.region = callbackJsonObject->AsObject()->GetStringField("region");
    LobbyInfo.createdAt = callbackJsonObject->AsObject()->GetStringField("createdAt");
    LobbyInfo.createdBy = callbackJsonObject->AsObject()->GetStringField("createdBy");
    LobbyInfo.roomId = callbackJsonObject->AsObject()->GetStringField("roomId");
    LobbyInfo.appId = callbackJsonObject->AsObject()->GetStringField("appId");
    LobbyInfo.visibility = EHathoraLobbyVisibility::PUBLIC;
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	Callback.ExecuteIfBound(LobbyInfo, Response->GetResponseCode(), bWasSuccessful);
}

void UHathoraAPIBPLibrary::OnLoginResponse(FHathoraAuthCallback Callback, FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	UE_LOG(LogTemp, Error, TEXT("HathoraAPI-Login: Response: %s"), *Response->GetContentAsString());
	
	TSharedPtr<FJsonValue> callbackJsonObject;
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	
	if(!FJsonSerializer::Deserialize(Reader, callbackJsonObject) || !callbackJsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize response JSON"));
		// ReSharper disable once CppExpressionWithoutSideEffects
		Callback.ExecuteIfBound("", Response->GetResponseCode(), false);
		return;
	}

	// ReSharper disable once CppExpressionWithoutSideEffects
	Callback.ExecuteIfBound(callbackJsonObject->AsObject()->GetStringField("token"), Response->GetResponseCode(), bWasSuccessful);
}

void UHathoraAPIBPLibrary::HathoraAPIGetRoomConnectionInfo(FHathoraRoomInfoCallback Callback, FString RoomId)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		UHathoraAPIBPLibrary::OnRoomInfoResponse(Callback, Request, Response, bWasSuccessful);
	});
	
	Request->SetURL(UHathoraAPIBPLibrary::API_URL + "/rooms/v2/" + UHathoraAPIBPLibrary::HathoraAPIGetAppId() + "/connectioninfo/" + RoomId);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void UHathoraAPIBPLibrary::OnRoomInfoResponse(FHathoraRoomInfoCallback Callback, FHttpRequestPtr Request,
	FHttpResponsePtr Response, bool bWasSuccessful)
{
	TSharedPtr<FJsonValue> callbackJsonObject;
	
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	FSRoomConnectionInfo ConnectionInfo = FSRoomConnectionInfo();
	
	if(!FJsonSerializer::Deserialize(Reader, callbackJsonObject) || !callbackJsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize response JSON"));
		// ReSharper disable once CppExpressionWithoutSideEffects
		Callback.ExecuteIfBound(ConnectionInfo, Response->GetResponseCode(), false);
		return;
	}

	FSRoomExposedPort ExposedPort = FSRoomExposedPort();
	ExposedPort.transportType = callbackJsonObject->AsObject()->GetObjectField("exposedPort")->GetStringField("transportType");
	ExposedPort.port = callbackJsonObject->AsObject()->GetObjectField("exposedPort")->GetIntegerField("port");
	ExposedPort.host = callbackJsonObject->AsObject()->GetObjectField("exposedPort")->GetStringField("host");
	ExposedPort.name = callbackJsonObject->AsObject()->GetObjectField("exposedPort")->GetStringField("name");

	TArray<FSRoomExposedPort> AdditionalExposedPorts = TArray<FSRoomExposedPort>();
	AdditionalExposedPorts.Push(ExposedPort);

	ConnectionInfo.status = callbackJsonObject->AsObject()->GetStringField("status");
	ConnectionInfo.exposedPort = ExposedPort;
	ConnectionInfo.roomId = callbackJsonObject->AsObject()->GetStringField("roomId");
	ConnectionInfo.additionalExposedPorts = AdditionalExposedPorts;
	

	// ReSharper disable once CppExpressionWithoutSideEffects
	Callback.ExecuteIfBound(ConnectionInfo, Response->GetResponseCode(), bWasSuccessful);
}


