#pragma once
#include "CSVToolkit.h"
#include "UDMCell.h"

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

	std::vector<UDMCell*> cells;
	int ID;
	double avgSuit;

	//test
	bool final;
};

