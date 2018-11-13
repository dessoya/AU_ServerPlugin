#include "M_structures.h"

#include "M_hook_helper.h"
#include "Messages.h"
#include <API/ARK/Ark.h>

DECLARE_HOOK(APrimalStructure_FinalStructurePlacement, bool, APrimalStructure *, APlayerController *, FVector, FRotator, FRotator, APawn *, FName, bool);
bool Hook_APrimalStructure_FinalStructurePlacement(APrimalStructure *this_, APlayerController *PC, FVector AtLocation, FRotator AtRotation,
	FRotator PlayerViewRotation, APawn * AttachToPawn, FName BoneName, bool bIsFlipped) {

	auto result = APrimalStructure_FinalStructurePlacement_original(this_, PC, AtLocation, AtRotation, PlayerViewRotation, AttachToPawn, BoneName, bIsFlipped);

	if (result) {

		/*
		long long timestamp = 0, playerId = 0;
		FString *steamId = 0;
		std::string steamNameutf = "none", playerNameutf8 = "none";
		FString s, s2;
		*/


		AShooterPlayerController *shooterPlayer = static_cast<AShooterPlayerController*>(PC);
		if (shooterPlayer) {

			// auto name = shooterPlayer->GetPlayerCharacterName(&s);
			// playerNameutf8 = ArkApi::Tools::Utf8Encode(**name);

			// auto steamName = PC->PlayerStateField()->PlayerNameField();

			// steamNameutf = ArkApi::Tools::Utf8Encode(*steamName);

			// steamId = shooterPlayer->GetUniqueNetIdAsString(&s2);

			auto playerId = shooterPlayer->LinkedPlayerIDField();

			auto structureName = _bp(this_);
			/*
			FString structureName = "none";
			if (this_ && this_->IsA(APrimalStructure::GetPrivateStaticClass())) {

				APrimalStructure* structure = static_cast<APrimalStructure*>(this_);

				structureName = _bp(structure);
			}
			*/

			_msg_m_build_structure(playerId, structureName.ToString(), &AtLocation);
				
		}
	}

	return result;
}

bool M_structures_init() {
	__register_hook(APrimalStructure, FinalStructurePlacement);
	return true;
}

bool M_structures_done() {
	__unregister_hook(APrimalStructure, FinalStructurePlacement);
	return true;
}
