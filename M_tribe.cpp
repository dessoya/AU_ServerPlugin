#include <API/ARK/Ark.h>
#include "M_hook_helper.h"
#include "M_tribe.h"
#include "Utils.h"
#include "Messages.h"
#include "M_online_players.h"
#include "M_storage.h"

// void BPDoHarvestAttack(int harvestIndex) { NativeCall<void, int>(this, "APrimalDinoCharacter.BPDoHarvestAttack", harvestIndex); }
// DECLARE_HOOK(APrimalDinoCharacter_BPDoHarvestAttack, void, APrimalDinoCharacter *, int harvestIndex);

/*
11 / 22 / 18 21:44[ExtendedEventLog][info][*] SetSleeping before
11 / 22 / 18 21 : 44[ExtendedEventLog][info][*] APrimalDinoCharacter_SetCharacterStatusTameable before
11 / 22 / 18 21 : 44[ExtendedEventLog][info][*] APrimalDinoCharacter_SetCharacterStatusTameable after
11 / 22 / 18 21 : 44[ExtendedEventLog][info][*] SetSleeping after
11 / 22 / 18 21 : 44[ExtendedEventLog][info][*] update player activity 1 id 244960722 ms 4005
11 / 22 / 18 21 : 44[ExtendedEventLog][info][*] update player activity 0 id 244960722 ms 4008
11 / 22 / 18 21 : 44[ExtendedEventLog][info][*] update player activity 0 id 244960722 ms 4035
11 / 22 / 18 21 : 44[ExtendedEventLog][info][*] update player activity 0 id 244960722 ms 4042
11 / 22 / 18 21 : 44[ExtendedEventLog][info][*] update player activity 0 id 244960722 ms 4044
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] update player activity 0 id 244960722 ms 4005
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] update player activity 0 id 244960722 ms 4042
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] update player activity 0 id 244960722 ms 4037
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] SetSleeping before
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] SetSleeping after
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] update player activity 0 id 244960722 ms 4053
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] APrimalDinoCharacter_SetCharacterStatusTameable before
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] APrimalDinoCharacter_SetCharacterStatusTameable after
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] APrimalDinoCharacter_SetupTamed before
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] APrimalDinoCharacter_SetupTamed after
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] SetSleeping before
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] APrimalDinoCharacter_SetCharacterStatusTameable before
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] APrimalDinoCharacter_SetCharacterStatusTameable after
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] SetSleeping after
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] SetSleeping before
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] SetSleeping after
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] AddToTribeLog 1862935903 < RichColor Color = "0, 1, 0, 1" > kossad приручает a Додо - Ур 52 (Додо)!< / > before
11 / 22 / 18 21 : 45[ExtendedEventLog][info][*] AddToTribeLog 1862935903 < RichColor Color = "0, 1, 0, 1" > kossad приручает a Додо - Ур 52 (Додо)!< / > after
*/


/*
void ServerSleepingTick() { NativeCall<void>(this, "APrimalDinoCharacter.ServerSleepingTick"); }
void ServerTamedTick() { NativeCall<void>(this, "APrimalDinoCharacter.ServerTamedTick"); }
void OnPrimalCharacterSleeped() { NativeCall<void>(this, "APrimalDinoCharacter.OnPrimalCharacterSleeped"); }
void TameDino(AShooterPlayerController * ForPC, bool bIgnoreMaxTameLimit, int OverrideTamingTeamID, bool bPreventNameDialog, bool bSkipAddingTamedLevels) { NativeCall<void, AShooterPlayerController *, bool, int, bool, bool>(this, "APrimalDinoCharacter.TameDino", ForPC, bIgnoreMaxTameLimit, OverrideTamingTeamID, bPreventNameDialog, bSkipAddingTamedLevels); }
*/

/*
void ServerRequestLevelUp_Implementation(UPrimalCharacterStatusComponent * forStatusComp, EPrimalCharacterStatusValue::Type ValueType) { NativeCall<void, UPrimalCharacterStatusComponent *, EPrimalCharacterStatusValue::Type>(this, "AShooterPlayerController.ServerRequestLevelUp_Implementation", forStatusComp, ValueType); }
DECLARE_HOOK(APrimalDinoCharacter_SetupTamed, void, APrimalDinoCharacter *, bool bWasJustTamed);
void Hook_APrimalDinoCharacter_SetupTamed(APrimalDinoCharacter *this_, bool bWasJustTamed) {
*/

// void InitializeInventory() { NativeCall<void>(this, "UPrimalInventoryComponent.InitializeInventory"); }
DECLARE_HOOK(UPrimalInventoryComponent_InitializeInventory, void, UPrimalInventoryComponent*);
void Hook_UPrimalInventoryComponent_InitializeInventory(UPrimalInventoryComponent *this_) {

	auto owner = this_->GetOwner();

	auto mode = ArkApi::GetApiUtils().GetShooterGameMode();
	if (mode) {
		auto m = mode->SupplyCrateLootQualityMultiplierField();
		Log::GetLog()->info("[*] SupplyCrateLootQualityMultiplierField {}", m);
	}

	if (owner) {
		Log::GetLog()->info("[*] InitializeInventory before {} {}", _bp(owner).ToString(), this_->InventoryItemsField().Max());
	}

	UPrimalInventoryComponent_InitializeInventory_original(this_);

	if (owner) {
		Log::GetLog()->info("[*] InitializeInventory after {} {}", _bp(owner).ToString(), this_->InventoryItemsField().Max());
	}
}

