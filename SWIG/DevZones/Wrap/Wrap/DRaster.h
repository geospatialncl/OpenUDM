#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

class DRaster
{
public:
	//constructor/destructor
	DRaster(void);
	~DRaster(void);

	//header data
	int ncols;
	int nrows;
	double xllcorner;
	double yllcorner;
	double cellsize;
	double NODATA_value;
	//float weight;

	//raster data
	double** data;	

	void Setup(std::string ipfile);
	void Setup(int ncols, int nrows);
	void Cleanup();
	void Read(std::string ipfile);
	void Write(std::string writefile);
	void FromPGBinary(std::string binData);
	void ToPGBinary(std::string binData);
	void ToPGBinary(std::string hdrPadFtrPath, std::string binData);
	void FromCSV(std::string csvFile);
	void ToCSV(std::string csvFile);
};

