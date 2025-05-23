// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"

#include "NetTPSCharacter.h"

UNetPlayerAnimInstance::UNetPlayerAnimInstance()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> FireAnimMontage(
		TEXT("/Script/Engine.AnimMontage'/Game/Net/Animations/AM_Pistol_Fire_Montage.AM_Pistol_Fire_Montage'"));
	if (FireAnimMontage.Succeeded())
	{
		FireMontage = FireAnimMontage.Object;
	}
	ConstructorHelpers::FObjectFinder<UAnimMontage> ReloadAnimMontage(
		TEXT("/Script/Engine.AnimMontage'/Game/Net/Animations/AM_Pistol_Reload_Montage.AM_Pistol_Reload_Montage'"));
	if (ReloadAnimMontage.Succeeded())
	{
		ReloadMontage = ReloadAnimMontage.Object;
	}
}

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
		
		// 회전값 가져오기
		PitchAngle = -Player->GetBaseAimRotation().GetNormalized().Pitch;
		PitchAngle = FMath::Clamp(PitchAngle, -60.f, 60.f);
	}
}

void UNetPlayerAnimInstance::PlayFireAnimation()
{
	if (!bHasPistol) return;

	if (FireMontage)
	{
		Montage_Play(FireMontage, FireMontageRate);
	}
}

void UNetPlayerAnimInstance::PlayReloadAnimation()
{
	if (!bHasPistol) return;

	if (ReloadMontage)
	{
		Montage_Play(ReloadMontage, ReloadMontageRate);
	}
}

void UNetPlayerAnimInstance::AnimNotify_OnReloadFinish()
{
	Player->InitBulletUI();
}
