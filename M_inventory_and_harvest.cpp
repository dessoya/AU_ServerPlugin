#include "M_inventory_and_harvest.h"
#include <API/ARK/Ark.h>
#include "Utils.h"
#include "M_hook_helper.h"
#include "M_harvest_marker.h"
#include "M_online_players.h"
#include "Messages.h"
#include "M_console_helper.h"


int E_inventory_and_harvest = 1;

void _l_object_type_info(AActor *owner_, ObjectTypes ownerClassType) {
	long long _oid = 0;
	unsigned int id1_ = 0, id2_ = 0, teamid_ = 0;
	FString ownerType = __getClassName(owner_);

	if (ownerClassType == ot_AShooterCharacter) {
		AShooterCharacter *player_ = static_cast<AShooterCharacter *>(owner_);
		_oid = player_->LinkedPlayerDataIDField();
	}
	else if (ownerClassType == ot_APrimalDinoCharacter) {
		APrimalDinoCharacter *dino_ = static_cast<APrimalDinoCharacter *>(owner_);
		id1_ = dino_->DinoID1Field();
		id2_ = dino_->DinoID2Field();
		teamid_ = dino_->TargetingTeamField();

		auto rider = dino_->RiderField().Get();
		if (rider) {
			auto riderId = rider->LinkedPlayerDataIDField();
		}

	}

	_l(SN_Inventory, "owner %s:%lld %lx:%lx teamid %ld", ownerType.ToString().c_str(), _oid, id1_, id2_, teamid_);
}

