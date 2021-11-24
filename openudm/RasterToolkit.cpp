#include "RasterToolkit.h"

//tile vector sorting function by lowest dph
bool LowestTileDPH(RTTile a, RTTile b) {
	return a.dph < b.dph;
}

bool DevCompare(const std::string& iRas1, const std::string& iRas2, const std::string& result) {

	//set comparison to true initially
	bool comp = true;

	//load rasters to be compared
	IRaster ras1;
	ras1.Setup(iRas1);
	ras1.Read(iRas1);

	IRaster ras2;
	ras2.Setup(iRas2);
	ras2.Read(iRas2);

	int nc = ras1.ncols;
	int nr = ras1.nrows;

	//setup result raster
	IRaster res;
	res.Setup(iRas1);			///initialised to 0 by default


	//loop through all cells
	for (int r = 0; r != nr; ++r) {
		for (int c = 0; c != nc; ++c) {

			if (ras1.data[r][c] == ras1.NODATA_value && ras2.data[r][c] == ras2.NODATA_value) {		//NODATA_value
				res.data[r][c] = res.NODATA_value;
			}

			if (ras1.data[r][c] == 0 && ras2.data[r][c] == 0) {			//no development
				res.data[r][c] = 0;
			}
			
			if (ras1.data[r][c] == 1 && ras2.data[r][c] == 1) {			//current development
				res.data[r][c] = 1;
			}

			if (ras1.data[r][c] == 2 && ras2.data[r][c] == 2) {			//common future development
				res.data[r][c] = 2;
			}

			if (ras1.data[r][c] == 2 && ras2.data[r][c] != 2) {			//ras1 only future development
				res.data[r][c] = 3;
			}

			if (ras1.data[r][c] != 2 && ras2.data[r][c] == 2) {			//ras2 only future development
				res.data[r][c] = 4;
			}			
		}
	}

	if (!comp) {
		res.Write(result);
	}

	return comp;
}

bool IRasterCompare(const std::string& iRas1, const std::string& iRas2, const std::string& result) {

	//set comparison to true initially
	bool comp = true;

	//load rasters to be compared
	IRaster ras1;
	ras1.Setup(iRas1);
	ras1.Read(iRas1);

	IRaster ras2;
	ras2.Setup(iRas2);
	ras2.Read(iRas2);

	int nc = ras1.ncols;
	int nr = ras1.nrows;

	//setup result raster
	IRaster res;
	res.Setup(iRas1);			///initialised to 0 by default
	

	//loop through all cells
	for (int r = 0; r != nr; ++r) {
		for (int c = 0; c != nc; ++c) {
			if (ras1.data[r][c] == ras1.NODATA_value && ras2.data[r][c] == ras2.NODATA_value) {
				res.data[r][c] = res.NODATA_value;
			}
			else if (ras1.data[r][c] != ras2.data[r][c]) {
				res.data[r][c] = 1;
				comp = false;
			}
		}
	}

	if (!comp) {
		res.Write(result);
	}

	return comp;
}

bool DRasterCompare(const std::string& dRas1, const std::string& dRas2, const std::string& result) {

	//set comparison to true initially
	bool comp = true;

	//load rasters to be compared
	DRaster ras1;
	ras1.Setup(dRas1);
	ras1.Read(dRas1);

	DRaster ras2;
	ras2.Setup(dRas2);
	ras2.Read(dRas2);

	int nc = ras1.ncols;
	int nr = ras1.nrows;

	//setup result raster
	IRaster res;
	res.Setup(dRas1);			///initialised to 0 by default


	//loop through all cells
	for (int r = 0; r != nr; ++r) {
		for (int c = 0; c != nc; ++c) {
			if (ras1.data[r][c] == ras1.NODATA_value && ras2.data[r][c] == ras2.NODATA_value) {
				res.data[r][c] = res.NODATA_value;
			}
			else if (ras1.data[r][c] != ras2.data[r][c]) {
				res.data[r][c] = 1.0;
				comp = false;
			}
		}
	}

	if (!comp) {
		res.Write(result);
	}

	return comp;
}

void IRasterFixNoDataVal(const std::string& srcRas, int srcVal) {

	//declare src raster
	IRaster src;
	//setup src raster
	src.Setup(srcRas);
	//read src raster
	src.Read(srcRas);

	//set NODATA_value to default
	src.NODATA_value = -1;

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {

			if (src.data[r][c] == srcVal) {				//set cells equal to to srcVal
				src.data[r][c] = src.NODATA_value;		//to NODATA_value
			}
		}
	}

	//write src raster to file
	src.Write(srcRas);
}

void IRasterSetNoDataToRef(const std::string& srcRas, const std::string& refRas) {

	//declare src raster
	IRaster src;
	//setup src raster
	src.Setup(srcRas);
	//read src raster
	src.Read(srcRas);

	//declare ref raster
	IRaster ref;
	//setup ref raster
	ref.Setup(refRas);
	//read ref raster
	ref.Read(refRas);

	//set NODATA_value to default
	src.NODATA_value = -1;

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {

			if(ref.data[r][c] == ref.NODATA_value) {		//if ref raster is NODATA_value
				src.data[r][c] = src.NODATA_value;			//set src raster to NODATA_value
			}	
		}
	}

	//write src raster to file
	src.Write(srcRas);
}

void DRasterFixNoDataVal(const std::string& srcRas, int srcVal) {

	//declare src raster
	DRaster src;
	//setup src raster
	src.Setup(srcRas);
	//read src raster
	src.Read(srcRas);

	//set NODATA_value to default
	src.NODATA_value = -1;

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {

			if (src.data[r][c] == srcVal) {				//set cells equal to to srcVal
				src.data[r][c] = src.NODATA_value;		//to NODATA_value
			}
		}
	}

	//write src raster to file
	src.Write(srcRas);
}

