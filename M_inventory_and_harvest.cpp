#include "M_inventory_and_harvest.h"
#include <API/ARK/Ark.h>
#include "Utils.h"
#include "M_hook_helper.h"
#include "M_harvest_marker.h"
#include "M_online_players.h"
#include "Messages.h"


/*


VERSION: 285.104

ExtendedEventLog.dll!Hook_UPrimalInventoryComponent_AddItem() (0x00007ffd6ad4d663) + 0 bytes [d:\zh\myarkplugins\extendedeventlog\extendedeventlog.cpp:1445]
ShooterGameServer.exe!UPrimalHarvestingComponent::GiveHarvestResource() (0x00007ff7e97067ea) + 69 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalharvestingcomponent.cpp:870]
ShooterGameServer.exe!FAttachedInstancedHarvestingElement::ApplyHarvestDamageEntryDamage() (0x00007ff7e970362f) + 0 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalharvestingcomponent.cpp:487]
ShooterGameServer.exe!FAttachedInstancedHarvestingElement::ReceiveComponentDamage() (0x00007ff7e97028b2) + 0 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalharvestingcomponent.cpp:364]
ShooterGameServer.exe!UPrimalHarvestingComponent::TryMultiUse() (0x00007ff7e9706f88) + 0 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalharvestingcomponent.cpp:986]
ShooterGameServer.exe!FAttachedInstancedHarvestingElement::TryMultiUse() (0x00007ff7e9700671) + 0 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalharvestingcomponent.cpp:60]
ShooterGameServer.exe!UInstancedStaticMeshComponent::TryMultiUse() (0x00007ff7ea914389) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\instancedstaticmesh.cpp:1777]
ShooterGameServer.exe!AInstancedFoliageActor::TryMultiUse() (0x00007ff7ea90cd67) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\instancedfoliage.cpp:868]
*/

