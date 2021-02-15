#include "UDMCell.h"
#include "CSVToolkit.h"


UDMCell::UDMCell(int col, int row)
:	col(col),
	row(row),
	devStatus(false),
	suit(0.0),
	dwellingsAvailable(0)
{ }