// static UPrimalItem * AddNewItem(TSubclassOf<UPrimalItem> ItemArchetype, UPrimalInventoryComponent * GiveToInventory, 
// bool bEquipItem, bool bDontStack, float ItemQuality, bool bForceNoBlueprint, int quantityOverride, bool bForceBlueprint, 
// float MaxItemDifficultyClamp, bool CreateOnClient, TSubclassOf<UPrimalItem> ApplyItemSkin, float MinRandomQuality)
// { return NativeCall<UPrimalItem *, TSubclassOf<UPrimalItem>, UPrimalInventoryComponent *, bool, bool, float, bool, int, bool, float, bool,
// TSubclassOf<UPrimalItem>, float>(nullptr, "UPrimalItem.AddNewItem", ItemArchetype, GiveToInventory, bEquipItem, bDontStack, ItemQuality, 
// bForceNoBlueprint, quantityOverride, bForceBlueprint, MaxItemDifficultyClamp, CreateOnClient, ApplyItemSkin, MinRandomQuality); }

DECLARE_HOOK(UPrimalItem_AddNewItem, UPrimalItem *, TSubclassOf<UPrimalItem> ItemArchetype, UPrimalInventoryComponent * GiveToInventory,
	bool bEquipItem, bool bDontStack, float ItemQuality, bool bForceNoBlueprint, int quantityOverride, bool bForceBlueprint, 
	float MaxItemDifficultyClamp, bool CreateOnClient, TSubclassOf<UPrimalItem> ApplyItemSkin, float MinRandomQuality);

UPrimalItem *Hook_UPrimalItem_AddNewItem(TSubclassOf<UPrimalItem> ItemArchetype, UPrimalInventoryComponent * GiveToInventory,
	bool bEquipItem, bool bDontStack, float ItemQuality, bool bForceNoBlueprint, int quantityOverride, bool bForceBlueprint,
	float MaxItemDifficultyClamp, bool CreateOnClient, TSubclassOf<UPrimalItem> ApplyItemSkin, float MinRandomQuality) {

	auto r = UPrimalItem_AddNewItem_original(ItemArchetype, GiveToInventory,
		bEquipItem, bDontStack, ItemQuality, bForceNoBlueprint, quantityOverride, bForceBlueprint,
		MaxItemDifficultyClamp, CreateOnClient, ApplyItemSkin, MinRandomQuality);

	if (r && GiveToInventory) {
		auto n1 = _bp(r);
		auto n2 = _bp(GiveToInventory);
		Log::GetLog()->info("[*] UPrimalItem_AddNewItem {} {} {} {}", n2.ToString(), n1.ToString(), ItemQuality, MaxItemDifficultyClamp);
	}

	return r;
}


void dump_stats(UPrimalCharacterStatusComponent *s) {

	if (s == 0) {
		Log::GetLog()->info("[!] UPrimalCharacterStatusComponent == 0");
		return;
	}
	// wild / pre tame pts
	{
		Log::GetLog()->info("[!] NumberOfLevelUpPointsAppliedField");
		auto c = s->NumberOfLevelUpPointsAppliedField();
		auto c2 = s->MaxStatusValuesField();
		for (int i = 0; i < c.GetSize(); i++) {
			auto item = c()[i];
			auto item2 = c2()[i];
			Log::GetLog()->info("{} {}", (int)item, item2);
		}
	}
	// tame pts
	/*
	{
		Log::GetLog()->info("[!] NumberOfLevelUpPointsAppliedTamedField");
		auto c = s->NumberOfLevelUpPointsAppliedTamedField();
		for (int i = 0; i < c.GetSize(); i++) {
			auto item = c()[i];
			Log::GetLog()->info("{}", (int)item);
		}
	}
	*/
	/*
	{
		Log::GetLog()->info("[!] BaseLevelMaxStatusValuesField");
		auto c = s->BaseLevelMaxStatusValuesField();
		for (int i = 0; i < c.GetSize(); i++) {
			auto item = c()[i];
			Log::GetLog()->info("{}", item);
		}
	}
	*/
	// max value of stat

	/*
	{
		Log::GetLog()->info("[!] MaxStatusValuesField");
		auto c = s->MaxStatusValuesField();
		for (int i = 0; i < c.GetSize(); i++) {
			auto item = c()[i];
			Log::GetLog()->info("{}", item);
		}
	}
	*/


	// current value of stat
	/*
	{
		Log::GetLog()->info("[!] CurrentStatusValuesField");
		auto c = s->CurrentStatusValuesField();
		for (int i = 0; i < c.GetSize(); i++) {
			auto item = c()[i];
			Log::GetLog()->info("{}", item);
		}
	}
	*/
	/*
	{
		Log::GetLog()->info("[!] AdditionalStatusValuesField");
		auto c = s->AdditionalStatusValuesField();
		for (int i = 0; i < c.GetSize(); i++) {
			auto item = c()[i];
			Log::GetLog()->info("{}", item);
		}
	}
	*/
}

// void PostInitializeComponents() { NativeCall<void>(this, "APrimalDinoCharacter.PostInitializeComponents"); }
// void RegisterAllComponents() { NativeCall<void>(this, "APrimalDinoCharacter.RegisterAllComponents"); }

// void NetUpdateDinoNameStrings_Implementation(FString * NewTamerString, FString * NewTamedName) { NativeCall<void, FString *, FString *>(this, "APrimalDinoCharacter.NetUpdateDinoNameStrings_Implementation", NewTamerString, NewTamedName); }
// void NetUpdateDinoNameStrings(FString * NewTamerString, FString * NewTamedName) { NativeCall<void, FString *, FString *>(this, "APrimalDinoCharacter.NetUpdateDinoNameStrings", NewTamerString, NewTamedName); }
DECLARE_HOOK(APrimalDinoCharacter_NetUpdateDinoNameStrings_Implementation, void, APrimalDinoCharacter *, FString * NewTamerString, FString * NewTamedName);
void Hook_APrimalDinoCharacter_NetUpdateDinoNameStrings_Implementation(APrimalDinoCharacter *this_, FString * NewTamerString, FString * NewTamedName) {
	APrimalDinoCharacter_NetUpdateDinoNameStrings_Implementation_original(this_, NewTamerString, NewTamedName);

	std::string tnameutf = ArkApi::Tools::Utf8Encode(**NewTamedName);
	_msg_m_dino_change_name(this_, tnameutf);
	// Log::GetLog()->info("[*] dino change name '{}'", tnameutf);

}

