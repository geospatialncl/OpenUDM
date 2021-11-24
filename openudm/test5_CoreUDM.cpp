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

    //NEW INTERFACE BEGIN

    //RASTER HEADER AND HARDCODED NAMES

    //set the raster header name - including swap_path
    string rast_hdr = swap_path + "rasterHeader.hdr";

    //set the zone identity raster name - including swap_path    
    string zone_id_ras = swap_path + "zone_identity.asc";

    //set name of combined constraint raster - including swap_path
    string constraint_ras = swap_path + "constraint.asc";

    //set name of current development raster - including swap_path 
    string current_dev_ras = swap_path + "current_development.asc";

    //set name of suitability raster (mce output) - including swap_path   
    string cell_suit_ras = swap_path + "out_cell_suit.asc";

    //set name of development area identity raster - including swap_path    
    string dev_area_id_ras = swap_path + "dev_area_id.asc";

    //set name of development area average suitability raster - including swap_path    
    string dev_area_suit_ras = swap_path + "dev_area_suit.asc";

    //set name of output development raster - including swap_path    
    string cell_dev_output_ras = swap_path + "out_cell_dev.asc";

    //TABLE NAMES

    //set name of constraints table - including swap_path    
    string constraints_tbl = swap_path + "constraints.csv";

    //set name of attractors table - including swap_path    
    string attractors_tbl = swap_path + "attractors.csv";

    //set name of population table - including swap_path    
    string population_tbl = swap_path + "population.csv";

    //set name of overflow data table - including swap_path    
    string overflow_tbl = swap_path + "out_cell_overflow.csv";

    //NEW INTERFACE END

    //PARAMETERS FROM TABLES

    //number of rows in population_tbl
    int num_zones = 8;

    //number of rows in constraints_tbl
    int num_constraints = 5;

    //number of rows in attractors_tbl
    int num_attractors = 3;

    //PARAMETERS FROM RASTER HEADER

    int ras_columns = 0;
    int ras_rows = 0;
    int ras_cellsize = 0;

    //generate raster header from zone identity raster
    IRasterToHeader(zone_id_ras, rast_hdr);

    //extract variables from raster header - cellsize and number of rows/columns
    //these variables are used in the cellular model setup function
    ras_columns = ParameterFromHeader(rast_hdr, "ncols");
    ras_rows = ParameterFromHeader(rast_hdr, "nrows");
    ras_cellsize = ParameterFromHeader(rast_hdr, "cellsize");

    //test
    cout << "ras_columns = " << ras_columns << endl;
    cout << "ras_rows = " << ras_rows << endl;
    cout << "ras_cellsize = " << ras_cellsize << endl;   
    cout << "rast_hdr = " << rast_hdr << endl;

    //boolean flags which should be read from parameters
    bool binary = false;
    bool reverse = false;

    //COVERAGE TO CONSTRAINT    

    //generate combined constraint and current development rasters
    RasteriseAreaThresholds(swap_path, rast_hdr, constraint_ras, current_dev_ras, constraints_tbl, num_constraints, 50.0);

    //flip constraint raster to form boolean suitability
    IRasterNotBoolean(constraint_ras);
    
    //MULTI CRITERIA EVALUATION

    //generate suitability raster
    MaskedWeightedSum(binary, constraint_ras, num_attractors, attractors_tbl, cell_suit_ras, rast_hdr, swap_path, reverse);

    //CREATE DEVELOPMENT AREAS

    //should be read from parameters
    bool moore = false;
    int min_dev_area = 4;    

    //generate development area identity raster
    CreateDevZones(binary, min_dev_area, moore, constraint_ras, dev_area_id_ras, rast_hdr, swap_path, zone_id_ras);
     
    //COMPUTE DEVELOPMENT AREA SUITABILITY

    //generate development area average suitability raster
    DevZoneAVGSuit(binary, dev_area_id_ras, cell_suit_ras, dev_area_suit_ras, rast_hdr, swap_path);
    
    //URBAN DEVELOPMENT MODEL

    //cellular model class
    CellularModel cm;    

    cm.Setup(num_zones, ras_cellsize, ras_columns, ras_rows);

    cm.UseBinaryRasters(binary);

    cm.SetRasterHeader(rast_hdr);

    cm.SetPathToBinaryConfigFiles(swap_path);    

    //set parameters to read zone codes from population table
    int label_total = 4;
    int label_col = 1;

    //read zone codes from population table
    cm.LoadWardLabels(population_tbl, label_col, label_total);

    //set parameters to read population data from population table
    int pop_total = 4;
    int cur_pop_col = 2;
    int fut_pop_col = 3;

    //read population data from population table
    cm.LoadCurrentPopulation(population_tbl, cur_pop_col, pop_total);
    cm.LoadFuturePopulation(population_tbl, fut_pop_col, pop_total);

    //load zone identity raster
    cm.LoadWardIDRaster(zone_id_ras);

    //load development area identity raster
    cm.LoadZoneIDRaster(dev_area_id_ras);

    //load development area average suitability raster
    cm.LoadZoneAVGRaster(dev_area_suit_ras);

    //load current development raster
    cm.LoadDevLandRaster(current_dev_ras);

    //load development suitability raster
    cm.LoadCellSuitRaster(cell_suit_ras);    

    cm.RunModel();

    cm.OutputRasterResult(cell_dev_output_ras);
    cm.WriteOverflowWards(overflow_tbl);
}