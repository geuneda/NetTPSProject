// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActor.h"

#include "NetTPS.h"
#include "NetTPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ANetActor::ANetActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/VREditor/BasicMeshes/SM_Ball_01.SM_Ball_01'"));
	if (tempMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(tempMesh.Object);
	}
	MeshComp->SetRelativeScale3D(FVector( 0.5));

	// 서버와 동기화 할지 여부
	bReplicates = true;

	// 대역폭 조정
	//SetNetUpdateFrequency(1.0f);
}

// Called when the game starts or when spawned
void ANetActor::BeginPlay()
{
	Super::BeginPlay();

	Mat = MeshComp->CreateDynamicMaterialInstance(0);
	// 타이머를 이용해서 색상을 랜덤으로 일정시간에 한번씩 랜덤으로 변경시켜주자.
	if (HasAuthority())
	{
		FTimerHandle timerHandle;
		GetWorldTimerManager().SetTimer(timerHandle, [&]()
		{
			MatColor = FLinearColor::MakeRandomColor();
			// 서버에서도 적용
			OnRep_ChangeMatColor();
		},
		1.f,
		true);
	}
}

// MatColor가 네트워크에서 변경됐을 때 호출되는 콜백함수
// -> 클라에서만 호출되는 함수
void ANetActor::OnRep_ChangeMatColor()
{
	if (Mat)
	{
		Mat->SetVectorParameterValue(TEXT("FloorColor"), MatColor);
	}
}

void ANetActor::OnRep_RotYaw()
{
	FRotator newRot = GetActorRotation();
	newRot.Yaw = RotYaw;
	SetActorRotation(newRot);

	// 업데이트된 경과시간 저장
	LastTime = CurrentTime;
	CurrentTime = 0;
}

// 서버에서 동기화 할 속성을 작성
void ANetActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetActor, RotYaw);
	DOREPLIFETIME(ANetActor, MatColor);
}

void ANetActor::FindAndSetNearestOwner()
{
	// 캐릭터가 검색 범위 안에 들어오면 Owner를 설정한다.
	// 서버 일때만 처리 해야 한다. (이 로직이 서버의 액터에서만 작동해야한다는 뜻)
	// 1. 서버 검증
	if (HasAuthority())
	{
		// 2. 검색 범위 안에 캐릭터를 찾아서 가장 가까운 플레이어를 찾고
		TArray<AActor*> searchActors;
		AActor* nearestActor = nullptr;
		
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetTPSCharacter::StaticClass(), searchActors);
		
		for (auto actor : searchActors)
		{
			if (SearchRadius > GetDistanceTo(actor))
			{
				if (GetDistanceTo(actor) < GetDistanceTo(nearestActor) || nearestActor == nullptr)
				{
					nearestActor = actor;
				}
			}
		}
		
		// 3. Owner 설정
		if (GetOwner() != nearestActor) SetOwner(nearestActor);
	}
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();

	FindAndSetNearestOwner();

	// 검출 영역 시각화
	DrawDebugSphere(GetWorld(), GetActorLocation(), SearchRadius, 30, FColor::Red, false, 0, 0, 1);

	// if (서버일때만) 회전 처리
	if (HasAuthority())
	{
		AddActorLocalRotation(FRotator(0, RotSpeed * DeltaTime, 0));
		RotYaw = GetActorRotation().Yaw;
	}
	else
	{
		// 네트워크 지연이 생기면 끊김 현상이 생긴다.
		// 이를 보간처리

		// 1. t를 구해야함
		CurrentTime += DeltaTime;
		// LastTime 유효성 검증
		if (LastTime < KINDA_SMALL_NUMBER) return;
		float t = CurrentTime / LastTime;
		
		// 2. 회전
		// R = R0 + vt (+ Lerp)
		float newYaw = RotYaw + RotSpeed * LastTime;
		float lerpYaw = FMath::Lerp(RotYaw, newYaw, t);
		// RotYaw = 시작점, newYaw = 끝점, t = 보간할 비율

		SetActorRotation(FRotator(0, lerpYaw, 0));
	}
}

void ANetActor::PrintNetLog()
{
	// 네트워크 상태 로그 출력
	const FString conStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("InValid Connection");
	// Owner 출력
	const FString ownerName = GetOwner() ? GetOwner()->GetName() : TEXT("No Owner");
	
	const FString conStr2 = FString::Printf(TEXT("Owner: %s\nConnection: %s\nLocal Role: %s\nRemote Role: %s"), *ownerName, *conStr, *LOCALROLE, *REMOTEROLE);
	
	DrawDebugString(GetWorld(), GetActorLocation(), conStr2, nullptr, FColor::Yellow, 0, true, 1);
}