void DRasterSetNoDataToRef(const std::string& srcRas, const std::string& refRas) {

	//declare src raster
	DRaster src;
	//setup src raster
	src.Setup(srcRas);
	//read src raster
	src.Read(srcRas);

	//declare ref raster
	IRaster ref;
	//setup ref raster
	ref.Setup(refRas);
	//read ref raster
	ref.Read(refRas);

	//set NODATA_value to default
	src.NODATA_value = -1;

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {

			if (ref.data[r][c] == ref.NODATA_value) {		//if ref raster is NODATA_value
				src.data[r][c] = src.NODATA_value;			//set src raster to NODATA_value
			}
		}
	}

	//write src raster to file
	src.Write(srcRas);
}

//void DRasterTidyNoDataVal(const std::string& srcRas, double srcVal) {
//
//	//declare src raster
//	DRaster src;
//	//setup src raster
//	src.Setup(srcRas);
//	//read src raster
//	src.Read(srcRas);
//
//	//set NODATA_value to default
//	src.NODATA_value = -1;
//
//	//for all cells in srcRas
//	for (int r = 0; r != src.nrows; ++r) {
//		for (int c = 0; c != src.ncols; ++c) {
//
//			if (src.data[r][c] == srcVal) {	
//
//				int nbrs = 0;
//				double nbrsVal = 0.0;
//
//				//search neighbouring cells for valid data
//				if (r > 0) {
//
//					if (src.data[r-1][c] != src.NODATA_value) {
//						++nbrs;
//						nbrsVal += src.data[r-1][c];
//					}
//
//					if (c > 0) {
//						if (src.data[r-1][c-1] != src.NODATA_value) {
//							++nbrs;
//							nbrsVal += src.data[r-1][c-1];
//						}						
//					}
//
//					if (c < src.ncols - 1) {
//						if (src.data[r-1][c+1] != src.NODATA_value) {
//							++nbrs;
//							nbrsVal += src.data[r-1][c+1];
//						}
//					}
//				}
//
//				if (r < src.nrows - 1) {
//
//					if (src.data[r + 1][c] != src.NODATA_value) {
//						++nbrs;
//						nbrsVal += src.data[r + 1][c];
//					}
//
//					if (c > 0) {
//						if (src.data[r + 1][c - 1] != src.NODATA_value) {
//							++nbrs;
//							nbrsVal += src.data[r + 1][c - 1];
//						}
//					}
//
//					if (c < src.ncols - 1) {
//						if (src.data[r + 1][c + 1] != src.NODATA_value) {
//							++nbrs;
//							nbrsVal += src.data[r + 1][c + 1];
//						}
//					}
//				}
//
//				if (c > 0) {
//					if (src.data[r][c - 1] != src.NODATA_value) {
//						++nbrs;
//						nbrsVal += src.data[r][c - 1];
//					}
//				}
//
//				if (c < src.ncols - 1) {
//					if (src.data[r][c + 1] != src.NODATA_value) {
//						++nbrs;
//						nbrsVal += src.data[r][c + 1];
//					}
//				}
//
//				if (nbrs > 0) {
//					src.data[r][c] = nbrsVal / (double)nbrs;
//				}
//				else {
//					src.data[r][c] = 0.0;
//				}				
//			}
//		}
//	}
//
//	//write src raster to file
//	src.Write(srcRas);
//}

void DRasterTidyNoDataVal(const std::string& srcRas, double srcVal) {

	//declare src raster
	DRaster src;
	//setup src raster
	src.Setup(srcRas);
	//read src raster
	src.Read(srcRas);

	//set NODATA_value to default
	src.NODATA_value = -1;

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {

			if (src.data[r][c] == srcVal) {

				int nbrs = 0;
				double nbrsVal = 0.0;

				//search neighbouring cells for valid data
				if (r > 0) {

					if (src.data[r - 1][c] > 0.0f) {
						++nbrs;
						nbrsVal += src.data[r - 1][c];
					}

					if (c > 0) {
						if (src.data[r - 1][c - 1] > 0.0f) {
							++nbrs;
							nbrsVal += src.data[r - 1][c - 1];
						}
					}

					if (c < src.ncols - 1) {
						if (src.data[r - 1][c + 1] > 0.0f) {
							++nbrs;
							nbrsVal += src.data[r - 1][c + 1];
						}
					}
				}

				if (r < src.nrows - 1) {

					if (src.data[r + 1][c] > 0.0f) {
						++nbrs;
						nbrsVal += src.data[r + 1][c];
					}

					if (c > 0) {
						if (src.data[r + 1][c - 1] > 0.0f) {
							++nbrs;
							nbrsVal += src.data[r + 1][c - 1];
						}
					}

					if (c < src.ncols - 1) {
						if (src.data[r + 1][c + 1] > 0.0f) {
							++nbrs;
							nbrsVal += src.data[r + 1][c + 1];
						}
					}
				}

				if (c > 0) {
					if (src.data[r][c - 1] > 0.0f) {
						++nbrs;
						nbrsVal += src.data[r][c - 1];
					}
				}

				if (c < src.ncols - 1) {
					if (src.data[r][c + 1] > 0.0f) {
						++nbrs;
						nbrsVal += src.data[r][c + 1];
					}
				}

				if (nbrs > 0) {
					src.data[r][c] = nbrsVal / (double)nbrs;
				}
				else {
					src.data[r][c] = 0.0;
				}
			}
		}
	}

	//write src raster to file
	src.Write(srcRas);
}

