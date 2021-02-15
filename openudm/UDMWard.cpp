#include "UDMWard.h"


UDMWard::UDMWard()
:	popChange(0.0),
	devReq(true),
	curDevCells(0),
	curDevArea(0),
	density(0.0),
	cellDensity(0.0),
	reqDevCells(0),
	suitDevCells(0),
	overflow(false),
	//test
	devCells(0),

	//DPH
	dwellingsRequired(0),
	dwellingsAvailable(0),
	dwellingsAssigned(0)
{ }


