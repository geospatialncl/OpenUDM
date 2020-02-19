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

void CreateDevZones(bool useBin, int minSize, bool moore, std::string inputMask, std::string outputZone, std::string rastHdr, std::string pathToBinConfig, std::string inputWard = std::string());

void DevZoneAVGSuit(bool useBin, std::string zoneID, std::string zoneData, std::string zoneAVG, std::string rastHdr, std::string pathToBinConfig);