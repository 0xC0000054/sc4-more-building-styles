////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-more-building-styles, a DLL Plugin for
// SimCity 4 that adds support for more building styles.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "IBuildingSelectWinManager.h"
#include "AvailableBuildingStyles.h"
#include "BuildingSelectWinContext.h"
#include "cIGZMessageTarget2.h"
#include "cISC4ZoneManager.h"

class cIGZMessage2Standard;
class cIGZMessageServer2;
class cISC4Lot;
class cISC4LotManager;

class BuildingSelectWinManager : public IBuildingSelectWinManager, private cIGZMessageTarget2
{
public:

	BuildingSelectWinManager();

	bool Initialize();
	bool Shutdown();

private:

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;
	bool DoMessage(cIGZMessage2* pMsg);

	void PreCityShutdown();
	void PostCityInit(cIGZMessage2Standard* pStandardMsg);
	void Load(cIGZMessage2Standard* pStandardMsg);
	void Save(cIGZMessage2Standard* pStandardMsg);
	void StateChanged(cIGZMessage2Standard* pStandardMsg);
	void LotActivated(cISC4Lot* pLot);

	void GrowifyLot(cISC4Lot* pLotCopy, cISC4ZoneManager::ZoneType zoneType);

	cISC4TractDeveloper* GetTractDeveloper() const override;

	bool IsBuildingStyleAvailable(uint32_t style) const override;
	const std::map<uint32_t, std::string>& GetAvailableBuildingStyles() const override;

	bool GetOptionalCheckBoxState(uint32_t buttonID) const override;
	void UpdateOptionalCheckBoxState(cIGZWin* pWin, uint32_t buttonID) override;

	void SendActiveBuildingStyleCheckboxChangedMessage(bool checked, uint32_t styleID) override;

	uint32_t refCount;
	cIGZMessageServer2* pMS2;
	cISC4LotManager* pLotManager;
	cISC4TractDeveloper* pTractDeveloper;
	cISC4ZoneManager* pZoneManager;
	AvailableBuildingStyles availableBuildingStyles;
	BuildingSelectWinContext context;
	bool initialized;
};

