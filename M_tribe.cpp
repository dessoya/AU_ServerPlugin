#include <API/ARK/Ark.h>
#include "M_hook_helper.h"
#include "M_tribe.h"
#include "Messages.h"

/*
11 / 21 / 18 19:58[ExtendedEventLog][info][*] AddTribe Племя игрока kossad pid 244960722 before
11 / 21 / 18 19 : 58[ExtendedEventLog][info][*] UpdateTribeData 1626734326 before
11 / 21 / 18 19 : 58[ExtendedEventLog][info] memcount 0 1
11 / 21 / 18 19 : 58[ExtendedEventLog][info][*] UpdateTribeData 1626734326 after
11 / 21 / 18 19 : 58[ExtendedEventLog][info][*] AddTribe Племя игрока kossad : 1626734326 pid 244960722 after

11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] RemovePlayerFromTribe 1503726575 pid 522988804 before
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] RemoveTribe 1503726575 before
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] RemoveTribe 1503726575 after
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] RemovePlayerFromTribe 1503726575 pid 522988804 after

11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] UpdateTribeData 1626734326 before
11 / 21 / 18 19 : 59[ExtendedEventLog][info] memcount 1 2
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] UpdateTribeData 1626734326 after

11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] UpdateTribeData 1626734326 before
11 / 21 / 18 19 : 59[ExtendedEventLog][info] memcount 2 2
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] UpdateTribeData 1626734326 after

11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] RemovePlayerFromTribe 1626734326 pid 522988804 before
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] UpdateTribeData 1626734326 before
11 / 21 / 18 19 : 59[ExtendedEventLog][info] memcount 2 1
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] UpdateTribeData 1626734326 after
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] RemovePlayerFromTribe 1626734326 pid 522988804 after

11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] AddTribe Племя игрока Kola4 pid 522988804 before
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] UpdateTribeData 1217793269 before
11 / 21 / 18 19 : 59[ExtendedEventLog][info] memcount 0 1
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] UpdateTribeData 1217793269 after
11 / 21 / 18 19 : 59[ExtendedEventLog][info][*] AddTribe Племя игрока Kola4 : 1217793269 pid 522988804 after

11 / 21 / 18 20 : 00[ExtendedEventLog][info][*] Logout steamId 76561198135095595 playerId 522988804 steamName SbIN_SoBaki_228 playerName Kola4
*/

// unsigned __int64 AddNewTribe(AShooterPlayerState * PlayerOwner, FString * TribeName, FTribeGovernment * TribeGovernment) { return NativeCall<unsigned __int64, AShooterPlayerState *, FString *, FTribeGovernment *>(this, "AShooterGameMode.AddNewTribe", PlayerOwner, TribeName, TribeGovernment); }
DECLARE_HOOK(AShooterGameMode_AddNewTribe, unsigned __int64, AShooterGameMode *, AShooterPlayerState * PlayerOwner, FString * TribeName, FTribeGovernment * TribeGovernment);
unsigned __int64 Hook_AShooterGameMode_AddNewTribe(AShooterGameMode *this_, AShooterPlayerState * PlayerOwner, FString * TribeName, FTribeGovernment * TribeGovernment) {

	std::string tnameutf = ArkApi::Tools::Utf8Encode(**TribeName);
	auto pid = PlayerOwner->MyPlayerDataField()->MyDataField()->PlayerDataIDField();

	Log::GetLog()->info("[*] AddTribe {} pid {} before", tnameutf.c_str(), pid);

	auto tid = AShooterGameMode_AddNewTribe_original(this_, PlayerOwner, TribeName, TribeGovernment);

	Log::GetLog()->info("[*] AddTribe {}:{} pid {} after", tnameutf.c_str(), tid, pid);

	return tid;
}

// void RemoveTribe(unsigned __int64 TribeID) { NativeCall<void, unsigned __int64>(this, "AShooterGameMode.RemoveTribe", TribeID); }
DECLARE_HOOK(AShooterGameMode_RemoveTribe, void, AShooterGameMode *, unsigned __int64 TribeID);
void Hook_AShooterGameMode_RemoveTribe(AShooterGameMode *this_, unsigned __int64 TribeID) {

	Log::GetLog()->info("[*] RemoveTribe {} before", TribeID);

	AShooterGameMode_RemoveTribe_original(this_, TribeID);

	Log::GetLog()->info("[*] RemoveTribe {} after", TribeID);

	_msg_m_tribe_remove(TribeID);
}

// void RemovePlayerFromTribe(unsigned __int64 TribeID, unsigned __int64 PlayerDataID, bool bDontUpdatePlayerState) { NativeCall<void, unsigned __int64, unsigned __int64, bool>(this, "AShooterGameMode.RemovePlayerFromTribe", TribeID, PlayerDataID, bDontUpdatePlayerState); }
DECLARE_HOOK(AShooterGameMode_RemovePlayerFromTribe, void, AShooterGameMode *, unsigned __int64 TribeID, unsigned __int64 PlayerDataID, bool bDontUpdatePlayerState);
void Hook_AShooterGameMode_RemovePlayerFromTribe(AShooterGameMode *this_, unsigned __int64 TribeID, unsigned __int64 PlayerDataID, bool bDontUpdatePlayerState) {

	Log::GetLog()->info("[*] RemovePlayerFromTribe {} pid {} before", TribeID, PlayerDataID);

	AShooterGameMode_RemovePlayerFromTribe_original(this_, TribeID, PlayerDataID, bDontUpdatePlayerState);

	Log::GetLog()->info("[*] RemovePlayerFromTribe {} pid {} after", TribeID, PlayerDataID);

	_msg_m_tribe_remove_player(TribeID, PlayerDataID);
}

