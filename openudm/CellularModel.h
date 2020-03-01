#pragma once
#include "CSVToolkit.h"
#include "IRaster.h"
#include "DRaster.h"
#include "UDMWard.h"
#include <cmath>
#include <algorithm>

class CellularModel
{
public:

	CellularModel();
	~CellularModel();

	void Setup(int wards, int res, int cols, int rows);	

	void UseBinaryRasters(bool useBin);
	void SetRasterHeader(std::string hdr);
	void SetPathToBinaryConfigFiles(std::string binConfigPath);

	void LoadWardLabels(std::string label, int labelColumn, int numColumns);
	void LoadCurrentPopulation(std::string popData, int curPopColumn, int numColumns);
	void LoadFuturePopulation(std::string popData, int futPopColumn, int numColumns);
	void LoadWardDensity(std::string densityData, int densityColumn, int numColumns);

	void LoadWardIDRaster(std::string iWardIDData);
	void LoadZoneIDRaster(std::string zoneIDData);
	void LoadZoneAVGRaster(std::string zoneAVGData);
	void LoadDevLandRaster(std::string devLandData);
	void LoadCellSuitRaster(std::string cellSuitData);
	void SetupFinalDevRaster();

	void RunModel();	

	void OutputRasterResult(std::string rasterData);
	void WriteOverflowWards(std::string overflowData);

	void Cleanup();

private:

	//wards and raster parameters
	size_t numWards; 
	size_t devRes; 
	size_t rasterCols; 
	size_t rasterRows;

	//input rasters
	IRaster iWardID;
	IRaster zoneID;
	DRaster zoneAVG;
	IRaster devLand;
	DRaster cellSuit;
	IRaster finalDev;
	std::string rastHdr;
	std::string pathToBinConfig;
	bool bin_ras;

	//top level of the wards->zones->cells hierarchy
	std::vector<UDMWard*> wards;	

	//read strings
	std::string* wardLabel;
	std::string* tempCP;	
	std::string* tempFP;
	std::string* tempWardStr;
	
	//current and future population arrays
	double* curPop;	
	double* futPop;

	//density array
	double* wardDensity;
	bool densityProvided;
	
	//memory management
	void AllocateAll();	
	void FreeAll();

	//RunModel() functions
	void CreateWards();
	void AssignWardCells();
	void CalculatePopulationChange();
	void AssignZones();
	void CalculateRequiredDevelopment();
	void FindOverflowWards();
	void DevelopNonOverflowWards();
	void DevelopOverflowWards();
	void SetCurrentDev();
	void SetNoData();
	void SetFutureDev();
};

