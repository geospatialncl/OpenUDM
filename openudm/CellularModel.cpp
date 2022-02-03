#include "CellularModel.h"

//Vector sorting functions
bool HighestZoneAVG(UDMZonePtr a, UDMZonePtr b) {
	return a->avgSuit > b->avgSuit;
}
bool HighestCellSuit(UDMCellPtr a, UDMCellPtr b) {
	return a->suit > b->suit;
}
bool LowestCellSuit(UDMCellPtr a, UDMCellPtr b) {
	return a->suit < b->suit;
}


CellularModel::CellularModel()
{
}


CellularModel::~CellularModel()
{
}

void CellularModel::Setup(int wards, int res, int cols, int rows) {

	//set parameters from arguments
	numWards = wards;
	devRes = res;
	rasterCols = cols;
	rasterRows = rows;	

	//enable binary rasters by default
	bin_ras = true;

	//calculate density by default
	densityProvided = false;

	//VARIABLE DENSITY - DISABLE BY DEFAULT
	useDwellingsData = false;
	
	//allocate dynamic memory using numWards
	AllocateAll();
}

void CellularModel::AllocateAll() {

	//allocate dynamic memory
	wardLabel.resize(numWards);	
	tempCP.resize(numWards);	
	tempFP.resize(numWards);	
	tempWardStr.resize(numWards);
	curPop.resize(numWards);	
	futPop.resize(numWards);
	wardDensity.resize(numWards);
}

void CellularModel::UseBinaryRasters(bool useBin) {
	bin_ras = useBin;
}

void CellularModel::SetRasterHeader(const std::string& hdr) {
	rastHdr = hdr;
	SetupFinalDevRaster();
}

void CellularModel::SetPathToBinaryConfigFiles(const std::string& binConfigPath) {
	pathToBinConfig = binConfigPath;
}

void CellularModel::LoadWardLabels(const std::string& label, int labelColumn, int numColumns) {

	//read column from csv
	ExtractCSV(label, numColumns, labelColumn, wardLabel);
}

void CellularModel::LoadCurrentPopulation(const std::string& popData, int curPopColumn, int numColumns) {

	//read column from csv
	ExtractCSV(popData, numColumns, curPopColumn, tempCP);

	//convert string data to double
	for (size_t w = 0; w != numWards; ++w) {
		curPop[w] = std::stod(tempCP[w]);
	}
}

void CellularModel::LoadFuturePopulation(const std::string& popData, int futPopColumn, int numColumns) {

	//read column from csv
	ExtractCSV(popData, numColumns, futPopColumn, tempFP);

	//convert string data to double
	for (size_t w = 0; w != numWards; ++w) {
		futPop[w] = std::stod(tempFP[w]);
	}
}

void CellularModel::LoadWardDensity(const std::string& densityData, int densityColumn, int numColumns) {

	//read column from csv
	ExtractCSV(densityData, numColumns, densityColumn, tempWardStr);

	//convert string data to double
	for (size_t w = 0; w != numWards; ++w) {
		wardDensity[w] = std::stod(tempWardStr[w]);
	}

	//density values have been provided
	densityProvided = true;
}

//VARIABLE DENSITY
void CellularModel::LoadDwellingsRaster(const std::string& dwellingsData) {	

	//setup and read raster
	dwellingsRaster.Setup(dwellingsData);
	dwellingsRaster.Read(dwellingsData);
	//if (bin_ras) {
	//	dwellingsRaster.FromPGBinary(dwellingsData);
	//}
	//else {
	//	dwellingsRaster.FromCSV(dwellingsData);
	//}

	//dwellings values are provided
	useDwellingsData = true;
}

void CellularModel::LoadWardIDRaster(const std::string& iWardIDData) {	

	//setup and read raster
	iWardID.Setup(iWardIDData);
	iWardID.Read(iWardIDData);
	//if (bin_ras) {
	//	iWardID.FromPGBinary(iWardIDData);
	//}
	//else {
	//	iWardID.FromCSV(iWardIDData);
	//}
}

void CellularModel::LoadZoneIDRaster(const std::string& zoneIDData) {

	//setup and read raster
	zoneID.Setup(zoneIDData);
	zoneID.Read(zoneIDData);
	//if (bin_ras) {
	//	zoneID.FromPGBinary(zoneIDData);
	//}
	//else {
	//	zoneID.FromCSV(zoneIDData);
	//}
}

void CellularModel::LoadZoneAVGRaster(const std::string& zoneAVGData) {

	//setup and read raster
	zoneAVG.Setup(zoneAVGData);
	zoneAVG.Read(zoneAVGData);
	//if (bin_ras) {
	//	zoneAVG.FromPGBinary(zoneAVGData);
	//}
	//else {
	//	zoneAVG.FromCSV(zoneAVGData);
	//}
}

