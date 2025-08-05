/*
 * This file is part of sc4-more-building-styles, a DLL Plugin for
 * SimCity 4 that adds support for more building styles.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
 *
 * sc4-more-building-styles is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-more-building-styles is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-more-building-styles.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "IBuildingSelectWinManager.h"
#include "AvailableBuildingStyles.h"
#include "BuildingSelectWinContext.h"
#include "cIGZMessageTarget2.h"
#include "cISC4ZoneManager.h"
#include "UnavailableUIBuildingStyles.h"

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

	bool IsStyleButtonIDValid(uint32_t style) const override;
	const BuildingStyleCollection& GetAvailableBuildingStyles() const override;

	IBuildingSelectWinContext& GetContext() override;
	const IBuildingSelectWinContext& GetContext() const override;

	void SendActiveBuildingStyleCheckboxChangedMessage(
		bool checked,
		const BuildingStyleCollectionEntry& entry) override;

	uint32_t refCount;
	cIGZMessageServer2* pMS2;
	cISC4LotManager* pLotManager;
	cISC4TractDeveloper* pTractDeveloper;
	cISC4ZoneManager* pZoneManager;
	AvailableBuildingStyles availableBuildingStyles;
	BuildingSelectWinContext context;
	UnavailableUIBuildingStyles unavailableUIBuildingStyles;
	bool initialized;
};