// UPrimalItem * AddItem(FItemNetInfo * theItemInfo, bool bEquipItem, bool AddToSlot, bool bDontStack, FItemNetID * InventoryInsertAfterItemID, bool ShowHUDNotification, bool bDontRecalcSpoilingTime, bool bForceIncompleteStacking, AShooterCharacter * OwnerPlayer, bool bIgnoreAbsoluteMaxInventory) { return NativeCall<UPrimalItem *, FItemNetInfo *, bool, bool, bool, FItemNetID *, bool, bool, bool, AShooterCharacter *, bool>(this, "UPrimalInventoryComponent.AddItem", theItemInfo, bEquipItem, AddToSlot, bDontStack, InventoryInsertAfterItemID, ShowHUDNotification, bDontRecalcSpoilingTime, bForceIncompleteStacking, OwnerPlayer, bIgnoreAbsoluteMaxInventory); }
DECLARE_HOOK(UPrimalInventoryComponent_AddItem, UPrimalItem *, UPrimalInventoryComponent *, FItemNetInfo * theItemInfo, bool bEquipItem, bool AddToSlot,bool bDontStack, FItemNetID * InventoryInsertAfterItemID, bool ShowHUDNotification, bool bDontRecalcSpoilingTime, bool bForceIncompleteStacking, AShooterCharacter * OwnerPlayer, bool bIgnoreAbsoluteMaxInventory);
UPrimalItem *Hook_UPrimalInventoryComponent_AddItem(UPrimalInventoryComponent *this_, FItemNetInfo * theItemInfo, bool bEquipItem, bool AddToSlot,
	bool bDontStack, FItemNetID * InventoryInsertAfterItemID, bool ShowHUDNotification, bool bDontRecalcSpoilingTime, bool bForceIncompleteStacking,
	AShooterCharacter * OwnerPlayer, bool bIgnoreAbsoluteMaxInventory) {

	unsigned long long lid = 0,_pid = 0;
	OnlinePlayerInfo *oi = 0;
	AShooterCharacter *_player = 0;
	ObjectTypes ownerClassType = ot_unknown;

	auto iid = theItemInfo->ItemIDField();

	if (theItemInfo->LastOwnerPlayerField().Get()) {
		lid = theItemInfo->LastOwnerPlayerField()->LinkedPlayerDataIDField();
	}

	auto owner_ = this_->GetOwner();
	if (owner_) {
		ownerClassType = __getClassType(owner_);
	}

	APrimalDinoCharacter *dino_ = 0;
	unsigned long long riderId = 0;
	OnlinePlayerInfo *roi = 0;

	if (ownerClassType == ot_AShooterCharacter) {
		_player = static_cast<AShooterCharacter *>(owner_);
		
		_pid = _player->LinkedPlayerDataIDField();
		oi = onlinePlayers->get(_pid);		
	}
	else if (ownerClassType == ot_APrimalDinoCharacter) {
		dino_ = static_cast<APrimalDinoCharacter *>(owner_);
		auto rider = dino_->RiderField().Get();
		if (rider) {
			riderId = rider->LinkedPlayerDataIDField();
			roi = onlinePlayers->get(riderId);
		}
	}

	auto q = theItemInfo->ItemQuantityField();
	q++;

	if (roi && lid == 0) {
		roi->onDino = true;
	}

	if (oi) {
		if (oi->inAddItem) {
			if (E_inventory_and_harvest) {
				_l(SN_Inventory, "");
				_l(SN_Inventory, "[*] Inventory.AddItem ----- skip -----");
				_l(SN_Inventory, "lid %lld q %d iid %lx:%lx", lid, q, iid.ItemID1, iid.ItemID2);

				_l_object_type_info(owner_, ownerClassType);
				_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
			}

			return UPrimalInventoryComponent_AddItem_original(this_, theItemInfo, bEquipItem, AddToSlot, bDontStack, InventoryInsertAfterItemID, ShowHUDNotification,
				bDontRecalcSpoilingTime, bForceIncompleteStacking, OwnerPlayer, bIgnoreAbsoluteMaxInventory);
		}
		oi->inAddItem = true;
		oi->lastUpdatedItem = 0;

		if (!oi->skipAddRemoveUpdateItems && !oi->inTransferFrom && lid == 0) {
			oi->needharvest = true;
		}		
	}

	if (E_inventory_and_harvest) {

		// auto c = anItem->CanCraftInInventory(this_);

		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] Inventory.AddItem ----- before -----");
		_l(SN_Inventory, "lid %lld q %d iid %lx:%lx", lid, q, iid.ItemID1, iid.ItemID2);

		_l_object_type_info(owner_, ownerClassType);

		int idx;
		auto item = this_->FindItem(&iid, false, true, &idx);
		if (item) _l(SN_Inventory, "inventiry slot %d idx %d", item->SlotIndexField(), idx);
		item = this_->FindItem(&iid, true, true, &idx);
		if (item) _l(SN_Inventory, "equiped slot %d idx %d", item->SlotIndexField(), idx);

		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}

	auto r = UPrimalInventoryComponent_AddItem_original(this_, theItemInfo, bEquipItem, AddToSlot, bDontStack, InventoryInsertAfterItemID, ShowHUDNotification,
		bDontRecalcSpoilingTime, bForceIncompleteStacking, OwnerPlayer, bIgnoreAbsoluteMaxInventory);

	if (roi && lid == 0) {
		roi->onDino = false;
		if (roi->lastUpdatedItem) {

			auto itemName = _bp(roi->lastUpdatedItem);
			_msg_m_player_harvest2(riderId, itemName.ToString(), q, ht_dino);

			if (E_inventory_and_harvest) {

				_l(SN_Inventory, "");
				_l(SN_Inventory, "[*] Harvest from dino pid %lld q %d name %s", riderId, q, itemName.ToString().c_str());
				_l(SN_Inventory, "");
			}
		}
	}

	if (E_inventory_and_harvest) {

		q = theItemInfo->ItemQuantityField();

		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] Inventory.AddItem ----- after -----");
		_l(SN_Inventory, "lid %lld q %u iid %lx:%lx", lid, q, iid.ItemID1, iid.ItemID2);
		
		_l_object_type_info(owner_, ownerClassType);

		auto item = this_->FindItem(&iid, false, true, 0);
		if (item) {
			FString name = _bp(item);
			_l(SN_Inventory, "%s q %d slot %d", name.ToString().c_str(), item->GetItemQuantity(), item->SlotIndexField());
		}

		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}

	if (ownerClassType == ot_AShooterCharacter && oi && !oi->skipAddRemoveUpdateItems) {

		auto item = this_->FindItem(&iid, false, true, 0);
		if (item && oi->lastUpdatedItem == 0 && !item->bIsEngram().Get()) {

			/*
			auto isbp = item->bIsBlueprint().Get();
			unsigned int qua = 0;
			qua = item->ItemQualityIndexField();

			auto info = makeItemInfo(item);
			*/

			/*
			if (E_inventory_and_harvest) {				
				int sz = 8;				
				for (int idx = 0; idx < sz; idx++) {
					_l(SN_Inventory, "stat %d v %0.2f", idx, item->GetItemStatModifier((EPrimalItemStat::Type)idx));
				}

				auto req = item->BaseCraftingResourceRequirementsField();
				for (int i = 0, m = req.Max(); i < m; i++) {
					auto r = req[i];
					auto t = _bp(r.ResourceItemType.uClass);
					_l(SN_Inventory, "req %d q %0.2f name %s", i, r.BaseResourceRequirement, t.ToString().c_str());
				}


				_l(SN_Inventory, "");
				_l(SN_Inventory, "[*] Add item pid %lld q %d name %s", _pid, q, info->name.ToString().c_str());
				_l(SN_Inventory, "");
			}
			*/
			
			_msg_m_player_update_inventory2(_pid, item, q);

			//_msg_m_player_update_inventory_with_quality(_pid, info->name.ToString(), isbp ? 1 : 0, __formatText(info->stats), qua,
				//__formatText(info->req), q);

			// delete info;
		}
	}
	
	if (oi) {
		oi->inAddItem = false;
		oi->needharvest = false;
		oi->lastUpdatedItem = 0;
	}

	if (roi) {
		roi->lastUpdatedItem = 0;
		roi->onDino = 0;
	}

	return r;
}

