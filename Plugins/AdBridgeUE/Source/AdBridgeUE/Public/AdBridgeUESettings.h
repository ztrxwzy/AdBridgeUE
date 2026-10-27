// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AdBridgeUESettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="AdBridgeUE"))
class ADBRIDGEUE_API UAdBridgeUESettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config,EditAnywhere, Category="AdBridgeUESettings")
	bool bUseGoogleTestIds = true;
	UPROPERTY(Config,EditAnywhere, Category="AdBridgeUESettings")
	FString AdMobApplicationId;
	UPROPERTY(Config,EditAnywhere, Category="AdBridgeUESettings")
	FString InterstitialAdUnitId;
};
