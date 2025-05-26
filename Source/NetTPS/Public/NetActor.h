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

public: // ------------회전 동기화 처리
	UPROPERTY(ReplicatedUsing = OnRep_RotYaw) // 변경 됐을 때 OnRep_RotYaw 함수를 호출
	float RotYaw = 0;

	// RotYaw가 변경 됐을 때 호출되는 이벤트 함수
	UFUNCTION()
	void OnRep_RotYaw();
	
	float RotSpeed = 50;

	// 부드럽게 회전 보간 하기 위한 변수
	float CurrentTime = 0;
	float LastTime = 0;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