// void NotifyItemQuantityUpdated(UPrimalItem * anItem, int amount) { NativeCall<void, UPrimalItem *, int>(this, "UPrimalInventoryComponent.NotifyItemQuantityUpdated", anItem, amount); }
DECLARE_HOOK(UPrimalInventoryComponent_NotifyItemQuantityUpdated, void, UPrimalInventoryComponent *, UPrimalItem * anItem, int amount);
void Hook_UPrimalInventoryComponent_NotifyItemQuantityUpdated(UPrimalInventoryComponent *this_, UPrimalItem * anItem, int amount) {

	//-------------------------------------------------------------------------------

	unsigned long long lid = 0;
	if (anItem->LastOwnerPlayerField().Get()) {
		lid = anItem->LastOwnerPlayerField()->LinkedPlayerDataIDField();
	}

	OnlinePlayerInfo *oi = 0;
	auto iid = anItem->ItemIDField();
	ObjectTypes ownerClassType;
	auto owner_ = this_->GetOwner();
	if (owner_) {
		ownerClassType = __getClassType(owner_);
	}

	unsigned long long pid = 0;
	AShooterCharacter *player;

	APrimalDinoCharacter *dino_ = 0;
	unsigned long long riderId = 0;
	OnlinePlayerInfo *roi = 0;

	if (ownerClassType == ot_AShooterCharacter) {
		player = static_cast<AShooterCharacter *>(owner_);
		pid = player->LinkedPlayerDataIDField();
		oi = onlinePlayers->get(pid);
	}
	else if (ownerClassType == ot_APrimalDinoCharacter) {
		dino_ = static_cast<APrimalDinoCharacter *>(owner_);
		auto rider = dino_->RiderField().Get();
		if (rider) {
			riderId = rider->LinkedPlayerDataIDField();
			roi = onlinePlayers->get(riderId);
		}
	}
	
	if (roi && roi->onDino) {
		roi->lastUpdatedItem = anItem;
	}

	//-------------------------------------------------------------------------------

	FString itemName("");
	if (E_inventory_and_harvest) {

		auto c = anItem->CanCraftInInventory(this_);

		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] Inventory.NotifyItemQuantityUpdated ----- before -----");
		_l(SN_Inventory, "lid %lld q %d a %d slot %d iid %lx:%lx cc %d", lid, anItem->GetItemQuantity(), amount, anItem->SlotIndexField(), iid.ItemID1, iid.ItemID2, c ? 1 : 0);

		itemName = _bp(anItem);
		_l(SN_Inventory, "%s", itemName.ToString().c_str());

		_l_object_type_info(owner_, ownerClassType);

	}

	auto isengram = anItem->bIsEngram().Get();

	if (!isengram && oi && !oi->skipAddRemoveUpdateItems) {
		
		/*
		auto slot = anItem->SlotIndexField();
		
		auto isbp = anItem->bIsBlueprint().Get();
		unsigned int qua = 0;
		qua = anItem->ItemQualityIndexField();
		auto q = anItem->GetItemQuantity();

		auto info = makeItemInfo(anItem);

		if (E_inventory_and_harvest) {

			int sz = 8;
			for (int idx = 0; idx < sz; idx++) {										
				_l(SN_Inventory, "stat %d v %0.2f", idx, anItem->GetItemStatModifier((EPrimalItemStat::Type)idx));
			}

			auto req = anItem->BaseCraftingResourceRequirementsField();
			for (int i = 0, m = req.Max(); i < m; i++) {
				auto r = req[i];
				auto t = _bp(r.ResourceItemType.uClass);
				_l(SN_Inventory, "req %d q %0.2f name %s", i, r.BaseResourceRequirement, t.ToString().c_str());
			}

			_l(SN_Inventory, "");
			_l(SN_Inventory, "[*] Add item pid %lld q %d name %s", pid, amount, info->name.ToString().c_str());
			_l(SN_Inventory, "");
		}
		*/

		_msg_m_player_update_inventory2(pid, anItem, amount);

		/*
		_msg_m_player_update_inventory_with_quality(pid, info->name.ToString(), isbp ? 1 : 0, __formatText(info->stats), qua,
			__formatText(info->req), amount);
			*/

		if (oi->needharvest && !oi->inTransferAll) {
			
			auto itemName = _bp(anItem);
			// _msg_m_player_update_inventory(pid, info->name.ToString(), amount, 1, 0);
			_msg_m_player_harvest2(pid, itemName.ToString(), amount, ht_hand);

			if (E_inventory_and_harvest) {

				auto c = anItem->CanCraftInInventory(this_);

				_l(SN_Inventory, "");
				_l(SN_Inventory, "[*] Harvest pid %lld q %d name %s cc %d", pid, amount, itemName.ToString().c_str(), c ? 1 : 0);
				_l(SN_Inventory, "");
			}
		}

		// delete info;

		if (oi->inAddItem) {
			oi->lastUpdatedItem = anItem;
		}

	}

	if (E_inventory_and_harvest) {
		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}

	UPrimalInventoryComponent_NotifyItemQuantityUpdated_original(this_, anItem, amount);

	if (E_inventory_and_harvest) {

		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] Inventory.NotifyItemQuantityUpdated ----- after -----");
		_l(SN_Inventory, "lid %lld q %d a %d slot %d iid %lx:%lx", lid, anItem->GetItemQuantity(), amount, anItem->SlotIndexField(), iid.ItemID1, iid.ItemID2);
		_l(SN_Inventory, "%s", itemName.ToString().c_str());

		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}
}