void CellularModel::LoadDevLandRaster(const std::string& devLandData) {

	//setup and read raster
	devLand.Setup(devLandData);
	devLand.Read(devLandData);
	//if (bin_ras) {
	//	devLand.FromPGBinary(devLandData);
	//}
	//else {
	//	devLand.FromCSV(devLandData);
	//}
}

void CellularModel::LoadCellSuitRaster(const std::string& cellSuitData) {

	//setup and read raster
	cellSuit.Setup(cellSuitData);
	cellSuit.Read(cellSuitData);
	//if (bin_ras) {
	//	cellSuit.FromPGBinary(cellSuitData);
	//}
	//else {
	//	cellSuit.FromCSV(cellSuitData);
	//}
}

void CellularModel::SetupFinalDevRaster() {

	//setup raster from header
	finalDev.Setup(rastHdr);
}

void CellularModel::RunModel() {

	//run the following functions in order...
	CreateWards();
	AssignWardCells();
	CalculatePopulationChange();
	AssignZones();
	
	if (useDwellingsData) {
		CalculateRequiredDevelopment_DPH();
		FindOverflowWards_DPH();
		DevelopNonOverflowWards_DPH();
	}
	else {
		CalculateRequiredDevelopment();
		FindOverflowWards();
		DevelopNonOverflowWards();
	}	

	DevelopOverflowWards();
	SetCurrentDev();
	SetNoData();
	SetFutureDev();
}


void CellularModel::CreateWards() {

	//create wards
	for (size_t w = 0; w != numWards; ++w) {
		wards.push_back(std::make_shared<UDMWard>());
	}	
}

void CellularModel::AssignWardCells() {

	//ASSIGN RASTER CELLS TO WARDS 		

	//--read pixels into allocated wards using wardID
	for (size_t r = 0; r != rasterRows; ++r) {
		for (size_t c = 0; c != rasterCols; ++c) {
			if (iWardID.data[r][c] != iWardID.NODATA_value) {
				wards[iWardID.data[r][c]]->cells.push_back(std::make_shared<UDMCell>(c, r));
			}			
		}
	}	
}

void CellularModel::CalculatePopulationChange() {

	//CALCULATE WARD POPULATION CHANGE 

	//calculate population change for all wards and flag devReq as false where popChange !> 0
	for (size_t w = 0; w != numWards; ++w) {
		wards[w]->popChange = futPop[w] - curPop[w];

		if (wards[w]->popChange <= 0.0) {
			wards[w]->devReq = false;		//allows these wards to be skipped in future calculations				
		}
	}		
}

void CellularModel::AssignZones() {

	//assign zones
	for (size_t w = 0; w != numWards; ++w) {	//for all wards
		//if (wards[w]->devReq) {				//which require development
			if (!wards[w]->cells.empty()) {		//if ward has some cells
				for (size_t c = 0; c != wards[w]->cells.size(); ++c) {	//for all cells within ward

					int zid = zoneID.data[wards[w]->cells[c]->row][wards[w]->cells[c]->col];	//read zoneID 

					if (zid != zoneID.NODATA_value) {	//check if valid i.e. not NODATA_val

						if (!wards[w]->zones.empty()) {	//if ward has zones

							bool foundZ = false;

							//find out if we already have a zone with this ID
							for (size_t z = 0; z != wards[w]->zones.size(); ++z) {
								if (wards[w]->zones[z]->ID == zid) {
									//add cell to zone[z] and flag found
									wards[w]->zones[z]->cells.push_back(wards[w]->cells[c]);
									foundZ = true;
								}
							}

							if (!foundZ) {	//zone not found so create new one with zid and suit, then add current cell

								double suit = zoneAVG.data[wards[w]->cells[c]->row][wards[w]->cells[c]->col];	//read zone avgSuit
								wards[w]->zones.push_back(std::make_shared<UDMZone>(zid, suit));
								wards[w]->zones[wards[w]->zones.size() - 1]->cells.push_back(wards[w]->cells[c]);
							}
						}
						else {	//no zones so create new one with zid and suit, then add current cell

							double suit = zoneAVG.data[wards[w]->cells[c]->row][wards[w]->cells[c]->col];	//read zone avgSuit
							wards[w]->zones.push_back(std::make_shared<UDMZone>(zid, suit));
							wards[w]->zones[0]->cells.push_back(wards[w]->cells[c]);
						}
					}
				}
			}
		//}
	}	
}

//void CellularModel::CalculateRequiredDevelopment() {
//
//	//--calculate:
//	//total developed pixels and area (m^2) in each ward
//	//pop density per cell and pop density (area)
//	//required cells for new development
//
//	for (int w = 0; w != numWards; ++w) {														//all wards
//		if (wards[w]->devReq) {
//			int devCells = 0;																	//reset for each ward
//			for (int c = 0; c != wards[w]->cells.size(); ++c) {									//all cells in ward
//				int dev = devLand.data[wards[w]->cells[c]->row][wards[w]->cells[c]->col];
//				if (dev == 1) {
//					++devCells;
//				}
//			}
//			wards[w]->curDevCells = devCells;
//			wards[w]->curDevArea = devCells * devRes * devRes;									//devArea m^2
//			wards[w]->cellDensity = curPop[w] / (double)devCells;
//			wards[w]->density = curPop[w] / (double)wards[w]->curDevArea;
//
//			double futDev = wards[w]->popChange / wards[w]->cellDensity;
//			wards[w]->reqDevCells = (int)std::ceil(futDev);										//rounded up to next integer	
//		}
//	}
//}