// void PostInitProperties() { NativeCall<void>(this, "APrimalDinoCharacter.PostInitProperties"); }
DECLARE_HOOK(APrimalDinoCharacter_BeginPlay, void, APrimalDinoCharacter *);
void Hook_APrimalDinoCharacter_BeginPlay(APrimalDinoCharacter *this_) {
	APrimalDinoCharacter_BeginPlay_original(this_);

	// auto name = _bp(this_);
	// Log::GetLog()->info("[*] APrimalDinoCharacter_BeginPlay {}", name.ToString());
	unsigned long long tid = this_->TargetingTeamField();
	unsigned long long pid = this_->OwningPlayerIDField();
	// Log::GetLog()->info("[*] {} {}", tid, pid);
	if (pid == 0 && tid < 100) {
		_msg_m_dino_spawn_wild(this_);
	}

	// dump_stats(this_->MyCharacterStatusComponentField());
}


// void ServerRequestLevelUp_Implementation(UPrimalCharacterStatusComponent * forStatusComp, EPrimalCharacterStatusValue::Type ValueType) { NativeCall<void, UPrimalCharacterStatusComponent *, EPrimalCharacterStatusValue::Type>(this, "AShooterPlayerController.ServerRequestLevelUp_Implementation", forStatusComp, ValueType); }
DECLARE_HOOK(AShooterPlayerController_ServerRequestLevelUp_Implementation, void, AShooterPlayerController *, UPrimalCharacterStatusComponent * forStatusComp, EPrimalCharacterStatusValue::Type ValueType);
void Hook_AShooterPlayerController_ServerRequestLevelUp_Implementation(AShooterPlayerController *this_, UPrimalCharacterStatusComponent * forStatusComp, EPrimalCharacterStatusValue::Type ValueType) {

	// auto name = __getClassName(this_);
	// Log::GetLog()->info("[*] AShooterPlayerController_ServerRequestLevelUp_Implementation before");
	
	AShooterPlayerController_ServerRequestLevelUp_Implementation_original(this_, forStatusComp, ValueType);

	bool lvlupfordino = false;
	auto inv = this_->RemoteViewingInventoriesField();
	for (int i = 0; i < inv.Max(); i++) {
		// Log::GetLog()->info("[!] inventory index {}", i);
		auto item = &(*inv[i]);
		auto p = item->GetOwner();
		if (p) {
			// auto pname = __getClassName(p);
			auto ptype = __getClassType(p);
			// Log::GetLog()->info("[!] onwer class name {}", pname.ToString());

			if (ptype == ot_APrimalDinoCharacter) {
				auto dino_ = static_cast<APrimalDinoCharacter *>(p);
				if (forStatusComp == dino_->MyCharacterStatusComponentField()) {
					// Log::GetLog()->info("[!] lvlupfordino");
					lvlupfordino = true;

					_msg_m_dino_level_up(dino_, this_->LinkedPlayerIDField(), (unsigned short)ValueType);

					break;
				}
			}
		}
	}

	if (!lvlupfordino) {
		// Log::GetLog()->info("[!] lvlupforplayer");
		long long pid = this_->LinkedPlayerIDField();
		if (pid) {
			unsigned short stat_id = (unsigned short)ValueType;
			auto points = forStatusComp->NumberOfLevelUpPointsAppliedField();
			auto values = forStatusComp->MaxStatusValuesField();
			_msg_m_player_level_up(pid, stat_id, points()[stat_id], values()[stat_id]);
		}
	}
	// Log::GetLog()->info("[!] stat index {}", (int)ValueType);	
	// dump_stats(forStatusComp);
	// Log::GetLog()->info("[*] AShooterPlayerController_ServerRequestLevelUp_Implementation after");

}


// void ClientNotifyLevelUp_Implementation(APrimalCharacter * ForChar, int NewLevel) { NativeCall<void, APrimalCharacter *, int>(this, "AShooterPlayerController.ClientNotifyLevelUp_Implementation", ForChar, NewLevel); }
DECLARE_HOOK(AShooterPlayerController_ClientNotifyLevelUp_Implementation, void, AShooterPlayerController *, APrimalCharacter * ForChar, int NewLevel);
void Hook_AShooterPlayerController_ClientNotifyLevelUp_Implementation(AShooterPlayerController *this_, APrimalCharacter * ForChar, int NewLevel) {

	auto name = __getClassName(this_);
	Log::GetLog()->info("[*] AShooterPlayerController_ClientNotifyLevelUp_Implementation before {} {} {}", name.ToString(), NewLevel);


	AShooterPlayerController_ClientNotifyLevelUp_Implementation_original(this_, ForChar, NewLevel);

	dump_stats(ForChar->MyCharacterStatusComponentField());
	Log::GetLog()->info("[*] AShooterPlayerController_ClientNotifyLevelUp_Implementation after {} {} {}", name.ToString(), NewLevel);

}


// int LevelUpPlayerAddedStat(TEnumAsByte<enum EPrimalCharacterStatusValue::Type> StatToLevel, int NumLevels, AShooterPlayerController * ForPlayer) { return NativeCall<int, TEnumAsByte<enum EPrimalCharacterStatusValue::Type>, int, AShooterPlayerController *>(this, "APrimalCharacter.LevelUpPlayerAddedStat", StatToLevel, NumLevels, ForPlayer); }
DECLARE_HOOK(APrimalCharacter_LevelUpPlayerAddedStat, int, APrimalCharacter *, TEnumAsByte<enum EPrimalCharacterStatusValue::Type> StatToLevel, int NumLevels, AShooterPlayerController * ForPlayer);
int Hook_APrimalCharacter_LevelUpPlayerAddedStat(APrimalCharacter *this_, TEnumAsByte<enum EPrimalCharacterStatusValue::Type> StatToLevel,
	int NumLevels, AShooterPlayerController * ForPlayer) {

	auto name = __getClassName(this_);
	auto v = (int)StatToLevel.GetValue();
	Log::GetLog()->info("[*] APrimalCharacter_LevelUpPlayerAddedStat before {} {} {}", name.ToString(), v, NumLevels);


	auto r = APrimalCharacter_LevelUpPlayerAddedStat_original(this_, StatToLevel, NumLevels, ForPlayer);

	dump_stats(this_->MyCharacterStatusComponentField());
	Log::GetLog()->info("[*] APrimalCharacter_LevelUpPlayerAddedStat after {} {} {}", name.ToString(), v, NumLevels);

	return r;
}



