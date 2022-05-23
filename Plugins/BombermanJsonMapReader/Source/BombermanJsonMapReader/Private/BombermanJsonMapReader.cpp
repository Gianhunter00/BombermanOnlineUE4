// Copyright Epic Games, Inc. All Rights Reserved.

#include "BombermanJsonMapReader.h"

#define LOCTEXT_NAMESPACE "FBombermanJsonMapReaderModule"

void FBombermanJsonMapReaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	MapCreateActor.Add("0", &FBombermanJsonMapReaderModule::CreatePlatform);
	MapCreateActor.Add("1", &FBombermanJsonMapReaderModule::CreateBreakableWall);
	MapCreateActor.Add("2", &FBombermanJsonMapReaderModule::CreateUnbreakableWall);
	MapCreationTool = new BombermanMapCreationTool();
	OnPathChosenFromDialog = MapCreationTool->OnPathChosenFromDialog->CreateRaw(this, &FBombermanJsonMapReaderModule::CreateMapFromPath);
	OnCreateMapTool = MapCreationTool->OnCreateMapTool->CreateRaw(this, &FBombermanJsonMapReaderModule::CreateMap);
	MapCreationTool->OnPathChosenFromDialog = &OnPathChosenFromDialog;
	MapCreationTool->OnCreateMapTool = &OnCreateMapTool;
}

void FBombermanJsonMapReaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	MapCreateActor.Empty();
}
//D:\Test.txt
bool FBombermanJsonMapReaderModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FParse::Command(&Cmd, TEXT("newlevel")))
	{
		FString Path = FParse::Token(Cmd, true);
		CreateMapFromPath(Path);

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
	UE_LOG(LogTemp, Error, TEXT("%s"), *StaticMesh->GetBounds().GetBox().GetSize().ToString());
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

void FBombermanJsonMapReaderModule::CreateMapFromPath(FString Path)
{
	TArray<FString> FileArr;
	FFileHelper::LoadFileToStringArray(FileArr, *Path);
	CreateMap(FileArr);
}

void FBombermanJsonMapReaderModule::CreateMap(TArray<FString> Data)
{
	int32 StartX = 0;
	int32 StartY = 0;
	float Offset = DefaultSize * 0.5f;
	if (Data.Num() > 0)
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
			AActor* SkySphere = WorldCasted->SpawnActor<AActor>(BPSkySphere->GeneratedClass);
			FProperty* DirLightProp = BPSkySphere->GeneratedClass->FindPropertyByName(TEXT("Directional light actor"));
			if (DirLightProp)
			{
				if (FObjectProperty* DirLightObjectProp = CastField<FObjectProperty>(DirLightProp))
				{
					UKismetSystemLibrary::SetObjectPropertyByName(SkySphere, FName("Directional light actor"), MyDirectionalLight);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Cant Cast Prop"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Cant find Prop"));
			}
		}
		for (FString Line : Data)
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
}

FVector FBombermanJsonMapReaderModule::AdjustScale(const FVector InMeshSize) const
{
	FVector Scale;
	Scale.X = !FMath::IsNearlyZero(InMeshSize.Z) ? (DefaultSize / InMeshSize.X) : 1;
	Scale.Y = !FMath::IsNearlyZero(InMeshSize.Z) ? (DefaultSize / InMeshSize.Y) : 1;
	Scale.Z = !FMath::IsNearlyZero(InMeshSize.Z) ? (DefaultSize / InMeshSize.Z) : 1;
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