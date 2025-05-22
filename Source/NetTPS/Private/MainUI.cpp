// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"

UMainUI::UMainUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> tempBulletUI(
		TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Net/UI/WBP_Bullet.WBP_Bullet_C'"));
	if (tempBulletUI.Succeeded())
	{
		BulletUIFactory = tempBulletUI.Class;
	}
}

void UMainUI::ShowCrosshair(bool bShow)
{
	Image_Crosshair->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

// 총알 위젯을 만들어서 패널에 추가
void UMainUI::AddBullet()
{
	auto bulletUI = CreateWidget<UUserWidget>(GetWorld(), BulletUIFactory);
	BulletPanel->AddChildToUniformGrid(bulletUI, 0, BulletPanel->GetChildrenCount());
}

void UMainUI::PopBullet(int32 index)
{
	BulletPanel->RemoveChildAt(index);
}