/*
	Health = 0x0,
	Stamina = 0x1,
	Torpidity = 0x2,
	Oxygen = 0x3,
	Food = 0x4,
	Water = 0x5,
	Temperature = 0x6,
	Weight = 0x7,
	MeleeDamageMultiplier = 0x8,
	SpeedMultiplier = 0x9,
	TemperatureFortitude = 0xA,
	CraftingSpeedMultiplier = 0xB,

*/

// TArray<FString> * GetCustomFolders(TArray<FString> * result, int InventoryCompType) { return NativeCall<TArray<FString> *, TArray<FString> *, int>(this, "UPrimalInventoryComponent.GetCustomFolders", result, InventoryCompType); }
// void AddCustomFolder(FString CFolder, int InventoryCompType) { NativeCall<void, FString, int>(this, "UPrimalInventoryComponent.AddCustomFolder", CFolder, InventoryCompType); }
// void RemoveCustomFolder(AShooterPlayerController * PC, FString FolderName, int InventoryCompType) { NativeCall<void, AShooterPlayerController *, FString, int>(this, "UPrimalInventoryComponent.RemoveCustomFolder", PC, FolderName, InventoryCompType); }


// void ServerDeleteCustomFolder_Implementation(UPrimalInventoryComponent * forInventory, FString * CFolderName, int InventoryCompType) { NativeCall<void, UPrimalInventoryComponent *, FString *, int>(this, "AShooterPlayerController.ServerDeleteCustomFolder_Implementation", forInventory, CFolderName, InventoryCompType); }
// void ServerDeleteCustomFolder(UPrimalInventoryComponent * forInventory, FString * CFolderName, int InventoryCompType) { NativeCall<void, UPrimalInventoryComponent *, FString *, int>(this, "AShooterPlayerController.ServerDeleteCustomFolder", forInventory, CFolderName, InventoryCompType); }

// DECLARE_HOOK(AShooterPlayerController_ServerDeleteCustomFolder_Implementation, void, AShooterPlayerController *, UPrimalInventoryComponent * forInventory, FString * CFolderName, int InventoryCompType);
// void Hook_AShooterPlayerController_ServerDeleteCustomFolder_Implementation(AShooterPlayerController *this_, UPrimalInventoryComponent * forInventory, FString * CFolderName, int InventoryCompType) {
DECLARE_HOOK(AShooterPlayerController_ServerDeleteCustomFolder_Implementation, void, AShooterPlayerController *, UPrimalInventoryComponent * forInventory, FString * CFolderName, int InventoryCompType);
void Hook_AShooterPlayerController_ServerDeleteCustomFolder_Implementation(AShooterPlayerController *this_, UPrimalInventoryComponent * forInventory, FString * CFolderName, int InventoryCompType) {

	auto folder = CFolderName->ToString();
	Log::GetLog()->info("[*] RemoveCustomFolder '{}' {} {}", folder, InventoryCompType, (long long)forInventory->GetOwner());
	if (InventoryCompType == 768) {

		ObjectTypes ownerClassType = ot_unknown;
		AActor *owner = forInventory->GetOwner();
		if(owner) {
			ownerClassType = __getClassType(owner);
		}

		if (ownerClassType == ot_APrimalDinoCharacter) {
			auto dino_ = static_cast<APrimalDinoCharacter *>(owner);
			if (folder.find("#hp-") == 0 || folder.find("#oxy-") == 0) {
				Log::GetLog()->info("[*] skip deleting dino stat folder");
				return;
			}
			
			storage_write(new SCMD_DelFolder(makeItemId(dino_->DinoID1Field(), dino_->DinoID2Field()), folder));
		}
	}

	AShooterPlayerController_ServerDeleteCustomFolder_Implementation_original(this_, forInventory, CFolderName, InventoryCompType);
}