// bool RemoveItem(FItemNetID * itemID, bool bDoDrop, bool bSecondryAction, bool bForceRemoval, bool showHUDMessage) { return NativeCall<bool, FItemNetID *, bool, bool, bool, bool>(this, "UPrimalInventoryComponent.RemoveItem", itemID, bDoDrop, bSecondryAction, bForceRemoval, showHUDMessage); }
DECLARE_HOOK(UPrimalInventoryComponent_RemoveItem, bool, UPrimalInventoryComponent *, FItemNetID * itemID, bool bDoDrop, bool bSecondryAction, bool bForceRemoval, bool showHUDMessage);
bool Hook_UPrimalInventoryComponent_RemoveItem(UPrimalInventoryComponent *this_, FItemNetID * itemID, bool bDoDrop, bool bSecondryAction, bool bForceRemoval,
	bool showHUDMessage) {

	ObjectTypes ownerClassType = ot_unknown;
	unsigned long long _pid = 0;
	OnlinePlayerInfo *oi = 0;
	AShooterCharacter *_player = 0;

	auto owner_ = this_->GetOwner();
	if (owner_) ownerClassType = __getClassType(owner_);
	if (ownerClassType == ot_AShooterCharacter) {
		_player = static_cast<AShooterCharacter *>(owner_);
		_pid = _player->LinkedPlayerDataIDField();
		if (onlinePlayers) oi = onlinePlayers->get(_pid);
	}

	if (E_inventory_and_harvest) {
		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] Inventory.RemoveItem ----- before -----");

		int idx;
		UPrimalItem *_item = this_->FindItem(itemID, false, true, &idx);
		if (owner_ && ownerClassType == ot_AShooterCharacter) {
			if (_item) {
				_l(SN_Inventory, "eq q %d slot %d iid %lx:%lx", _item->GetItemQuantity(), _item->SlotIndexField(), itemID->ItemID1, itemID->ItemID2);

				FString itemName = _bp(_item);
				_l(SN_Inventory, "%s", itemName.ToString().c_str());
			}
		}

		_l_object_type_info(owner_, ownerClassType);
	}

	if (oi && !oi->skipAddRemoveUpdateItems) {
		UPrimalItem *_item = 0;
		auto i = _player->MyInventoryComponentField();
		if (i) _item = i->FindItem(itemID, false, true, 0);

		if (_item && !_item->bIsEngram().Get()) {
			int amount = _item->GetItemQuantity();
			if (amount > 0) {

				amount *= -1;
				/*
				auto info = makeItemInfo(_item);
				amount *= -1;
				auto isbp = _item->bIsBlueprint().Get();
				unsigned int qua = 0;
				qua = _item->ItemQualityIndexField();

				if (E_inventory_and_harvest) {
					_l(SN_Inventory, "");
					_l(SN_Inventory, "[*] Del item pid %lld q %d name %s", _pid, amount, info->name.ToString().c_str());
					_l(SN_Inventory, "");
				}
				*/

				// _msg_m_player_update_inventory_with_quality(_pid, info->name.ToString(), isbp ? 1 : 0, __formatText(info->stats), qua,
					// __formatText(info->req), amount);

				// delete info;

				_msg_m_player_update_inventory2(_pid, _item, amount);
			}
		}
	}

	if (E_inventory_and_harvest) {
		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}

	auto r = UPrimalInventoryComponent_RemoveItem_original(this_, itemID, bDoDrop, bSecondryAction, bForceRemoval, showHUDMessage);

	if (E_inventory_and_harvest) {
		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] Inventory.RemoveItem ----- after -----");
		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}

	return r;
}



