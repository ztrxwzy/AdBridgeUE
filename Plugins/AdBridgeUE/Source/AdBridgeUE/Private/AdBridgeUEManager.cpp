// Fill out your copyright notice in the Description page of Project Settings.


#include "AdBridgeUEManager.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJavaEnv.h"
#endif
#include "Async/Async.h"

DEFINE_LOG_CATEGORY_STATIC(LogAdBridgeUEManager,Log,All);

static TWeakObjectPtr<UAdBridgeUEManager> GAdBridgeUEActiveManager;

void UAdBridgeUEManager::LoadInterstitial()
{
	if (InterstitialState != EInterstitialState::NotLoaded && 
		InterstitialState != EInterstitialState::Closed &&
		InterstitialState != EInterstitialState::LoadFailed &&
		InterstitialState != EInterstitialState::ShowFailed)
	{
		UE_LOG(LogAdBridgeUEManager,
		Warning,
		TEXT("LoadInterstitial rejected. Current state does not allow loading."));
		return;
	}
	
	SetInterstitialState(
		EInterstitialState::Loading,
		TEXT("Interstitial load requested.")
	);
	GAdBridgeUEActiveManager = this;

#if PLATFORM_ANDROID

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	if (Env == nullptr)
	{
		HandleInterstitialLoadFailed(
			-1,
			TEXT("AdBridgeUE.JNI"),
			TEXT("Could not obtain Android JNI environment.")
		);

		return;
	}


	jmethodID Method =
		FJavaWrapper::FindMethod(
			Env,
			FJavaWrapper::GameActivityClassID,
			"ABUE_LoadInterstitial",
			"()V",
			true
		);


	if (Method == nullptr)
	{
		HandleInterstitialLoadFailed(
			-1,
			TEXT("AdBridgeUE.JNI"),
			TEXT("Could not find ABUE_LoadInterstitial in GameActivity.")
		);

		return;
	}


	FJavaWrapper::CallVoidMethod(
		Env,
		FJavaWrapper::GameActivityThis,
		Method
	);

#else

	HandleInterstitialLoadFailed(
		-1,
		TEXT("AdBridgeUE.Platform"),
		TEXT("Interstitial ads are only supported on Android.")
	);

#endif
}

void UAdBridgeUEManager::ShowInterstitial()
{
	if (InterstitialState != EInterstitialState::Ready)
	{
		return;
	}
	
	SetInterstitialState(
		EInterstitialState::ShowRequested,
		TEXT("Interstitial show requested.")
	);
	GAdBridgeUEActiveManager = this;
	
#if PLATFORM_ANDROID

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	if (Env == nullptr)
	{
		HandleInterstitialShowFailed(
			-1,
			TEXT("AdBridgeUE.JNI"),
			TEXT("Could not obtain Android JNI environment.")
		);

		return;
	}


	jmethodID Method =
		FJavaWrapper::FindMethod(
			Env,
			FJavaWrapper::GameActivityClassID,
			"ABUE_ShowInterstitial",
			"()V",
			true
		);


	if (Method == nullptr)
	{
		HandleInterstitialShowFailed(
			-1,
			TEXT("AdBridgeUE.JNI"),
			TEXT("Could not find ABUE_ShowInterstitial in GameActivity.")
		);

		return;
	}


	FJavaWrapper::CallVoidMethod(
		Env,
		FJavaWrapper::GameActivityThis,
		Method
	);

#else

	HandleInterstitialShowFailed(
		-1,
		TEXT("AdBridgeUE.Platform"),
		TEXT("Interstitial ads are only supported on Android.")
	);

#endif
}


void UAdBridgeUEManager::HandleInterstitialLoaded()
{
	SetInterstitialState(
		EInterstitialState::Ready,
		TEXT("Interstitial loaded successfully.")
	);
	OnInterstitialLoaded.Broadcast();
}

void UAdBridgeUEManager::HandleInterstitialLoadFailed(int32 ErrorCode,const FString& ErrorDomain, const FString& ErrorMessage)
{
	SetInterstitialState(
		EInterstitialState::LoadFailed,
		ErrorMessage,
		ErrorDomain,
		ErrorCode
	);
}

void UAdBridgeUEManager::HandleInterstitialShown()
{
	SetInterstitialState(
		EInterstitialState::Ready,
		TEXT("Interstitial is now showing.")
	);
}

void UAdBridgeUEManager::HandleInterstitialShowFailed(int32 ErrorCode,const FString& ErrorDomain, const FString& ErrorMessage)
{
	SetInterstitialState(
		EInterstitialState::ShowFailed,
		ErrorMessage,
		ErrorDomain,
		ErrorCode
	);
}

