// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AdBridgeUEManager.generated.h"

UENUM(BlueprintType)
enum class EInterstitialState : uint8
{
	NotLoaded UMETA(DisplayName = "NotLoaded"),
	Loading UMETA(DisplayName = "Loading"),
	Ready UMETA(DisplayName = "Ready"),
	ShowRequested UMETA(DisplayName = "ShowRequested"),
	Showing UMETA(DisplayName = "Showing"),
	Closed UMETA(DisplayName = "Closed"),
	LoadFailed UMETA(DisplayName = "LoadFailed"),
	ShowFailed UMETA(DisplayName = "ShowFailed")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInterstitialClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInterstitialLoaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnInterstitialStateChanged,
	EInterstitialState, NewState,
	int32, ErrorCode,
	FString, ErrorDomain,
	FString, Message
);


/**
 * 
 */
UCLASS(BlueprintType)
class ADBRIDGEUE_API UAdBridgeUEManager : public UObject
{
public:
	// -------------------------
	// Commands
	// -------------------------
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable, Category = "AdBridgeUE|Interstitial")
	void LoadInterstitial();
	UFUNCTION(BlueprintCallable, Category = "AdBridgeUE|Interstitial")
	void ShowInterstitial();
	
	// -------------------------
	// Query
	// -------------------------
	UFUNCTION(BlueprintPure, Category = "AdBridgeUE|Interstitial")
	EInterstitialState GetInterstitialState() const;
	
	// -------------------------
	// Android / Google responses
	// -------------------------
	void HandleInterstitialLoaded();
	void HandleInterstitialLoadFailed(int32 ErrorCode,const FString& ErrorDomain, const FString& ErrorMessage);
	void HandleInterstitialShown();
	void HandleInterstitialShowFailed(int32 ErrorCode,const FString& ErrorDomain, const FString& ErrorMessage);
	void HandleInterstitialClosed();
	
	// -------------------------
	// Blueprint Events
	// -------------------------
	UPROPERTY(BlueprintAssignable, Category = "AdBridgeUE|Interstitial")
	FOnInterstitialClosed OnInterstitialClosed;
	UPROPERTY(BlueprintAssignable, Category = "AdBridgeUE|Interstitial")
	FOnInterstitialLoaded OnInterstitialLoaded;
	UPROPERTY(BlueprintAssignable,Category="AdBridgeUE|Debug")
	FOnInterstitialStateChanged OnInterstitialStateChanged;

private:
	EInterstitialState InterstitialState = EInterstitialState::NotLoaded;
	void SetInterstitialState(EInterstitialState NewState, 
		const FString& Message,
		const FString& ErrorDomain = FString(), 
		int32 ErrorCode = -1);
};
