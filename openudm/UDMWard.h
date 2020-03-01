#pragma once
#include "CSVToolkit.h"
#include "UDMCell.h"
#include "UDMZone.h"

class UDMWard final
{
public:
	UDMWard();
	~UDMWard() = default;
	
	std::vector<UDMZone*> zones;
	std::vector<UDMCell*> cells;
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
};