void IRasterMaskedAddValue(const std::string& zoneIDRas, int val) {

	//load input zoneID raster	
	IRaster zID;
	zID.Setup(zoneIDRas);
	zID.Read(zoneIDRas);

	for (int r = 0; r != zID.nrows; ++r) {
		for (int c = 0; c != zID.ncols; ++c) {
			
			if (zID.data[r][c] != zID.NODATA_value) {	//mask nodata
				zID.data[r][c] += val;
			}
		}
	}

	//write results
	zID.Write(zoneIDRas);	
}

void IRasterNotBoolean(const std::string& srcRas) {

	//declare src raster
	IRaster src;
	//setup src raster
	src.Setup(srcRas);
	//read src raster
	src.Read(srcRas);

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {

			if (src.data[r][c] != src.NODATA_value) {		//where src raster is not NODATA_value

				//if (src.data[r][c] > 0) {					//set ones to zeros
				//	src.data[r][c] = 0;
				//}
				//if (src.data[r][c] < 1) {					//set zeros to ones
				//	src.data[r][c] = 1;
				//}

				if (src.data[r][c] > 0) {					//set ones to zeros
					src.data[r][c] = 0;
				}
				else if (src.data[r][c] < 1) {					//set zeros to ones
					src.data[r][c] = 1;
				}
			}
		}
	}

	//write src raster to file
	src.Write(srcRas);
}

void IRasterAddBoolean(const std::string& ras1Str, const std::string& ras2Str, const std::string& resultStr) {

	//declare ras1 raster
	IRaster ras1;
	//setup ras1 raster
	ras1.Setup(ras1Str);
	//read ras1 raster
	ras1.Read(ras1Str);

	//declare ras2 raster
	IRaster ras2;
	//setup ras2 raster
	ras2.Setup(ras2Str);
	//read ras2 raster
	ras2.Read(ras2Str);

	//declare res raster
	IRaster res;
	//setup res raster
	res.Setup(ras1Str);

	//for all cells in res
	for (int r = 0; r != res.nrows; ++r) {
		for (int c = 0; c != res.ncols; ++c) {

			if (ras1.data[r][c] == ras1.NODATA_value) {		//where ras1 is NODATA_value
				res.data[r][c] = res.NODATA_value;				
			}
			else {
				res.data[r][c] = ras1.data[r][c] + ras2.data[r][c];
			}
		}
	}

	//write res raster to file
	res.Write(resultStr);
}

void Standardise(const std::string& srcRas, const std::string& maskRas) {

	//declare src raster
	DRaster src;
	//setup src raster
	src.Setup(srcRas);
	//read src raster
	src.Read(srcRas);

	//declare mask raster
	IRaster mask;
	//setup mask raster
	mask.Setup(maskRas);
	//read mask raster
	mask.Read(maskRas);

	//set src NODATA_value to mask NODATA_value
	src.NODATA_value = mask.NODATA_value;

	//set min and max
	double min = std::numeric_limits<double>::max();
	double max = std::numeric_limits<double>::min();

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {

			if (mask.data[r][c] == mask.NODATA_value) {		//mask is NODATA_value

				src.data[r][c] = src.NODATA_value;			//set src to NODATA_value
			}
			else {											//mask is not NODATA_value

				if (src.data[r][c] < min) {					//find min
					min = src.data[r][c];
				}
				if (src.data[r][c] > max) {					//find max
					max = src.data[r][c];
				}
			}			
		}
	}

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {
			if (src.data[r][c] != src.NODATA_value) {
				src.data[r][c] = (src.data[r][c] - min) / (max - min);		//standard polarity				
			}
		}
	}

	//write result to file
	src.Write(srcRas);
}

void RevPolarityStandardise(const std::string& srcRas, const std::string& maskRas) {

	//declare src raster
	DRaster src;
	//setup src raster
	src.Setup(srcRas);
	//read src raster
	src.Read(srcRas);

	//declare mask raster
	IRaster mask;
	//setup mask raster
	mask.Setup(maskRas);
	//read mask raster
	mask.Read(maskRas);

	//set src NODATA_value to mask NODATA_value
	src.NODATA_value = mask.NODATA_value;

	//set min and max
	double min = std::numeric_limits<double>::max();
	double max = std::numeric_limits<double>::min();

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {

			if (mask.data[r][c] == mask.NODATA_value) {		//mask is NODATA_value
				src.data[r][c] = src.NODATA_value;			//set src to NODATA_value
			}
			else {											//mask is not NODATA_value

				if (src.data[r][c] < min) {					//find min
					min = src.data[r][c];
				}
				if (src.data[r][c] > max) {					//find max
					max = src.data[r][c];
				}
			}
		}
	}

	//for all cells in srcRas
	for (int r = 0; r != src.nrows; ++r) {
		for (int c = 0; c != src.ncols; ++c) {
			if (src.data[r][c] != src.NODATA_value) {				
				src.data[r][c] = (max - src.data[r][c]) / (max - min);		//reverse polarity
			}
		}
	}

	//write result to file
	src.Write(srcRas);
}