// void AddCustomFolder(FString CFolder, int InventoryCompType) { NativeCall<void, FString, int>(this, "UPrimalInventoryComponent.AddCustomFolder", CFolder, InventoryCompType); }
DECLARE_HOOK(UPrimalInventoryComponent_AddCustomFolder, void, UPrimalInventoryComponent *, FString CFolder, int InventoryCompType, FServerCustomFolder *CustomFolder);
void Hook_UPrimalInventoryComponent_AddCustomFolder(UPrimalInventoryComponent *this_, FString CFolder, int InventoryCompType, FServerCustomFolder *CustomFolder) {

	// auto f = this_->GetCustomFolders(&r, InventoryCompType);
	// Log::GetLog()->info("[*] AddCustomFolder '{}', {} {} {}", CFolder.ToString(), InventoryCompType, CustomFolder.FolderName.ToString(), CustomFolder.InventoryCompType);
	// Log::GetLog()->info("[*] AddCustomFolder '{}', {} {} {}", CFolder.ToString(), InventoryCompType, (long long)CustomFolder, (long long)this_);

	// Log::GetLog()->info("[*] {}", (long long)this_->GetOwner(), this_->b);
	// Log::GetLog()->info("[*] {}", (long long)this_->GetOwnerController());
	// Log::GetLog()->info("[*] {}", (long long)this_->GetCharacterStatusComponent());
	if (InventoryCompType == 768) {
		auto inv = (UPrimalInventoryComponent *)(((char *)this_) - 216);
		ObjectTypes ownerClassType = ot_unknown;
		AActor *owner = 0;
		try {

			owner = inv->GetOwner();
			if (owner) {
				ownerClassType = __getClassType(owner);
			}
		}
		catch (...) {
			ownerClassType = ot_unknown;
			owner = 0;
		}

		if (ownerClassType == ot_APrimalDinoCharacter) {
			auto dino_ = static_cast<APrimalDinoCharacter *>(owner);
			Log::GetLog()->info("[*] add folder {} to dino {}", CFolder.ToString(), _bp(dino_).ToString());
			storage_write(new SCMD_AddFolder(makeItemId(dino_->DinoID1Field(), dino_->DinoID2Field()), CFolder.ToString()));

		}
	}

	// 768 0x300

	

	/*
	auto folders = this_->ServerCustomFolderField();
	for (int i = 0, l = folders.Max(); i < l; i++) {
		auto item = folders[i];
		Log::GetLog()->info("[*] 1 {} {}", i, item.ToString());
	}
	*/

	/*
	TArray<FString> f1;
	auto f2 = this_->GetCustomFolders(&f1, InventoryCompType);
	for (int i = 0, l = f1.Max(); i < l; i++) {
		auto item = f1[i];
		Log::GetLog()->info("[*] 2 {} {}", i, item.ToString());
	}
	for (int i = 0, l = f2->Max(); i < l; i++) {
		auto item = (*f2)[i];
		Log::GetLog()->info("[*] 3 {} {}", i, item.ToString());
	}
	*/

	// char *a = 0; *a = 0;

	UPrimalInventoryComponent_AddCustomFolder_original(this_, CFolder, InventoryCompType, 0);
	// UPrimalInventoryComponent_AddCustomFolder_original(this_, FString("test1"), InventoryCompType, 0);

}


// void ClientAddFolderToInventoryComponent_Implementation(UPrimalInventoryComponent * forInventory, FString * NewCustomFolderName, int InventoryCompType) { NativeCall<void, UPrimalInventoryComponent *, FString *, int>(this, "AShooterPlayerController.ClientAddFolderToInventoryComponent_Implementation", forInventory, NewCustomFolderName, InventoryCompType); }
typedef std::map<unsigned long long, APrimalDinoCharacter *> DinoMap;
DinoMap *tamed_queue = 0;

void __check_for_tamed_folders(APrimalDinoCharacter *dino_) {

	bool gotFolder = false;

	unsigned long long did = makeItemId(dino_->DinoID1Field(), dino_->DinoID2Field());
	auto folders = get_dino_folders(did);

	auto inv = dino_->MyInventoryComponentField();
	
	// auto folders = inv->ServerCustomFolderField();

	if (folders) {
		int cnt = 0;
		for (auto it = folders->begin(); it != folders->end(); it++) {
			auto folder = *it;
			Log::GetLog()->info("[*] folder {}", folder);
			if (folder.find("#hp-") == 0 || folder.find("#oxy-") == 0) {
				cnt++;
			}
		}
		if (cnt > 1) gotFolder = true;

	}

	if (!gotFolder) {

		/*
		unsigned long long tid = dino_->TargetingTeamField();
		unsigned long long pid = dino_->OwningPlayerIDField();

		_msg_m_dino_tame(tid, pid, dino_);
		*/
		(*tamed_queue)[did] = dino_;

		auto folderInv = (UPrimalInventoryComponent *)(((char *)inv) + 216);

		auto s = dino_->MyCharacterStatusComponentField();
		auto c = s->NumberOfLevelUpPointsAppliedField();
		auto c2 = s->MaxStatusValuesField();
		FString f1;

		f1 = FString::Format("#hp-{} #st-{} #wgh-{} #dmg-{}",
			int(c()[0]),
			int(c()[1]),
			int(c()[7]),
			int(c()[8])
		);

		inv->RemoteDeleteCustomFolder(&f1, 768);
		folderInv->AddCustomFolder(f1, 768, 0);

		f1 = FString::Format("#oxy-{} #fd-{} #spd-{}",
			int(c()[3]),
			int(c()[4]),
			int(c()[9])
		);

		inv->RemoteDeleteCustomFolder(&f1, 768);
		folderInv->AddCustomFolder(f1, 768, 0);
		
	}

}

// void ServerViewRemoteInventory(AShooterPlayerController * ByPC) { NativeCall<void, AShooterPlayerController *>(this, "UPrimalInventoryComponent.ServerViewRemoteInventory", ByPC); }
DECLARE_HOOK(UPrimalInventoryComponent_ServerViewRemoteInventory, void, UPrimalInventoryComponent *, AShooterPlayerController * ByPC);
void Hook_UPrimalInventoryComponent_ServerViewRemoteInventory(UPrimalInventoryComponent *this_, AShooterPlayerController * ByPC) {
	
	// check for dino tamed inventory
	auto owner = this_->GetOwner();
	// auto owner = this_;
	if (owner) {
		auto ownerClassType = __getClassType(owner);
		Log::GetLog()->info("[*] view inventory ownerClassType {}", ownerClassType);
		if (ownerClassType == ot_APrimalDinoCharacter) {
			auto dino_ = static_cast<APrimalDinoCharacter *>(owner);
			unsigned long long tid = dino_->TargetingTeamField();
			unsigned long long pid = dino_->OwningPlayerIDField();
			if (tid > 100 || pid != 0) {
				// tamed dino

				__check_for_tamed_folders(dino_);

				// auto name = _bp(dino_);
				// Log::GetLog()->info("[*] tamed dino {} {} {}", name.ToString(), tid, pid);
			}
		}
	}

	UPrimalInventoryComponent_ServerViewRemoteInventory_original(this_, ByPC);
}

