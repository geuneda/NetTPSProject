// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainUI(const FObjectInitializer& ObjectInitializer);

public:

	// 크로스헤어
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_Crosshair;
	// 총알 위젯 추가할 패널
	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* BulletPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bullet)
	TSubclassOf<class UUserWidget> BulletUIFactory;
public:
	// 크로스헤어 Show/Hide
	void ShowCrosshair(bool bShow);
	
	// 총알 위젯 추가 함수
	void AddBullet();
	// 총알 하나씩 제거하는 함수
	void PopBullet(int32 index);
	// 모든 총알 UI 제거
	void RemoveAllBullet();
};
