#include "DRaster.h"
#include "IRaster.h"
#include "CSVToolkit.h"
#include <chrono>

//simple cell structure
struct Cell {
	int c;
	int r;
	int z;
	Cell(int col, int row, int zone) : c(col), r(row), z(zone) { }
};

//simple zone structure
struct Zone {
	int id;
	std::vector<Cell> cells;
	Zone(int zid) : id(zid) { }
};

//simple ward structure
typedef std::vector<Cell> Ward;

bool DevCompare(const std::string& iRas1, const std::string& iRas2, const std::string& result);
bool IRasterCompare(const std::string& iRas1, const std::string& iRas2, const std::string& result);
bool DRasterCompare(const std::string& dRas1, const std::string& dRas2, const std::string& result);

void IRasterFixNoDataVal(const std::string& srcRas, int srcVal);
void IRasterSetNoDataToRef(const std::string& srcRas, const std::string& refRas);
void DRasterFixNoDataVal(const std::string& srcRas, int srcVal);
void DRasterSetNoDataToRef(const std::string& srcRas, const std::string& refRas);
void DRasterTidyNoDataVal(const std::string& srcRas, double srcVal);

void IRasterMaskedAddValue(const std::string& zoneIDRas, int val);
void IRasterNotBoolean(const std::string& srcRas);
void IRasterAddBoolean(const std::string& ras1Str, const std::string& ras2Str, const std::string& resultStr);
void Standardise(const std::string& srcRas, const std::string& maskRas);
void RevPolarityStandardise(const std::string& srcRas, const std::string& maskRas);
void AreaFromRaster(const std::string& wardIDRas, int numWards, const std::string& wardDataRas, int refVal, const std::string& areaOutput);
void BooleanDownsampler(const std::string& inputRas, const std::string& outputRas, int singleDimCellsToCell, int twoDimCellThreshold);
void DRasterSumRows(const std::string& inputRas, const std::string& outputCSV);
void DRasterSumColumns(const std::string& inputRas, const std::string& outputCSV);

void DRasterAscToBin(const std::string& input, const std::string& output, const std::string& pathToBinaryConfig);
void DRasterAscToCsv(const std::string& input, const std::string& output);
void IRasterAscToBin(const std::string& input, const std::string& output, const std::string& pathToBinaryConfig);
void IRasterAscToCsv(const std::string& input, const std::string& output);

void DRasterBinToAsc(const std::string& input, const std::string& output, const std::string& hdrFile);
void DRasterCsvToAsc(const std::string& input, const std::string& output, const std::string& hdrfile);
void IRasterBinToAsc(const std::string& input, const std::string& output, const std::string& hdrFile);
void IRasterCsvToAsc(const std::string& input, const std::string& output, const std::string& hdrfile);

void DRasterAscToODVal(const std::string& inputRas, const std::string& inputZoneCodes, const std::string& outputCSV);
void DRasterSubRaster(const std::string& inRasStr, const std::string& inCodeStr, const std::string& outRasStr, const std::string& outCodeStr, const std::string& outHdrFile);