// void ServerTransferFromRemoteInventory_Implementation(UPrimalInventoryComponent * inventoryComp, FItemNetID itemID, int requestedQuantity, int ToSlotIndex, bool bEquipItem) { NativeCall<void, UPrimalInventoryComponent *, FItemNetID, int, int, bool>(this, "AShooterPlayerController.ServerTransferFromRemoteInventory_Implementation", inventoryComp, itemID, requestedQuantity, ToSlotIndex, bEquipItem); }
DECLARE_HOOK(AShooterPlayerController_ServerTransferFromRemoteInventory_Implementation, void, AShooterPlayerController *, UPrimalInventoryComponent * inventoryComp, FItemNetID itemID, int requestedQuantity, int ToSlotIndex, bool bEquipItem);
void Hook_AShooterPlayerController_ServerTransferFromRemoteInventory_Implementation(AShooterPlayerController *this_,
	UPrimalInventoryComponent * inventoryComp, FItemNetID itemID, int requestedQuantity, int ToSlotIndex, bool bEquipItem) {

	if (E_inventory_and_harvest) {
		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] before ASPlayerCtrl.TransferFrom");
		_l(SN_Inventory, "[*] toslot %d", ToSlotIndex);
	}

	auto pid = this_->LinkedPlayerIDField();
	OnlinePlayerInfo *oi = 0;
	if (onlinePlayers && (oi = onlinePlayers->get(pid))) {

		oi->inTransferFrom = true;

		if (0 && ToSlotIndex == -1) {
			auto item = inventoryComp->FindItem(&itemID, false, true, 0);
			if (item) {

				int q = 1000000 == requestedQuantity ? item->GetItemQuantity() : requestedQuantity;

				/*
				auto info = makeItemInfo(item);
				auto isbp = item->bIsBlueprint().Get();
				unsigned int qua = 0;
				qua = item->ItemQualityIndexField();


				if (E_inventory_and_harvest) {

					int sz = 8;
					for (int idx = 0; idx < sz; idx++) {
						_l(SN_Inventory, "stat %d v %0.2f", idx, item->GetItemStatModifier((EPrimalItemStat::Type)idx));
					}

					auto req = item->BaseCraftingResourceRequirementsField();
					for (int i = 0, m = req.Max(); i < m; i++) {
						auto r = req[i];
						auto t = _bp(r.ResourceItemType.uClass);
						_l(SN_Inventory, "req %d q %0.2f name %s", i, r.BaseResourceRequirement, t.ToString().c_str());
					}

					_l(SN_Inventory, "");
					_l(SN_Inventory, "[*] Add item pid %lld q %d name %s", pid, q, info->name.ToString().c_str());
					_l(SN_Inventory, "");
				}


				_msg_m_player_update_inventory_with_quality(pid, info->name.ToString(), isbp ? 1 : 0, __formatText(info->stats), qua,
					__formatText(info->req), q);

				delete info;
				*/

				_msg_m_player_update_inventory2(pid, item, q);
			}
		}
	}

	if (E_inventory_and_harvest) {
		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}

	AShooterPlayerController_ServerTransferFromRemoteInventory_Implementation_original(this_,
		inventoryComp, itemID, requestedQuantity, ToSlotIndex, bEquipItem);

	if (oi) oi->inTransferFrom = false;

	if (E_inventory_and_harvest) {
		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] after ASPlayerCtrl.TransferFrom");
		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}
}

// void ServerTransferAllFromRemoteInventory_Implementation(UPrimalInventoryComponent * inventoryComp, FString * CurrentCustomFolderFilter, FString * CurrentNameFilter, FString * CurrentDestinationFolder, bool bNoFolderView) { NativeCall<void, UPrimalInventoryComponent *, FString *, FString *, FString *, bool>(this, "AShooterPlayerController.ServerTransferAllFromRemoteInventory_Implementation", inventoryComp, CurrentCustomFolderFilter, CurrentNameFilter, CurrentDestinationFolder, bNoFolderView); }

DECLARE_HOOK(AShooterPlayerController_ServerTransferAllFromRemoteInventory_Implementation, void, AShooterPlayerController *,
	UPrimalInventoryComponent * inventoryComp, FString * CurrentCustomFolderFilter, FString * CurrentNameFilter, FString * CurrentDestinationFolder,
	bool bNoFolderView);

