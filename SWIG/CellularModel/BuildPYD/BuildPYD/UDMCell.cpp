#include "UDMCell.h"
#include "CSVToolkit.h"


UDMCell::UDMCell(int col, int row)
{
	this->col = col;
	this->row = row;
	devStatus = false;
	suit = 0.0;
}


UDMCell::~UDMCell(void)
{
}
