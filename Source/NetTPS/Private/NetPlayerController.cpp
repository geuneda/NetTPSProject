// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"

#include "NetTPSGameMode.h"

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Server 일때만 게임 모드가 존재.
	if (HasAuthority())
		gm = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
	
}

void ANetPlayerController::ServerRPC_RespawnPlayer_Implementation()
{
	// -> possess를 풀어야 한다.
	auto player = GetPawn();
	UnPossess();
	
	// -> 기존 Pawn은 삭제
	player->Destroy();
	
	// -> 새로운 Pawn을 스폰
	gm->RestartPlayer(this);
}
