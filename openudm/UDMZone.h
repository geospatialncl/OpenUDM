#pragma once
#include "CSVToolkit.h"
#include "UDMCell.h"

#include <vector>

class UDMZone final
{
public:
	UDMZone(int ID, double avgSuit);
	~UDMZone() = default;

	std::vector<UDMCellPtr> cells;
	int ID;
	double avgSuit;
	//test
	bool final;
	//DPH	
	int dwellingsAvailable;
};


// convenience typedef
typedef std::shared_ptr<UDMZone> UDMZonePtr;

