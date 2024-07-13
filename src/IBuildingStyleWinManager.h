#pragma once

class IBuildingSelectWinContext
{
	virtual bool AutomaticallyMarkBuildingsAsHistorical() const = 0;
	virtual void SetAutomaticallyMarkBuildingsAsHistorical(bool value) = 0;
};