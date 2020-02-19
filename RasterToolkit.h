#include "DRaster.h"
#include "IRaster.h"
#include "CSVToolkit.h"
#include <chrono>
#define NOZONE -1

using namespace std;

//simple cell structure
struct Cell {
	int c;
	int r;
	int z;
	Cell(int col, int row, int zone) {
		c = col;
		r = row;
		z = zone;
	}
};

//simple zone structure
struct Zone {
	int id;
	vector<Cell> cells;
	Zone(int zid) {
		id = zid;
	}
};

//simple ward structure
struct Ward {
	vector<Cell> cells;	
};

bool DevCompare(std::string iRas1, std::string iRas2, std::string result);
bool IRasterCompare(std::string iRas1, std::string iRas2, std::string result);
bool DRasterCompare(std::string dRas1, std::string dRas2, std::string result);

void IRasterFixNoDataVal(std::string srcRas, int srcVal);
void IRasterSetNoDataToRef(std::string srcRas, std::string refRas);
void DRasterFixNoDataVal(std::string srcRas, int srcVal);
void DRasterSetNoDataToRef(std::string srcRas, std::string refRas);
void DRasterTidyNoDataVal(std::string srcRas, double srcVal);

void IRasterMaskedAddValue(std::string zoneIDRas, int val);
void IRasterNotBoolean(std::string srcRas);
void IRasterAddBoolean(std::string ras1Str, std::string ras2Str, std::string resultStr);
void Standardise(std::string srcRas, std::string maskRas);
void RevPolarityStandardise(std::string srcRas, std::string maskRas);
void AreaFromRaster(std::string wardIDRas, int numWards, std::string wardDataRas, int refVal, std::string areaOutput);
void BooleanDownsampler(std::string inputRas, std::string outputRas, int singleDimCellsToCell, int twoDimCellThreshold);
void DRasterSumRows(std::string inputRas, std::string outputCSV);
void DRasterSumColumns(std::string inputRas, std::string outputCSV);

void DRasterAscToBin(std::string input, std::string output, std::string pathToBinaryConfig);
void DRasterAscToCsv(std::string input, std::string output);
void IRasterAscToBin(std::string input, std::string output, std::string pathToBinaryConfig);
void IRasterAscToCsv(std::string input, std::string output);

void DRasterBinToAsc(std::string input, std::string output, std::string hdrFile);
void DRasterCsvToAsc(std::string input, std::string output, std::string hdrfile);
void IRasterBinToAsc(std::string input, std::string output, std::string hdrFile);
void IRasterCsvToAsc(std::string input, std::string output, std::string hdrfile);

void DRasterAscToODVal(std::string inputRas, std::string inputZoneCodes, std::string outputCSV);
void DRasterSubRaster(std::string inRasStr, std::string inCodeStr, std::string outRasStr, std::string outCodeStr, std::string outHdrFile);