// void SetupTamed(bool bWasJustTamed) { NativeCall<void, bool>(this, "APrimalDinoCharacter.SetupTamed", bWasJustTamed); }
DECLARE_HOOK(APrimalDinoCharacter_SetupTamed, void, APrimalDinoCharacter *, bool bWasJustTamed);
void Hook_APrimalDinoCharacter_SetupTamed(APrimalDinoCharacter *this_, bool bWasJustTamed) {

	// auto i = this_->MyInventoryComponentField();
	// i->GetOwner();

	auto name = _bp(this_);
	unsigned long long tid = this_->TargetingTeamField();
	unsigned long long pid = this_->OwningPlayerIDField();
	Log::GetLog()->info("[*] SetupTamed before {} {} {} {}", bWasJustTamed, name.ToString(), tid, pid);

	// check for dino tamed inventory
	// auto owner = this_->GetOwner();
	
	APrimalDinoCharacter_SetupTamed_original(this_, bWasJustTamed);

	auto owner = this_;
	if (owner) {
		auto ownerClassType = __getClassType(owner);
		Log::GetLog()->info("[*] view inventory ownerClassType {}", ownerClassType);
		if (ownerClassType == ot_APrimalDinoCharacter) {
			auto dino_ = static_cast<APrimalDinoCharacter *>(owner);
			unsigned long long tid = dino_->TargetingTeamField();
			unsigned long long pid = dino_->OwningPlayerIDField();
			if (tid > 100 || pid != 0) {
				// tamed dino
				auto name = _bp(dino_);
				Log::GetLog()->info("[*] tamed dino {} {} {}", name.ToString(), tid, pid);

				__check_for_tamed_folders(dino_);

			}
		}
	}

	Log::GetLog()->info("[*] SetupTamed after {} {} {} {}", bWasJustTamed, name.ToString(), tid, pid);
	auto s = this_->MyCharacterStatusComponentField();
	dump_stats(s);
}

//
//	knockdown
//
// [*] APrimalDinoCharacter_SetCharacterStatusTameable before 1 0 true true false Dodo_Character_BP

// float TakeDamage(float Damage, FDamageEvent * DamageEvent, AController * EventInstigator, AActor * DamageCauser) { return NativeCall<float, float, FDamageEvent *, AController *, AActor *>(this, "APrimalDinoCharacter.TakeDamage", Damage, DamageEvent, EventInstigator, DamageCauser); }
// void AdjustDamage(float * Damage, FDamageEvent * DamageEvent, AController * EventInstigator, AActor * DamageCauser) { NativeCall<void, float *, FDamageEvent *, AController *, AActor *>(this, "APrimalDinoCharacter.AdjustDamage", Damage, DamageEvent, EventInstigator, DamageCauser); }
DECLARE_HOOK(APrimalDinoCharacter_AdjustDamage, void, APrimalDinoCharacter *, float * Damage, FDamageEvent * DamageEvent, AController * EventInstigator, AActor * DamageCauser);
void Hook_APrimalDinoCharacter_AdjustDamage(APrimalDinoCharacter *this_, float *Damage, FDamageEvent *DamageEvent,
	AController *EventInstigator, AActor *DamageCauser) {
	
	unsigned long long pid = 0;
	unsigned long long did = makeItemId(this_->DinoID1Field(), this_->DinoID2Field());

	if (DamageCauser) {
		auto ot1 = __getClassType(DamageCauser);
		if (ot1 == ot_AShooterCharacter) {
			AShooterCharacter *player = static_cast<AShooterCharacter *>(DamageCauser);
			if (player) {
				pid = player->LinkedPlayerDataIDField();
				if (onlinePlayers) {
					auto oi = onlinePlayers->get(pid);
					if (oi) {
						oi->lastHitDino = this_;
					}
				}
			}
		}
	}

	// Log::GetLog()->info("[*] APrimalDinoCharacter_AdjustDamage before {} {} {}", did, (unsigned long long)ot1, pid);

	APrimalDinoCharacter_AdjustDamage_original(this_, Damage, DamageEvent, EventInstigator, DamageCauser);

	// Log::GetLog()->info("[*] APrimalDinoCharacter_AdjustDamage after {} {} {}", did, (unsigned long long)ot1, pid);
}


// int GetRandomBaseLevel() { return NativeCall<int>(this, "APrimalDinoCharacter.GetRandomBaseLevel"); }
DECLARE_HOOK(APrimalDinoCharacter_GetRandomBaseLevel, int, APrimalDinoCharacter *);
int Hook_APrimalDinoCharacter_GetRandomBaseLevel(APrimalDinoCharacter *this_) {

	// Log::GetLog()->info("[*] APrimalDinoCharacter_GetRandomBaseLevel before");

	auto r = APrimalDinoCharacter_GetRandomBaseLevel_original(this_);

	r = rand();
	if (r < 0) r *= -1;
	int total_ = 151 + 60 + 7;
	r = r % total_ ;

	if (r < 48) {
		r /= 3;
	}
	else if (r >= total_ - (47 + 7)) {
		
		r -= (total_ - (47 + 7));
		r /= 3;
		r += 130;
		if (r > 145) r = 145;

	}
	else {
		r -= 47;
		r += 15;
	}

	r += 5;

	// Log::GetLog()->info("[*] APrimalDinoCharacter_GetRandomBaseLevel after {}", r);

	return r;
}

