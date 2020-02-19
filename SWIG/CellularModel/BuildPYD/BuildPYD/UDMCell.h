#pragma once
class UDMCell
{
public:
	UDMCell(int col, int row);
	~UDMCell(void);

	int col;
	int row;
	bool devStatus;
	double suit;
};