void CellularModel::CalculateRequiredDevelopment() {

	//--calculate:
	//total developed pixels and area (m^2) in each ward
	//pop density per cell and pop density (area)
	//required cells for new development

	for (size_t w = 0; w != static_cast<size_t>(numWards); ++w) {														//all wards
		//if (wards[w]->devReq) {
			int devCells = 0;																	//reset for each ward
			for (size_t c = 0; c != wards[w]->cells.size(); ++c) {									//all cells in ward
				int dev = devLand.data[wards[w]->cells[c]->row][wards[w]->cells[c]->col];
				if (dev == 1) {
					++devCells;
				}
			}
			wards[w]->curDevCells = devCells;
			wards[w]->curDevArea = devCells * devRes * devRes;									//devArea m^2


			if (densityProvided) {

				for (size_t w = 0; w != numWards; ++w) {
					wards[w]->density = wardDensity[w];
					wards[w]->cellDensity = wardDensity[w] * devRes * devRes;
				}

			}
			else {
				wards[w]->density = curPop[w] / (double)wards[w]->curDevArea;
				wards[w]->cellDensity = curPop[w] / (double)devCells;
			}

			double futDev = wards[w]->popChange / wards[w]->cellDensity;
			wards[w]->reqDevCells = (int)std::ceil(futDev);										//rounded up to next integer	
		//}
	}
}

void CellularModel::CalculateRequiredDevelopment_DPH() {	//VARIABLE DENSITY VERSION

	//--calculate:	
	//required dewllings for new development

	for (size_t w = 0; w != static_cast<size_t>(numWards); ++w) {														//all wards
		
		//DPH
		wards[w]->dwellingsRequired = wards[w]->popChange;
	}
}

void CellularModel::FindOverflowWards() {

	//FIND OVERFLOW WARDS 

	//find out if we have any overflow wards
	for (size_t w = 0; w != numWards; ++w) {
		if (wards[w]->devReq) {
			int suitCells = 0;											//reset for each ward iteration 
			for (size_t z = 0; z != wards[w]->zones.size(); ++z) {
				suitCells += wards[w]->zones[z]->cells.size();
			}
			wards[w]->suitDevCells = suitCells;
			if (wards[w]->suitDevCells < wards[w]->reqDevCells) {		//if not enough cells for development record overflow
				wards[w]->overflow = true;				
			}
		}
	}	
}

// void CellularModel::FindOverflowWards_DPH() {	//VARIABLE DENSITY VERSION

// 	//find out if we have any overflow wards	
	
// 	for (size_t w = 0; w != static_cast<size_t>(numWards); ++w) {					//all wards		
	
// 		//calculate total dwellings in each development zone
// 		for (size_t z = 0; z != wards[w]->zones.size(); ++z) {						//all zones
// 			for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {		//all cells

// 				wards[w]->zones[z]->dwellingsAvailable += dwellingsRaster.data[wards[w]->zones[z]->cells[c]->row][wards[w]->zones[z]->cells[c]->col];				
// 			}
// 		}

// 		//sum for each ward and test available dwellings against required dwellings
		
// 		for (size_t z = 0; z != wards[w]->zones.size(); ++z) {						//all zones

// 			wards[w]->dwellingsAvailable += wards[w]->zones[z]->dwellingsAvailable; 
// 		}

// 		if (wards[w]->dwellingsAvailable < wards[w]->dwellingsRequired) {		//if not enough dwellings for development record overflow
// 	 			wards[w]->overflow = true;				
// 	 	}
// 	}
// }

void CellularModel::FindOverflowWards_DPH() {	//VARIABLE DENSITY VERSION

	//find out if we have any overflow wards	
	
	for (size_t w = 0; w != static_cast<size_t>(numWards); ++w) {					//all wards	

		int wardDwellings = 0;	
	
		//calculate total dwellings in each development zone
		for (size_t z = 0; z != wards[w]->zones.size(); ++z) {						//all zones

			int zoneDwellings = 0;

			for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {		//all cells

				wards[w]->zones[z]->cells[c]->dwellingsAvailable = dwellingsRaster.data[wards[w]->zones[z]->cells[c]->row][wards[w]->zones[z]->cells[c]->col];	
				zoneDwellings += wards[w]->zones[z]->cells[c]->dwellingsAvailable;			
			}

			wards[w]->zones[z]->dwellingsAvailable = zoneDwellings;
		}

		//sum for each ward and test available dwellings against required dwellings
		
		for (size_t z = 0; z != wards[w]->zones.size(); ++z) {						//all zones

			wardDwellings += wards[w]->zones[z]->dwellingsAvailable; 
		}

		wards[w]->dwellingsAvailable = wardDwellings;

		if (wards[w]->dwellingsAvailable < wards[w]->dwellingsRequired) {		//if not enough dwellings for development record overflow
	 			wards[w]->overflow = true;				
	 	}
	}
}

