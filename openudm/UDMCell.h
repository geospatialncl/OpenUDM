#pragma once

#include <memory>
class UDMCell
{
public:
	UDMCell(int col, int row);
	~UDMCell() = default;

	int col;
	int row;
	bool devStatus;
	double suit;

	int dwellingsAvailable;
};

// convenience typedef
typedef std::shared_ptr<UDMCell> UDMCellPtr;

