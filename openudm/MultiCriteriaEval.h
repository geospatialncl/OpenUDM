#include "DRaster.h"
#include "IRaster.h"
#include "CSVToolkit.h"
#include <chrono>


void MaskedWeightedSum(bool useBin, 
											 const std::string& constraintRas, 
											 int numAttractors, 
											 const std::string& attractorInputs, 
											 const std::string& output,
											 const std::string& rastHdr, 
											 const std::string& binConfigPath, 
											 bool reverse);

//void MaskedWeightedSum(bool useBin,
//	const std::string& iRasCount,
//	const std::string& iRasInputs,
//	const std::string& dRasCount,
//	const std::string& dRasInputs,
//	const std::string& output,
//	const std::string& rastHdr,
//	const std::string& binConfigPath,
//	bool reverse);