void CellularModel::DevelopNonOverflowWards() {

	//DEVELOP NON-OVERFLOW WARDS BEGIN--------	

	//sort all zones in ward by highest avgSuit
	for (size_t w = 0; w != numWards; ++w) {
		if (wards[w]->devReq) {
			if (!wards[w]->overflow) {
				std::sort(wards[w]->zones.begin(), wards[w]->zones.end(), HighestZoneAVG);
			}
		}
	}

	//develop cells in non-overflow wards
	for (size_t w = 0; w != numWards; ++w) {
		if (wards[w]->devReq) {
			if (!wards[w]->overflow) {

				int devCells = 0;	//keep track of how many cells have been developed - reset for each ward

				for (size_t z = 0; z != wards[w]->zones.size(); ++z) {		//all zones
					if (devCells < wards[w]->reqDevCells) {				//requiring further development 

						//find out if we're in the spread zone (if there is one... <=)
						if ((static_cast<int>(wards[w]->zones[z]->cells.size()) + devCells) <= wards[w]->reqDevCells) {	//not in spread zone

							//develop all cells in zone - keep track of devCells
							for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
								wards[w]->zones[z]->cells[c]->devStatus = true;
								++devCells;
								
								wards[w]->devCells = devCells;		//keep track of how many cells have been developed
							}
						}
						else {	//spread zone - seed and spread development from most suitable cell

							//read cell suitability from raster
							for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
								wards[w]->zones[z]->cells[c]->suit =
									cellSuit.data[wards[w]->zones[z]->cells[c]->row][wards[w]->zones[z]->cells[c]->col];
							}

							//sort all cells in zone by highest suitability
							std::sort(wards[w]->zones[z]->cells.begin(), wards[w]->zones[z]->cells.end(), HighestCellSuit);

							//test**
							wards[w]->zones[z]->final = true;

							//initial seed
							UDMCellPtr seed = wards[w]->zones[z]->cells[0];
							//vetors of seeds and neighbours
							std::vector<UDMCellPtr> seeds;
							std::vector<UDMCellPtr> nbrs;

							//develop initial seed cell
							seed->devStatus = true;
							++devCells;
							seeds.push_back(seed);
							
							wards[w]->devCells = devCells;		//keep track of how many cells have been developed

							while (devCells < wards[w]->reqDevCells && !seeds.empty()) {

								nbrs.clear();

								//get neighbours
								for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
									int cDiff = (seeds[seeds.size() - 1]->col + 1) - (wards[w]->zones[z]->cells[c]->col + 1);
									int rDiff = (seeds[seeds.size() - 1]->row + 1) - (wards[w]->zones[z]->cells[c]->row + 1);

									cDiff = abs(cDiff);
									rDiff = abs(rDiff);

									if ((cDiff <= 1) && (rDiff <= 1)) {
										if ((cDiff + rDiff) > 0) {
											nbrs.push_back(wards[w]->zones[z]->cells[c]);	//don't include cell itself!
										}										
									}
								}

								//remove current seed
								seeds.pop_back();								

								//sort neighbours by highest suit
								if (nbrs.size() > 1) {
									std::sort(nbrs.begin(), nbrs.end(), HighestCellSuit);
								}
								
								//develop neighbouring cells
								for (size_t n = 0; n != nbrs.size(); ++n) {
									if (!nbrs[n]->devStatus) {						//not already developed										
										if (devCells < wards[w]->reqDevCells) {
											nbrs[n]->devStatus = true;
											++devCells;
											seeds.push_back(nbrs[n]);				//add neighbour as potential seed
											
											wards[w]->devCells = devCells;			//keep track of how many cells have been developed										
										}
									}
								}

								//sort seeds by lowest suit 
								//can then access most suitable using seeds[seeds.size()-1]
								//and can remove when finished with using seeds.pop_back()

								//**Replicate current development including possible error
								if (!seeds.empty()) {
									if (seeds.size() > 1) {
										std::sort(seeds.begin(), seeds.end(), LowestCellSuit);
									}
								}
								else {							
									wards[w]->overflow = true;		//flag as overflow ward									
								}															
							}
						}
					}
				}
			}
		}
	}	
}

// void CellularModel::DevelopNonOverflowWards_DPH() {		//VARIABLE DENSITY

// 	//DEVELOP NON-OVERFLOW WARDS BEGIN--------	

