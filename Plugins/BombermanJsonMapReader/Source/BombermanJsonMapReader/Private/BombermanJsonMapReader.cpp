// Copyright Epic Games, Inc. All Rights Reserved.

#include "BombermanJsonMapReader.h"
#include "AssetRegistryModule.h"
#include "BreakableWall.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/WorldFactory.h"
#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "FBombermanJsonMapReaderModule"

void FBombermanJsonMapReaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	MapCreateActor.Add("0", &FBombermanJsonMapReaderModule::CreatePlatform);
	MapCreateActor.Add("1", &FBombermanJsonMapReaderModule::CreateBreakableWall);
	MapCreateActor.Add("2", &FBombermanJsonMapReaderModule::CreateUnbreakableWall);
}

void FBombermanJsonMapReaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

}
//D:\Test.txt
bool FBombermanJsonMapReaderModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FParse::Command(&Cmd, TEXT("newlevel")))
	{
		FString Path = FParse::Token(Cmd, true);
		UE_LOG(LogTemp, Error, TEXT("%s"), *Path);
		TArray<FString> FileArr;
		FFileHelper::LoadFileToStringArray(FileArr, *Path);
		int32 StartX = 0;
		int32 StartY = 0;
		float Offset = DefaultSize * 0.5f;
		if (FileArr.Num() > 0)
		{
			UWorldFactory* NewWorld = NewObject<UWorldFactory>();

			uint64 SuffixAssetName = FPlatformTime::Cycles64();
			FString AssetName = FString::Printf(TEXT("M_NewLevel_%llu"), SuffixAssetName);
			UPackage* Package = CreatePackage(*FString::Printf(TEXT("/Game/%s"), *AssetName));

			UObject* NewLevelObject = NewWorld->FactoryCreateNew(NewWorld->SupportedClass, Package, *AssetName, EObjectFlags::RF_Standalone | EObjectFlags::RF_Public, nullptr, GWarn);
			FAssetRegistryModule::AssetCreated(NewLevelObject);

			UWorld* WorldCasted = Cast<UWorld>(NewLevelObject);
			WorldCasted->Modify();
			WorldCasted->SpawnActor<ASkyLight>();
			ADirectionalLight* MyDirectionalLight = WorldCasted->SpawnActor<ADirectionalLight>();
			MyDirectionalLight->SetCastShadows(false);
			FTransform MyDirectionalLightNewTransform = MyDirectionalLight->GetTransform();
			FVector NewLightRot = FVector(0, 275, 0);
			FQuat MyDirectionalLightNewRot = FQuat::MakeFromEuler(NewLightRot);
			MyDirectionalLightNewTransform.SetRotation(MyDirectionalLightNewRot);
			MyDirectionalLight->SetActorTransform(MyDirectionalLightNewTransform);
			UBlueprint* BPSkySphere = LoadObject<UBlueprint>(nullptr, *BombermanSkySphereBlueprintPath);
			if (BPSkySphere)
			{
				WorldCasted->SpawnActor(BPSkySphere->GeneratedClass);
			}
			for (FString Line : FileArr)
			{
				UE_LOG(LogTemp, Error, TEXT("%s"), *Line);
				TArray<FString> BlockInLine;
				Line.ParseIntoArray(BlockInLine, TEXT(","));
				for (int32 Index = 0; Index < BlockInLine.Num(); Index++)
				{
					(this->* * MapCreateActor.Find("0"))(WorldCasted, StartX * DefaultSize + (int32)Offset, StartY * DefaultSize + (int32)Offset);
					if (BlockInLine[Index].Equals("0"))
					{
						++StartX;
						continue;
					}
					(this->* * MapCreateActor.Find(BlockInLine[Index]))(WorldCasted, StartX * DefaultSize + (int32)Offset, StartY * DefaultSize + (int32)Offset);

					++StartX;
				}
				StartX = 0;
				++StartY;
			}
			WorldCasted->PostEditChange();
			WorldCasted->MarkPackageDirty();
		}

		return true;
	}

	return false;
}
//StaticMesh'/Game/Geometry/Meshes/TemplateFloor.TemplateFloor'
UObject* FBombermanJsonMapReaderModule::CreatePlatform(UWorld* InWorld, const int32 InX, const int32 InY) const
{
	AStaticMeshActor* MyActor = InWorld->SpawnActor<AStaticMeshActor>();
	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *BombermanFloorMeshPath);
	UMaterial* Material = LoadObject<UMaterial>(nullptr, *BombermanFloorMatPath);
	FVector MeshSize = StaticMesh->GetBounds().GetBox().GetSize();
	FTransform MyTransform = AdjustTransform(MeshSize, InX, InY, 0);
	MyActor->SetActorTransform(MyTransform);
	StaticMesh->SetMaterial(0, Material);
	MyActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
	return MyActor;
}

