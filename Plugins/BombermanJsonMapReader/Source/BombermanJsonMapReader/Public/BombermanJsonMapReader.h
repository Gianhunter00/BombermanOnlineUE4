// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBombermanJsonMapReaderModule : public IModuleInterface, public FSelfRegisteringExec
{
public:
	typedef UObject* (FBombermanJsonMapReaderModule::* CreateObject)(UWorld*, const int32, const int32) const;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	UObject* CreatePlatform(UWorld* InWorld, const int32 InX, const int32 InY) const;
	UObject* CreateUnbreakableWall(UWorld* InWorld, const int32 InX, const int32 InY) const;
	UObject* CreateBreakableWall(UWorld* InWorld, const int32 InX, const int32 InY) const;

private:
	TMap<FString, CreateObject> MapCreateActor;
	const FString BombermanFloorMeshPath = FString("StaticMesh'/Game/BombermanMapAsset/Mesh/FloorMesh.FloorMesh'");
	const FString BombermanUnbreakableWallMeshPath = FString("StaticMesh'/Game/BombermanMapAsset/Mesh/UnbreakableWallMesh.UnbreakableWallMesh'");
	const FString BombermanBreakableWallBlueprintPath = FString("Blueprint'/Game/BombermanMapAsset/Blueprint/BPBreakableWall.BPBreakableWall'");

	const FString BombermanFloorMatPath = FString("Material'/Game/BombermanMapAsset/Material/FloorMat.FloorMat'");
	const FString BombermanUnbreakableWallMatPath = FString("Material'/Game/BombermanMapAsset/Material/UnbreakableMat.UnbreakableMat'");
	
	const FString BombermanSkySphereBlueprintPath = FString("Blueprint'/Game/BombermanMapAsset/Lights/BombermanSkySphere.BombermanSkySphere'");

	const int32 DefaultSize = 100;
	FVector	AdjustScale(const FVector InMeshSize) const;
	FTransform AdjustTransform(const FVector InMeshSize, const int32 InX, const int32 InY, const int32 InZ) const;
};