void AreaFromRaster(const std::string& wardIDRas, int numWards, const std::string& wardDataRas, int refVal, const std::string& areaOutput) {

	//load wardID raster
	IRaster wardID;
	wardID.Setup(wardIDRas);
	wardID.Read(wardIDRas);

	//load wardData raster
	IRaster wardData;
	wardData.Setup(wardDataRas);
	wardData.Read(wardDataRas);

	//create a vector of wards whose size = numWards
	std::vector<RTWard> wards;
	for (int w = 0; w != numWards; ++w) {
		wards.push_back(RTWard());
	}

	//loop through all cells in wardID raster and gather cells into wards
	for (int r = 0; r != wardID.nrows; ++r) {
		for (int c = 0; c != wardID.ncols; ++c) {
			if (wardID.data[r][c] != wardID.NODATA_value) {										// ignore NODATA_value cells
				wards[wardID.data[r][c]].push_back(RTCell(c, r, wardID.data[r][c]));
			}
		}
	}

	//create storage for areaData
	std::vector<double> areaData(numWards);

	//loop through all cells in all wards 
	for (int w = 0; w != numWards; ++w) {
		int cellCount = 0;							//reset for each ward
		if (!wards[w].empty()) {
			for (size_t c = 0; c != wards[w].size(); ++c) {			

				if (wardData.data[wards[w][c].r][wards[w][c].c] == refVal) {		//count cells equal to refVal
					++cellCount;
				}
			}
		}
		areaData[w] = (double)cellCount * (wardData.cellsize * wardData.cellsize);				//multiply cellCount by cellSize^2
	}

	//write areaData to CSV	

	//create an ofstream object
	std::ofstream opfile(areaOutput);

	//set precision to max = 15 digits...	
	opfile.precision(std::numeric_limits<double>::digits10);
	//might need digits10 + 2 i.e. 17 digits if doubles need to be "round-trippable"

	//check the file opened OK
	if (opfile.is_open()) {
		
		//write header
		opfile << "Area" << "\n";

		for (int w = 0; w != numWards; ++w) {

			//write areaData
			opfile << areaData[w] << "\n";
		}

		//close opfile
		opfile.close();
	}
	else {
		std::cout << "Unable to open areaOutput file";
	}
}

void BooleanDownsampler(const std::string& inputRas, const std::string& outputRas, int singleDimCellsToCell, int twoDimCellThreshold) {

	//load inputRas
	IRaster in;
	in.Setup(inputRas);
	in.Read(inputRas);

	//copy inputRas header values
	int ncols = in.ncols;
	int nrows = in.nrows;
	int xllcorner = in.xllcorner;
	int yllcorner = in.yllcorner;
	int cellsize = in.cellsize;
	int NODATA_value = in.NODATA_value;

	//modify ncols,nrows,cellsize using singleDimCellsToCells
	ncols /= singleDimCellsToCell;
	nrows /= singleDimCellsToCell;
	cellsize *= singleDimCellsToCell;

	//setup outputRas
	IRaster out;
	out.Setup(ncols, nrows);	//all cells initialised to zero by default

	out.xllcorner = xllcorner;
	out.yllcorner = yllcorner;
	out.cellsize = cellsize;
	out.NODATA_value = NODATA_value;

	//set 2d stride to move to top left of each region to downsample
	for (int r = 0; r != in.nrows; r += singleDimCellsToCell) {
		for (int c = 0; c != in.ncols; c += singleDimCellsToCell) {

			//intitialise count for each region to downsample
			int count = 0;

			//set 2d loop to read region to be downsampled
			for (int dr = r; dr != r + singleDimCellsToCell; ++dr) {
				for (int dc = c; dc != c + singleDimCellsToCell; ++dc) {

					//count reference values
					if (in.data[dr][dc] == 1) {
						++count;
					}
				}
			}

			//if twoDimCellThreshold is reached set outputRas to 1
			if (count >= twoDimCellThreshold) {
				out.data[r / singleDimCellsToCell][c / singleDimCellsToCell] = 1;
			}
		}
	}

	//write results
	out.Write(outputRas);
}

void DRasterSumRows(const std::string& inputRas, const std::string& outputCSV) {

	DRaster ras;
	ras.Setup(inputRas);
	ras.Read(inputRas);

	std::vector<double> rows;

	for (int r = 0; r != ras.nrows; ++r) {
		
		double rowTotal = 0.0f;
		for (int c = 0; c != ras.ncols; ++c) {

			if (ras.data[r][c] != ras.NODATA_value) {
				rowTotal += ras.data[r][c];
			}
		}
		rows.push_back(rowTotal);
	}

	//create an ofstream object
	std::ofstream opfile(outputCSV);

	//set precision to max = 15 digits...	
	opfile.precision(std::numeric_limits<double>::digits10);
	//might need digits10 + 2 i.e. 17 digits if doubles need to be "round-trippable"

	//check the file opened OK
	if (opfile.is_open()) {

		//write header
		opfile << "row_total" << "\n";

		for (size_t r = 0; r != rows.size(); ++r) {
			opfile << rows[r] << "\n";
		}		

		//close opfile
		opfile.close();
	}
	else {
		std::cout << "Unable to open csv output file";
	}
}

void DRasterSumColumns(const std::string& inputRas, const std::string& outputCSV) {

	DRaster ras;
	ras.Setup(inputRas);
	ras.Read(inputRas);

	std::vector<double> rows;
	std::vector<double> cols;

	/*for (int r = 0; r != ras.nrows; ++r) {

		double rowTotal = 0.0f;
		for (int c = 0; c != ras.ncols; ++c) {

			if (ras.data[r][c] != ras.NODATA_value) {
				rowTotal += ras.data[r][c];
			}
		}
		rows.push_back(rowTotal);
	}*/

	for (int c = 0; c != ras.ncols; ++c) {

		double colTotal = 0.0f;
		for (int r = 0; r != ras.nrows; ++r) {

			if (ras.data[r][c] != ras.NODATA_value) {
				colTotal += ras.data[r][c];
			}
		}
		cols.push_back(colTotal);
	}

	//create an ofstream object
	std::ofstream opfile(outputCSV);

	//set precision to max = 15 digits...	
	opfile.precision(std::numeric_limits<double>::digits10);
	//might need digits10 + 2 i.e. 17 digits if doubles need to be "round-trippable"

	//check the file opened OK
	if (opfile.is_open()) {

		//write header
		//opfile << "row_total" << "\n";
		opfile << "col_total" << "\n";

		//for (int r = 0; r != rows.size(); ++r) {
		//	opfile << rows[r] << "\n";
		//}
		for (size_t c = 0; c != cols.size(); ++c) {
			opfile << cols[c] << "\n";
		}

		//close opfile
		opfile.close();
	}
	else {
		std::cout << "Unable to open csv output file";
	}
}

