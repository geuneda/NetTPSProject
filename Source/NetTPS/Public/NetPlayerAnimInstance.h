// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UNetPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UNetPlayerAnimInstance();

public:
	UPROPERTY()
	class ANetTPSCharacter* Player;
	
	// 총을 소지하고 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyAnimSettings)
	bool bHasPistol;
	// 죽었는지 상태 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyAnimSettings)
	bool bIsDead;
	
	// 방향 변수 등록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyAnimSettings)
	float Direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyAnimSettings)
	float Speed;

	// 총 쏘기 몽타주
	UPROPERTY(EditDefaultsOnly, Category=Montage)
	UAnimMontage* FireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage)
	float FireMontageRate = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyAnimSettings)
	float PitchAngle;

	// 재장전 몽타주
	UPROPERTY(EditDefaultsOnly, Category=Montage)
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage)
	float ReloadMontageRate = 1.f;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 총쏘기 애니메이션 재생 함수
	void PlayFireAnimation();

	// 재장전 애니메이션 재생 함수
	void PlayReloadAnimation();
	// 재장전 애니메이션 노티파이 콜백 처리 함수
	UFUNCTION()
	void AnimNotify_OnReloadFinish();
	
};