UObject* FBombermanJsonMapReaderModule::CreateUnbreakableWall(UWorld* InWorld, const int32 InX, const int32 InY) const
{
	AStaticMeshActor* MyActor = InWorld->SpawnActor<AStaticMeshActor>();
	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *BombermanUnbreakableWallMeshPath);
	UMaterial* Material = LoadObject<UMaterial>(nullptr, *BombermanUnbreakableWallMatPath);
	FVector MeshSize = StaticMesh->GetBounds().GetBox().GetSize();
	FTransform MyTransform = AdjustTransform(MeshSize, InX, InY, 50);
	MyActor->SetActorTransform(MyTransform);
	StaticMesh->SetMaterial(0, Material);
	MyActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
	return MyActor;
}

UObject* FBombermanJsonMapReaderModule::CreateBreakableWall(UWorld* InWorld, const int32 InX, const int32 InY) const
{
	UBlueprint* BPBreakableWall = LoadObject<UBlueprint>(nullptr, *BombermanBreakableWallBlueprintPath);
	if (BPBreakableWall == nullptr || !BPBreakableWall->GeneratedClass->IsChildOf<AActor>())
	{
		UE_LOG(LogTemp, Error, TEXT("FAILED LOAD"));
		return nullptr;
	}
	AActor* BPBreakableWallActor = InWorld->SpawnActor<AActor>(BPBreakableWall->GeneratedClass);
	if (BPBreakableWallActor)
	{
		ABreakableWall* BreakableWall = Cast<ABreakableWall>(BPBreakableWallActor);
		if (BreakableWall)
		{
			FVector MeshSize = BreakableWall->StaticMeshComponent->GetStaticMesh()->GetBounds().GetBox().GetSize();
			FTransform MyTransform = AdjustTransform(MeshSize, InX, InY, 50);
			BreakableWall->SetActorTransform(MyTransform);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FAILED CAST"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FAILED SPAWN"));
	}
	return BPBreakableWall;
}

FVector FBombermanJsonMapReaderModule::AdjustScale(const FVector InMeshSize) const
{
	FVector Scale;
	Scale.X = Scale.X != 0 ? (1 / InMeshSize.X) * DefaultSize : 1;
	Scale.Y = Scale.Y != 0 ? (1 / InMeshSize.Y) * DefaultSize : 1;
	Scale.Z = Scale.Z != 0 ? (1 / InMeshSize.Z) * DefaultSize : 1;
	return Scale;
}

FTransform FBombermanJsonMapReaderModule::AdjustTransform(const FVector InMeshSize, const int32 InX, const int32 InY, const int32 InZ) const
{
	FTransform MyTransform;
	FVector MyLocation = FVector(InX, InY, InZ);
	FVector Scale = AdjustScale(InMeshSize);
	MyTransform.SetScale3D(Scale);
	MyTransform.AddToTranslation(MyLocation);
	return MyTransform;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBombermanJsonMapReaderModule, BombermanJsonMapReader)