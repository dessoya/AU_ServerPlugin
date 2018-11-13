#include "M_player_common_activity.h"
#include "M_hook_helper.h"
#include "Messages.h"
#include "M_online_players.h"

#include "EventMessage.h"
#include "M_event_writer.h"

#include <API/ARK/Ark.h>

DECLARE_HOOK(AShooterCharacter_Die, bool, AShooterCharacter*, float, FDamageEvent*, AController*, AActor*);
bool Hook_AShooterCharacter_Die(AShooterCharacter* _this, float KillingDamage, FDamageEvent* DamageEvent, AController* Killer, AActor* DamageCauser) {
	unsigned long long id = 0;

	auto d1 = _this->GetPlayerData();
	if (d1) {
		auto d2 = d1->MyDataField();
		if (d2) {
			id = d2->PlayerDataIDField();
		}
	}

	auto r = AShooterCharacter_Die_original(_this, KillingDamage, DamageEvent, Killer, DamageCauser);

	if (id > 0) {
		Log::GetLog()->info("AShooterCharacter_Die playerId {} r {}", id, r);
		if (r) {
			_msg_m_player_die(id);
		}
	}
	return r;
}

DECLARE_HOOK(AShooterGameMode_Logout, void, AShooterGameMode*, AController*);
void Hook_AShooterGameMode_Logout(AShooterGameMode* _this, AController* Exiting) {

	if (Exiting) {

		AShooterPlayerController *shooterPlayer = static_cast<AShooterPlayerController*>(Exiting);
		if (shooterPlayer) {
			APlayerController *player = static_cast<APlayerController*>(shooterPlayer);
			if (player) {


				FString s, s2;
				auto name = shooterPlayer->GetPlayerCharacterName(&s);
				std::string nameutf8 = ArkApi::Tools::Utf8Encode(**name);

				auto steamName = player->PlayerStateField()->PlayerNameField();

				std::string steamNameutf = ArkApi::Tools::Utf8Encode(*steamName);

				auto steamId = shooterPlayer->GetUniqueNetIdAsString(&s2);

				auto id = shooterPlayer ? shooterPlayer->LinkedPlayerIDField() : 0;

				Log::GetLog()->info("[*] Logout steamId {} playerId {} steamName {} playerName {}",
					steamId->ToString(), id, steamNameutf.c_str(), nameutf8.c_str());

				FVector pos = player->DefaultActorLocationField();
				_msg_m_player_set_pos(id, &pos);

				_msg_m_player_logout(id, steamId->ToString(), steamNameutf, nameutf8);

				if (onlinePlayers) {
					onlinePlayers->erase(id);
				}

			}
		}

	}

	AShooterGameMode_Logout_original(_this, Exiting);
}

DECLARE_HOOK(AShooterGameMode_StartNewShooterPlayer, void, AShooterGameMode *, APlayerController *, bool, bool, FPrimalPlayerCharacterConfigStruct *, UPrimalPlayerData *);

void Hook_AShooterGameMode_StartNewShooterPlayer(AShooterGameMode* _this, APlayerController* new_player,
	bool force_create_new_player_data, bool is_from_login, FPrimalPlayerCharacterConfigStruct* char_config,
	UPrimalPlayerData* ark_player_data) {

	AShooterGameMode_StartNewShooterPlayer_original(_this, new_player, force_create_new_player_data, is_from_login, char_config, ark_player_data);

	if (new_player) {
		
		AShooterPlayerController* player = static_cast<AShooterPlayerController*>(new_player);

		if (player) {

			int messageType = m_stop_work;
			std::string eventName = "unknown";

			if (is_from_login) { messageType = m_player_login; eventName = "Login"; }
			else if (force_create_new_player_data) { messageType = m_player_create; eventName = "CreatePlayer"; }

			FString s, s2;
			auto name = player->GetPlayerCharacterName(&s);
			std::string nameutf8 = ArkApi::Tools::Utf8Encode(**name);

			const FString steamName = new_player->PlayerStateField()->PlayerNameField();
			std::string steamNameutf = ArkApi::Tools::Utf8Encode(*steamName);

			auto steamId = player->GetUniqueNetIdAsString(&s2);

			auto id = player->LinkedPlayerIDField();
			auto sid = stoull(steamId->ToString());

			Log::GetLog()->info("[*] {} steamId {} playerId {} steamName {} playerName {}",
				eventName.c_str(), steamId->ToString(), id, steamNameutf.c_str(), nameutf8.c_str());

			_msg_m_player_login(id, messageType, sid, steamNameutf, nameutf8);

			if (onlinePlayers) {
				onlinePlayers->insertPlayer(player, sid);
			}

			FVector pos = player->DefaultActorLocationField();

			_msg_m_player_set_pos(id, &pos);
		}
	}
}


bool M_player_common_activity_init() {
	__register_hook(AShooterCharacter, Die);
	__register_hook(AShooterGameMode, StartNewShooterPlayer);
	__register_hook(AShooterGameMode, Logout);
	return true;
}

bool M_player_common_activity_done() {
	__unregister_hook(AShooterCharacter, Die);
	__unregister_hook(AShooterGameMode, StartNewShooterPlayer);
	__unregister_hook(AShooterGameMode, Logout);
	return true;
}