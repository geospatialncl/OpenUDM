#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

class IRaster final
{
public:
	//constructor/destructor
	IRaster();
	~IRaster();

	//header data
	int ncols;
	int nrows;
	double xllcorner;
	double yllcorner;
	double cellsize;
	static int NODATA_value;
	//float weight;

	//raster data
	std::vector<std::vector<int>> data;	

	void Setup(const std::string& ipfile);
	void Setup(int ncols, int nrows, int init = 0);
	void Cleanup();
	void Read(const std::string& ipfile);
	void Write(const std::string& writefile);
	void FromPGBinary(const std::string& binData);
	void ToPGBinary(const std::string& binData);
	void ToPGBinary(const std::string& hdrPadFtrPath, const std::string& binData);
	void FromCSV(const std::string& csvFile);
	void ToCSV(const std::string& csvFile);
};