void Hook_AShooterPlayerController_ServerTransferAllFromRemoteInventory_Implementation(AShooterPlayerController *this_,
	UPrimalInventoryComponent * inventoryComp, FString * CurrentCustomFolderFilter, FString * CurrentNameFilter, FString * CurrentDestinationFolder,
	bool bNoFolderView) {

	if (E_inventory_and_harvest) {
		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] before ASPlayerCtrl.TransferAll");
		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}

	auto pid = this_->LinkedPlayerIDField();
	OnlinePlayerInfo *oi = 0;
	if (onlinePlayers && (oi = onlinePlayers->get(pid))) {
		oi->inTransferAll = true;
	}

	AShooterPlayerController_ServerTransferAllFromRemoteInventory_Implementation_original(this_, inventoryComp, CurrentCustomFolderFilter,
		CurrentNameFilter, CurrentDestinationFolder, bNoFolderView);

	if (oi) oi->inTransferAll = false;

	if (E_inventory_and_harvest) {
		_l(SN_Inventory, "");
		_l(SN_Inventory, "[*] after ASPlayerCtrl.TransferAll");
		_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
	}
}


/*
// void DropItem(FItemNetInfo * theInfo, bool bOverrideSpawnTransform, FVector * LocationOverride, FRotator * RotationOverride, bool bPreventDropImpulse, bool bThrow, bool bSecondryAction, bool bSetItemDropLocation) { NativeCall<void, FItemNetInfo *, bool, FVector *, FRotator *, bool, bool, bool, bool>(this, "UPrimalInventoryComponent.DropItem", theInfo, bOverrideSpawnTransform, LocationOverride, RotationOverride, bPreventDropImpulse, bThrow, bSecondryAction, bSetItemDropLocation); }
DECLARE_HOOK(UPrimalInventoryComponent_DropItem, void, UPrimalInventoryComponent *, FItemNetInfo * theInfo, bool bOverrideSpawnTransform, FVector * LocationOverride, FRotator * RotationOverride, bool bPreventDropImpulse, bool bThrow, bool bSecondryAction, bool bSetItemDropLocation);
void Hook_UPrimalInventoryComponent_DropItem(UPrimalInventoryComponent *this_, FItemNetInfo * theInfo, bool bOverrideSpawnTransform, FVector * LocationOverride, FRotator * RotationOverride, bool bPreventDropImpulse, bool bThrow, bool bSecondryAction, bool bSetItemDropLocation) {

	_l(SN_Inventory, "");
	_l(SN_Inventory, "[*] Inventory.DropItem ----- before -----");
	_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

	UPrimalInventoryComponent_DropItem_original(this_, theInfo, bOverrideSpawnTransform, LocationOverride, RotationOverride, bPreventDropImpulse, bThrow,
		bSecondryAction, bSetItemDropLocation);

	_l(SN_Inventory, "");
	_l(SN_Inventory, "[*] Inventory.DropItem ----- after -----");
	_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

	// Log::GetLog()->info("UPrimalInventoryComponent_DropItem");

}

DECLARE_HOOK(UPrimalInventoryComponent_ServerEquipItem, bool, UPrimalInventoryComponent *, FItemNetID * itemID);
bool Hook_UPrimalInventoryComponent_ServerEquipItem(UPrimalInventoryComponent *this_, FItemNetID * itemID) {
	auto r = UPrimalInventoryComponent_ServerEquipItem_original(this_, itemID);
	// Log::GetLog()->info("UPrimalInventoryComponent_ServerEquipItem");

	return r;
}

// void ServerRemoveItemFromSlot_Implementation(FItemNetID ItemID) { NativeCall<void, FItemNetID>(this, "UPrimalInventoryComponent.ServerRemoveItemFromSlot_Implementation", ItemID); }
DECLARE_HOOK(UPrimalInventoryComponent_ServerRemoveItemFromSlot_Implementation, void, UPrimalInventoryComponent *, FItemNetID ItemID);
void Hook_UPrimalInventoryComponent_ServerRemoveItemFromSlot_Implementation(UPrimalInventoryComponent *this_, FItemNetID ItemID) {
	
	ObjectTypes ownerClassType = ot_unknown;
	unsigned long long _pid = 0;
	OnlinePlayerInfo *oi = 0;
	AShooterCharacter *_player = 0;

	auto owner_ = this_->GetOwner();
	if (owner_) ownerClassType = __getClassType(owner_);
	if (ownerClassType == ot_AShooterCharacter) {
		_player = static_cast<AShooterCharacter *>(owner_);
		_pid = _player->LinkedPlayerDataIDField();
		if (onlinePlayers) oi = onlinePlayers->get(_pid);
	}

	_l(SN_Inventory, "");
	_l(SN_Inventory, "[*] Inventory.RemoveFromSlot ----- before -----");
	_l_object_type_info(owner_, ownerClassType);
	_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

	UPrimalInventoryComponent_ServerRemoveItemFromSlot_Implementation_original(this_, ItemID);
	// Log::GetLog()->info("UPrimalInventoryComponent_ServerRemoveItemFromSlot_Implementation");

	_l(SN_Inventory, "");
	_l(SN_Inventory, "[*] Inventory.RemoveFromSlot ----- after -----");
	_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

}

// void ServerAddItemToSlot_Implementation(FItemNetID ItemID, int SlotIndex) { NativeCall<void, FItemNetID, int>(this, "UPrimalInventoryComponent.ServerAddItemToSlot_Implementation", ItemID, SlotIndex); }
DECLARE_HOOK(UPrimalInventoryComponent_ServerAddItemToSlot_Implementation, void, UPrimalInventoryComponent *, FItemNetID ItemID, int SlotIndex);
void Hook_UPrimalInventoryComponent_ServerAddItemToSlot_Implementation(UPrimalInventoryComponent *this_, FItemNetID ItemID, int SlotIndex) {

	ObjectTypes ownerClassType = ot_unknown;
	unsigned long long _pid = 0;
	OnlinePlayerInfo *oi = 0;
	AShooterCharacter *_player = 0;

	auto owner_ = this_->GetOwner();
	if (owner_) ownerClassType = __getClassType(owner_);
	if (ownerClassType == ot_AShooterCharacter) {
		_player = static_cast<AShooterCharacter *>(owner_);
		_pid = _player->LinkedPlayerDataIDField();
		if (onlinePlayers) oi = onlinePlayers->get(_pid);
	}
	_l(SN_Inventory, "");
	_l(SN_Inventory, "[*] Inventory.AddToSlot ----- before -----");
	_l_object_type_info(owner_, ownerClassType);
	_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

	UPrimalInventoryComponent_ServerAddItemToSlot_Implementation_original(this_, ItemID, SlotIndex);
	// Log::GetLog()->info("UPrimalInventoryComponent_ServerAddItemToSlot_Implementation");

	_l(SN_Inventory, "");
	_l(SN_Inventory, "[*] Inventory.AddToSlot ----- after -----");
	_l(SN_Inventory, "[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

}
*/

