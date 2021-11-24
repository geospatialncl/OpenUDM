#include "DevZones.h"

void CreateDevZones(bool useBin, 
										int minSize, 
										bool moore, 
										const std::string& inputMask, 
										const std::string& outputZone, 
										const std::string& rastHdr, 
										const std::string& pathToBinConfig, 
										const std::string& inputWard) 
{

	std::vector<Zone> zones;

	//INPUT PARAMETERS BEGIN-------------------------------------------------------------------------------------------------------------

	bool bin_ras = useBin;									//FROM FUNCTION ARGUMENTS
	bool mooreNbrs = moore;									//FROM FUNCTION ARGUMENTS
	size_t minZoneSize = minSize;								//FROM FUNCTION ARGUMENTS
	std::string outputZoneIDData = outputZone;				//FROM FUNCTION ARGUMENTS
	std::string maskData = inputMask;						//FROM FUNCTION ARGUMENTS
	std::string wardIDData = inputWard;						//FROM FUNCTION ARGUMENTS	
	
	//load mask raster from binary
	IRaster mask;
	mask.Setup(maskData);
	mask.Read(maskData);
	//if (bin_ras) {
	//	mask.FromPGBinary(maskData);
	//}
	//else {
	//	mask.FromCSV(maskData);
	//}

	//obtain raster dimensions from inputSuit raster
	int rCols = mask.ncols;
	int rRows = mask.nrows;	

	//load wardID raster from binary	
	IRaster wardID;
	if (wardIDData.empty()) {					//if no wardID raster provided then create zones at global raster level
		wardID.Setup(rastHdr);					//setup from hdr file
		wardID.Setup(rCols, rRows, 1);			//initialise cells to 1
	}
	else {										//if wardID raster provided then create zones within wards 
		wardID.Setup(wardIDData);
		wardID.Read(wardIDData);
		//if (bin_ras) {
		//	wardID.FromPGBinary(wardIDData);
		//}
		//else {
		//	wardID.FromCSV(wardIDData);
		//}
	}

	//setup inital zone raster
	IRaster zoneID;
	zoneID.Setup(rastHdr);						//setup from hdr file
	zoneID.Setup(rCols, rRows, NOZONE);			//initialise cells to NOZONE	

	//setup final zone raster
	IRaster outputZoneID;
	outputZoneID.Setup(rastHdr);					//setup from hdr file
	outputZoneID.Setup(rCols, rRows, NOZONE);		//initialise cells to NOZONE

	//INPUT PARAMETERS END---------------------------------------------------------------------------------------------------------------	

	//initial zoneID - incrimented when each new zone added	
	int zID = 0;

	for (int r = 0; r != rRows; ++r) {						 //down rows
		for (int c = 0; c != rCols; ++c) {		//scan line	//across columns

			//check current cell validity
			if (mask.data[r][c] > 0) {

				//define current cell
				int currentZ = zoneID.data[r][c];
				Cell curCell(c, r, currentZ);
				int currentW = wardID.data[r][c];

				//gather valid neighbour cells
				std::vector<Cell> nbrCells;

				//range limited read of neighbouring zone codes--------

				//VON NEUMANN NEIGHBOURS begin

				//-x-
				//-o-
				//---
				if (r > 0) {
					if (mask.data[r - 1][c] > 0) {
						int w = wardID.data[r - 1][c];
						if (w == currentW) {
							int z = zoneID.data[r - 1][c];
							nbrCells.push_back(Cell(c, r - 1, z));
						}
					}
				}

				//---
				//-o-
				//-x-
				if (r < rRows - 1) {
					if (mask.data[r + 1][c] > 0) {
						int w = wardID.data[r + 1][c];
						if (w == currentW) {
							int z = zoneID.data[r + 1][c];
							nbrCells.push_back(Cell(c, r + 1, z));
						}
					}
				}

				//---
				//xo-
				//---
				if (c > 0) {
					if (mask.data[r][c - 1] > 0) {
						int w = wardID.data[r][c - 1];
						if (w == currentW) {
							int z = zoneID.data[r][c - 1];
							nbrCells.push_back(Cell(c - 1, r, z));
						}
					}
				}

				//---
				//-ox
				//---
				if (c < rCols - 1) {
					if (mask.data[r][c + 1] > 0) {
						int w = wardID.data[r][c + 1];
						if (w == currentW) {
							int z = zoneID.data[r][c + 1];
							nbrCells.push_back(Cell(c + 1, r, z));
						}
					}
				}

				//VON NEUMANN NEIGHBOURS end

				//MOORE NEIGHBOURS begin (if option is enabled)
				if (mooreNbrs) {

					//x--
					//-o-
					//---
					if (r > 0) {
						if (c > 0) {
							if (mask.data[r - 1][c - 1] > 0) {
								int w = wardID.data[r - 1][c - 1];
								if (w == currentW) {
									int z = zoneID.data[r - 1][c - 1];
									nbrCells.push_back(Cell(c - 1, r - 1, z));
								}
							}
						}
					}

					//--x
					//-o-
					//---
					if (r > 0) {
						if (c < rCols - 1) {
							if (mask.data[r - 1][c + 1] > 0) {
								int w = wardID.data[r - 1][c + 1];
								if (w == currentW) {
									int z = zoneID.data[r - 1][c + 1];
									nbrCells.push_back(Cell(c + 1, r - 1, z));
								}
							}
						}
					}

					//---
					//-o-
					//x--
					if (r < rRows - 1) {
						if (c > 0) {
							if (mask.data[r + 1][c - 1] > 0) {
								int w = wardID.data[r + 1][c - 1];
								if (w == currentW) {
									int z = zoneID.data[r + 1][c - 1];
									nbrCells.push_back(Cell(c - 1, r + 1, z));
								}
							}
						}
					}

					//---
					//-o-
					//--x
					if (r < rRows - 1) {
						if (c < rCols - 1) {
							if (mask.data[r + 1][c + 1] > 0) {
								int w = wardID.data[r + 1][c + 1];
								if (w == currentW) {
									int z = zoneID.data[r + 1][c + 1];
									nbrCells.push_back(Cell(c + 1, r + 1, z));
								}
							}
						}
					}
				}

				//MOORE NEIGHBOURS end

				//gather valid neighbour cells which are already in valid zones
				std::vector<Cell> zoneCells;
				std::vector<Cell> noZoneCells;
				if (!nbrCells.empty()) {
					for (size_t n = 0; n != nbrCells.size(); ++n) {
						if (nbrCells[n].z == NOZONE) {
							noZoneCells.push_back(nbrCells[n]);
						}
						else {
							zoneCells.push_back(nbrCells[n]);
						}
					}
				}

				//gather valid neighbour cells with lowest zoneID into loZoneCells
				//gather other valid zone cells into hiZoneCells
				std::vector<Cell> loZoneCells;
				std::vector<Cell> hiZoneCells;
				if (zoneCells.size() > 1) {

					//find lowest zone
					int lo = zoneCells[0].z;
					for (size_t z = 0; z != zoneCells.size(); ++z) {
						if (zoneCells[z].z < lo) {
							lo = zoneCells[z].z;
						}
					}

					//group into lo and hi
					for (size_t z = 0; z != zoneCells.size(); ++z) {
						if (zoneCells[z].z > lo) {
							hiZoneCells.push_back(zoneCells[z]);
						}
						else {
							loZoneCells.push_back(zoneCells[z]);
						}
					}
				}

				//gather (int)zoneID for current cell and hiZoneCells in order to combine and merge zones
				std::vector<int> mergeZoneIDs;
				if (!loZoneCells.empty()) {
					if (currentZ != NOZONE && currentZ != loZoneCells[0].z) {
						mergeZoneIDs.push_back(currentZ);
					}
					if (!hiZoneCells.empty()) {
						for (size_t h = 0; h != hiZoneCells.size(); ++h) {
							bool found = false;
							if (mergeZoneIDs.empty()) {
								mergeZoneIDs.push_back(hiZoneCells[h].z);
							}
							else {
								for (size_t z = 0; z != mergeZoneIDs.size(); ++z) {
									if (hiZoneCells[h].z == mergeZoneIDs[z]) {
										found = true;
									}
								}
								if (!found) {
									mergeZoneIDs.push_back(hiZoneCells[h].z);
								}
							}
						}
					}
				}

				switch (nbrCells.size()) {
				case(0) :
					//current cell is valid but has no neighbours
					//CURRENT CELL - BEGIN---------------------------------

					//seed new zone from current cell
					curCell.z = zID;
					zoneID.data[curCell.r][curCell.c] = curCell.z;

					//create new zone and add cells
					zones.push_back(Zone(zID));
					zones[zID].cells.push_back(curCell);

					//CURRENT CELL - END-----------------------------------

					++zID;		//new zone added so incriment

					break;

				case(1) :
					if (zoneCells.empty()) {
						//current cell is valid and has a single valid neighbour which is not currently assigned to a zone

						//CURRENT CELL - BEGIN---------------------------------

						//seed new zone from current cell
						curCell.z = zID;
						zoneID.data[curCell.r][curCell.c] = curCell.z;

						//create new zone and add cells
						zones.push_back(Zone(zID));
						zones[zID].cells.push_back(curCell);

						//CURRENT CELL - END-----------------------------------

						//NO ZONE CELL - BEGIN---------------------------------

						//seed neighbouring cell from current cell
						noZoneCells[0].z = curCell.z;
						zoneID.data[noZoneCells[0].r][noZoneCells[0].c] = noZoneCells[0].z;

						//add neighbouring cell to newly created zone
						zones[zID].cells.push_back(noZoneCells[0]);

						//NO ZONE CELL - END-----------------------------------

						++zID;		//new zone added so incriment
					}
					else {
						//current cell is valid and has a single valid neighbour which is already assigned to a zone

						//THIS SECTION IS REDUNDANT FOR VON NEUMANN NEIGHBOURHOODS - cells have already been zoned correctly
						//BUT NOT FOR MOORE NEIGHBOURRHOODS

						//-X0---	--scan line algorithm from top-left to bottom-right
						//--x1-x2	--x2 has single neighbour x3 which has already been zoned by x1
						//---x3-	--x2 must take zone from x3

						//CURRENT CELL - BEGIN---------------------------------

						//only add current cell if zone is different
						bool addCurCell = false;
						if (curCell.z != zoneCells[0].z) {
							addCurCell = true;
						}

						//ensure current cell is in same zone as neighbour
						curCell.z = zoneCells[0].z;
						zoneID.data[curCell.r][curCell.c] = curCell.z;

						//only add current cell if zone is different
						for (size_t z = 0; z != zones.size(); ++z) {
							if (zones[z].id == curCell.z) {
								if (addCurCell) {
									zones[z].cells.push_back(curCell);
								}
							}
						}

						//CURRENT CELL - END-----------------------------------

					}
					break;

				default:
					//current cell is valid and has a multiple valid neighbours 

					//no neighbours in valid zones
					if (zoneCells.empty()) {

						//CURRENT CELL - BEGIN---------------------------------

						//seed new zone from current cell
						curCell.z = zID;
						zoneID.data[curCell.r][curCell.c] = curCell.z;

						//create new zone and add current cell
						zones.push_back(Zone(zID));
						zones[zID].cells.push_back(curCell);

						//CURRENT CELL - END-----------------------------------

						//NO ZONE CELLS - BEGIN--------------------------------

						//seed new zone from current cell
						for (size_t n = 0; n != noZoneCells.size(); ++n) {
							noZoneCells[n].z = curCell.z;
							zoneID.data[noZoneCells[n].r][noZoneCells[n].c] = noZoneCells[n].z;
						}

						//add no zone cells to newly created zone
						for (size_t n = 0; n != noZoneCells.size(); ++n) {
							zones[zID].cells.push_back(noZoneCells[n]);
						}

						//NO ZONE CELLS - END----------------------------------

						++zID;		//new zone added so incriment
					}

					//single neighbour in valid zone
					if (zoneCells.size() == 1) {

						//CURRENT CELL - BEGIN---------------------------------

						//only add current cell if zone is different
						bool addCurCell = false;
						if (curCell.z != zoneCells[0].z) {
							addCurCell = true;
						}

						//seed zone from single valid neighbour
						curCell.z = zoneCells[0].z;
						zoneID.data[curCell.r][curCell.c] = curCell.z;

						//add current cell to pre existing zone - if not already in zone
						for (size_t z = 0; z != zones.size(); ++z) {
							if (zones[z].id == curCell.z) {
								if (addCurCell) {
									zones[z].cells.push_back(curCell);
								}
							}
						}

						//CURRENT CELL - END-----------------------------------

						//NO ZONE CELLS - BEGIN---------------------------------

						//seed zone from single valid neighbour
						for (size_t n = 0; n != noZoneCells.size(); ++n) {
							noZoneCells[n].z = zoneCells[0].z;
							zoneID.data[noZoneCells[n].r][noZoneCells[n].c] = noZoneCells[n].z;
						}

						//add cells to pre-existing zone
						//only add current cell if zone is different??
						for (size_t z = 0; z != zones.size(); ++z) {
							if (zones[z].id == zoneCells[0].z) {
								for (size_t n = 0; n != noZoneCells.size(); ++n) {
									zones[z].cells.push_back(noZoneCells[n]);
								}
							}
						}

						//NO ZONE CELLS - END-----------------------------------
					}

					//multiple neighbours in valid zones
					if (zoneCells.size() > 1) {

						//CURRENT CELL - BEGIN---------------------------------

						bool addCurCell = false;
						if (curCell.z == NOZONE) {
							addCurCell = true;
						}

						//seed zone from lowest zoneID
						curCell.z = loZoneCells[0].z;
						zoneID.data[curCell.r][curCell.c] = curCell.z;

						//add current cell zone to pre existing zone - only if current cell is not already allocated a zone
						for (size_t z = 0; z != zones.size(); ++z) {
							if (zones[z].id == curCell.z) {
								if (addCurCell) {
									zones[z].cells.push_back(curCell);
								}
							}
						}

						//CURRENT CELL - END-----------------------------------

						//NO ZONE CELLS - BEGIN--------------------------------

						//seed noZone cells
						if (!noZoneCells.empty()) {
							for (size_t n = 0; n != noZoneCells.size(); ++n) {
								noZoneCells[n].z = loZoneCells[0].z;
								zoneID.data[noZoneCells[n].r][noZoneCells[n].c] = noZoneCells[n].z;
							}
						}

						//add noZone cells to pre-existing zone
						if (!noZoneCells.empty()) {
							for (size_t z = 0; z != zones.size(); ++z) {
								if (zones[z].id == noZoneCells[0].z) {
									for (size_t n = 0; n != noZoneCells.size(); ++n) {
										zones[z].cells.push_back(noZoneCells[n]);
									}
								}
							}
						}

						//NO ZONE CELLS - END----------------------------------

						//MERGE CELLS - BEGIN----------------------------------

						//merge zones into zone with lowest id
						if (!mergeZoneIDs.empty()) {
							for (size_t r = 0; r != mergeZoneIDs.size(); ++r) {

								if (!zones.empty()) {
									for (size_t z = 0; z != zones.size(); ++z) {

										if (zones[z].id == loZoneCells[0].z) {

											if (!zones[mergeZoneIDs[r]].cells.empty()) {
												for (size_t c = 0; c != zones[mergeZoneIDs[r]].cells.size(); ++c) {

													//seed mergeZoneCells
													zoneID.data[zones[mergeZoneIDs[r]].cells[c].r][zones[mergeZoneIDs[r]].cells[c].c] = loZoneCells[0].z;
													//add mergeZoneCells to pre existing zone
													zones[z].cells.push_back(zones[mergeZoneIDs[r]].cells[c]);
												}
											}
										}
									}
								}

							}
						}

						//clear cells from merged zones
						if (!mergeZoneIDs.empty()) {
							for (size_t r = 0; r != mergeZoneIDs.size(); ++r) {
								zones[mergeZoneIDs[r]].cells.clear();
							}
						}

						//MERGE CELLS - END------------------------------------
					}
					break;
				}
			}
		}
	}

	//--Gather zones which have cells >= minZoneSize
	//--Re-number zone codes for valid zones
	int opZCode = 0;

	if (!zones.empty()) {
		for (size_t z = 0; z != zones.size(); ++z) {
			if (zones[z].cells.size() >= minZoneSize) {
				for (size_t c = 0; c != zones[z].cells.size(); ++c) {
					outputZoneID.data[zones[z].cells[c].r][zones[z].cells[c].c] = opZCode;
				}
				++opZCode;
			}
		}
	}
	//cout << "Number of zones created = " << opZCode << endl;

	//write outputZoneID raster	
	outputZoneID.Write(outputZoneIDData);
	//if (bin_ras) {
	//	outputZoneID.ToPGBinary(pathToBinConfig, outputZoneIDData);
	//}
	//else {
	//	outputZoneID.ToCSV(outputZoneIDData);
	//}
}

