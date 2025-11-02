#ifndef SPECIALACTOR
#define SPECIALACTOR
#pragma once
#include "Utils.h"
__int64(__fastcall* ConstructSlateBrushOG)(__int64 a1, FSlateBrush* a2);
__int64 __fastcall ConstructSlateBrushHK(__int64 a1, FSlateBrush* a2) {
    return ConstructSlateBrushOG(a1, a2);
}
int8 RegisterSlateBrush(const FSlateBrush& Brush)
{
    for (int32 i = 0; i < GetGameState()->SpecialActorData->SlateBrushes.Num(); i++)
    {
        if (GetGameState()->SpecialActorData->SlateBrushes[i] == Brush)
        {
            return i;
        }
    }

    GetGameState()->SpecialActorData->SlateBrushes.Add(Brush);
    return GetGameState()->SpecialActorData->SlateBrushes.Num() - 1;
}

int8 RegisterBrushScale(const FVector2D& Scale)
{
    for (int32 i = 0; i < GetGameState()->SpecialActorData->BrushScales.Num(); i++)
    {
        if (GetGameState()->SpecialActorData->BrushScales[i] == Scale)
        {
            return i;
        }
    }

    GetGameState()->SpecialActorData->BrushScales.Add(Scale);
    return GetGameState()->SpecialActorData->BrushScales.Num() - 1;
}

void UpdateActorTransform(FName SpecialActorID, const FVector& Location, float Yaw)
{
    for (int32 i = 0; i < GetGameState()->SpecialActorData->SpecialActorRepList.SpecialActorList.Num(); i++)
    {
        if (GetGameState()->SpecialActorData->SpecialActorRepList.SpecialActorList[i].SpecialActorID == SpecialActorID)
        {
            if (i < GetGameState()->SpecialActorData->SpecialActorTransforms.Num())
            {
                GetGameState()->SpecialActorData->SpecialActorTransforms[i].Location = (FVector_NetQuantize)Location;
                GetGameState()->SpecialActorData->SpecialActorTransforms[i].Yaw = Yaw;
            }
            else
            {
                FSpecialActorTransform NewTransform;
                NewTransform.Location = (FVector_NetQuantize)Location;
                NewTransform.Yaw = Yaw;
                GetGameState()->SpecialActorData->SpecialActorTransforms.Add(NewTransform);
            }
            break;
        }
    }
}


void RegisterSpecialActor(AActor* SpecialActor, UAthenaSpecialActorComponent* SpecialActorComponent)
{
    if (!SpecialActor || !SpecialActorComponent)
        return;

    FName SpecialActorID = SpecialActorComponent->SpecialActorID;

    FSpecialActorRepData NewRepData = {};
    
    NewRepData.SpecialActorID = SpecialActorID;
    NewRepData.SpecialActor = SpecialActor;
    NewRepData.CategoryTag = SpecialActorComponent->SpecialActorTag;
    NewRepData.PlayerState = nullptr; 
    NewRepData.ReplicationInterval = 0.1f;
    NewRepData.ReplicationIntervalDeviation = 0.05f;
    NewRepData.NextReplicationTime = GetStatics()->GetTimeSeconds(GetWorld());

    NewRepData.bDrawCompassIcon = SpecialActorComponent->bUseCompassIcon;
    NewRepData.bDrawCompassDistanceText = SpecialActorComponent->bDrawCompassDistanceText;
    NewRepData.bDrawRotated = SpecialActorComponent->bDrawRotated;
    NewRepData.YawOffset = SpecialActorComponent->YawOffset;
    NewRepData.ElevationArrowData = SpecialActorComponent->ElevationArrowData;
    NewRepData.RenderOnMapDuringPhases = SpecialActorComponent->RenderOnMapDuringPhases;
    NewRepData.DrawLayer = SpecialActorComponent->DrawLayer;
    NewRepData.SeasonItemTag = SpecialActorComponent->SeasonItemTag;
    NewRepData.Priority = SpecialActorComponent->Priority;
    NewRepData.RenderDistance = EvaluateScalableFloat(SpecialActorComponent->RenderDistance);
    NewRepData.RenderTags = SpecialActorComponent->RenderTags;

    auto BrushIdx = RegisterSlateBrush(SpecialActorComponent->SpecialActorMinimapIconBrush);
    auto Sclidx = RegisterBrushScale(SpecialActorComponent->SpecialActorMinimapIconScale);
    NewRepData.MinimapIconBrushIndex = BrushIdx;
    NewRepData.MinimapScaleIndex = Sclidx;
    NewRepData.FullScreenMapIconBrushIndex = BrushIdx;
    NewRepData.FullScreenMapScaleIndex = Sclidx;
    NewRepData.CompassIconBrushIndex = BrushIdx;
    NewRepData.CompassScaleIndex = Sclidx;
    NewRepData.DisplayName = UKismetTextLibrary::Conv_StringToText(L"Idk what should be done here");

    FSpecialActorTransform InitialTransform;
    InitialTransform.Location = (FVector_NetQuantize)SpecialActor->K2_GetActorLocation();
    InitialTransform.Yaw = SpecialActor->K2_GetActorRotation().Yaw;
    GetGameState()->SpecialActorData->SpecialActorTransforms.Add(InitialTransform);
    NewRepData.ReplicatedTransformIndex = GetGameState()->SpecialActorData->SpecialActorTransforms.NumElements - 1;

    GetGameState()->SpecialActorData->SpecialActorRepList.SpecialActorList.Add(NewRepData);
    GetGameState()->SpecialActorData->SpecialActorRepList.MarkArrayDirty();
    GetGameState()->OnRep_SpecialActorData();
}

void UnregisterSpecialActor(AFortSpecialActorReplicationInfo* RepInfo,AActor* SpecialActor, UAthenaSpecialActorComponent* SpecialActorComponent)
{
    if (!SpecialActor || !SpecialActorComponent)
        return;

    FName SpecialActorID = SpecialActorComponent->SpecialActorID;

    for (int32 i = 0; i < RepInfo->SpecialActorRepList.SpecialActorList.Num(); i++)
    {
        if (GetGameState()->SpecialActorData->SpecialActorRepList.SpecialActorList[i].SpecialActorID == SpecialActorID)
        {
            GetGameState()->SpecialActorData->SpecialActorRepList.SpecialActorList.Remove(i);
            GetGameState()->SpecialActorData->SpecialActorRepList.MarkArrayDirty();

            if (i < RepInfo->SpecialActorTransforms.Num())
            {
                GetGameState()->SpecialActorData->SpecialActorTransforms.Remove(i);
            }

            break;
        }
    }
}
void UpdateSpeialActors() {
    GetGameState()->SpecialActorData->SpecialActorRepList.SpecialActorList.Free();
    GetGameState()->SpecialActorData->SpecialActorTransforms.Free();
    GetGameState()->SpecialActorData->SpecialActorTransforms.Free();
    for (int i = 0; i < UObject::GObjects->Num(); i++)
    {
        auto Object = UObject::GObjects->GetByIndex(i);

        if (!Object)
            continue;

        if (Object->IsA(UAthenaSpecialActorComponent::StaticClass()) && !Object->IsDefaultObject())
        {
            auto Component = (UAthenaSpecialActorComponent*)Object;
            auto Owner = Component->GetOwner();
            if (Owner) {
                if (!Owner->GetFullName().contains("RadioTower") || !Component->GetFullName().contains("RadioTower")) {
                    RegisterSpecialActor(Owner, Component);
                }
            }
        }
    }
}
#endif