// 	//sort all zones in ward by highest avgSuit
// 	for (size_t w = 0; w != numWards; ++w) {
// 		//if (wards[w]->devReq) {
// 			//if (!wards[w]->overflow) {
// 				std::sort(wards[w]->zones.begin(), wards[w]->zones.end(), HighestZoneAVG);
// 			//}
// 		//}
// 	}

// 	//develop cells in non-overflow wards
// 	for (size_t w = 0; w != numWards; ++w) {

// 		dwellingsAssigned = 0;

// 		for (size_t z = 0; z != wards[w]->zones.size(); ++z) {		//all zones

// 			//develop all cells in zone - keep track of devCells
// 			for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {

// 				if (dwellingsAssigned < dwellingsRequired) {

// 					wards[w]->zones[z]->cells[c]->devStatus = true;

// 					dwellingsAssigned += dwellingsRaster.data[wards[w]->zones[z]->cells[c]->row][wards[w]->zones[z]->cells[c]->col];
// 				}
// 			}
// 		}
// 	}

// 	std::cout << "dwellingsRequired = " << dwellingsRequired << std::endl;
// 	std::cout << "dwellingsAssigned = " << dwellingsAssigned << std::endl;

// 	// //develop cells in non-overflow wards
// 	// for (size_t w = 0; w != numWards; ++w) {
// 	// 	if (wards[w]->devReq) {
// 	// 		if (!wards[w]->overflow) {

// 	// 			int devCells = 0;	//keep track of how many cells have been developed - reset for each ward

// 	// 			for (size_t z = 0; z != wards[w]->zones.size(); ++z) {		//all zones
// 	// 				if (devCells < wards[w]->reqDevCells) {				//requiring further development 

// 	// 					//find out if we're in the spread zone (if there is one... <=)
// 	// 					if ((static_cast<int>(wards[w]->zones[z]->cells.size()) + devCells) <= wards[w]->reqDevCells) {	//not in spread zone

// 	// 						//develop all cells in zone - keep track of devCells
// 	// 						for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
// 	// 							wards[w]->zones[z]->cells[c]->devStatus = true;
// 	// 							++devCells;
								
// 	// 							wards[w]->devCells = devCells;		//keep track of how many cells have been developed
// 	// 						}
// 	// 					}
// 	// 					else {	//spread zone - seed and spread development from most suitable cell

// 	// 						//read cell suitability from raster
// 	// 						for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
// 	// 							wards[w]->zones[z]->cells[c]->suit =
// 	// 								cellSuit.data[wards[w]->zones[z]->cells[c]->row][wards[w]->zones[z]->cells[c]->col];
// 	// 						}

// 	// 						//sort all cells in zone by highest suitability
// 	// 						std::sort(wards[w]->zones[z]->cells.begin(), wards[w]->zones[z]->cells.end(), HighestCellSuit);

// 	// 						//test**
// 	// 						wards[w]->zones[z]->final = true;

// 	// 						//initial seed
// 	// 						UDMCellPtr seed = wards[w]->zones[z]->cells[0];
// 	// 						//vetors of seeds and neighbours
// 	// 						std::vector<UDMCellPtr> seeds;
// 	// 						std::vector<UDMCellPtr> nbrs;

// 	// 						//develop initial seed cell
// 	// 						seed->devStatus = true;
// 	// 						++devCells;
// 	// 						seeds.push_back(seed);
							
// 	// 						wards[w]->devCells = devCells;		//keep track of how many cells have been developed

// 	// 						while (devCells < wards[w]->reqDevCells && !seeds.empty()) {

// 	// 							nbrs.clear();

// 	// 							//get neighbours
// 	// 							for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
// 	// 								int cDiff = (seeds[seeds.size() - 1]->col + 1) - (wards[w]->zones[z]->cells[c]->col + 1);
// 	// 								int rDiff = (seeds[seeds.size() - 1]->row + 1) - (wards[w]->zones[z]->cells[c]->row + 1);

// 	// 								cDiff = abs(cDiff);
// 	// 								rDiff = abs(rDiff);

// 	// 								if ((cDiff <= 1) && (rDiff <= 1)) {
// 	// 									if ((cDiff + rDiff) > 0) {
// 	// 										nbrs.push_back(wards[w]->zones[z]->cells[c]);	//don't include cell itself!
// 	// 									}										
// 	// 								}
// 	// 							}

// 	// 							//remove current seed
// 	// 							seeds.pop_back();								

// 	// 							//sort neighbours by highest suit
// 	// 							if (nbrs.size() > 1) {
// 	// 								std::sort(nbrs.begin(), nbrs.end(), HighestCellSuit);
// 	// 							}
								
// 	// 							//develop neighbouring cells
// 	// 							for (size_t n = 0; n != nbrs.size(); ++n) {
// 	// 								if (!nbrs[n]->devStatus) {						//not already developed										
// 	// 									if (devCells < wards[w]->reqDevCells) {
// 	// 										nbrs[n]->devStatus = true;
// 	// 										++devCells;
// 	// 										seeds.push_back(nbrs[n]);				//add neighbour as potential seed
											