void DRasterAscToBin(const std::string& input, const std::string& output, const std::string& pathToBinaryConfig) {

	DRaster ras;
	ras.Setup(input);
	ras.Read(input);
	ras.ToPGBinary(pathToBinaryConfig, output);
}

void DRasterAscToCsv(const std::string& input, const std::string& output) {

	DRaster ras;
	ras.Setup(input);
	ras.Read(input);
	ras.ToCSV(output);
}

void DRasterBinToAsc(const std::string& input, const std::string& output, const std::string& hdrFile) {

	DRaster ras;
	ras.Setup(hdrFile);
	ras.FromPGBinary(input);
	ras.Write(output);
}

void DRasterCsvToAsc(const std::string& input, const std::string& output, const std::string& hdrFile) {

	DRaster ras;
	ras.Setup(hdrFile);
	ras.FromCSV(input);
	ras.Write(output);
}

void IRasterAscToBin(const std::string& input, const std::string& output, const std::string& pathToBinaryConfig) {

	IRaster ras;
	ras.Setup(input);
	ras.Read(input);
	ras.ToPGBinary(pathToBinaryConfig, output);
}

void IRasterAscToCsv(const std::string& input, const std::string& output) {

	IRaster ras;
	ras.Setup(input);
	ras.Read(input);
	ras.ToCSV(output);
}

void IRasterBinToAsc(const std::string& input, const std::string& output, const std::string& hdrFile) {

	IRaster ras;
	ras.Setup(hdrFile);
	ras.FromPGBinary(input);
	ras.Write(output);
}

void IRasterCsvToAsc(const std::string& input, const std::string& output, const std::string& hdrFile) {

	IRaster ras;
	ras.Setup(hdrFile);
	ras.FromCSV(input);
	ras.Write(output);
}

void DRasterAscToODVal(const std::string& inputRas, const std::string& inputZoneCodes, const std::string& outputCSV) {

	//setup double precision raster from csv
	DRaster ras;
	ras.Setup(inputRas);
	ras.Read(inputRas);	

	std::vector<std::string> zoneCodeStr(ras.nrows);	

	//read from input csv label file into zoneLabels
	ExtractCSV(inputZoneCodes, 1, 0, zoneCodeStr);

	//write 3-column csv using origin-destination-value format

	//create an ofstream object
	std::ofstream opfile(outputCSV);

	//set precision to max = 15 digits...	
	opfile.precision(std::numeric_limits<double>::digits10);
	//might need digits10 + 2 i.e. 17 digits if doubles need to be "round-trippable"

	//check the file opened OK
	if (opfile.is_open()) {

		//write header
		opfile << "ORIGIN_ZONE_CODE" << ",";
		opfile << "DESTINATION_ZONE_CODE" << ",";
		opfile << "GENERALISED_TRAVEL_COST" << "\n";

		//write data
		for (int r = 0; r != ras.nrows; ++r) {
			for (int c = 0; c != ras.ncols; ++c) {
				opfile << zoneCodeStr[r] << ",";
				opfile << zoneCodeStr[c] << ",";
				opfile << ras.data[r][c] << "\n";
			}
		}		

		//close opfile
		opfile.close();
	}
	else {
		std::cout << "Unable to open csv output file";
	}

}

void DRasterSubRaster(const std::string& inRasStr, const std::string& inCodeStr, const std::string& outRasStr, const std::string& outCodeStr, const std::string& outHdrFile) {

	//setup
	DRaster inRas;
	inRas.Setup(inRasStr);
	inRas.Read(inRasStr);

	std::vector<std::string> inCodes(inRas.nrows);

	//read from input csv 
	ExtractCSV(inCodeStr, 1, 0, inCodes);

	DRaster outRas;
	outRas.Setup(outHdrFile);

	std::vector<std::string> outCodes(outRas.nrows);

	//read from input csv 
	ExtractCSV(outCodeStr, 1, 0, outCodes);

	std::vector<int> outIndex(outRas.nrows);	

	for (int ocode = 0; ocode != outRas.nrows; ++ocode) {
		for (int icode = 0; icode != inRas.nrows; ++icode) {
			if (outCodes[ocode] == inCodes[icode]) {
				outIndex[ocode] = icode;
			}
		}
	}

	for (int ocode = 0; ocode != outRas.nrows; ++ocode) {
		std::cout << outCodes[ocode] << " = " << outIndex[ocode] << std::endl;
	}

	//use the outIndex[ocode] to copy values from input to output rasters	
	for (int r = 0; r != outRas.nrows; ++r) {
		for (int c = 0; c != outRas.ncols; ++c) {
			outRas.data[r][c] = inRas.data[outIndex[r]][outIndex[c]];
		}
	}

	outRas.Write(outRasStr);		

}