//  void UpdateTribeData(FTribeData* NewTribeData) { NativeCall<void, FTribeData*>(this, "AShooterGameMode.UpdateTribeData", NewTribeData); }
DECLARE_HOOK(AShooterGameMode_UpdateTribeData, void, AShooterGameMode *, FTribeData* NewTribeData);
void Hook_AShooterGameMode_UpdateTribeData(AShooterGameMode *this_, FTribeData* NewTribeData) {

	Log::GetLog()->info("[*] UpdateTribeData {} before", NewTribeData->TribeIDField());

	FTribeData *tribe_data = static_cast<FTribeData*>(FMemory::Malloc(0x128 + 0x28));
	RtlSecureZeroMemory(tribe_data, 0x128 + 0x28);
	unsigned long long tid = NewTribeData->TribeIDField();
	if (this_->GetOrLoadTribeData(tid, tribe_data)) {
		auto omemids = tribe_data->MembersPlayerDataIDField();
		auto nmemids = NewTribeData->MembersPlayerDataIDField();
		Log::GetLog()->info("memcount {} {}", omemids.Max(), nmemids.Max());
		if (omemids.Max() == 0 && nmemids.Max() == 1) {
			// create tribe
			auto tname = NewTribeData->TribeNameField();
			std::string tnameutf = ArkApi::Tools::Utf8Encode(*tname);
			_msg_m_tribe_create(tid, nmemids[0], tnameutf);
		}
		else {
			// change tribe name
			if (tribe_data->TribeNameField() != NewTribeData->TribeNameField()) {
				std::string tribenameutf = ArkApi::Tools::Utf8Encode(*NewTribeData->TribeNameField());
				_msg_m_tribe_change_name(tid, tribenameutf);
			}

			// detect add player
			int c = nmemids.Max();
			for (int i = 0; i < c; i++) {
				auto pid = nmemids[i];
				auto idx = omemids.Find(pid);
				if (INDEX_NONE == idx) {
					_msg_m_tribe_add_player(tid, pid);
				}
			}
			// detect change admin

			// detect owner change
			if (tribe_data->OwnerPlayerDataIDField() != NewTribeData->OwnerPlayerDataIDField()) {
				_msg_m_tribe_set_owner(tid, NewTribeData->OwnerPlayerDataIDField());
			}
		}
	}
	FMemory::Free(tribe_data);

	AShooterGameMode_UpdateTribeData_original(this_, NewTribeData);

	Log::GetLog()->info("[*] UpdateTribeData {} after", NewTribeData->TribeIDField());
}

// void AddToTribeLog(int TribeId, FString * NewLog) { NativeCall<void, int, FString *>(this, "AShooterGameMode.AddToTribeLog", TribeId, NewLog); }
DECLARE_HOOK(AShooterGameMode_AddToTribeLog, void, AShooterGameMode *, int TribeId, FString * NewLog);
void Hook_AShooterGameMode_AddToTribeLog(AShooterGameMode *this_, int TribeId, FString * NewLog) {

	std::string log = ArkApi::Tools::Utf8Encode(**NewLog);

	Log::GetLog()->info("[*] AddToTribeLog {} {} before", TribeId, log.c_str());

	AShooterGameMode_AddToTribeLog_original(this_, TribeId, NewLog);

	Log::GetLog()->info("[*] AddToTribeLog {} {} after", TribeId, log.c_str());

	_msg_m_tribe_log(TribeId, log);
}

// int ForceAddPlayerToTribe(AShooterPlayerState * ForPlayerState, FString * TribeName) { return NativeCall<int, AShooterPlayerState *, FString *>(this, "AShooterGameMode.ForceAddPlayerToTribe", ForPlayerState, TribeName); }
// int ForceCreateTribe(FString * TribeName, int TeamOverride) { return NativeCall<int, FString *, int>(this, "AShooterGameMode.ForceCreateTribe", TribeName, TeamOverride); }
// void UpdateTribeAllianceData(FTribeAlliance * TribeAllianceData, TArray<unsigned int> * OldMembersArray, bool bIsAdd) { NativeCall<void, FTribeAlliance *, TArray<unsigned int> *, bool>(this, "AShooterGameMode.UpdateTribeAllianceData", TribeAllianceData, OldMembersArray, bIsAdd); }



bool M_tribe_init() {

	// __register_hook(AShooterGameMode, AddNewTribe);

	__register_hook(AShooterGameMode, RemoveTribe);
	__register_hook(AShooterGameMode, RemovePlayerFromTribe);
	__register_hook(AShooterGameMode, UpdateTribeData);
	__register_hook(AShooterGameMode, AddToTribeLog);

	return true;
}

bool M_tribe_done() {

	// __unregister_hook(AShooterGameMode, AddNewTribe);
	__unregister_hook(AShooterGameMode, RemoveTribe);
	__unregister_hook(AShooterGameMode, RemovePlayerFromTribe);
	__unregister_hook(AShooterGameMode, UpdateTribeData);
	__unregister_hook(AShooterGameMode, AddToTribeLog);

	return true;
}