// void ServerSplitItemStack_Implementation(FItemNetID ItemID, int AmountToSplit) { NativeCall<void, FItemNetID, int>(this, "UPrimalInventoryComponent.ServerSplitItemStack_Implementation", ItemID, AmountToSplit); }
DECLARE_HOOK(UPrimalInventoryComponent_ServerSplitItemStack_Implementation, void, UPrimalInventoryComponent *, FItemNetID ItemID, int AmountToSplit);
void Hook_UPrimalInventoryComponent_ServerSplitItemStack_Implementation(UPrimalInventoryComponent *this_, FItemNetID ItemID, int AmountToSplit) {

	ObjectTypes ownerClassType = ot_unknown;
	auto owner_ = this_->GetOwner();
	if (owner_) {
		ownerClassType = __getClassType(owner_);
	}

	unsigned long long _pid = 0;
	OnlinePlayerInfo *oi = 0;
	AShooterCharacter *_player = 0;
	if (ownerClassType == ot_AShooterCharacter) {
		_player = static_cast<AShooterCharacter *>(owner_);
		_pid = _player->LinkedPlayerDataIDField();
		if (onlinePlayers) {
			oi = onlinePlayers->get(_pid);
			oi->skipAddRemoveUpdateItems = true;
		}
	}

	UPrimalInventoryComponent_ServerSplitItemStack_Implementation_original(this_, ItemID, AmountToSplit);

	if (oi) oi->skipAddRemoveUpdateItems = false;
}

