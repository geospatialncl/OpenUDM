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
    string swap_path = "C://Users//njv7//projects//CoreUDM//Data8//";
    
    string rast_hdr = swap_path + "rasterHeader.hdr";

    //string ward_id_str = swap_path + "glasgowZID.asc";
    string ward_id_str = swap_path + "zone_identity.asc";

    //generate raster header from zone identity raster
    IRasterToHeader(ward_id_str, rast_hdr);

    //extract variables from raster header

    int hdrCols = 0;
    int hdrRows = 0;
    int hdrCellsize = 0;

    hdrCols = ParameterFromHeader(rast_hdr, "ncols");
    hdrRows = ParameterFromHeader(rast_hdr, "nrows");
    hdrCellsize = ParameterFromHeader(rast_hdr, "cellsize");

    cout << "ncols = " << hdrCols << endl;
    cout << "nRows = " << hdrRows << endl;
    cout << "cellsize = " << hdrCellsize << endl;

    //test fully qualified filename
    cout << "rast_hdr = " << rast_hdr << endl;

    bool binary = false;
    bool reverse = false;

    //coverage to constraint

    string constraint_ras = swap_path + "constraint.asc";
    //string tbl = "glasgowTBL.csv";
    string tbl = "constraints.csv";

    string current_dev_ras = swap_path + "current_development.asc";

    //void RasteriseAreaThresholds(const std::string & swapPath, const std::string & rastHdr, const std::string & constraintRas, const std::string & devRas,const std::string & inputTbl, int numTblRows, double summedLayerThreshold);
    RasteriseAreaThresholds(swap_path, rast_hdr, constraint_ras, current_dev_ras, tbl, 5, 50.0);

    //flip constraint
    IRasterNotBoolean(constraint_ras);

    //Multi Criteria Evaluation--------------------------------------------------------------

    //string mce_i_raster_str = swap_path + "in_mce_ras_int.csv";
    //string mce_d_raster_str = swap_path + "in_mce_ras_dbl.csv";
    string attractors_csv = swap_path + "attractors.csv";

    //string mce_i_raster_count_str = swap_path + "mce_int_count.csv";
    //string mce_d_raster_count_str = swap_path + "mce_dbl_count.csv";
    //string attractors_count_str = swap_path + "attractors_count.csv";

    string mce_output_raster_str = swap_path + "mceOutput.asc";    

    //MaskedWeightedSum(binary,mce_i_raster_count_str,mce_i_raster_str,mce_d_raster_count_str,mce_d_raster_str,mce_output_raster_str,rast_hdr,swap_path,reverse);
    //MaskedWeightedSum(binary, constraint_ras, mce_d_raster_count_str, mce_d_raster_str, mce_output_raster_str, rast_hdr, swap_path, reverse);
    //MaskedWeightedSum(binary, constraint_ras, attractors_count_str, attractors_csv, mce_output_raster_str, rast_hdr, swap_path, reverse);
    MaskedWeightedSum(binary, constraint_ras, 3, attractors_csv, mce_output_raster_str, rast_hdr, swap_path, reverse);

    //---------------------------------------------------------------------------------------

    //Create Development Zones---------------------------------------------------------------

     bool moore = false;
     int min_dev_area = 4;
     
     //string mask_str = swap_path + "glasgowConstraint.asc"; 
     //string mask_str = swap_path + "constraint.asc";
     string zone_id_str = swap_path + "zoneID.asc";   
     
     //string ward_id_str = swap_path + "glasgowZID.asc";
     //generate raster header from zone identity raster
     //IRasterToHeader(ward_id_str, rast_hdr);

    //CreateDevZones(binary, min_dev_area, moore, mask_str, zone_id_str, rast_hdr, swap_path, grid_id_csv);
    CreateDevZones(binary, min_dev_area, moore, constraint_ras, zone_id_str, rast_hdr, swap_path, ward_id_str);

    //---------------------------------------------------------------------------------------

    //Development Zones AVG Suitability------------------------------------------------------

    string zone_avg_str = swap_path + "zoneAVG.asc";  

    DevZoneAVGSuit(binary, zone_id_str, mce_output_raster_str, zone_avg_str, rast_hdr, swap_path);

    //---------------------------------------------------------------------------------------

    //Variable Density Urban Development Model-----------------------------------------------

    CellularModel cm;

    int num_zones = 8;
    //int cell_size = 100;
    //int num_cols = 950;
    //int num_rows = 910;

    //cm.Setup(num_zones,cell_size,num_cols,num_rows);
    cm.Setup(num_zones, hdrCellsize, hdrCols, hdrRows);

    cm.UseBinaryRasters(binary);

    cm.SetRasterHeader(rast_hdr);

    cm.SetPathToBinaryConfigFiles(swap_path);

    //string wards_str = swap_path + "in_zone_order.csv";
    string pop_str = swap_path + "population.csv";

    //int label_total = 1;
    //int label_col = 0;
    int label_total = 4;
    int label_col = 1;

    cm.LoadWardLabels(pop_str,label_col,label_total);

    //string pop_str = swap_path + "in_zonal_pop.csv";
    //string pop_str = swap_path + "population.csv";

    //int pop_total = 2;
    //int cur_pop_col = 0;
    //int fut_pop_col = 1;
    int pop_total = 4;
    int cur_pop_col = 2;
    int fut_pop_col = 3;

    cm.LoadCurrentPopulation(pop_str,cur_pop_col,pop_total);
    cm.LoadFuturePopulation(pop_str,fut_pop_col,pop_total);

    //bool variable_density = false;
    //string dph_raster_csv = swap_path + "expansion_dwellings_high.asc";
    //if (variable_density) {
    //    cm.LoadDwellingsRaster(dph_raster_csv);
    //}

    //string ward_id_str = swap_path + "exampleZoneIdentity.csv";
    //string ward_id_str = swap_path + "rasSingleZoneID.csv";

    cm.LoadWardIDRaster(ward_id_str);

    cm.LoadZoneIDRaster(zone_id_str);

    cm.LoadZoneAVGRaster(zone_avg_str);

    //string current_dev_ras = swap_path + "glasgowDev.asc";    

    cm.LoadDevLandRaster(current_dev_ras);

    cm.LoadCellSuitRaster(mce_output_raster_str);

    //---------------------------------------------------------------------------------------

    string cell_dev_output_str = swap_path + "cellDev.asc";
    string overflow_str = swap_path + "out_cell_overflow.csv";

    cm.RunModel();
    cm.OutputRasterResult(cell_dev_output_str);
    cm.WriteOverflowWards(overflow_str);

    //string cell_dev_csv = swap_path + "cellDev.csv";
    //string cell_dev_asc = swap_path + "out_cell_dev_test.asc";
    //IRasterCsvToAsc(cell_dev_csv, cell_dev_asc, rast_hdr);

}