void IRasterDevToDPH(const std::string& devInStr, const std::string& dphInStr, const std::string& devOutStr, const std::string& dphOutStr) {

	IRaster devIn;
	devIn.Setup(devInStr);
	devIn.Read(devInStr);

	IRaster dphIn;
	dphIn.Setup(dphInStr);
	dphIn.Read(dphInStr);

	IRaster devOut;
	devOut.Setup(devIn.ncols, devIn.nrows, 0);
	devOut.cellsize = devIn.cellsize;
	devOut.xllcorner = devIn.xllcorner;
	devOut.yllcorner = devIn.yllcorner;
	devOut.NODATA_value = devIn.NODATA_value;

	IRaster dphOut;
	dphOut.Setup(dphIn.ncols, dphIn.nrows, 0);
	dphOut.cellsize = dphIn.cellsize;
	dphOut.xllcorner = dphIn.xllcorner;
	dphOut.yllcorner = dphIn.yllcorner;
	dphOut.NODATA_value = dphIn.NODATA_value;

	for (int r = 0; r != devIn.nrows; ++r) {
		for (int c = 0; c != devIn.ncols; ++c) {

			//set output dev and dph to nodata
			devOut.data[r][c] = devOut.NODATA_value;
			dphOut.data[r][c] = dphOut.NODATA_value;

			//copy dev to output
			devOut.data[r][c] = devIn.data[r][c];
			
			//for new dev
			if (devIn.data[r][c] == 2) {				

				//correct output dev for 0dph
				if (dphIn.data[r][c] == 0) {
					devOut.data[r][c] = 0;
				}

				//copy dph > 0 to output 
				if (dphIn.data[r][c] > 0) {
					dphOut.data[r][c] = dphIn.data[r][c];
				}
			}
		}
	}

	//write output dev and dph
	devOut.Write(devOutStr);
	dphOut.Write(dphOutStr);
}

void UrbanFabricGenerator(const std::string& in_dphPath, const std::string& out_dphPath, const std::string& in_tilePath, int in_numTiles) {

	//path to input / output data	
	std::string tilePath = in_tilePath;

	//name of tile table	
	std::string tileTable = tilePath + "in_tile_table.csv";

	//number of tiles in table
	int numTiles = in_numTiles;	

	//input dph and output downScaleDev rasters
	IRaster dph, uf;

	//setup and read input dph raster	
	std::string dphRas = in_dphPath;
	dph.Setup(dphRas);
	dph.Read(dphRas);

	//seup vector of tiles
	std::vector<RTTile> tiles;
	for (int t = 0; t != numTiles; ++t) {
		tiles.push_back(RTTile(0));
	}

	//storage to read tile rasters	
	std::vector<std::string> tileStr(numTiles);	

	//storage to read tile90 rasters	
	std::vector<std::string> tile90Str(numTiles);	

	//storage to read tile dph
	int* tileDPH;
	tileDPH = new int[numTiles];

	//std::vector<std::string>& data

	//read tile dph from input csv
	ExtractCSV(tileTable, 3, 0, tileStr);

	//convert to int
	for (int t = 0; t != numTiles; ++t) {
		tileDPH[t] = std::stoi(tileStr[t]);		
	}

	//std::stoi(s);

	//read tile raster namefrom input csv
	ExtractCSV(tileTable, 3, 1, tileStr);

	//read tile90 raster namefrom input csv
	ExtractCSV(tileTable, 3, 2, tile90Str);

	//copy data to vector of tiles
	for (int t = 0; t != numTiles; ++t) {
		tiles[t].dph = tileDPH[t];
		tiles[t].str = tileStr[t];
		tiles[t].str90 = tile90Str[t];
	}

	//sort tiles by dph
	std::sort(tiles.begin(), tiles.end(), LowestTileDPH);

	//output information for tiles ordered by dph 
	for (int t = 0; t != numTiles; ++t) {

		std::cout << "tile " << t << ":" << std::endl << std::endl;
		std::cout << "dph = " << tiles[t].dph << std::endl;
		std::cout << "str = " << tiles[t].str << std::endl;
		std::cout << "str90 = " << tiles[t].str90 << std::endl << std::endl;
	}

	//copy data read from tile table csv to vector of tiles
	for (int t = 0; t != numTiles; ++t) {
		tiles[t].ras.Setup(tilePath + tiles[t].str);
		tiles[t].ras.Read(tilePath + tiles[t].str);
		tiles[t].ras90.Setup(tilePath + tiles[t].str90);
		tiles[t].ras90.Read(tilePath + tiles[t].str90);
	}

	//linear xy scale - input dph raster is multiplied by this in each dimension
	int xyScale = tiles[0].ras.nrows;

	//setup output urban fabric raster	
	std::string urbanFabricRas = out_dphPath;
	uf.Setup(dph.ncols * xyScale, dph.nrows * xyScale, -1);	//initialise all cells to nodata value	
	uf.cellsize = dph.cellsize / xyScale;
	uf.xllcorner = dph.xllcorner;
	uf.yllcorner = dph.yllcorner;
	uf.NODATA_value = dph.NODATA_value;

	//setup random tile rotation
	std::vector<bool> rotate;
	rotate.push_back(true);
	rotate.push_back(false);

	//for all cells in input dph raster
	for (int r = 0; r != dph.nrows; ++r) {
		for (int c = 0; c != dph.ncols; ++c) {

			//setup random device for shuffle
			std::random_device rd;
			std::mt19937 g(rd());			

			//random tile rotation using above device			
			std::shuffle(rotate.begin(), rotate.end(), g);

			//apply to all valid development dph cells
			if (dph.data[r][c] > 0) {

				//have we found a tile with suitable dph? - not yet..
				bool found = false;

				//search for tile with lowest dph to house development
				for (int t = 0; t != numTiles; ++t) {

					//if we haven't already found a suitable tile and the current tile can house development at the input dph..
					if (!found && (tiles[t].dph > dph.data[r][c])) {

						//found a tile with suitable dph
						found = true;

						//copy tile data to output urban fabric raster
						for (int rPatch = 0; rPatch != xyScale; ++rPatch) {
							for (int cPatch = 0; cPatch != xyScale; ++cPatch) {

								//select either tile or rotated tile
								if (rotate[0]) {
									uf.data[(r * xyScale) + rPatch][(c * xyScale) + cPatch] = tiles[t].ras90.data[rPatch][cPatch];
								}
								else {
									uf.data[(r * xyScale) + rPatch][(c * xyScale) + cPatch] = tiles[t].ras.data[rPatch][cPatch];
								}
							}
						}
					}

				}

				//didn't find a tile with suitable dph - use tile with max available dph
				if (!found) {

					//report that suitable tile was not found
					std::cout << "Suitable tile dph not found for input dph = " << dph.data[r][c] << ", setting to max dph = " << tiles[numTiles - 1].dph << std::endl;

					//copy tile data to output urban fabric raster
					for (int rPatch = 0; rPatch != xyScale; ++rPatch) {
						for (int cPatch = 0; cPatch != xyScale; ++cPatch) {

							//select either tile or rotated tile with max available dph
							if (rotate[0]) {
								uf.data[(r * xyScale) + rPatch][(c * xyScale) + cPatch] = tiles[numTiles - 1].ras90.data[rPatch][cPatch];
							}
							else {
								uf.data[(r * xyScale) + rPatch][(c * xyScale) + cPatch] = tiles[numTiles - 1].ras.data[rPatch][cPatch];
							}
						}
					}
				}
			}
		}
	}

	//write urban fabric to raster
	uf.Write(urbanFabricRas);

	//free dynamically allocated memory	
	delete[] tileDPH;
	tiles.clear();
	rotate.clear();
}