// 	// 										wards[w]->devCells = devCells;			//keep track of how many cells have been developed										
// 	// 									}
// 	// 								}
// 	// 							}

// 	// 							//sort seeds by lowest suit 
// 	// 							//can then access most suitable using seeds[seeds.size()-1]
// 	// 							//and can remove when finished with using seeds.pop_back()

// 	// 							//**Replicate current development including possible error
// 	// 							if (!seeds.empty()) {
// 	// 								if (seeds.size() > 1) {
// 	// 									std::sort(seeds.begin(), seeds.end(), LowestCellSuit);
// 	// 								}
// 	// 							}
// 	// 							else {							
// 	// 								wards[w]->overflow = true;		//flag as overflow ward									
// 	// 							}															
// 	// 						}
// 	// 					}
// 	// 				}
// 	// 			}
// 	// 		}
// 	// 	}
// 	// }	
// }

void CellularModel::DevelopNonOverflowWards_DPH() {		//VARIABLE DENSITY

	//DEVELOP NON-OVERFLOW WARDS BEGIN--------	

	//sort all zones in ward by highest avgSuit
	for (size_t w = 0; w != numWards; ++w) {
		if (wards[w]->devReq) {
			if (!wards[w]->overflow) {
				std::sort(wards[w]->zones.begin(), wards[w]->zones.end(), HighestZoneAVG);
			}
		}
	}

	//develop cells in non-overflow wards
	for (size_t w = 0; w != numWards; ++w) {
		if (wards[w]->devReq) {
			if (!wards[w]->overflow) {

				//DPH - change
				//int devCells = 0;	//keep track of how many cells have been developed - reset for each ward
				int dwellings = 0;	//keep track of how many dwellings have been developed - reset for each ward

				for (size_t z = 0; z != wards[w]->zones.size(); ++z) {		//all zones

					//DPH - change
					//if (devCells < wards[w]->reqDevCells) {				//requiring further development 
					if (dwellings < wards[w]->dwellingsRequired) {				//requiring further development 

						//find out if we're in the spread zone (if there is one... <=)

						//DPH - change
						//if ((static_cast<int>(wards[w]->zones[z]->cells.size()) + devCells) <= wards[w]->reqDevCells) {	//not in spread zone
						//if ((static_cast<int>(wards[w]->zones[z]->dwellingsAvailable + dwellings) <= wards[w]->dwellingsRequired)) {	//not in spread zone
						if ((wards[w]->zones[z]->dwellingsAvailable + dwellings) <= wards[w]->dwellingsRequired) {	//not in spread zone

							//DPH - change
							//develop all cells in zone - keep track of devCells
							for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
								wards[w]->zones[z]->cells[c]->devStatus = true;
								
								//DPH - change
								//++devCells;								
								//wards[w]->devCells = devCells;		//keep track of how many cells have been developed
								dwellings += wards[w]->zones[z]->dwellingsAvailable;
								wards[w]->dwellingsAssigned = dwellings;							
							}
						}
						else {	//spread zone - seed and spread development from most suitable cell

							//read cell suitability from raster
							for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
								wards[w]->zones[z]->cells[c]->suit =
									cellSuit.data[wards[w]->zones[z]->cells[c]->row][wards[w]->zones[z]->cells[c]->col];
							}

							//sort all cells in zone by highest suitability
							std::sort(wards[w]->zones[z]->cells.begin(), wards[w]->zones[z]->cells.end(), HighestCellSuit);

							//test**
							wards[w]->zones[z]->final = true;

							//initial seed
							UDMCellPtr seed = wards[w]->zones[z]->cells[0];
							//vectors of seeds and neighbours
							std::vector<UDMCellPtr> seeds;
							std::vector<UDMCellPtr> nbrs;

							//develop initial seed cell
							seed->devStatus = true;

							//DPH - change
							//++devCells;
							dwellings += seed->dwellingsAvailable;

							seeds.push_back(seed);
							
							//DPH - change
							//wards[w]->devCells = devCells;		//keep track of how many cells have been developed
							wards[w]->dwellingsAssigned = dwellings;

							//DPH - change
							//while (devCells < wards[w]->reqDevCells && !seeds.empty()) {
							while (dwellings < wards[w]->dwellingsRequired && !seeds.empty()) {

								nbrs.clear();

								//get neighbours
								for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
									int cDiff = (seeds[seeds.size() - 1]->col + 1) - (wards[w]->zones[z]->cells[c]->col + 1);
									int rDiff = (seeds[seeds.size() - 1]->row + 1) - (wards[w]->zones[z]->cells[c]->row + 1);

									cDiff = abs(cDiff);
									rDiff = abs(rDiff);

									if ((cDiff <= 1) && (rDiff <= 1)) {
										if ((cDiff + rDiff) > 0) {
											nbrs.push_back(wards[w]->zones[z]->cells[c]);	//don't include cell itself!
										}										
									}
								}

								//remove current seed
								seeds.pop_back();								

								//sort neighbours by highest suit
								if (nbrs.size() > 1) {
									std::sort(nbrs.begin(), nbrs.end(), HighestCellSuit);
								}
								
								//develop neighbouring cells
								for (size_t n = 0; n != nbrs.size(); ++n) {
									if (!nbrs[n]->devStatus) {						//not already developed	

										//DPH - change	
										//if (devCells < wards[w]->reqDevCells) {								
										if (dwellings < wards[w]->dwellingsRequired) {
											nbrs[n]->devStatus = true;

											//DPH - change
											//++devCells;
											dwellings += nbrs[n]->dwellingsAvailable;

											seeds.push_back(nbrs[n]);				//add neighbour as potential seed
											
											//DPH - change
											//wards[w]->devCells = devCells;			//keep track of how many cells have been developed	
											wards[w]->dwellingsAssigned = dwellings;									
										}
									}
								}

								//sort seeds by lowest suit 
								//can then access most suitable using seeds[seeds.size()-1]
								//and can remove when finished with using seeds.pop_back()

								//**Replicate current development including possible error
								if (!seeds.empty()) {
									if (seeds.size() > 1) {
										std::sort(seeds.begin(), seeds.end(), LowestCellSuit);
									}
								}
								else {							
									wards[w]->overflow = true;		//flag as overflow ward									
								}															
							}
						}
					}
				}
			}
		}
	}	
}

