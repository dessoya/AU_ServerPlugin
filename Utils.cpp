#include "Utils.h"

ItemInfo *makeItemInfo(UPrimalItem *item) {
	auto info = new ItemInfo;
	info->isBP = item->bIsBlueprint().Get();
	info->name = _bp(item);
	info->quantity = item->GetItemQuantity();
	info->qindex = (unsigned int)item->ItemQualityIndexField();

	item->GetCraftingRequirementsString(&info->req, 0);
	item->GetItemStatsString(&info->stats);

	info->id = info->name + (info->isBP ? "1" : "0") + info->req + info->stats;

	return info;
}

std::string __formatText(FString s) {
	auto s2 = s.Replace(L"\n", L"|");
	auto utf = ArkApi::Tools::Utf8Encode(*s2);
	return utf;
}


FString _c("_C");
FString _e("");
FString _child("_Child_C"); // mod stacks
const FString __bp("_BP");

FString _bp(UObjectBase* object) {
	if (!object) return _e;

	/*
	auto cf = object->ClassField();
	if (!cf) return _e;
	*/

	return _bp(object->ClassField());
}

FString _bp(UClass *cf) {

	FString path_name("");

	if (cf) {
		cf->GetDefaultObject(true)->GetFullName(&path_name, nullptr);
		if (int find_index = 0; path_name.FindChar(' ', find_index)) {

			if (path_name.Mid(find_index - 8, 8) == _child) {
				path_name = path_name.Mid(0, find_index - 8);
			}
			else if (path_name.Mid(find_index - 2, 2) == _c) {
				path_name = path_name.Mid(0, find_index - 2);
			}
			else {
				path_name = path_name.Mid(0, find_index);
			}

			auto bp = path_name.Find(__bp);
			if (bp != -1) {
				// check for end
				if (bp + __bp.Len() == path_name.Len()) {
					path_name = path_name.Mid(0, bp);					 
				}
				else {
					path_name = path_name.Mid(0, bp) + path_name.Mid(bp + __bp.Len());
				}
			}

		}
	}
	return path_name;
}

FString __getClassName(AActor *o) {

	if (o == 0) return "zero";

	if (o->IsA(AShooterCharacter::GetPrivateStaticClass())) {
		return "AShooterCharacter";
	}

	if (o->IsA(APrimalDinoCharacter::GetPrivateStaticClass())) {
		return "APrimalDinoCharacter";
	}

	if (o->IsA(APrimalStructureItemContainer::GetPrivateStaticClass())) {
		return "APrimalStructureItemContainer";
	}


	if (o->IsA(APrimalCharacter::GetPrivateStaticClass())) {
		return "APrimalCharacter";
	}

	if (o->IsA(ACharacter::GetPrivateStaticClass())) {
		return "ACharacter";
	}



	return "unknown";
}

ObjectTypes __getClassType(AActor *o) {

	if (o->IsA(AShooterCharacter::GetPrivateStaticClass())) {
		return ot_AShooterCharacter;
	}

	if (o->IsA(APrimalDinoCharacter::GetPrivateStaticClass())) {
		return ot_APrimalDinoCharacter;
	}

	if (o->IsA(APrimalStructureItemContainer::GetPrivateStaticClass())) {
		return ot_APrimalStructureItemContainer;
	}


	if (o->IsA(APrimalCharacter::GetPrivateStaticClass())) {
		return ot_APrimalCharacter;
	}

	if (o->IsA(ACharacter::GetPrivateStaticClass())) {
		return ot_ACharacter;
	}


	if (o->IsA(APrimalDinoAIController::GetPrivateStaticClass())) {
		return ot_APrimalDinoAIController;
	}

	if (o->IsA(AAIController::GetPrivateStaticClass())) {
		return ot_AAIController;
	}

	if (o->IsA(AShooterPlayerController::GetPrivateStaticClass())) {
		return ot_AShooterPlayerController;
	}

	if (o->IsA(APlayerController::GetPrivateStaticClass())) {
		return ot_APlayerController;
	}

	if (o->IsA(AController::GetPrivateStaticClass())) {
		return ot_AController;
	}


	return ot_unknown;
}