// void SetCharacterStatusTameable(bool bSetTameable, bool bCreateInventory, bool keepInventoryForWakingTame) { NativeCall<void, bool, bool, bool>(this, "APrimalDinoCharacter.SetCharacterStatusTameable", bSetTameable, bCreateInventory, keepInventoryForWakingTame); }
DECLARE_HOOK(APrimalDinoCharacter_SetCharacterStatusTameable, void, APrimalDinoCharacter *, bool bSetTameable, bool bCreateInventory, bool keepInventoryForWakingTame);
void Hook_APrimalDinoCharacter_SetCharacterStatusTameable(APrimalDinoCharacter *this_, bool bSetTameable, bool bCreateInventory, bool keepInventoryForWakingTame) {
	
	auto name = _bp(this_);
	Log::GetLog()->info("[*] APrimalDinoCharacter_SetCharacterStatusTameable before {}", name.ToString());
		
	APrimalDinoCharacter_SetCharacterStatusTameable_original(this_, bSetTameable, bCreateInventory, keepInventoryForWakingTame);

	Log::GetLog()->info("[*] APrimalDinoCharacter_SetCharacterStatusTameable after {}", name.ToString());

	unsigned long long tid = this_->TargetingTeamField();
	unsigned long long pid = this_->OwningPlayerIDField();

	Log::GetLog()->info("[*] {} {} {}", bSetTameable, bCreateInventory, keepInventoryForWakingTame);
	Log::GetLog()->info("[*] tid {} pid {}", tid, pid);

	if (!bSetTameable && bCreateInventory && tid < 100 && pid == 0) {
		// wake
		_msg_m_dino_wake(this_);
	}

	else if (!bSetTameable && bCreateInventory && ( (tid > 100 && pid == tid) || (pid == 0 && tid > 100) ) ) {
		// tame
		if (onlinePlayers) {
			// Log::GetLog()->info("[*] 1");
			auto oi = onlinePlayers->getByDinoDamage(this_);
			if (oi) {
				// Log::GetLog()->info("[*] 2");
				unsigned long long tid = 0;
				auto state = oi->shooterPlayer->PlayerStateField();
				AShooterPlayerState *shstate = 0;
				if (state) {
					shstate = static_cast<AShooterPlayerState *>(oi->shooterPlayer->PlayerStateField());
					if (shstate) {
						tid = shstate->MyPlayerDataField()->MyDataField()->TribeIDField();
					}
				}

				_msg_m_dino_tame(oi->id, tid, this_);
			}
		}

	}

	else if (bSetTameable && bCreateInventory && (tid == 1 || tid == 4) && pid == 0) {
		// knockdown

		// auto s = this_->MyCharacterStatusComponentField();
		if (onlinePlayers) {
			// Log::GetLog()->info("[*] 1");
			auto oi = onlinePlayers->getByDinoDamage(this_);
			if (oi) {
				// Log::GetLog()->info("[*] 2");
				unsigned long long tid = 0;
				auto state = oi->shooterPlayer->PlayerStateField();
				AShooterPlayerState *shstate = 0;
				if (state) {
					shstate = static_cast<AShooterPlayerState *>(oi->shooterPlayer->PlayerStateField());
					if (shstate) {
						tid = shstate->MyPlayerDataField()->MyDataField()->TribeIDField();
					}
				}
				_msg_m_dino_knockdown(oi->id, tid, this_);
			}
		}

	}

	/*
		FString name = _bp(this_);
	Log::GetLog()->info("[*] APrimalDinoCharacter_SetCharacterStatusTameable before {} {} {} {} {} {}",
		this_->TargetingTeamField(), this_->OwningPlayerIDField(), bSetTameable, bCreateInventory, keepInventoryForWakingTame, name.ToString());

	dump_stats(s);




	Log::GetLog()->info("[*] APrimalDinoCharacter_SetCharacterStatusTameable after {} {} {} {} {} {}",
		this_->TargetingTeamField(), this_->OwningPlayerIDField(), bSetTameable, bCreateInventory, keepInventoryForWakingTame, name.ToString());
	*/

}

// bool Die(float KillingDamage, FDamageEvent * DamageEvent, AController * Killer, AActor * DamageCauser) { return NativeCall<bool, float, FDamageEvent *, AController *, AActor *>(this, "APrimalDinoCharacter.Die", KillingDamage, DamageEvent, Killer, DamageCauser); }
DECLARE_HOOK(APrimalDinoCharacter_Die, bool, APrimalDinoCharacter *, float KillingDamage, FDamageEvent * DamageEvent, AController * Killer, AActor * DamageCauser);
bool Hook_APrimalDinoCharacter_Die(APrimalDinoCharacter *this_, float KillingDamage, FDamageEvent * DamageEvent, AController * Killer, AActor * DamageCauser) {
	auto r = APrimalDinoCharacter_Die_original(this_, KillingDamage, DamageEvent, Killer, DamageCauser);
	if (r) {
		_msg_m_dino_die(this_);
		storage_write(new SCMD_DinoDie(makeItemId(this_->DinoID1Field(), this_->DinoID2Field())));
	}
	return r;
}



// void SetSleeping(bool bSleeping, bool bUseRagdollLocationOffset) { NativeCall<void, bool, bool>(this, "APrimalCharacter.SetSleeping", bSleeping, bUseRagdollLocationOffset); }
DECLARE_HOOK(APrimalCharacter_SetSleeping, void, APrimalCharacter *, bool bSleeping, bool bUseRagdollLocationOffset);
void Hook_APrimalCharacter_SetSleeping(APrimalCharacter *this_, bool bSleeping, bool bUseRagdollLocationOffset) {

	Log::GetLog()->info("[*] SetSleeping before {} {}", bSleeping, bUseRagdollLocationOffset);

	APrimalCharacter_SetSleeping_original(this_, bSleeping, bUseRagdollLocationOffset);

	Log::GetLog()->info("[*] SetSleeping after {} {}", bSleeping, bUseRagdollLocationOffset);
}

// void OnPrimalCharacterSleeped() { NativeCall<void>(this, "APrimalCharacter.OnPrimalCharacterSleeped"); }
DECLARE_HOOK(APrimalCharacter_OnPrimalCharacterSleeped, void, APrimalCharacter *);
void Hook_APrimalCharacter_OnPrimalCharacterSleeped(APrimalCharacter *this_) {

	Log::GetLog()->info("[*] OnPrimalCharacterSleeped before");

	APrimalCharacter_OnPrimalCharacterSleeped_original(this_);

	Log::GetLog()->info("[*] OnPrimalCharacterSleeped after");
}

