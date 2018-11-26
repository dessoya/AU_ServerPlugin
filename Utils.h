#pragma once
#include <API/ARK/Ark.h>

typedef unsigned long long ull;

struct ItemInfo {
	bool isBP;
	FString id, name, req, stats;
	int quantity;
	unsigned int qindex;
};

enum ObjectTypes {
	ot_unknown = 0,
	ot_AShooterCharacter,
	ot_APrimalDinoCharacter,
	ot_APrimalStructureItemContainer,
	ot_APrimalCharacter,
	ot_ACharacter,
	ot_AController	= 20,
	ot_APlayerController = 30,
	ot_AShooterPlayerController = 40,
	ot_APrimalDinoAIController = 50,
	ot_AAIController = 60
};

ItemInfo *makeItemInfo(UPrimalItem *item);
FString _bp(UClass *cf);
FString _bp(UObjectBase* object);
std::string __formatText(FString s);
FString __getClassName(AActor *o);
ObjectTypes __getClassType(AActor *o);

#define makeItemId(iid1, iid2) ( (unsigned long long) (  (((unsigned long long)iid1) << 32) | ((unsigned long long)iid2) ) )
