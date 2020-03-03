#include "DRaster.h"
#include "IRaster.h"
#include "CSVToolkit.h"
#include <chrono>


const int NOZONE = -1;


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
	std::vector<Cell> cells;
	Zone(int zid) {
		id = zid;
	}
};

//simple ward structure
struct Ward {
	std::vector<Cell> cells;
};

void CreateDevZones(bool useBin, int minSize, bool moore, 
										const std::string& inputMask, 
										const std::string& outputZone, 
										const std::string& rastHdr, 
										const std::string& pathToBinConfig, 
										const std::string& inputWard = std::string());

void DevZoneAVGSuit(bool useBin, 
										const std::string& zoneID, 
										const std::string& zoneData, 
										const std::string& zoneAVG, 
										const std::string& rastHdr, 
										const std::string& pathToBinConfig);