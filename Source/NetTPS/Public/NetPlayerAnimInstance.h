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

public:
	UPROPERTY()
	class ANetTPSCharacter* Player;
	
	// 총을 소지하고 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyAnimSettings")
	bool bHasPistol = false;
	
	// 방향 변수 등록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyAnimSettings")
	float Direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyAnimSettings")
	float Speed;

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
};
