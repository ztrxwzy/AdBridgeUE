// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdBridgeUE.h"

#define LOCTEXT_NAMESPACE "FAdBridgeUEModule"

void FAdBridgeUEModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Log, TEXT("AdBridgeUE module loaded."));

}

void FAdBridgeUEModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Log, TEXT("AdBridgeUE module unloaded."));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAdBridgeUEModule, AdBridgeUE)