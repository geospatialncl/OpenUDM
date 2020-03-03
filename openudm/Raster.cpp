#include "Raster.h"


template<>
Raster<int>::Raster()
  :	//header data
  ncols(0),
  nrows(0),
  xllcorner(0.0),
  yllcorner(0.0),
  cellsize(100.0),
  NODATA_value(-9999)
{ }

template<>
Raster<double>::Raster()
  :	//header data
  ncols(0),
  nrows(0),
  xllcorner(0.0),
  yllcorner(0.0),
  cellsize(100.0),
  NODATA_value(-1.0)
{ }


// specialise for int
template<>
int Raster<int>::getFromString(const std::string& s)
{
  return std::stoi(s);
}

// specialise for double
template<>
double Raster<double>::getFromString(const std::string& s)
{
  return std::stod(s);
}