void CellularModel::WriteOverflowWards(const std::string& overflowData) {

	if(useDwellingsData) {
		Overflow_DPH(overflowData);
	}
	else {
		Overflow(overflowData);
	}
}

void CellularModel::Overflow_DPH(const std::string& overflowData) {

	//create an ofstream object
	std::ofstream opfile(overflowData);

	//check the file opened OK
	if (opfile.is_open()) {
		//write header
		opfile << "WardLabel" << ",";

		opfile << "Overflow" << ",";

		opfile << "Dwellings Required" << ",";		

		opfile << "Dwellings Assigned" << "\n";

		for (size_t w = 0; w != wards.size(); ++w) {

			//ward label
			opfile << wardLabel[w] << ",";

			//overflow
			opfile << wards[w]->overflow << ",";

			//dwellings required
			opfile << wards[w]->dwellingsRequired << ",";

			//dwellings assigned
			opfile << wards[w]->dwellingsAssigned << "\n";		

		}

		//close opfile
		opfile.close();
	}
	else {
		std::cout << "Unable to open overflow file";
	}
}

void CellularModel::Overflow(const std::string& overflowData) {

	//create an ofstream object
	std::ofstream opfile(overflowData);

	//check the file opened OK
	if (opfile.is_open()) {
		//write header
		opfile << "WardLabel" << ",";

		opfile << "Overflow" << ",";

		opfile << "CurrentPop" << ",";

		opfile << "FuturePop" << ",";

		opfile << "PopAssigned" << ",";

		opfile << "PopNotAssigned" << ",";

		opfile << "ExpectedCellsDev" << ",";

		opfile << "ActualCellsDev" << ",";

		opfile << "AreaRequired" << ",";

		opfile << "AreaDeveloped" << ",";

		opfile << "CurrentPopCellDensity" << ",";

		opfile << "ReqPopCellDensityInFreeCells" << ",";

		opfile << "ReqPopCellDensityInWardCells" << "\n";

		for (size_t w = 0; w != wards.size(); ++w) {

			//ward label
			opfile << wardLabel[w] << ",";

			//overflow
			opfile << wards[w]->overflow << ",";

			//current pop = curPop[w]
			opfile << curPop[w] << ",";

			//target pop = futPop[w]
			opfile << futPop[w] << ",";

			//PopulationAssigned  = curPop[w] + (wards[w]->suitDevCells * wards[w]->cellDensity)
			opfile << curPop[w] + (wards[w]->devCells * wards[w]->cellDensity) << ",";

			//PopulationNotAssigned = futPop[w] - (curPop[w] + (wards[w]->suitDevCells * wards[w]->cellDensity))
			opfile << futPop[w] - (curPop[w] + (wards[w]->devCells * wards[w]->cellDensity)) << ",";

			//ExpectedPixelsDeveloped = wards[w]->reqDevCells
			opfile << wards[w]->reqDevCells << ",";			

			//ActualPixelsDeveloped = wards[w]->suitDevCells
			opfile << wards[w]->devCells << ",";

			//AreaRequired = wards[w]->reqDevCells * devRes * devRes
			opfile << wards[w]->reqDevCells * devRes * devRes << ",";

			//AreaDeveloped = wards[w]->suitDevCells * devRes * devRes
			opfile << wards[w]->devCells * devRes * devRes << ",";

			//CurrentCellDensity = wards[w]->cellDensity
			opfile << wards[w]->cellDensity << ",";

			if (wards[w]->suitDevCells == 0) {	//avoid division by zero - output -8888 to represent infinite pop density
				opfile << -8 << ",";
			}
			else {
				//RequiredPopDensityinFreePixels = wards[w]->popChange / wards[w]->suitDevCells
				opfile << wards[w]->popChange / wards[w]->suitDevCells << ",";
			}

			if (wards[w]->suitDevCells == 0 && wards[w]->curDevCells == 0) {	//avoid division by zero - output -8888 to represent infinite pop density
				opfile << -8 << "\n";
			}
			else {
				//RequiredWardPopDensity = futPop[w] / (wards[w]->curDevCells + wards[w]->suitDevCells)
				opfile << futPop[w] / (wards[w]->curDevCells + wards[w]->suitDevCells) << "\n";
			}

		}

		//close opfile
		opfile.close();
	}
	else {
		std::cout << "Unable to open overflow file";
	}
}

