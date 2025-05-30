// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(NetTPS, Log, All);

#define NETMODE (GetNetMode() == ENetMode::NM_Client ? TEXT("클라이언트") : \
	GetNetMode() == ENetMode::NM_Standalone ? TEXT("스탠드얼론") : TEXT("서버"))

// 호출하는 함수와 줄번호 정보
#define APPINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define PRINT_APPINFO() UE_LOG(NetTPS, Warning, TEXT("%s"), *APPINFO)
#define PRINTLOG(fmt,...) \
	UE_LOG(NetTPS, Warning, TEXT("[%s]%s : %s"), NETMODE, *APPINFO, *FString::Printf(fmt, ##__VA_ARGS__))

// Role
#define LOCALROLE (UEnum::GetValueAsString<ENetRole>(GetLocalRole()))
#define REMOTEROLE (UEnum::GetValueAsString<ENetRole>(GetRemoteRole()))