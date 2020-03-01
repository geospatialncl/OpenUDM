#pragma once
class UDMCell
{
public:
	UDMCell(int col, int row);
	~UDMCell() = default;

	int col;
	int row;
	bool devStatus;
	double suit;
};

