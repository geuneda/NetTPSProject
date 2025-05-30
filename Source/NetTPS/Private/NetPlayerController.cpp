// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"

#include "NetTPSGameMode.h"
#include "GameFramework/SpectatorPawn.h"

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Server 일때만 게임 모드가 존재.
	if (HasAuthority())
		gm = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
	
}

void ANetPlayerController::SeverRPC_ChangeToSpectator_Implementation()
{
	// 관전자가 플레이어의 위치에서 생성될 수 있도록 플레이어 정보를 가져온다.
	auto player = GetPawn();
	if (player)
	{
		// 관전자 객체 생성
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		auto spectator = GetWorld()->SpawnActor<ASpectatorPawn>(gm->SpectatorClass, player->GetActorTransform(), spawnParams);

		// 관전자 Possess
		Possess(spectator);

		// 이전 플레이어는 제거
		player->Destroy();
	}
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