void CellularModel::DevelopOverflowWards() {

	//--develop all cells in all zones of overflow wards - strictly adhere to min.dev.zone??
	/*for (int w = 0; w != numWards; ++w) {
		if (wards[w]->devReq) {
			if (wards[w]->overflow) {
				for (int z = 0; z != wards[w]->zones.size(); ++z) {
					for (int c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
						wards[w]->zones[z]->cells[c]->devStatus = true;
					}
				}
			}
		}
	}*/

	for (size_t w = 0; w != numWards; ++w) {

		int devCells = 0;	//keep track of how many cells have been developed - reset for each ward

		if (wards[w]->devReq) {
			if (wards[w]->overflow) {
				for (size_t z = 0; z != wards[w]->zones.size(); ++z) {
					for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {
						wards[w]->zones[z]->cells[c]->devStatus = true;
						++devCells;
						wards[w]->devCells = devCells;			//keep track of how many cells have been developed	
					}
				}
			}
		}
	}
}

void CellularModel::SetCurrentDev() {

	//set currently developed land 
	for (size_t r = 0; r != rasterRows; ++r) {
		for (size_t c = 0; c != rasterCols; ++c) {
			if (devLand.data[r][c] == 1) {
				finalDev.data[r][c] = 1;
			}
		}
	}
}

void CellularModel::SetNoData() {

	//create NODATA mask using iWardID raster
	for (size_t r = 0; r != rasterRows; ++r) {
		for (size_t c = 0; c != rasterCols; ++c) {
			if (iWardID.data[r][c] == iWardID.NODATA_value) {
				finalDev.data[r][c] = finalDev.NODATA_value;
			}
		}
	}
}

void CellularModel::SetFutureDev() {

	//set future developed land
	for (size_t w = 0; w != wards.size(); ++w) {
		if (wards[w]->devReq) {
			if (!wards[w]->zones.empty()) {
				for (size_t z = 0; z != wards[w]->zones.size(); ++z) {
					for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {

						if (wards[w]->zones[z]->cells[c]->devStatus) {
							finalDev.data[wards[w]->zones[z]->cells[c]->row][wards[w]->zones[z]->cells[c]->col] = 2;
						}
					}
				}
			}
		}
	}
}

void CellularModel::OutputRasterResult(const std::string& rasterData) {

	//write results to raster
	finalDev.Write(rasterData);
	//if (bin_ras) {
	//	finalDev.ToPGBinary(pathToBinConfig, rasterData);
	//}
	//else {
	//	finalDev.ToCSV(rasterData);
	//}
}

void CellularModel::OutputDevelopmentDensity(const std::string& popDensityRaster, const std::string& dwellingDensityRaster, double people_per_dwelling) {

	DRaster popDensity;
	popDensity.Setup(rastHdr);

	DRaster dwellingDensity;
	dwellingDensity.Setup(rastHdr);

	//CurrentCellDensity = wards[w]->cellDensity
	//opfile << wards[w]->cellDensity << ",";

	//set future development population density
	for (size_t w = 0; w != wards.size(); ++w) {
		if (wards[w]->devReq) {
			if (!wards[w]->zones.empty()) {
				for (size_t z = 0; z != wards[w]->zones.size(); ++z) {
					for (size_t c = 0; c != wards[w]->zones[z]->cells.size(); ++c) {

						if (wards[w]->zones[z]->cells[c]->devStatus) {
							popDensity.data[wards[w]->zones[z]->cells[c]->row][wards[w]->zones[z]->cells[c]->col] = wards[w]->cellDensity;
							dwellingDensity.data[wards[w]->zones[z]->cells[c]->row][wards[w]->zones[z]->cells[c]->col] = wards[w]->cellDensity / people_per_dwelling;
						}
					}
				}
			}
		}
	}

	popDensity.Write(popDensityRaster);
	dwellingDensity.Write(dwellingDensityRaster);

}

