#include "DRaster.h"
#include "IRaster.h"
#include "CSVToolkit.h"
#include <chrono>

using namespace std;

void MaskedWeightedSum(bool useBin, std::string iRasCount, std::string iRasInputs, std::string dRasCount, std::string dRasInputs, std::string output,
						std::string rastHdr, std::string binConfigPath, bool reverse);