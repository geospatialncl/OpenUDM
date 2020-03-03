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
	void SetRasterHeader(const std::string& hdr);
	void SetPathToBinaryConfigFiles(const std::string& binConfigPath);

	void LoadWardLabels(const std::string& label, int labelColumn, int numColumns);
	void LoadCurrentPopulation(const std::string& popData, int curPopColumn, int numColumns);
	void LoadFuturePopulation(const std::string& popData, int futPopColumn, int numColumns);
	void LoadWardDensity(const std::string& densityData, int densityColumn, int numColumns);

	void LoadWardIDRaster(const std::string& iWardIDData);
	void LoadZoneIDRaster(const std::string& zoneIDData);
	void LoadZoneAVGRaster(const std::string& zoneAVGData);
	void LoadDevLandRaster(const std::string& devLandData);
	void LoadCellSuitRaster(const std::string& cellSuitData);
	void SetupFinalDevRaster();

	void RunModel();	

	void OutputRasterResult(const std::string& rasterData);
	void WriteOverflowWards(const std::string& overflowData);

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
	std::vector<UDMWardPtr> wards;	

	//read strings
	std::vector<std::string> wardLabel;
	std::vector<std::string> tempCP;	
	std::vector<std::string> tempFP;
	std::vector<std::string> tempWardStr;
	
	//current and future population arrays
	std::vector<double> curPop;	
	std::vector<double> futPop;

	//density array
	std::vector<double> wardDensity;
	bool densityProvided;
	
	//memory management
	void AllocateAll();	

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