void DevZoneAVGSuit(bool useBin, 
										const std::string& zoneID, 
										const std::string& zoneData, 
										const std::string& zoneAVG, 
										const std::string& rastHdr, 
										const std::string& pathToBinConfig) {

	bool bin_ras = useBin;

	//load input zoneID raster	
	IRaster zID;	
	zID.Setup(zoneID);
	zID.Read(zoneID);
	//if (bin_ras) {
	//	zID.FromPGBinary(zoneID);
	//}
	//else {
	//	zID.FromCSV(zoneID);
	//}

	//load input zoneData raster	
	DRaster zData;	
	zData.Setup(zoneData);
	zData.Read(zoneData);
	//if (bin_ras) {
	//	zData.FromPGBinary(zoneData);
	//}
	//else {
	//	zData.FromCSV(zoneData);
	//}

	//setup output zoneStats raster	
	DRaster zStats;	
	zStats.Setup(rastHdr);

	//obtain raster dimensions from input zoneID raster
	size_t rCols = zID.ncols;
	size_t rRows = zID.nrows;

	int numZones = 0;
	std::vector<Zone> zones;

	//push into zones----------------------------------------------------------------------------------------

	for (size_t r = 0; r != rRows; ++r) {						 //down rows
		for (size_t c = 0; c != rCols; ++c) {		//scan line	//across columns

			//read current cell zoneID
			int z = zID.data[r][c];

			//if current cell belongs to valid zone
			if (z > -1) {

				if (static_cast<size_t>(z) == zones.size()) {							//create new zone
					zones.push_back(Zone(z));
					zones[z].cells.push_back(Cell(c, r, z));
					++numZones;
				}
				else											//add to existing zone
					zones[z].cells.push_back(Cell(c, r, z));

				if (static_cast<size_t>(z) > zones.size())
					std::cout << "ERROR... ZONE NOT CREATED YET" << std::endl;

			}
		}
	}

	//cout << "numZones = " << numZones << endl;

	//perform some calculation--------------------------------------------------------------------------

	//allocate storage for zoneStats
	std::vector<double> zoneStats(numZones);

	if (!zones.empty()) {
		for (size_t z = 0; z != zones.size(); ++z) {
			double sum = 0.0;	//reset for each zone
			if (!zones[z].cells.empty()) {
				for (size_t c = 0; c != zones[z].cells.size(); ++c) {
					sum += zData.data[zones[z].cells[c].r][zones[z].cells[c].c];
				}
			}
			zoneStats[z] = sum / (double)zones[z].cells.size();		//avg
		}
	}

	//write results to output raster

	for (size_t r = 0; r != rRows; ++r) {						 //down rows
		for (size_t c = 0; c != rCols; ++c) {		//scan line	//across columns

			//read current cell zoneID
			int z = zID.data[r][c];

			//if current cell belongs to valid zone
			if (z > -1) {
				zStats.data[r][c] = zoneStats[z];
			}
			else {
				if (zData.data[r][c] == zData.NODATA_value) {
					zStats.data[r][c] = zStats.NODATA_value;
				}
				else {
					zStats.data[r][c] = 0.0;
				}
			}
		}
	}

	//write to file	
	zStats.Write(zoneAVG);
	//if (bin_ras) {
	//	zStats.ToPGBinary(pathToBinConfig, zoneAVG);
	//}
	//else {
	//	zStats.ToCSV(zoneAVG);
	//}
}

