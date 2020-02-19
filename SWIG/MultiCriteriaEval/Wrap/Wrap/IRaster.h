#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

class IRaster
{
public:
	//constructor/destructor
	IRaster(void);
	~IRaster(void);

	//header data
	int ncols;
	int nrows;
	double xllcorner;
	double yllcorner;
	double cellsize;
	int NODATA_value;
	//float weight;

	//raster data
	int** data;	

	void Setup(std::string ipfile);
	void Setup(int ncols, int nrows, int init = 0);
	void Cleanup();
	void Read(std::string ipfile);
	void Write(std::string writefile);
	void FromPGBinary(std::string binData);
	void ToPGBinary(std::string binData);
	void ToPGBinary(std::string hdrPadFtrPath, std::string binData);
	void FromCSV(std::string csvFile);
	void ToCSV(std::string csvFile);
};