// UPrimalItem * AddItem(FItemNetInfo * theItemInfo, bool bEquipItem, bool AddToSlot, bool bDontStack, FItemNetID * InventoryInsertAfterItemID, bool ShowHUDNotification, bool bDontRecalcSpoilingTime, bool bForceIncompleteStacking, AShooterCharacter * OwnerPlayer, bool bIgnoreAbsoluteMaxInventory) { return NativeCall<UPrimalItem *, FItemNetInfo *, bool, bool, bool, FItemNetID *, bool, bool, bool, AShooterCharacter *, bool>(this, "UPrimalInventoryComponent.AddItem", theItemInfo, bEquipItem, AddToSlot, bDontStack, InventoryInsertAfterItemID, ShowHUDNotification, bDontRecalcSpoilingTime, bForceIncompleteStacking, OwnerPlayer, bIgnoreAbsoluteMaxInventory); }
DECLARE_HOOK(UPrimalInventoryComponent_AddItem, UPrimalItem *, UPrimalInventoryComponent *, FItemNetInfo * theItemInfo, bool bEquipItem, bool AddToSlot, bool bDontStack, FItemNetID * InventoryInsertAfterItemID, bool ShowHUDNotification, bool bDontRecalcSpoilingTime, bool bForceIncompleteStacking, AShooterCharacter * OwnerPlayer, bool bIgnoreAbsoluteMaxInventory);
UPrimalItem *Hook_UPrimalInventoryComponent_AddItem(UPrimalInventoryComponent *this_, FItemNetInfo * theItemInfo, bool bEquipItem, bool AddToSlot,
	bool bDontStack, FItemNetID * InventoryInsertAfterItemID, bool ShowHUDNotification, bool bDontRecalcSpoilingTime, bool bForceIncompleteStacking,
	AShooterCharacter * OwnerPlayer, bool bIgnoreAbsoluteMaxInventory) {


	// owner type == AShooterCharacter && lid == 0 -> setup harvest marker
	// check harvest marker in update quantity -> harvest message


	auto iid = theItemInfo->ItemIDField();

	unsigned long long lid = 0;
	if (theItemInfo->LastOwnerPlayerField().Get()) {
		lid = theItemInfo->LastOwnerPlayerField()->LinkedPlayerDataIDField();
	}

	// theItemInfo->

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
		oi = onlinePlayers->get(_pid);
	}

	if (lid == 0 && ownerClassType == ot_AShooterCharacter) {
		__add_harvest_marker(iid.ItemID1, iid.ItemID2);
		Log::GetLog()->info("add harvest check iid {}:{}", iid.ItemID1, iid.ItemID2);
	}




	auto oid = theItemInfo->OwnerPlayerDataIdField();
	auto q = theItemInfo->ItemQuantityField();
	unsigned int startQ = q;

	/*
	if (lid == 0 && _pid > 0 && q > 0) {
		char *a = 0;
		*a = 1;
	}
	*/

	Log::GetLog()->info("");
	Log::GetLog()->info("[*] Inventory.AddItem ----- before -----");
	// Log::GetLog()->info("equiped {} addslot {} dontstack {} bForceIncompleteStacking {} bIgnoreAbsoluteMaxInventory {}", bEquipItem, AddToSlot, bDontStack, bForceIncompleteStacking, bIgnoreAbsoluteMaxInventory);
	Log::GetLog()->info("oid {} lid {} q {} bIsEquipped {} bIsSlot {}", oid, lid, q, theItemInfo->bIsEquipped().Get(), theItemInfo->bIsSlot().Get());

	auto owner = this_->GetOwner();
	if (owner) {
		FString ownerType = __getClassName(owner);
		Log::GetLog()->info("ownerType {}", ownerType.ToString());

		UPrimalItem *oitem = 0;

		if (ownerType == FString("AShooterCharacter")) {
			AShooterCharacter *player = static_cast<AShooterCharacter *>(owner);
			auto pid = player->LinkedPlayerDataIDField();
			Log::GetLog()->info("AShooterCharacter id {}", pid);
			auto i = player->MyInventoryComponentField();
			if (i) {
				oitem = i->FindItem(&iid, false, true, 0);
			}
		}

		if (oitem) {
			FString name = _bp(oitem);
			Log::GetLog()->info("name {}", name.ToString());
		}
	}

	// auto n1 = theItemInfo->CustomItemNameField();
	// auto n2 = theItemInfo->CustomItemDescriptionField();

	// UPrimalItem *ii = (UPrimalItem *)theItemInfo->ItemArchetypeField().uClass;
	// theItemInfo

	// Log::GetLog()->info("iid {}:{} {}", iid.ItemID1, iid.ItemID2, _bp(ii).ToString());
	Log::GetLog()->info("iid {}:{}", iid.ItemID1, iid.ItemID2);

	int idx;
	auto item = this_->FindItem(&iid, false, true, &idx);
	if (item) {
		Log::GetLog()->info("inventiry exist not equiped idx {} slot {}", idx, item->SlotIndexField());
	}
	item = this_->FindItem(&iid, true, false, &idx);
	if (item) {
		Log::GetLog()->info("inventiry exist equiped idx {} slot {}", idx, item->SlotIndexField());
	}


	// auto pid = this_->LinkedPlayerIDField();
	// auto c = this_->CharacterField();

	if (OwnerPlayer) {
		auto pid = OwnerPlayer->LinkedPlayerDataIDField();
		Log::GetLog()->info("ownerPlayerId {}", pid);
	}

	Log::GetLog()->info("[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

	if (oi) {
		oi->lastUpdatedItem = 0;
	}
	auto r = UPrimalInventoryComponent_AddItem_original(this_, theItemInfo, bEquipItem, AddToSlot, bDontStack, InventoryInsertAfterItemID, ShowHUDNotification,
		bDontRecalcSpoilingTime, bForceIncompleteStacking, OwnerPlayer, bIgnoreAbsoluteMaxInventory);

	// UPrimalItem *oitem = 0;
	{
		q = theItemInfo->ItemQuantityField();

		Log::GetLog()->info("");
		Log::GetLog()->info("[*] Inventory.AddItem ----- after -----");
		Log::GetLog()->info("oid {} lid {} q {}", q, oid, lid);

		auto owner = this_->GetOwner();
		if (owner) {
			auto ownerType = __getClassName(owner);
			Log::GetLog()->info("ownerType {}", ownerType.ToString());
			UPrimalItem *oitem = 0;

			if (ownerType == FString("AShooterCharacter")) {
				AShooterCharacter *player = static_cast<AShooterCharacter *>(owner);
				auto pid = player->LinkedPlayerDataIDField();
				Log::GetLog()->info("AShooterCharacter id {}", pid);
				auto i = player->MyInventoryComponentField();
				if (i) {
					oitem = i->FindItem(&iid, false, true, 0);
				}
			}

			if (oitem) {
				FString name = _bp(oitem);
				Log::GetLog()->info("name {}", name.ToString());
			}
		}

		Log::GetLog()->info("iid {}:{}", iid.ItemID1, iid.ItemID2);

		item = this_->FindItem(&iid, false, true, &idx);
		if (item) {
			Log::GetLog()->info("inventiry exist not equiped idx {} slot {} q {}", idx, item->SlotIndexField(), item->GetItemQuantity());
		}

		item = this_->FindItem(&iid, true, false, &idx);
		if (item) {
			Log::GetLog()->info("inventiry exist equiped idx {} slot {}", idx, item->SlotIndexField());
		}


		Log::GetLog()->info("[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	}

	if (ownerClassType == ot_AShooterCharacter) {

		auto i = _player->MyInventoryComponentField();
		if (i) {
			auto item = i->FindItem(&iid, false, true, 0);
			if (item) {
				auto isengram = item->bIsEngram().Get();
				if (!isengram) {

					if (oi && !oi->skipAddRemoveUpdateItems && item && oi->lastUpdatedItem == 0 &&
						((item->MaxItemQuantityField() == 1) ||
						(lid == _pid)
							)
						) {
						auto slot = item->SlotIndexField();

						if (slot == -1) {

							auto isbp = item->bIsBlueprint().Get();
							unsigned int qua = 0;
							qua = item->ItemQualityIndexField();
							auto q = item->GetItemQuantity();

							/*
							auto info = makeItemInfo(item);

							Log::GetLog()->info("");
							Log::GetLog()->info("[*] Add item pid {} q {} name {}", _pid, q, info->name.ToString());
							Log::GetLog()->info("");


							oi->addItemToInventory(makeItemId(iid.ItemID1, iid.ItemID2));
							// oi->__add_add_item_marker(iid.ItemID1, iid.ItemID2);

							_msg_m_player_update_inventory_with_quality(_pid, info->name.ToString(), isbp ? 1 : 0, __formatText(info->stats), qua,
								__formatText(info->req), q);
							delete info;
							*/

						}
					}


					if (oi) {
						Log::GetLog()->info("harvest check iid {}:{} amount {} pid {} skip {}", iid.ItemID1, iid.ItemID2, startQ, _pid, oi->skipAddRemoveUpdateItems);
					}

					if (oi && !oi->skipAddRemoveUpdateItems && lid == 0 && startQ > 0 && __get_harvest_marker(iid.ItemID1, iid.ItemID2)) {
						startQ++;

						auto item = oi->lastUpdatedItem;
						if (item) {

							auto name = _bp(item);

							_msg_m_player_update_inventory(_pid, name.ToString(), startQ, 1, 0);

							Log::GetLog()->info("");
							Log::GetLog()->info("[*] Harvest pid {} q {} name {}", _pid, startQ, name.ToString());
							Log::GetLog()->info("");

							oi->lastUpdatedItem = 0;
						}

					}
				}
			}
		}
	}

	// MaxItemQuantityField

	return r;
}

// void NotifyItemQuantityUpdated(UPrimalItem * anItem, int amount) { NativeCall<void, UPrimalItem *, int>(this, "UPrimalInventoryComponent.NotifyItemQuantityUpdated", anItem, amount); }
DECLARE_HOOK(UPrimalInventoryComponent_NotifyItemQuantityUpdated, void, UPrimalInventoryComponent *, UPrimalItem * anItem, int amount);
void Hook_UPrimalInventoryComponent_NotifyItemQuantityUpdated(UPrimalInventoryComponent *this_, UPrimalItem * anItem, int amount) {

	Log::GetLog()->info("");
	Log::GetLog()->info("[*] Inventory.NotifyItemQuantityUpdated ----- before -----");

	// auto oid = theItemInfo->OwnerPlayerDataIdField();
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
	if (ownerClassType == ot_AShooterCharacter) {
		AShooterCharacter *player = static_cast<AShooterCharacter *>(owner_);
		pid = player->LinkedPlayerDataIDField();
		oi = onlinePlayers->get(pid);
	}

	Log::GetLog()->info("q {} a {} iid {}:{} pid {} lid {}", anItem->GetItemQuantity(), amount, anItem->ItemIDField().ItemID1, anItem->ItemIDField().ItemID2, pid, lid);

	Log::GetLog()->info("[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

	auto isengram = anItem->bIsEngram().Get();
	if (oi) {
		Log::GetLog()->info("harvest check iid {}:{} {} amount {} pid {} skip {}", iid.ItemID1, iid.ItemID2, isengram, amount, pid, oi->skipAddRemoveUpdateItems);
	}

	if (!isengram && amount > 0 && pid > 0) {
		if (!oi->skipAddRemoveUpdateItems) {

			auto hm = __get_harvest_marker(iid.ItemID1, iid.ItemID2);
			if (hm) {
				auto name = _bp(anItem);

				_msg_m_player_update_inventory(pid, name.ToString(), amount, 1, 0);

				Log::GetLog()->info("");
				Log::GetLog()->info("[*] Harvest pid {} q {} name {}", pid, amount, name.ToString());
				Log::GetLog()->info("");
			}
		}
	}

	if (pid && oi) {
		if (!isengram && !oi->skipAddRemoveUpdateItems) {
			oi->lastUpdatedItem = anItem;

			auto slot = anItem->SlotIndexField();
			auto isengram = anItem->bIsEngram().Get();
			auto _iid = makeItemId(iid.ItemID1, iid.ItemID2);

			bool process = false;

			if (!isengram) {
				if (slot == -1) { // && !oi->itemInInventory(_iid)) {

					process = true;

				}
				else if (slot == -1 && amount < 0) {
					process = true;
				}
			}

			if (process) {
				auto isbp = anItem->bIsBlueprint().Get();
				unsigned int qua = 0;
				qua = anItem->ItemQualityIndexField();
				auto q = anItem->GetItemQuantity();

				auto info = makeItemInfo(anItem);

				Log::GetLog()->info("");
				Log::GetLog()->info("[*] Add item pid {} q {} name {}", pid, amount, info->name.ToString());
				Log::GetLog()->info("");


				_msg_m_player_update_inventory_with_quality(pid, info->name.ToString(), isbp ? 1 : 0, __formatText(info->stats), qua,
					__formatText(info->req), amount);

				delete info;

			}
		}
	}

	UPrimalInventoryComponent_NotifyItemQuantityUpdated_original(this_, anItem, amount);

	Log::GetLog()->info("[*] Inventory.NotifyItemQuantityUpdated ----- after -----");

	Log::GetLog()->info("[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
}

/*






VERSION: 285.104

ExtendedEventLog.dll!Hook_UPrimalInventoryComponent_RemoveItem() (0x00007ffd6ad4ef83) + 0 bytes [d:\zh\myarkplugins\extendedeventlog\extendedeventlog.cpp:1767]
ShooterGameServer.exe!UPrimalItem::IncrementItemQuantity() (0x00007ff7e9743034) + 0 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalitem.cpp:3486]
ShooterGameServer.exe!UPrimalInventoryComponent::IncrementItemTemplateQuantity() (0x00007ff7e9717beb) + 34 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalinventorycomponent.cpp:2729]
ShooterGameServer.exe!UPrimalInventoryComponent::ServerForceMergeItemStack_Implementation() (0x00007ff7e971c3bd) + 0 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalinventorycomponent.cpp:3698]
ShooterGameServer.exe!UPrimalInventoryComponent::execServerForceMergeItemStack() (0x00007ff7e9f997f6) + 310 bytes [f:\build\extinction\projects\shootergame\source\shootergame\classes\primalinventorycomponent.h:139]

*/
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

	/*
	UPrimalItem *_item1 = 0, *_item2 = 0;
	int q1 = 0, q2 = 0;
	if (_pid > 0) {
		auto i = _player->MyInventoryComponentField();
		if (i) {
			_item1 = i->FindItem(&Item1ID, false, true, 0);
			q1 = _item1 ? _item1->GetItemQuantity() : 0;
			_item2 = i->FindItem(&Item2ID, false, true, 0);
			q2 = _item2 ? _item2->GetItemQuantity() : 0;
		}
	}
	*/


	UPrimalInventoryComponent_ServerForceMergeItemStack_Implementation_original(this_, Item1ID, Item2ID);

	if (oi) {
		oi->skipAddRemoveUpdateItems = false;
	}

	/*
	if (_pid > 0) {
		int q1_ = 0, q2_ = 0;
		auto i = _player->MyInventoryComponentField();
		if (i) {
			_item1 = i->FindItem(&Item1ID, false, true, 0);
			q1_ = _item1 ? _item1->GetItemQuantity() : 0;
			_item2 = i->FindItem(&Item2ID, false, true, 0);
			q2_ = _item2 ? _item2->GetItemQuantity() : 0;

			Log::GetLog()->info("[*] merge {}-{} {}-{}", q1, q1_, q2, q2_);
		}

		oi->mergeItemStack = false;
	}
	*/
}

/*
Fatal error!


ShooterGameServer.exe!AShooterPlayerController::ServerTransferFromRemoteInventory_Implementation() (0x00007ff7e99920b0) + 42 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\shooterplayercontroller.cpp:4319]
take items from another inventory


VERSION: 285.104
ExtendedEventLog.dll!Hook_UPrimalInventoryComponent_RemoveItem() (0x00007ffd62eded7f) + 0 bytes [d:\zh\myarkplugins\extendedeventlog\extendedeventlog.cpp:1798]
ShooterGameServer.exe!UPrimalItem::IncrementItemQuantity() (0x00007ff7e9743034) + 0 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalitem.cpp:3486]
ShooterGameServer.exe!UPrimalInventoryComponent::IncrementItemTemplateQuantity() (0x00007ff7e9717beb) + 34 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\primalinventorycomponent.cpp:2729]
ShooterGameServer.exe!AShooterPlayerController::ServerTransferFromRemoteInventory_Implementation() (0x00007ff7e99920b0) + 42 bytes [f:\build\extinction\projects\shootergame\source\shootergame\private\shooterplayercontroller.cpp:4319]
ShooterGameServer.exe!AShooterPlayerController::execServerTransferFromRemoteInventory() (0x00007ff7e9f7d5ef) + 735 bytes [f:\build\extinction\projects\shootergame\source\shootergame\classes\shooterplayercontroller.h:113]
ShooterGameServer.exe!UFunction::Invoke() (0x00007ff7ea345f4f) + 6 bytes [f:\build\extinction\engine\source\runtime\coreuobject\private\uobject\class.cpp:3801]
ShooterGameServer.exe!UObject::ProcessEvent() (0x00007ff7ea3869fc) + 0 bytes [f:\build\extinction\engine\source\runtime\coreuobject\private\uobject\scriptcore.cpp:931]
ShooterGameServer.exe!AActor::ProcessEvent() (0x00007ff7ea7f4342) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\actor.cpp:627]
ShooterGameServer.exe!FObjectReplicator::ReceivedBunch_WrappedNet() (0x00007ff7ea88c6ed) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\datareplication.cpp:989]
ShooterGameServer.exe!UActorChannel::ProcessBunch() (0x00007ff7ea881c62) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\datachannel.cpp:1763]
ShooterGameServer.exe!UChannel::ReceivedNextBunch() (0x00007ff7ea87e418) + 21 bytes [f:\build\extinction\engine\source\runtime\engine\private\datachannel.cpp:533]
ShooterGameServer.exe!UChannel::ReceivedRawBunch() (0x00007ff7ea87dfdb) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\datachannel.cpp:330]
ShooterGameServer.exe!UNetConnection::ReceivedPacket() (0x00007ff7ea986eed) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\netconnection.cpp:1164]
ShooterGameServer.exe!UNetConnection::ReceivedRawPacket() (0x00007ff7ea985db3) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\netconnection.cpp:621]
ShooterGameServer.exe!UIpNetDriver::TickDispatch() (0x00007ff7ea7be0d4) + 24 bytes [f:\build\extinction\engine\source\runtime\online\onlinesubsystemutils\private\ipnetdriver.cpp:454]
ShooterGameServer.exe!TBaseUObjectMethodDelegateInstance_OneParam<APrimalCharacter,void,float>::ExecuteIfSafe() (0x00007ff7e964f7c7) + 0 bytes [f:\build\extinction\engine\source\runtime\core\public\delegates\delegateinstancesimpl.inl:533]
ShooterGameServer.exe!TBaseMulticastDelegate_OneParam<void,float>::Broadcast() (0x00007ff7ea969939) + 14 bytes [f:\build\extinction\engine\source\runtime\core\public\delegates\delegatesignatureimpl.inl:1837]
ShooterGameServer.exe!UWorld::Tick() (0x00007ff7ea961d73) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\leveltick.cpp:1095]
ShooterGameServer.exe!UGameEngine::Tick() (0x00007ff7ea8c355f) + 0 bytes [f:\build\extinction\engine\source\runtime\engine\private\gameengine.cpp:1170]
ShooterGameServer.exe!FEngineLoop::Tick() (0x00007ff7e953c20c) + 0 bytes [f:\build\extinction\engine\source\runtime\launch\private\launchengineloop.cpp:2421]
ShooterGameServer.exe!GuardedMain() (0x00007ff7e9537b7c) + 0 bytes [f:\build\extinction\engine\source\runtime\launch\private\launch.cpp:140]
ShooterGameServer.exe!GuardedMainWrapper() (0x00007ff7e953d25a) + 5 bytes [f:\build\extinction\engine\source\runtime\launch\private\windows\launchwindows.cpp:125]
ShooterGameServer.exe!WinMain() (0x00007ff7e953d390) + 8 bytes [f:\build\extinction\engine\source\runtime\launch\private\windows\launchwindows.cpp:209]
ShooterGameServer.exe!__tmainCRTStartup() (0x00007ff7eb8ce401) + 21 bytes [f:\dd\vctools\crt\crtw32\dllstuff\crtexe.c:618]
KERNEL32.DLL!UnknownFunction (0x00007ffd91883034) + 0 bytes [UnknownFile:0]
ntdll.dll!UnknownFunction (0x00007ffd93421461) + 0 bytes [UnknownFile:0]
ntdll.dll!UnknownFunction (0x00007ffd93421461) + 0 bytes [UnknownFile:0]
*/
// bool RemoveItem(FItemNetID * itemID, bool bDoDrop, bool bSecondryAction, bool bForceRemoval, bool showHUDMessage) { return NativeCall<bool, FItemNetID *, bool, bool, bool, bool>(this, "UPrimalInventoryComponent.RemoveItem", itemID, bDoDrop, bSecondryAction, bForceRemoval, showHUDMessage); }
DECLARE_HOOK(UPrimalInventoryComponent_RemoveItem, bool, UPrimalInventoryComponent *, FItemNetID * itemID, bool bDoDrop, bool bSecondryAction, bool bForceRemoval, bool showHUDMessage);
bool Hook_UPrimalInventoryComponent_RemoveItem(UPrimalInventoryComponent *this_, FItemNetID * itemID, bool bDoDrop, bool bSecondryAction, bool bForceRemoval,
	bool showHUDMessage) {

	// auto iid = theItemInfo->ItemIDField();


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
		}
	}

	if (oi && !oi->skipAddRemoveUpdateItems) {
		UPrimalItem *_item = 0;
		if (_pid > 0) {
			auto i = _player->MyInventoryComponentField();
			if (i) {
				_item = i->FindItem(itemID, false, true, 0);
			}
		}

		if (_pid > 0 && _item) {
			if (_item->SlotIndexField() == -1) {
				auto amount = _item->GetItemQuantity();
				if (amount != 0) {
					auto _iid = makeItemId(itemID->ItemID1, itemID->ItemID2);
					oi->delItemFromInventory(_iid);

					auto info = makeItemInfo(_item);
					amount *= -1;
					auto isbp = _item->bIsBlueprint().Get();
					unsigned int qua = 0;
					qua = _item->ItemQualityIndexField();

					Log::GetLog()->info("");
					Log::GetLog()->info("[*] Del item pid {} q {} name {}", _pid, amount, info->name.ToString());
					Log::GetLog()->info("");


					_msg_m_player_update_inventory_with_quality(_pid, info->name.ToString(), isbp ? 1 : 0, __formatText(info->stats), qua,
						__formatText(info->req), amount);

					delete info;
				}
				/*
				else {
					char *a = 0;
					*a = 1;
				}
				*/
			}
		}
	}

	Log::GetLog()->info("");
	Log::GetLog()->info("[*] Inventory.RemoveItem ----- before -----");

	auto owner = this_->GetOwner();
	if (owner) {
		auto ownerType = __getClassName(owner);
		Log::GetLog()->info("ownerType {}", ownerType.ToString());
		UPrimalItem *oitem = 0;

		if (ownerType == FString("AShooterCharacter")) {
			AShooterCharacter *player = static_cast<AShooterCharacter *>(owner);
			auto pid = player->LinkedPlayerDataIDField();
			Log::GetLog()->info("AShooterCharacter id {}", pid);
			auto i = player->MyInventoryComponentField();
			if (i) {
				oitem = i->FindItem(itemID, false, true, 0);
			}
		}

		if (oitem) {
			FString name = _bp(oitem);
			Log::GetLog()->info("name {}", name.ToString());
		}
	}

	Log::GetLog()->info("[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

	auto r = UPrimalInventoryComponent_RemoveItem_original(this_, itemID, bDoDrop, bSecondryAction, bForceRemoval, showHUDMessage);

	Log::GetLog()->info("");
	Log::GetLog()->info("[*] Inventory.RemoveItem ----- after -----");

	owner = this_->GetOwner();
	if (owner) {
		auto ownerType = __getClassName(owner);
		Log::GetLog()->info("ownerType {}", ownerType.ToString());
		UPrimalItem *oitem = 0;

		if (ownerType == FString("AShooterCharacter")) {
			AShooterCharacter *player = static_cast<AShooterCharacter *>(owner);
			auto pid = player->LinkedPlayerDataIDField();
			Log::GetLog()->info("AShooterCharacter id {}", pid);
			auto i = player->MyInventoryComponentField();
			if (i) {
				oitem = i->FindItem(itemID, false, true, 0);
			}
		}

		if (oitem) {
			FString name = _bp(oitem);
			Log::GetLog()->info("name {}", name.ToString());
		}
	}

	Log::GetLog()->info("[*] ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

	return r;
}

// void DropItem(FItemNetInfo * theInfo, bool bOverrideSpawnTransform, FVector * LocationOverride, FRotator * RotationOverride, bool bPreventDropImpulse, bool bThrow, bool bSecondryAction, bool bSetItemDropLocation) { NativeCall<void, FItemNetInfo *, bool, FVector *, FRotator *, bool, bool, bool, bool>(this, "UPrimalInventoryComponent.DropItem", theInfo, bOverrideSpawnTransform, LocationOverride, RotationOverride, bPreventDropImpulse, bThrow, bSecondryAction, bSetItemDropLocation); }
DECLARE_HOOK(UPrimalInventoryComponent_DropItem, void, UPrimalInventoryComponent *, FItemNetInfo * theInfo, bool bOverrideSpawnTransform, FVector * LocationOverride, FRotator * RotationOverride, bool bPreventDropImpulse, bool bThrow, bool bSecondryAction, bool bSetItemDropLocation);
void Hook_UPrimalInventoryComponent_DropItem(UPrimalInventoryComponent *this_, FItemNetInfo * theInfo, bool bOverrideSpawnTransform, FVector * LocationOverride, FRotator * RotationOverride, bool bPreventDropImpulse, bool bThrow, bool bSecondryAction, bool bSetItemDropLocation) {
	UPrimalInventoryComponent_DropItem_original(this_, theInfo, bOverrideSpawnTransform, LocationOverride, RotationOverride, bPreventDropImpulse, bThrow,
		bSecondryAction, bSetItemDropLocation);
	Log::GetLog()->info("UPrimalInventoryComponent_DropItem");

}

/*
bool ServerEquipItem(FItemNetID * itemID) { return NativeCall<bool, FItemNetID *>(this, "UPrimalInventoryComponent.ServerEquipItem", itemID); }
*/
DECLARE_HOOK(UPrimalInventoryComponent_ServerEquipItem, bool, UPrimalInventoryComponent *, FItemNetID * itemID);
bool Hook_UPrimalInventoryComponent_ServerEquipItem(UPrimalInventoryComponent *this_, FItemNetID * itemID) {
	auto r = UPrimalInventoryComponent_ServerEquipItem_original(this_, itemID);
	Log::GetLog()->info("UPrimalInventoryComponent_ServerEquipItem");

	return r;
}

// void ServerRemoveItemFromSlot_Implementation(FItemNetID ItemID) { NativeCall<void, FItemNetID>(this, "UPrimalInventoryComponent.ServerRemoveItemFromSlot_Implementation", ItemID); }
DECLARE_HOOK(UPrimalInventoryComponent_ServerRemoveItemFromSlot_Implementation, void, UPrimalInventoryComponent *, FItemNetID ItemID);
void Hook_UPrimalInventoryComponent_ServerRemoveItemFromSlot_Implementation(UPrimalInventoryComponent *this_, FItemNetID ItemID) {
	UPrimalInventoryComponent_ServerRemoveItemFromSlot_Implementation_original(this_, ItemID);
	Log::GetLog()->info("UPrimalInventoryComponent_ServerRemoveItemFromSlot_Implementation");
}

// void ServerAddItemToSlot_Implementation(FItemNetID ItemID, int SlotIndex) { NativeCall<void, FItemNetID, int>(this, "UPrimalInventoryComponent.ServerAddItemToSlot_Implementation", ItemID, SlotIndex); }
DECLARE_HOOK(UPrimalInventoryComponent_ServerAddItemToSlot_Implementation, void, UPrimalInventoryComponent *, FItemNetID ItemID, int SlotIndex);
void Hook_UPrimalInventoryComponent_ServerAddItemToSlot_Implementation(UPrimalInventoryComponent *this_, FItemNetID ItemID, int SlotIndex) {
	UPrimalInventoryComponent_ServerAddItemToSlot_Implementation_original(this_, ItemID, SlotIndex);
	Log::GetLog()->info("UPrimalInventoryComponent_ServerAddItemToSlot_Implementation");
}


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
	Log::GetLog()->info("UPrimalInventoryComponent_ServerSplitItemStack_Implementation");

	if (oi) {
		oi->skipAddRemoveUpdateItems = false;
	}
}

// void ServerTransferFromRemoteInventory_Implementation(UPrimalInventoryComponent * inventoryComp, FItemNetID itemID, int requestedQuantity, int ToSlotIndex, bool bEquipItem) { NativeCall<void, UPrimalInventoryComponent *, FItemNetID, int, int, bool>(this, "AShooterPlayerController.ServerTransferFromRemoteInventory_Implementation", inventoryComp, itemID, requestedQuantity, ToSlotIndex, bEquipItem); }
DECLARE_HOOK(AShooterPlayerController_ServerTransferFromRemoteInventory_Implementation, void, AShooterPlayerController *, UPrimalInventoryComponent * inventoryComp, FItemNetID itemID, int requestedQuantity, int ToSlotIndex, bool bEquipItem);
void Hook_AShooterPlayerController_ServerTransferFromRemoteInventory_Implementation(AShooterPlayerController *this_, 
	UPrimalInventoryComponent * inventoryComp, FItemNetID itemID, int requestedQuantity, int ToSlotIndex, bool bEquipItem) {

	Log::GetLog()->info("AShooterPlayerController.TransferFromRemoteInventory before");
	Log::GetLog()->info("{} {} {}", requestedQuantity, ToSlotIndex, bEquipItem);

	auto pid = this_->LinkedPlayerIDField();
	OnlinePlayerInfo *oi = 0;
	if (onlinePlayers) {
		oi = onlinePlayers->get(pid);
		oi->skipAddRemoveUpdateItems = true;

		if (ToSlotIndex == -1) {
			auto item = inventoryComp->FindItem(&itemID, false, true, 0);
			if (item) {
				Log::GetLog()->info("{}", item->GetItemQuantity());

				int q = 1000000 == requestedQuantity ? item->GetItemQuantity(): requestedQuantity;

				auto _iid = makeItemId(itemID.ItemID1, itemID.ItemID2);
				oi->delItemFromInventory(_iid);

				auto info = makeItemInfo(item);
				// amount *= -1;
				auto isbp = item->bIsBlueprint().Get();
				unsigned int qua = 0;
				qua = item->ItemQualityIndexField();

				Log::GetLog()->info("");
				Log::GetLog()->info("[*] Add item pid {} q {} name {}", pid, q, info->name.ToString());
				Log::GetLog()->info("");


				_msg_m_player_update_inventory_with_quality(pid, info->name.ToString(), isbp ? 1 : 0, __formatText(info->stats), qua,
					__formatText(info->req), q);

				delete info;
			}
		}
	}

	AShooterPlayerController_ServerTransferFromRemoteInventory_Implementation_original(this_,
		inventoryComp, itemID, requestedQuantity, ToSlotIndex, bEquipItem);

	if (oi) {
		oi->skipAddRemoveUpdateItems = false;
	}

	Log::GetLog()->info("AShooterPlayerController.TransferFromRemoteInventory after");
}

DECLARE_HOOK(APrimalCharacter_NotifyItemAdded, void, APrimalCharacter *, UPrimalItem *, bool);
void Hook_APrimalCharacter_NotifyItemAdded(APrimalCharacter *this_, UPrimalItem * anItem, bool bEquipItem) {

	APrimalCharacter_NotifyItemAdded_original(this_, anItem, bEquipItem);

	unsigned long long id = 0;
	auto i = anItem->OwnerInventoryField().Get();
	AShooterPlayerController *player = 0;
	if (i) {
		player = i->GetOwnerController();
		if (player) {
			id = player->LinkedPlayerIDField();
		}
	}

	if (id) {

		auto isengram = anItem->bIsEngram().Get();
		if (isengram) {
			FString name = _bp(anItem);
			Log::GetLog()->info("[*] Add Engram To Player:{} engram:{}", id, name.ToString());
			_msg_m_player_learn_engram(id, name.ToString());
		}

	}

	/*
	auto slot = anItem->SlotIndexField();

		FString name = _bp(anItem);
		unsigned long long id1 = anItem->ItemIDField().ItemID1;
		unsigned long long id2 = anItem->ItemIDField().ItemID2;
		unsigned long long iid = (id1 << 32) | id2;
		auto isbp = anItem->bIsBlueprint().Get();

		auto stc = anItem->MaxItemQuantityField();

		if (stc == 1 && !bEquipItem) {

			if (isengram) {
				// Log::GetLog()->info("[*] Add Engram To Player:{} engram:{}", id, name.ToString());
				_msg_m_player_learn_engram(id, name.ToString());
			}
		*/
}

DECLARE_HOOK(APrimalCharacter_NotifyItemQuantityUpdated, void, APrimalCharacter *, UPrimalItem *, int);
void Hook_APrimalCharacter_NotifyItemQuantityUpdated(APrimalCharacter *this_, UPrimalItem * anItem, int amount) {

	APrimalCharacter_NotifyItemQuantityUpdated_original(this_, anItem, amount);
	unsigned long long id = 0;
	auto i = anItem->OwnerInventoryField().Get();
	if (i) {
		auto player = i->GetOwnerController();
		if (player) {
			id = player->LinkedPlayerIDField();
		}
	}

	if (id) {
		auto isengram = anItem->bIsEngram().Get();

		if (isengram) {
			FString name = _bp(anItem);
			_msg_m_player_learn_engram(id, name.ToString());
			Log::GetLog()->info("[*] Add Engram To Player:{} engram:{}", id, name.ToString());
		}
	}
}

bool M_inventory_and_harvest_init() {

	__register_hook(UPrimalInventoryComponent, AddItem);
	__register_hook(UPrimalInventoryComponent, NotifyItemQuantityUpdated);
	__register_hook(UPrimalInventoryComponent, RemoveItem);
	__register_hook(UPrimalInventoryComponent, DropItem);
	__register_hook(UPrimalInventoryComponent, ServerEquipItem);
	__register_hook(UPrimalInventoryComponent, ServerRemoveItemFromSlot_Implementation);
	__register_hook(UPrimalInventoryComponent, ServerAddItemToSlot_Implementation);
	__register_hook(UPrimalInventoryComponent, ServerForceMergeItemStack_Implementation);
	__register_hook(UPrimalInventoryComponent, ServerSplitItemStack_Implementation);
	__register_hook(AShooterPlayerController, ServerTransferFromRemoteInventory_Implementation);

	__register_hook(APrimalCharacter, NotifyItemAdded);
	__register_hook(APrimalCharacter, NotifyItemQuantityUpdated);

	return true;
}

bool M_inventory_and_harvest_done() {

	__unregister_hook(UPrimalInventoryComponent, AddItem);
	__unregister_hook(UPrimalInventoryComponent, NotifyItemQuantityUpdated);
	__unregister_hook(UPrimalInventoryComponent, RemoveItem);
	__unregister_hook(UPrimalInventoryComponent, DropItem);
	__unregister_hook(UPrimalInventoryComponent, ServerEquipItem);
	__unregister_hook(UPrimalInventoryComponent, ServerRemoveItemFromSlot_Implementation);
	__unregister_hook(UPrimalInventoryComponent, ServerAddItemToSlot_Implementation);
	__unregister_hook(UPrimalInventoryComponent, ServerForceMergeItemStack_Implementation);
	__unregister_hook(UPrimalInventoryComponent, ServerSplitItemStack_Implementation);
	__unregister_hook(AShooterPlayerController, ServerTransferFromRemoteInventory_Implementation);

	__unregister_hook(APrimalCharacter, NotifyItemAdded);
	__unregister_hook(APrimalCharacter, NotifyItemQuantityUpdated);

	return true;
}

