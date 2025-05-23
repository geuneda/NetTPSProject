// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Particles/ParticleSystem.h"
#include "NetTPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetTPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	ANetTPSCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:

	virtual void NotifyControllerChanged() override;


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	// 총을 자식으로 붙일 컴포넌트
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* GunComp;

public:
	// 총 잡기
	UPROPERTY(EditAnywhere, Category=Input)
	UInputAction* IA_TakePistol;

	// 필요속성 : 총 소유 여부, 소유중인 총, 총 검색 범위
	bool bHasPistol = false;

	UPROPERTY()
	AActor* OwnedPistol = nullptr;

	UPROPERTY(EditAnywhere, Category=Gun)
	float GetGunDistance = 200;

	// 월드에 배치 된 총들
	UPROPERTY()
	TArray<AActor*> PistolActors;

	void TakePistol(const struct FInputActionValue& Value);
	// 총을 컴포넌트에 붙이는 함수
	void AttachPistol(AActor* pistolActor);

	/* 총 놓기 **/
public:
	// 입력 처리
	UPROPERTY(EditDefaultsOnly, Category=Input)
	UInputAction* IA_ReleaseAction;
	// 총 놓기 함수
	void ReleasePistol(const struct FInputActionValue& Value);

public:
	// 총쏘기
	UPROPERTY(EditDefaultsOnly, Category=Input)
	UInputAction* IA_FireAction;

	// 총 쏘기 처리 함수
	void Fire(const struct FInputActionValue& Value);

	// 파티클
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UParticleSystem> HitParticle;

	public:/** UI */
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UMainUI> MainUIWidget;
	UPROPERTY()
	class UMainUI* MainUI;

	// 최대 총알 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bullet)
	int32 MaxBulletCount = 10;
	// 남은 총알 개수
	int32 BulletCount = MaxBulletCount;
	
	
	// UI 초기화 함수
	void InitUI();

public: // 재장전
	UPROPERTY(EditDefaultsOnly, Category=Input)
	class UInputAction* IA_Reload;

	// 재장전 중인지 여부
	bool bIsReloading = false;
	
	void ReloadPistol(const struct FInputActionValue& Value);
	void StopMontagesAndResetReload();

	// 총알 UI 초기화 함수
	void InitBulletUI();

public: // 플레이어 체력
	UPROPERTY(EditDefaultsOnly, Category=HP)
	float MaxHP = 3;
	float CurHP = MaxHP;

	// HP 프로퍼티
	__declspec(property(get = GetHP, put = SetHP)) float HP;
	
	float GetHP() const { return CurHP; }
	void SetHP(float value);

	UPROPERTY(VisibleAnywhere, Category=HP)
	class UWidgetComponent* HPUIComp;

public: // 피격 처리
	void DamageProcess();

public: // 죽음 처리
	bool bIsDead = false;

public: // Network

	// 네트워크 상태로그 출력 함수
	void PrintNetLog();
};