// void ServerForceMergeItemStack_Implementation(FItemNetID Item1ID, FItemNetID Item2ID) { NativeCall<void, FItemNetID, FItemNetID>(this, "UPrimalInventoryComponent.ServerForceMergeItemStack_Implementation", Item1ID, Item2ID); }
DECLARE_HOOK(UPrimalInventoryComponent_ServerForceMergeItemStack_Implementation, void, UPrimalInventoryComponent *, FItemNetID Item1ID, FItemNetID Item2ID);
void Hook_UPrimalInventoryComponent_ServerForceMergeItemStack_Implementation(UPrimalInventoryComponent *this_, FItemNetID Item1ID, FItemNetID Item2ID) {

	ObjectTypes ownerClassType = ot_unknown;
	auto owner_ = this_->GetOwner();
	if (owner_) {
		ownerClassType = __getClassType(owner_);
	}

	unsigned long long _pid = 0;
	OnlinePlayerInfo *oi = 0;
	AShooterCharacter *_player = 0;
	if (ownerClassType == ot_AShooterCharacter) {
		_player = static_cast<AShooterCharacter *>(owner_);
		_pid = _player->LinkedPlayerDataIDField();
		if (onlinePlayers) {
			oi = onlinePlayers->get(_pid);
			oi->skipAddRemoveUpdateItems = true;
		}
	}

	UPrimalInventoryComponent_ServerForceMergeItemStack_Implementation_original(this_, Item1ID, Item2ID);

	if (oi) oi->skipAddRemoveUpdateItems = false;
}


void __add_engram(UPrimalItem * anItem) {
	if (auto i = anItem->OwnerInventoryField().Get()) {
		auto player = i->GetOwnerController();
		if (auto player = i->GetOwnerController()) {
			auto id = player->LinkedPlayerIDField();
			if (id && anItem->bIsEngram().Get()) {
				FString name = _bp(anItem);
				_msg_m_player_learn_engram(id, name.ToString());
			}
		}
	}
}

DECLARE_HOOK(APrimalCharacter_NotifyItemAdded, void, APrimalCharacter *, UPrimalItem *, bool);
void Hook_APrimalCharacter_NotifyItemAdded(APrimalCharacter *this_, UPrimalItem * anItem, bool bEquipItem) {
	APrimalCharacter_NotifyItemAdded_original(this_, anItem, bEquipItem);
	__add_engram(anItem);
}

DECLARE_HOOK(APrimalCharacter_NotifyItemQuantityUpdated, void, APrimalCharacter *, UPrimalItem *, int);
void Hook_APrimalCharacter_NotifyItemQuantityUpdated(APrimalCharacter *this_, UPrimalItem * anItem, int amount) {
	APrimalCharacter_NotifyItemQuantityUpdated_original(this_, anItem, amount);
	__add_engram(anItem);
}

bool M_inventory_and_harvest_init() {

	__register_hook(UPrimalInventoryComponent, AddItem);
	__register_hook(UPrimalInventoryComponent, NotifyItemQuantityUpdated);
	__register_hook(UPrimalInventoryComponent, RemoveItem);
	
	__register_hook(AShooterPlayerController, ServerTransferFromRemoteInventory_Implementation);
	__register_hook(AShooterPlayerController, ServerTransferAllFromRemoteInventory_Implementation);
	

	__register_hook(UPrimalInventoryComponent, ServerForceMergeItemStack_Implementation);
	__register_hook(UPrimalInventoryComponent, ServerSplitItemStack_Implementation);

	/*
	__register_hook(UPrimalInventoryComponent, DropItem);
	__register_hook(UPrimalInventoryComponent, ServerEquipItem);
	__register_hook(UPrimalInventoryComponent, ServerRemoveItemFromSlot_Implementation);
	__register_hook(UPrimalInventoryComponent, ServerAddItemToSlot_Implementation);
	*/

	
	// engram

	__register_hook(APrimalCharacter, NotifyItemAdded);
	__register_hook(APrimalCharacter, NotifyItemQuantityUpdated);

	return true;
}

bool M_inventory_and_harvest_done() {

	__unregister_hook(UPrimalInventoryComponent, AddItem);
	__unregister_hook(UPrimalInventoryComponent, NotifyItemQuantityUpdated);
	__unregister_hook(UPrimalInventoryComponent, RemoveItem);

	__unregister_hook(AShooterPlayerController, ServerTransferFromRemoteInventory_Implementation);
	__unregister_hook(AShooterPlayerController, ServerTransferAllFromRemoteInventory_Implementation);

	__unregister_hook(UPrimalInventoryComponent, ServerForceMergeItemStack_Implementation);
	__unregister_hook(UPrimalInventoryComponent, ServerSplitItemStack_Implementation);
	
	/*
	__unregister_hook(UPrimalInventoryComponent, DropItem);
	__unregister_hook(UPrimalInventoryComponent, ServerEquipItem);
	__unregister_hook(UPrimalInventoryComponent, ServerRemoveItemFromSlot_Implementation);
	__unregister_hook(UPrimalInventoryComponent, ServerAddItemToSlot_Implementation);
	*/

	
	
	__unregister_hook(APrimalCharacter, NotifyItemAdded);
	__unregister_hook(APrimalCharacter, NotifyItemQuantityUpdated);

	return true;
}

