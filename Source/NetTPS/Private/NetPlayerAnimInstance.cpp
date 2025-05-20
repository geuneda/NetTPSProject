// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"

#include "NetTPSCharacter.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Player = Cast<ANetTPSCharacter>(TryGetPawnOwner());
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Player의 속도와 방향의 값을 가져와서 할당해주기
	if (Player)
	{
		FVector vel = Player->GetVelocity();
		Speed = FVector::DotProduct(vel, Player->GetActorForwardVector());
		Direction = FVector::DotProduct(vel, Player->GetActorRightVector());

		bHasPistol = Player->bHasPistol;
	}
}

