// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "Components/Image.h"

void UMainUI::ShowCrosshair(bool bShow)
{
	Image_Crosshair->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