void RasteriseAreaThresholds(const std::string& swapPath, const std::string& rastHdr, const std::string& constraintRas, const std::string& devRas, const std::string& inputTbl, int numTblRows, double summedLayerThreshold) {

	std::cout << "swapPath = " << swapPath << std::endl;

	//storage to read layer area rasters and thresholds
	std::vector<std::string> layerRasStr(numTblRows);
	std::vector<std::string> layerDevFlagStr(numTblRows);
	std::vector<std::string> layerThresholdStr(numTblRows);
	double* layerThreshold;
	double* layerThresholdArea;
	layerThreshold = new double[numTblRows];
	layerThresholdArea = new double[numTblRows];

	//inputTbl format: layerAreaRasterName | layerThresholdPercentageValue
	//inputTbl format: layerAreaRasterName | current_development_flag | layerThresholdPercentageValue

	bool* devFlag;
	devFlag = new bool[numTblRows];
	//read dev_flag column from csv
	ExtractCSV(inputTbl, 3, 1, layerDevFlagStr);
	//convert devFlag data to int
	for (int i = 0; i != numTblRows; ++i) {
		devFlag[i] = std::stoi(layerDevFlagStr[i]);
	}

	//read rasters column from csv
	//ExtractCSV(swapPath + inputTbl, 2, 0, layerRasStr);
	ExtractCSV(inputTbl, 3, 0, layerRasStr);

	//read thresholds column from csv
	//ExtractCSV(swapPath + inputTbl, 2, 1, layerThresholdStr);
	ExtractCSV(inputTbl, 3, 2, layerThresholdStr);

	//convert threshold data to double
	for (int i = 0; i != numTblRows; ++i) {
		layerThreshold[i] = std::stod(layerThresholdStr[i]);
	}

	//setup a vector of layer coverage rasters
	std::vector<IRaster> inputCoverage;

	//push back for each tbl row
	for (int i = 0; i != numTblRows; ++i) {
		inputCoverage.push_back(IRaster());
	}

	//setup and read rasters
	for (int i = 0; i != numTblRows; ++i) {

		inputCoverage[i].Setup(swapPath + layerRasStr[i]);
		inputCoverage[i].Read(swapPath + layerRasStr[i]);
		std::cout << "reading " << swapPath + layerRasStr[i] << std::endl;
	}

	//setup output coverage raster
	IRaster outputCoverage;
	outputCoverage.Setup(rastHdr);	//initialised to zero by default

	//calculate summed threshold area
	double summedThresholdArea;
	summedThresholdArea = (summedLayerThreshold / 100.0f) * (outputCoverage.cellsize * outputCoverage.cellsize);
	//cout << "summedThresholdArea " << summedThresholdArea << endl;

	//calculate individual constraint threshold areas
	for (int i = 0; i != numTblRows; ++i) {
		layerThresholdArea[i] = (layerThreshold[i] / 100.0f) * (outputCoverage.cellsize * outputCoverage.cellsize);
		//cout << "layerThresholdArea = " << layerThresholdArea[i] << endl;
	}

	//loop through all cells of all input layers
	for (int r = 0; r != outputCoverage.nrows; ++r) {
		for (int c = 0; c != outputCoverage.ncols; ++c) {

			//reset summedLayerArea for each cell
			double summedLayerArea = 0.0;

			for (int i = 0; i != numTblRows; ++i) {

				//add to summedLayerArea				
				summedLayerArea += inputCoverage[i].data[r][c];

				//test individual layer thresholds
				if (inputCoverage[i].data[r][c] > layerThresholdArea[i]) {
					outputCoverage.data[r][c] = 1;
				}
			}

			//cout << "summedLayerArea " << summedLayerArea << endl;

			//test summed layer threshold
			if (summedLayerArea > summedThresholdArea) {
				outputCoverage.data[r][c] = 1;
			}
		}
	}

	//write output coverage raster to file
	std::cout << "writing " << constraintRas << std::endl;
	outputCoverage.Write(constraintRas);

	//setup current development raster
	IRaster currentDev;
	currentDev.Setup(rastHdr);	//initialised to zero by default

	//find current development layer
	for (int i = 0; i != numTblRows; ++i) {
		if (devFlag[i]) {
			std::cout << "current development = " << layerRasStr[i] << std::endl;

			for (int r = 0; r != outputCoverage.nrows; ++r) {
				for (int c = 0; c != outputCoverage.ncols; ++c) {

					//test layer threshold
					if (inputCoverage[i].data[r][c] > layerThresholdArea[i]) {
						currentDev.data[r][c] = 1;
					}
				}
			}
		}
	}

	//write current development raster to file
	std::cout << "writing " << devRas << std::endl;
	currentDev.Write(devRas);

	//tidy up 	
	delete[] layerThreshold;
	delete[] layerThresholdArea;
	delete[] devFlag;
}

