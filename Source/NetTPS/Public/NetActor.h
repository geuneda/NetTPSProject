// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetActor.generated.h"

UCLASS()
class NETTPS_API ANetActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Owner 설정 함수
	void FindAndSetNearestOwner();
	// 네트워크 상태로그 출력 함수
	void PrintNetLog();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MeshComp;


	// Owner 검출 영역
	UPROPERTY(EditAnywhere)
	float SearchRadius = 200;

	float curDis = 0.f;
	float nearDis = 0.f;

};