// void OnPrimalCharacterUnsleeped() { NativeCall<void>(this, "APrimalCharacter.OnPrimalCharacterUnsleeped"); }
DECLARE_HOOK(APrimalCharacter_OnPrimalCharacterUnsleeped, void, APrimalCharacter *);
void Hook_APrimalCharacter_OnPrimalCharacterUnsleeped(APrimalCharacter *this_) {

	Log::GetLog()->info("[*] OnPrimalCharacterUnsleeped before");

	APrimalCharacter_OnPrimalCharacterUnsleeped_original(this_);

	Log::GetLog()->info("[*] OnPrimalCharacterUnsleeped after");
}

// void SleepBodies() { NativeCall<void>(this, "APrimalCharacter.SleepBodies"); }
DECLARE_HOOK(APrimalCharacter_SleepBodies, void, APrimalCharacter *);
void Hook_APrimalCharacter_SleepBodies(APrimalCharacter *this_) {

	Log::GetLog()->info("[*] SleepBodies before");

	APrimalCharacter_SleepBodies_original(this_);

	Log::GetLog()->info("[*] SleepBodies after");
}


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



void _t_check_tamed_dinos(TimerContext *tctx) {
	if (tamed_queue) {
		for (auto it = tamed_queue->begin(); it != tamed_queue->end(); it++) {
			auto dino_ = it->second;

			unsigned long long tid = dino_->TargetingTeamField();
			unsigned long long pid = dino_->OwningPlayerIDField();

			if (tid > 100 || (pid != 0 && pid != 2000000000)) {
				_msg_m_dino_tame(pid, tid, dino_);
				it = tamed_queue->erase(it);
			}
		}
	}
}

bool M_tribe_init() {

	tamed_queue = new DinoMap;

	addTimer(_t_check_tamed_dinos);

	// __register_hook(AShooterGameMode, AddNewTribe);

	__register_hook(AShooterGameMode, RemoveTribe);
	__register_hook(AShooterGameMode, RemovePlayerFromTribe);
	__register_hook(AShooterGameMode, UpdateTribeData);
	__register_hook(AShooterGameMode, AddToTribeLog); 

	//__register_hook(APrimalCharacter, SetSleeping);
	//__register_hook(APrimalCharacter, OnPrimalCharacterSleeped);
	//__register_hook(APrimalCharacter, OnPrimalCharacterUnsleeped);
	//__register_hook(APrimalCharacter, SleepBodies);

	__register_hook(APrimalDinoCharacter, SetupTamed);
	__register_hook(UPrimalInventoryComponent, ServerViewRemoteInventory);
	__register_hook(UPrimalInventoryComponent, AddCustomFolder);
	__register_hook(AShooterPlayerController, ServerDeleteCustomFolder_Implementation);


	__register_hook(APrimalDinoCharacter, SetCharacterStatusTameable);
	__register_hook(APrimalDinoCharacter, AdjustDamage);
	__register_hook(APrimalDinoCharacter, GetRandomBaseLevel);
	__register_hook(APrimalDinoCharacter, Die);
	__register_hook(APrimalDinoCharacter, BeginPlay);
	__register_hook(APrimalDinoCharacter, NetUpdateDinoNameStrings_Implementation);

	__register_hook(UPrimalItem, AddNewItem);
	__register_hook(UPrimalInventoryComponent, InitializeInventory);



	// __register_hook(APrimalCharacter, LevelUpPlayerAddedStat);
	// __register_hook(AShooterPlayerController, ClientNotifyLevelUp_Implementation);
	__register_hook(AShooterPlayerController, ServerRequestLevelUp_Implementation);
	
	

	return true;
}

bool M_tribe_done() {

	// __unregister_hook(AShooterGameMode, AddNewTribe);
	__unregister_hook(AShooterGameMode, RemoveTribe);
	__unregister_hook(AShooterGameMode, RemovePlayerFromTribe);
	__unregister_hook(AShooterGameMode, UpdateTribeData);
	__unregister_hook(AShooterGameMode, AddToTribeLog);

	// __unregister_hook(APrimalCharacter, SetSleeping);
	// __unregister_hook(APrimalCharacter, OnPrimalCharacterSleeped);
	// __unregister_hook(APrimalCharacter, OnPrimalCharacterUnsleeped);
	// __unregister_hook(APrimalCharacter, SleepBodies);

	__unregister_hook(APrimalDinoCharacter, SetupTamed);
	__unregister_hook(UPrimalInventoryComponent, ServerViewRemoteInventory);
	__unregister_hook(UPrimalInventoryComponent, AddCustomFolder);
	__unregister_hook(AShooterPlayerController, ServerDeleteCustomFolder_Implementation);
	
	
	__unregister_hook(APrimalDinoCharacter, SetCharacterStatusTameable);
	__unregister_hook(APrimalDinoCharacter, AdjustDamage);
	__unregister_hook(APrimalDinoCharacter, GetRandomBaseLevel);
	__unregister_hook(APrimalDinoCharacter, Die);
	__unregister_hook(APrimalDinoCharacter, BeginPlay);
	__unregister_hook(APrimalDinoCharacter, NetUpdateDinoNameStrings_Implementation);
	
	__unregister_hook(UPrimalItem, AddNewItem);
	__unregister_hook(UPrimalInventoryComponent, InitializeInventory);


	// __unregister_hook(APrimalCharacter, LevelUpPlayerAddedStat);
	// __unregister_hook(AShooterPlayerController, ClientNotifyLevelUp_Implementation);
	__unregister_hook(AShooterPlayerController, ServerRequestLevelUp_Implementation);

	delete tamed_queue;
	tamed_queue = 0;

	return true;
}