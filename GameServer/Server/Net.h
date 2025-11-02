#ifndef Net
#define Net
#pragma once
#include "Utils.h"
#include "GamePhases.h"
#include "DiscordIntegration.h"
#include "Ai.h"

enum class EReplicationSystemSendPass : unsigned
{
	Invalid,
	PostTickDispatch,
	TickFlush,
};

struct FSendUpdateParams
{
	EReplicationSystemSendPass SendPass = EReplicationSystemSendPass::TickFlush;

	float DeltaSeconds = 0.f;
};

bool (*InitListen)(void*, void*, FURL&, bool, FString&) = decltype(InitListen)(InSDKUtils::GetImageBase() + 0x6898440);
void (*SetWorld)(UNetDriver*, UWorld*) = decltype(SetWorld)(__int64(GetModuleHandleW(0)) + 0x242FAFC);
UNetDriver* (__fastcall* CreateNetDriver_Local)(void*, FWorldContext*, FName, FName) = decltype(CreateNetDriver_Local)(InSDKUtils::GetImageBase() + 0x1FD12D0);
FWorldContext* (*GetWorldContext)(UEngine* a1, UWorld* a2) = decltype(GetWorldContext)(uintptr_t(GetModuleHandleW(0)) + 0x12C8A48);
UClass* (__fastcall* NetworkPredictionStaticClass)() = decltype(NetworkPredictionStaticClass)(InSDKUtils::GetImageBase() + 0x3646E80);
void(__fastcall* SendClientAdjustmentOG)(void*) = decltype(SendClientAdjustmentOG)(InSDKUtils::GetImageBase() + 0x62B214C);
void(*UpdateReplicationViews)(UNetDriver*) = decltype(UpdateReplicationViews)(InSDKUtils::GetImageBase() + 0x6577FB4);
void(*PreSendUpdate)(UReplicationSystem*, const FSendUpdateParams&) = decltype(PreSendUpdate)(InSDKUtils::GetImageBase() + 0x58675D8);

void SendClientAdjustment(APlayerController* PC)
{
	if (!PC)
		return;

	if (PC->AcknowledgedPawn != PC->Pawn && !PC->SpectatorPawn)
		return;

	auto Pawn = (ACharacter*)(PC->Pawn ? PC->Pawn : PC->SpectatorPawn);
	if (Pawn)
	{
		UCharacterMovementComponent* MoveComp = Pawn->CharacterMovement;
		if (MoveComp) {
			if (bAllowUnsafeMoves) {
				MoveComp->bIgnoreClientMovementErrorChecksAndCorrection = true;
				MoveComp->bServerAcceptClientAuthoritativePosition = true;
				MoveComp->NetworkMaxSmoothUpdateDistance = 100000.f;
				MoveComp->NetworkMinTimeBetweenClientAckGoodMoves = 1.f;
			}
			auto Interface = GetInterfaceAddress(Pawn->CharacterMovement, NetworkPredictionStaticClass());

			if (Interface)
			{
				SendClientAdjustmentOG(Interface);
			}
		}

	}
}
void SendClientMoveAdjustments(UNetDriver* Driver)
{
	for (UNetConnection* Connection : Driver->ClientConnections)
	{
		if (Connection == nullptr || Connection->ViewTarget == nullptr)
			continue;

		if (APlayerController* PC = Connection->PlayerController)
			SendClientAdjustment(PC);

		for (UNetConnection* ChildConnection : Connection->Children)
		{
			if (ChildConnection == nullptr)
				continue;

			if (APlayerController* PC = ChildConnection->PlayerController)
				SendClientAdjustment(PC);
		}
	}
}
int LastPurgeTime = 0;
inline void (*TickFlush)(UNetDriver*, float DeltaTime);
void TickFlushHook(UNetDriver* NetDriver, float DeltaTime)
{
	static bool bStopGame = false;
	if (NetDriver->ClientConnections.Num() > 0)
	{
		bStopGame = true;
		GamePhasesTick();
		auto ReplicationSystem = *(UReplicationSystem**)(__int64(NetDriver) + 0x748);
		if (ReplicationSystem)
		{
			UpdateReplicationViews(NetDriver);
			SendClientMoveAdjustments(NetDriver);
			FSendUpdateParams Params = {};
			Params.SendPass = EReplicationSystemSendPass::TickFlush;
			Params.DeltaSeconds = DeltaTime;
			PreSendUpdate(ReplicationSystem, Params);

		}
		if (bLateGame) {
			auto TimeSeconds = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld());
			if (TimeSeconds > LastPurgeTime)
			{
				LastPurgeTime = TimeSeconds + 420;
				for (auto Actor : Builds)
				{
					auto Build = (ABuildingSMActor*)Actor;
					if (!Build)
						continue;
					Build->K2_DestroyActor();
						
				}
				Builds.Free();
			}
		}
		
	}
	if (NetDriver->ClientConnections.Num() == 0 && bStopGame)
	{
		auto Message = Messages::HostRestarting;
		std::string HostType = "Reload";
		if (bLateGame)
			HostType = "LateGame";
		Replace(Message, "%host%", HostType);
		SendWebhook(Message);
		TerminateProcess(GetCurrentProcess(), 1);
	}
		

	return TickFlush(NetDriver, DeltaTime);
}

int GetNetMode(__int64 a1) {
    return 1;
}

bool bIsListening = false;


void Listen()
{
	auto World = UWorld::GetWorld();
	auto Engine = UEngine::GetEngine();
	FWorldContext* Context = GetWorldContext(Engine, World);
	auto NetDriverName = Conv_StringToName(L"GameNetDriver");
	auto NetDriver = World->NetDriver = CreateNetDriver_Local(Engine, Context, NetDriverName, FName(0));
	*(bool*)(__int64(NetDriver) + 0x751) = true;

	for (auto& Collection : World->LevelCollections)
		Collection.NetDriver = NetDriver;

	NetDriver->NetDriverName = NetDriverName;
	NetDriver->World = World;

	SetWorld(NetDriver, World);
	FString Err;
	FURL URL;
	URL.Port = 7777;
	if (InitListen(NetDriver, World, URL, false, Err)) {

		SetWorld(NetDriver, World);
		bIsListening = true;
	}
}
#endif // !Net


