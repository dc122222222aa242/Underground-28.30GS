#ifndef SAFEZONELOCATIONS
#pragma once
#include "Utils.h"

constexpr float KINDA_SMALL_NUMBER = 1.e-4f;

struct FStormCircle
{
    FVector Center;
    float Radius;
};

std::vector<FStormCircle> StormCircles;

inline FVector2D GetSafeNormal(FVector2D v)
{
    float sizeSq = v.X * v.X + v.Y * v.Y;
    if (sizeSq > KINDA_SMALL_NUMBER)
    {
        float inv = 1.f / UKismetMathLibrary::sqrt(sizeSq);
        return FVector2D(v.X * inv, v.Y * inv);
    }
    return FVector2D(0.f, 0.f);
}

inline FVector GetSafeNormal(FVector v)
{
    float sizeSq = v.X * v.X + v.Y * v.Y + v.Z * v.Z;
    if (sizeSq > KINDA_SMALL_NUMBER)
    {
        float inv = 1.f / UKismetMathLibrary::sqrt(sizeSq);
        return FVector(v.X * inv, v.Y * inv, v.Z * inv);
    }
    return FVector(0.f, 0.f, 0.f);
}

inline bool IsNearlyZero(FVector2D v)
{
    return (v.X * v.X + v.Y * v.Y) < KINDA_SMALL_NUMBER * KINDA_SMALL_NUMBER;
}

FVector ClampToPlayableBounds(const FVector& Candidate, float Radius, const FBoxSphereBounds& Bounds)
{
    FVector Clamped = Candidate;

    Clamped.X = UKismetMathLibrary::clamp(Clamped.X, Bounds.Origin.X - Bounds.BoxExtent.X + Radius, Bounds.Origin.X + Bounds.BoxExtent.X - Radius);
    Clamped.Y = UKismetMathLibrary::clamp(Clamped.Y, Bounds.Origin.Y - Bounds.BoxExtent.Y + Radius, Bounds.Origin.Y + Bounds.BoxExtent.Y - Radius);

    return Clamped;
}

void GenerateStormCircles(AFortAthenaMapInfo* MapInfo, int32 Seed)
{
    StormCircles.clear();

    std::mt19937 rng(Seed);
    std::uniform_real_distribution<float> dist01(0.f, 1.f);
    auto FRandSeeded = [&]() { return dist01(rng); };

    float Radii[13] = { 150000, 120000, 95000, 70000, 55000, 32500, 20000, 10000, 5000, 2500, 1650, 1090, 0 };

    FVector FirstCenter = MapInfo->GetMapCenter();
    StormCircles.push_back(FStormCircle{ FirstCenter, Radii[0] });

    float DirAngle = FRandSeeded() * 2.f * PI;
    float DirSin, DirCos;
    SinCos(&DirSin, &DirCos, DirAngle);
    FVector2D Dir(DirCos, DirSin);
    for (int i = 1; i < 5; ++i)
    {
        FVector PrevCenter = StormCircles[i - 1].Center;
        float rPrev = StormCircles[i - 1].Radius;
        float rNew = Radii[i];

        float baseAngle = atan2(Dir.Y, Dir.X);
        float delta = (FRandSeeded() - 0.5f) * (PI / 36.f); 
        float angle = baseAngle + delta;

        FVector2D MoveDir(cos(angle), sin(angle));
        MoveDir = GetSafeNormal(MoveDir);

        float f_i = 0.4f + FRandSeeded() * 0.5f;
        float Offset = f_i * (rPrev - rNew);

        FVector NewCenter = PrevCenter + FVector(MoveDir.X, MoveDir.Y, 0.f) * Offset;

        StormCircles.push_back({ NewCenter, rNew });
        Dir = MoveDir;
    }

    for (int i = 5; i < 8; ++i)
    {
        FVector PrevCenter = StormCircles[i - 1].Center;
        float rPrev = StormCircles[i - 1].Radius;
        float rNew = Radii[i];

        float angle = FRandSeeded() * 2.f * PI;
        float s, c; SinCos(&s, &c, angle);
        FVector2D RandDir(c, s);
        RandDir = GetSafeNormal(RandDir);

        float d = sqrt(rPrev * rPrev - rNew * rNew);;
        FVector NewCenter = PrevCenter + FVector(RandDir.X, RandDir.Y, 0.f) * d;

        NewCenter = ClampToPlayableBounds(NewCenter, rNew, MapInfo->CachedPlayableBoundsForClients);

        StormCircles.push_back(FStormCircle{ NewCenter, rNew });
    }

    FVector RefCenter = StormCircles[6].Center;
    float RefRadius = StormCircles[6].Radius;

    for (int i = 8; i < 13; ++i)
    {
        float angle = FRandSeeded() * 2.f * PI;
        float s, c; SinCos(&s, &c, angle);
        float Dist = FRandSeeded() * RefRadius * 0.8f; 
        FVector2D RandDir(c, s);
        RandDir = GetSafeNormal(RandDir);

        FVector NewCenter = RefCenter + FVector(RandDir.X, RandDir.Y, 0.f) * Dist;

        NewCenter = ClampToPlayableBounds(NewCenter, Radii[i], MapInfo->CachedPlayableBoundsForClients);

        StormCircles.push_back(FStormCircle{ NewCenter, Radii[i] });
    }
}
#endif // SAFEZONELOCATIONS