//void IRasterToHeader(const std::string& inputRaster, const std::string& outputHeader, const std::string& swapPath) {
//
//	//setup and read input raster
//	IRaster input;
//	input.Setup(swapPath + inputRaster);
//	input.Read(swapPath + inputRaster);
//
//	//create an ofstream object	to write header
//	std::ofstream opfile(swapPath + outputHeader);
//
//	//check the file opened OK
//	if (opfile.is_open()) {
//
//		//write header
//		opfile << "ncols" << " " << input.ncols << "\n";
//		opfile << "nrows" << " " << input.nrows << "\n";
//		opfile << "xllcorner" << " " << input.xllcorner << "\n";
//		opfile << "yllcorner" << " " << input.yllcorner << "\n";
//		opfile << "cellsize" << " " << input.cellsize << "\n";
//		opfile << "NODATA_value" << " " << input.NODATA_value;
//
//		//close opfile
//		opfile.close();
//	}
//	else {
//		std::cout << "Unable to open output file";
//	}
//}

void IRasterToHeader(const std::string& inputRaster, const std::string& outputHeader) {

	//setup and read input raster
	IRaster input;
	input.Setup(inputRaster);
	input.Read(inputRaster);

	//create an ofstream object	to write header
	std::ofstream opfile(outputHeader);

	//check the file opened OK
	if (opfile.is_open()) {

		//write header
		opfile << "ncols" << " " << input.ncols << "\n";
		opfile << "nrows" << " " << input.nrows << "\n";
		opfile << "xllcorner" << " " << input.xllcorner << "\n";
		opfile << "yllcorner" << " " << input.yllcorner << "\n";
		opfile << "cellsize" << " " << input.cellsize << "\n";
		opfile << "NODATA_value" << " " << input.NODATA_value;

		//close opfile
		opfile.close();
	}
	else {
		std::cout << "Unable to open output file";
	}
}

int ParameterFromHeader(const std::string& header, const std::string& parameter) {

	//template<typename T>
	//void Raster<T>::Setup(const std::string & ipfile) {

	int hdrNCols = 0;
	int hdrNRows = 0;
	int hdrXllcorner = 0;
	int hdrYllcorner = 0;
	int hdrCellsize = 0;
	int hdrNodata = 0;



		//declare an ifstream object
		//ifstream ipfileHeader ("testRaster.asc");
		std::ifstream ipfileHeader(header);

		//declare a string object
		std::string line, read, value;

		//boolean header complete
		bool headerComplete = false;

		//check the file opened OK
		if (ipfileHeader.is_open())
		{
			//std::cout << "Reading Header..." << std::endl << std::endl;

			while (ipfileHeader.good() && !headerComplete)
			{

				std::stringstream lineStream;

				//read a line from the file into string line
				getline(ipfileHeader, line);
				//and then into linestream
				lineStream << line;
				//std::cout << line << std::endl;

				//and then out to string read
				lineStream >> read;

				if (read == "ncols") {
					//std::cout << "ncols data detected.." << std::endl;
					lineStream >> value;
					hdrNCols = std::stoi(value);
					//std::cout << "ncols = " << ncols << std::endl;
				}

				if (read == "nrows") {
					//std::cout << "nrows data detected.." << std::endl;
					lineStream >> value;
					hdrNRows = std::stoi(value);
					//std::cout << "nrows = " << nrows << std::endl;
				}

				if (read == "xllcorner") {
					//std::cout << "xllcorner data detected.." << std::endl;
					lineStream >> value;
					hdrXllcorner = std::stoi(value);
					//std::cout << "xllcorner = " << xllcorner << std::endl;
				}

				if (read == "yllcorner") {
					//std::cout << "yllcorner data detected.." << std::endl;
					lineStream >> value;
					hdrYllcorner = std::stoi(value);
					//std::cout << "yllcorner = " << yllcorner << std::endl;
				}

				if (read == "cellsize") {
					//std::cout << "cellsize data detected.." << std::endl;
					lineStream >> value;
					hdrCellsize = std::stoi(value);
					//std::cout << "cellsize = " << cellsize << std::endl;
				}

				if (read == "NODATA_value") {
					//std::cout << "NODATA_value data detected.." << std::endl;
					lineStream >> value;
					//NODATA_value = getFromString(value);
					hdrNodata = std::stoi(value);
					//std::cout << "NODATA_value = " << NODATA_value << std::endl << std::endl;
					headerComplete = true;
				}
			}
		}
		ipfileHeader.close();

		int val = 0;

		if (parameter == "ncols") {
			val = hdrNCols;
		}

		if (parameter == "nrows") {
			val = hdrNRows;
		}

		if (parameter == "cellsize") {
			val = hdrCellsize;
		}

		//std::cout << "val = " << val << std::endl;

		return(val);

		//Setup(nrows, ncols, 0);
		//Setup(ncols, nrows, 0);
	//}
}

void IRasterSetToValue(const std::string& rasterHeader, int value, const std::string& outputRaster) {

	//setup output raster
	IRaster ras;
	ras.Setup(rasterHeader);

	//set all cells to value provided	
	for (int r = 0; r != ras.nrows; ++r) {
		for (int c = 0; c != ras.ncols; ++c) {
			ras.data[r][c] = value;
		}
	}
	
	//write output raster
	ras.Write(outputRaster);
}




