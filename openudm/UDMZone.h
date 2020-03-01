#pragma once
#include "CSVToolkit.h"
#include "UDMCell.h"

#include <vector>

class UDMZone final
{
public:
	UDMZone(int ID, double avgSuit);
	~UDMZone() = default;

	// delete copy/assignment due to shallow copy of pointers in cells
	UDMZone(const UDMZone&) = delete;
	UDMZone& operator=(const UDMZone&) = delete;
	// move/move assign is ok 
	UDMZone(UDMZone&&) = default;
	UDMZone& operator=(UDMZone&&) = default;

	// TODO address memory issues
	std::vector<UDMCellPtr> cells;
	int ID;
	double avgSuit;

	//test
	bool final;
};


// convenience typedef
typedef std::shared_ptr<UDMZone> UDMZonePtr;

