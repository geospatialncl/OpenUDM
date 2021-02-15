#pragma once
#include "CSVToolkit.h"
#include "UDMCell.h"
#include "UDMZone.h"

class UDMWard final
{
public:
	UDMWard();
	~UDMWard() = default;

	std::vector<UDMZonePtr> zones;
	std::vector<UDMCellPtr> cells;
	double popChange;
	bool devReq;
	int curDevCells;
	int curDevArea;
	double density;
	double cellDensity;
	int reqDevCells;
	int suitDevCells;
	bool overflow;	

	//test
	int devCells;

	//DPH
	size_t dwellingsRequired;
	size_t dwellingsAvailable;
	size_t dwellingsAssigned;
};

// convenience typedef
typedef std::shared_ptr<UDMWard> UDMWardPtr;
