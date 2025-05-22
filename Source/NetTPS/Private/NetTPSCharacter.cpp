// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MainUI.h"
#include "NetPlayerAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetTPSCharacter

ANetTPSCharacter::ANetTPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0, 40, 60));
	CameraBoom->TargetArmLength = 150.0f;
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Gun 컴포넌트 추가
	GunComp = CreateDefaultSubobject<USceneComponent>(TEXT("GunComp"));
	GunComp->SetupAttachment(GetMesh(), TEXT("GunPosition"));
	GunComp->SetRelativeLocation(FVector(-5.000000,-8.660254,-0.000000));
	GunComp->SetRelativeRotation(FRotator(0, 60, 0));

	ConstructorHelpers::FObjectFinder<UInputAction> tempIA(TEXT("/Script/EnhancedInput.InputAction'/Game/Net/Inputs/IA_TakePistol.IA_TakePistol'"));
	if (tempIA.Succeeded())
	{
		IA_TakePistol = tempIA.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> tempIA_R(TEXT("/Script/EnhancedInput.InputAction'/Game/Net/Inputs/IA_ReleaseAction.IA_ReleaseAction'"));
	if (tempIA_R.Succeeded())
	{
		IA_ReleaseAction = tempIA_R.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> tempIA_F(TEXT("/Script/EnhancedInput.InputAction'/Game/Net/Inputs/IA_FireAction.IA_FireAction'"));
	if (tempIA_F.Succeeded())
	{
		IA_FireAction = tempIA_F.Object;
	}

	ConstructorHelpers::FObjectFinder<UParticleSystem> tempPaticle(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tempPaticle.Succeeded())
	{
		HitParticle = tempPaticle.Object;
	}

	ConstructorHelpers::FClassFinder<UMainUI> tempUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Net/UI/WBP_MainUI.WBP_MainUI_C'"));
	if (tempUI.Succeeded())
	{
		MainUIWidget = tempUI.Class;
	}
}

// Input

void ANetTPSCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

void ANetTPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Look);

		EnhancedInputComponent->BindAction(IA_TakePistol, ETriggerEvent::Started, this, &ANetTPSCharacter::TakePistol);

		EnhancedInputComponent->BindAction(IA_ReleaseAction, ETriggerEvent::Started, this, &ANetTPSCharacter::ReleasePistol);

		EnhancedInputComponent->BindAction(IA_FireAction, ETriggerEvent::Started, this, &ANetTPSCharacter::Fire);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PistolActors.Empty();
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), FName(TEXT("Gun")), PistolActors);

	InitUI();
	MainUI->ShowCrosshair(bHasPistol);
}

void ANetTPSCharacter::InitUI()
{
	if (!MainUIWidget) return;
	MainUI = Cast<UMainUI>(CreateWidget<UMainUI>(GetWorld(), MainUIWidget));
	MainUI->AddToViewport();

	BulletCount = MaxBulletCount;

	for (int i = 0; i < MaxBulletCount; i++)
	{
		MainUI->AddBullet();
	}
}

void ANetTPSCharacter::TakePistol(const struct FInputActionValue& Value)
{
	// F 키를 눌렀을 때 호출되는 이벤트 콜백 함수
	// 이미 총을 잡고 있지 않다면 일정 범위 안에 있는 총을 잡는다.
	// 1. 총을 잡고 있지 않아야 한다.
	if (bHasPistol) return;
	
	// 2. 범위 안에 총이 있어야 한다.
	for (auto gun : PistolActors)
	{
		if (!IsValid(gun))return;
		
		if (gun->GetOwner() != nullptr)
		{
			continue;
		}

		if (GetDistanceTo(gun) > GetGunDistance)
		{
			continue;
		}
		
		// 3. 총을 잡고싶다.
		OwnedPistol = gun;
		OwnedPistol->SetOwner(this);
		bHasPistol = true;
		MainUI->ShowCrosshair(bHasPistol);

		AttachPistol(gun);
		break;
	}
	
}

void ANetTPSCharacter::AttachPistol(AActor* pistolActor)
{
	auto meshComp = pistolActor->GetComponentByClass<UStaticMeshComponent>();

	meshComp->SetSimulatePhysics(false);
	meshComp->AttachToComponent(GunComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ANetTPSCharacter::ReleasePistol(const struct FInputActionValue& Value)
{
	if (OwnedPistol && bHasPistol)
	{
		OwnedPistol->SetOwner(nullptr);
		bHasPistol = false;
		MainUI->ShowCrosshair(bHasPistol);

		auto meshComp = OwnedPistol->GetComponentByClass<UStaticMeshComponent>();

		meshComp->SetSimulatePhysics(true);
		meshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}

void ANetTPSCharacter::Fire(const struct FInputActionValue& Value)
{
	// 총이 없으면 발사 x
	if (!bHasPistol || BulletCount <= 0) return;
	
	// 총쏘기 -> Line Trace
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 10000;
	FHitResult OutHit;
	FCollisionQueryParams Parms;
	Parms.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility,Parms);
	
	// 피격 시 파티클 출력

	if (bHit)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, OutHit.ImpactPoint, FRotator(0, 0, 0));
		// 파티클 출력
	}

	auto anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (anim)
	{
		anim->PlayFireAnimation();
	}

	// 총알 제거
	BulletCount--;
	MainUI->PopBullet(BulletCount);
}

void ANetTPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetTPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