void UAdBridgeUEManager::HandleInterstitialClosed()
{
	SetInterstitialState(
		EInterstitialState::Closed,
		TEXT("Interstitial closed.")
	);
	OnInterstitialClosed.Broadcast();
}

void UAdBridgeUEManager::SetInterstitialState(EInterstitialState NewState, const FString& Message,
	const FString& ErrorDomain, int32 ErrorCode)
{
	InterstitialState = NewState;
	
	if (NewState == EInterstitialState::LoadFailed || NewState == EInterstitialState::ShowFailed)
	{
		UE_LOG(
			LogAdBridgeUEManager,
			Error,
			TEXT("Interstitial Error | State=%s | Code=%d | Domain=%s | Message=%s"),
			*UEnum::GetValueAsString(NewState),
			ErrorCode,
			*ErrorDomain,
			*Message
		);
	}
	else
	{
		UE_LOG(
			LogAdBridgeUEManager,
			Log,
			TEXT("Interstitial State=%s | Message=%s"),
			*UEnum::GetValueAsString(NewState),
			*Message
		);
	}
	
	OnInterstitialStateChanged.Broadcast(NewState,ErrorCode,ErrorDomain,Message);
}

EInterstitialState UAdBridgeUEManager::GetInterstitialState() const
{
	return InterstitialState;
}

#if PLATFORM_ANDROID

extern "C"
JNIEXPORT void JNICALL
Java_com_epicgames_unreal_GameActivity_nativeABUEInterstitialLoaded(
	JNIEnv* Env,
	jobject Thiz
)
{
	AsyncTask(
		ENamedThreads::GameThread,
		[]()
		{
			if (GAdBridgeUEActiveManager.IsValid())
			{
				GAdBridgeUEActiveManager
					->HandleInterstitialLoaded();
			}
		}
	);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_epicgames_unreal_GameActivity_nativeABUEInterstitialLoadFailed(
	JNIEnv* Env,
	jobject Thiz,
	jint ErrorCode,
	jstring ErrorDomain,
	jstring ErrorMessage)
{
	const FString Domain =
		ErrorDomain
		? FJavaHelper::FStringFromParam(Env, ErrorDomain)
		: TEXT("");

	const FString Message =
		ErrorMessage
		? FJavaHelper::FStringFromParam(Env, ErrorMessage)
		: TEXT("");


	AsyncTask(
		ENamedThreads::GameThread,
		[ErrorCode, Domain, Message]()
		{
			if (GAdBridgeUEActiveManager.IsValid())
			{
				GAdBridgeUEActiveManager
					->HandleInterstitialLoadFailed(
						static_cast<int32>(ErrorCode),
						Domain,
						Message
					);
			}
		}
	);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_epicgames_unreal_GameActivity_nativeABUEInterstitialShown(
	JNIEnv* Env,
	jobject Thiz
)
{
	AsyncTask(
		ENamedThreads::GameThread,
		[]()
		{
			if (GAdBridgeUEActiveManager.IsValid())
			{
				GAdBridgeUEActiveManager
					->HandleInterstitialShown();
			}
		}
	);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_epicgames_unreal_GameActivity_nativeABUEInterstitialShowFailed(
	JNIEnv* Env,
	jobject Thiz,
	jint ErrorCode,
	jstring ErrorDomain,
	jstring ErrorMessage)
{
	const FString Domain =
		ErrorDomain
		? FJavaHelper::FStringFromParam(Env, ErrorDomain)
		: TEXT("");

	const FString Message =
		ErrorMessage
		? FJavaHelper::FStringFromParam(Env, ErrorMessage)
		: TEXT("");


	AsyncTask(
		ENamedThreads::GameThread,
		[ErrorCode, Domain, Message]()
		{
			if (GAdBridgeUEActiveManager.IsValid())
			{
				GAdBridgeUEActiveManager
					->HandleInterstitialShowFailed(
						static_cast<int32>(ErrorCode),
						Domain,
						Message
					);
			}
		}
	);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_epicgames_unreal_GameActivity_nativeABUEInterstitialClosed(
	JNIEnv* Env,
	jobject Thiz
)
{
	AsyncTask(
		ENamedThreads::GameThread,
		[]()
		{
			if (GAdBridgeUEActiveManager.IsValid())
			{
				GAdBridgeUEActiveManager
					->HandleInterstitialClosed();
			}
		}
	);
}

#endif