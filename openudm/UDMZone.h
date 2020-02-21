#pragma once
#include "CSVToolkit.h"
#include "UDMCell.h"

class UDMZone
{
public:
	UDMZone(int ID, double avgSuit);
	~UDMZone(void);

	std::vector<UDMCell*> cells;
	int ID;
	double avgSuit;

	//test
	bool final;
};

