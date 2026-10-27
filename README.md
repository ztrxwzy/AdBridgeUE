# AdBridgeUE

An **Unreal Engine 5.8** test project for developing and testing **AdBridgeUE**, a plugin that integrates Google AdMob interstitial ads on Android and exposes their controls to Blueprints.

The plugin has also been used in real game projects, including **Infinite Pathways**, a game by **TitanomaquiaGames**.

## Project structure

- `Plugins/AdBridgeUE/`: the main plugin, containing the C++ module, interstitial manager, settings, and Android integration.
- `Source/AdBridgeUEDemo/`: the test project's base module.
- `Content/TestingInterstitial.umap`: the test level, configured as the startup map.
- `Config/`: project settings, including the startup map and Android package name.

## Code architecture

1. A Blueprint calls `UAdBridgeUEManager` to load or show an interstitial ad.
2. The C++ manager calls methods on Android's `GameActivity` through JNI.
3. `AdBridgeUE_UPL.xml` adds the Google Mobile Ads SDK and the Java code that loads and shows the ad to the Android build.
4. Google's callbacks reach Java, pass through native JNI functions, and return to the C++ manager on the game thread. The manager updates its state and broadcasts events to Blueprints.

In short: **Blueprint → C++ → JNI → Java → Google Mobile Ads**, with results returning through **Java → JNI → C++ → Blueprint**.

The plugin uses `play-services-ads:23.6.0` and Google's official test IDs by default. `UAdBridgeUESettings` allows custom IDs to be configured for other environments. Ads are implemented for Android only; on other platforms, loading reports a platform error.
