#include <iostream>
#include <vector>
#include <string>

#include "MultiCriteriaEval.h"
#include "DevZones.h"
#include "CellularModel.h"

#include "RasterToolkit.h"

using namespace std;

int main()
{
    //set the swap_path - shared data folder for UDM
    string swap_path = "C://Users//njv7//projects//CoreUDM//Data8//";

    //set the raster header name - including swap_path
    string rast_hdr = swap_path + "rasterHeader.hdr";

    //set the zone identity raster name - including swap_path
    string ward_id_str = swap_path + "zone_identity.asc";

    //generate raster header from zone identity raster
    IRasterToHeader(ward_id_str, rast_hdr);

    //extract variables from raster header - cellsize and number of rows/columns
    int hdrCols = 0;
    int hdrRows = 0;
    int hdrCellsize = 0;

    //these variables are used in the cellular model setup function
    hdrCols = ParameterFromHeader(rast_hdr, "ncols");
    hdrRows = ParameterFromHeader(rast_hdr, "nrows");
    hdrCellsize = ParameterFromHeader(rast_hdr, "cellsize");

    cout << "ncols = " << hdrCols << endl;
    cout << "nRows = " << hdrRows << endl;
    cout << "cellsize = " << hdrCellsize << endl;

    //test fully qualified filename
    cout << "rast_hdr = " << rast_hdr << endl;

    //boolean flags which should be read from parameters
    bool binary = false;
    bool reverse = false;

    //coverage to constraint

    //set name of combined constraint raster - including swap_path
    string constraint_ras = swap_path + "constraint.asc";

    //set name of constraints table - NOT including swap_path
    string tbl = "constraints.csv";

    //set name of current development raster - including swap_path 
    string current_dev_ras = swap_path + "current_development.asc";

    //generate combined constraint and current development rasters
    RasteriseAreaThresholds(swap_path, rast_hdr, constraint_ras, current_dev_ras, tbl, 5, 50.0);

    //flip constraint raster to form boolean suitability
    IRasterNotBoolean(constraint_ras);

    //Multi Criteria Evaluation--------------------------------------------------------------

    //set name of attractors table - including swap_path
    string attractors_csv = swap_path + "attractors.csv";

    //set name of suitability raster (mce output) - including swap_path
    string mce_output_raster_str = swap_path + "mceOutput.asc";

    //generate suitability raster
    MaskedWeightedSum(binary, constraint_ras, 3, attractors_csv, mce_output_raster_str, rast_hdr, swap_path, reverse);

    //---------------------------------------------------------------------------------------

    //Create Development Zones---------------------------------------------------------------

    //should be read from parameters
    bool moore = false;
    int min_dev_area = 4;

    //set name of development area identity raster - including swap_path
    string zone_id_str = swap_path + "zoneID.asc";

    //generate development area identity raster
    CreateDevZones(binary, min_dev_area, moore, constraint_ras, zone_id_str, rast_hdr, swap_path, ward_id_str);

    //---------------------------------------------------------------------------------------

    //Development Zones AVG Suitability------------------------------------------------------

    //set name of development area average suitability raster - including swap_path
    string zone_avg_str = swap_path + "zoneAVG.asc";

    //generate development area average suitability raster
    DevZoneAVGSuit(binary, zone_id_str, mce_output_raster_str, zone_avg_str, rast_hdr, swap_path);

    //---------------------------------------------------------------------------------------

    //Variable Density Urban Development Model-----------------------------------------------

    //cellular model class
    CellularModel cm;

    //number of zones - should be generated from number of rows in population table
    int num_zones = 8;

    cm.Setup(num_zones, hdrCellsize, hdrCols, hdrRows);

    cm.UseBinaryRasters(binary);

    cm.SetRasterHeader(rast_hdr);

    cm.SetPathToBinaryConfigFiles(swap_path);

    //set name of population table - including swap_path
    string pop_str = swap_path + "population.csv";

    //set parameters to read zone codes from population table
    int label_total = 4;
    int label_col = 1;

    //read zone codes from population table
    cm.LoadWardLabels(pop_str, label_col, label_total);

    //set parameters to read population data from population table
    int pop_total = 4;
    int cur_pop_col = 2;
    int fut_pop_col = 3;

    //read population data from population table
    cm.LoadCurrentPopulation(pop_str, cur_pop_col, pop_total);
    cm.LoadFuturePopulation(pop_str, fut_pop_col, pop_total);

    //load zone identity raster
    cm.LoadWardIDRaster(ward_id_str);

    //load development area identity raster
    cm.LoadZoneIDRaster(zone_id_str);

    //load development area average suitability raster
    cm.LoadZoneAVGRaster(zone_avg_str);

    //load current development raster
    cm.LoadDevLandRaster(current_dev_ras);

    //load development suitability raster
    cm.LoadCellSuitRaster(mce_output_raster_str);

    //---------------------------------------------------------------------------------------

    //set name of output development raster - including swap_path
    string cell_dev_output_str = swap_path + "out_cell_dev.asc";

    //set name of overflow data table - including swap_path
    string overflow_str = swap_path + "out_cell_overflow.csv";

    cm.RunModel();
    cm.OutputRasterResult(cell_dev_output_str);
    cm.WriteOverflowWards(overflow_str);
}