#!/usr/bin/env  python
# -*- coding: utf-8 -*-
"""
udm_solo.py - Generic standalone model to carry out multi-criteria evaluation, zone id creation, zone avg suitability, and cellular urban development.

"""
__author__ = "James Virgo"

import csv
import sys
import RasterToolkit as rt
import MultiCriteriaEval as mce
import DevZones as dz
import CellularModel
import time

def main():

    ### GENERIC INTERFACE BEGIN----------------------------------------------------------------------------------------

    #edit the following line according to installation
    swap_path = 'C:/UDM_Solo/Data/'

    #n.b. could take swap_path as command line argument as in
    #for arg in sys.argv[1:]:
    #    swap_path = arg

    # INDATA BEGIN---------------------------------------------------------------------------------

    db_conn_str = 'no_db'
    rast_hdr = 'rasterHeader.hdr'

    # INPUT STRINGS
    mce_i_raster_str = swap_path + 'in_mce_ras_int.csv'
    mce_d_raster_str = swap_path + 'in_mce_ras_dbl.csv'
    cell_i_raster_str = swap_path + 'in_udm_ras.csv'
    wards_str = swap_path + 'in_zone_order.csv'
    pop_str = swap_path + 'in_zonal_pop.csv'
    density_str = swap_path + 'in_zonal_density.csv'

    mce_i_rasters = mce_i_raster_str
    mce_d_rasters = mce_d_raster_str
    cell_i_rasters = cell_i_raster_str
    ordered_wards = wards_str
    zonal_pop_output = pop_str
    driver_name = 'no_driver'
    density = density_str

    # INDATA END-----------------------------------------------------------------------------------

    # OUTDATA BEGIN--------------------------------------------------------------------------------

    cell_dev_asc_str = swap_path + 'out_cell_dev.asc'
    overflow_str = swap_path + 'out_cell_overflow.csv'

    output_raster = cell_dev_asc_str
    overflow_data = overflow_str

    # OUTDATA END----------------------------------------------------------------------------------

    # PARAMETERS BEGIN-----------------------------------------------------------------------------

    params_str = swap_path + 'in_params.csv'

    #import parameters
    with open(params_str) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            num_zones = int(row['num_zones'])
            density_provided = int(row['density_provided'])
            min_dev_area = int(row['min_dev_area'])
            cell_size = int(row['cell_size'])
            num_cols = int(row['num_cols'])
            num_rows = int(row['num_rows'])

    #params for table reading - hardcoded to simplify
    label_total = 1
    label_col = 0

    #params for table reading - hardcoded to simplify
    pop_total = 2
    cur_pop_col = 0
    fut_pop_col = 1

    #params not relevant to this udm standalone version - leave as 0
    bin_ras = 0
    unlog_ras = 0
    is_driven = 0
    reverse = 0
    moore = 0

    # PARAMETERS END-------------------------------------------------------------------------------

    ### GENERIC INTERFACE END------------------------------------------------------------------------------------------

    # OUTPUT STRINGS
    full_rast_hdr = swap_path + rast_hdr
    mce_i_raster_count_str = swap_path + 'mce_int_count.csv'
    mce_d_raster_count_str = swap_path + 'mce_dbl_count.csv'

    mce_output_raster_str = ''
    zone_id_str = ''
    zone_avg_str = ''
    cell_dev_output_str = ''

    if bin_ras:
        mce_output_raster_str = swap_path + 'mceOutput.bin'
        zone_id_str = swap_path + 'zoneID.bin'
        zone_avg_str = swap_path + 'zoneAVG.bin'
        cell_dev_output_str = swap_path + 'cellDev.bin'
    else:
        mce_output_raster_str = swap_path + 'mceOutput.csv'
        zone_id_str = swap_path + 'zoneID.csv'
        zone_avg_str = swap_path + 'zoneAVG.csv'
        cell_dev_output_str = swap_path + 'cellDev.csv'

    ### MULTI CRITERIA EVALUATION------------------------------------------------------------------------

    #import table and convert rasters from .asc named in 'asc' column to .csv named in 'csv' column
    with open(mce_i_raster_str) as csvfile:
        reader = csv.DictReader(csvfile)
        num_ras = 0
        for row in reader:
            num_ras += 1
            if row['convert'] is 'y':
                #rt.IRasterAscToCsv(row['asc'], row['csv'])
                rt.IRasterAscToCsv(swap_path + row['asc'], swap_path + row['csv'])

    #write num_ras to csv
    with open(mce_i_raster_count_str, "wb") as f:
        writer = csv.writer(f)
        writer.writerow(['row_count'])
        writer.writerow([num_ras])

    #import table and convert rasters from .asc named in 'asc' column to .csv named in 'csv' column
    with open(mce_d_raster_str) as csvfile:
        reader = csv.DictReader(csvfile)
        num_ras = 0
        for row in reader:
            num_ras += 1
            if row['convert'] is 'y':
                #rt.DRasterAscToCsv(row['asc'], row['csv'])
                rt.DRasterAscToCsv(swap_path + row['asc'], swap_path + row['csv'])

    #write num_ras to csv
    with open(mce_d_raster_count_str, "wb") as f:
        writer = csv.writer(f)
        writer.writerow(['row_count'])
        writer.writerow([num_ras])

    # CALL SWIG-WRAPPED C++ FUNCTION----------------------------------------------------------------------

    #set bval based upon boolean input (bin_ras) - it can then be tested in place as function argument
    bval = 0
    if bin_ras:
        bval = 1

    #set rval based upon boolean input (reverse) - it can then be tested in place as function argument
    rval = 0
    if reverse:
        rval = 1


    mce.MaskedWeightedSum((bval>0),mce_i_raster_count_str,mce_i_raster_str,mce_d_raster_count_str,mce_d_raster_str,mce_output_raster_str,full_rast_hdr,swap_path,(rval>0))

    ###ZONE IDS-------------------------------------------------------------------------------------------

    #setup stack to hold raster filenames
    stack = []

    #import table and convert rasters from .asc named in 'asc' column to .csv named in 'csv' column
    with open(cell_i_raster_str) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            stack.append(row['csv'])
            if row['convert'] is 'y':
                #rt.IRasterAscToCsv(row['asc'], row['csv'])
                rt.IRasterAscToCsv(swap_path + row['asc'], swap_path + row['csv'])

    #retrieve filenames from stack
    cur_dev_str = swap_path + stack.pop()
    ward_id_str = swap_path + stack.pop()
    mask_str = swap_path + stack.pop()

    #set mval based upon boolean input (moore) - it can then be tested in place as an argument to CreateDevZones()
    mval = 0
    if moore:
        mval = 1

    # CALL SWIG-WRAPPED C++ FUNCTION------------------------------------------------------------------------

    dz.CreateDevZones((bval>0), min_dev_area, (mval>0), mask_str, zone_id_str, full_rast_hdr, swap_path, ward_id_str)

    ###ZONE AVG---------------------------------------------------------------------------------------------

    # CALL SWIG-WRAPPED C++ FUNCTION------------------------------------------------------------------------

    dz.DevZoneAVGSuit((bval>0), zone_id_str, mce_output_raster_str, zone_avg_str, full_rast_hdr, swap_path)


    ###CELLULAR MODEL---------------------------------------------------------------------------------------

    # CALL SWIG-WRAPPED C++ FUNCTION-------------------------------------------------------------------------

    cm = CellularModel.CellularModel()

    cm.Setup(num_zones, cell_size, num_cols, num_rows)

    cm.UseBinaryRasters((bval>0))
    cm.SetRasterHeader(full_rast_hdr)
    cm.SetPathToBinaryConfigFiles(swap_path)

    cm.LoadWardLabels(wards_str, label_col, label_total)
    cm.LoadCurrentPopulation(pop_str, cur_pop_col, pop_total)
    cm.LoadFuturePopulation(pop_str, fut_pop_col, pop_total)

    if density_provided:
        cm.LoadWardDensity(density_str,0,1)

    cm.LoadWardIDRaster(ward_id_str)
    cm.LoadZoneIDRaster(zone_id_str)
    cm.LoadZoneAVGRaster(zone_avg_str)
    cm.LoadDevLandRaster(cur_dev_str)
    cm.LoadCellSuitRaster(mce_output_raster_str)

    cm.RunModel()

    cm.OutputRasterResult(cell_dev_output_str)

    cm.WriteOverflowWards(overflow_str)

    cm.Cleanup();

    # WRITE RESULTS TO RASTER-----------------------------------------------------------------------------------

    #convert development output raster from csv to asc
    rt.IRasterCsvToAsc(cell_dev_output_str,cell_dev_asc_str,full_rast_hdr)

    ### METADATA------------------------------------------------------------------------------------------

    name = []
    value = []

    name.append("model")
    value.append("udm_solo")

    ###

    # INDATA
    name.append("indata")
    value.append("indata")

    name.append("db_conn_str")
    name.append("path_to_swap_data")
    name.append("rast_hdr")
    name.append("mce_i_raster_inputs")

    value.append(db_conn_str)
    value.append(swap_path)
    value.append(rast_hdr)
    value.append(mce_i_rasters)

    ### MCE I RASTERS
    mce_iras_str = mce_i_raster_str

    # reimport table to python
    with open(mce_iras_str) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            name.append("raster")
            value.append(row['asc'])
            name.append("weight")
            value.append(row['weight'])

    ###

    name.append("mce_d_raster_inputs")
    value.append(mce_d_rasters)

    ### MCE D RASTERS
    mce_dras_str = mce_d_raster_str

    # reimport table to python
    with open(mce_dras_str) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            name.append("raster")
            value.append(row['asc'])
            name.append("weight")
            value.append(row['weight'])

    ###

    name.append("cell_i_raster_inputs")
    value.append(cell_i_rasters)

    ### CELL I RASTERS
    cell_iras_str = cell_i_raster_str

    # reimport table to python
    with open(cell_iras_str) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            name.append("raster")
            value.append(row['asc'])

    ###

    name.append("ordered_wards")
    name.append("zonal_pop_output")

    value.append(ordered_wards)
    value.append(zonal_pop_output)

    name.append("driver_name")
    value.append(driver_name)

    name.append("density")
    value.append(density)


    # OUTDATA
    name.append("outdata")
    value.append("outdata")

    name.append("output_raster")
    name.append("overflow_data")

    value.append(output_raster)
    value.append(overflow_data)

    # PARAMETERS
    name.append("parameters")
    value.append("parameters")

    name_p = ["min_dev_area","moore","num_zones","cell_size","num_cols","num_rows","label_total","label_col","pop_total","cur_pop_col","fut_pop_col",
              "bin_ras","unlog_ras","is_driven","density_provided","reverse"]
    name.extend(name_p)

    value_p = [min_dev_area,moore,num_zones,cell_size,num_cols,num_rows,label_total,label_col,pop_total,cur_pop_col,fut_pop_col,bin_ras,unlog_ras,is_driven,density_provided,reverse]
    value.extend(value_p)

    ###

    #create metadata list of lists
    md = [name, value]

    #zip metadata into name-value rows
    md = zip(*md)

    #set metadata csv output string
    md_str = swap_path + 'out_cell_metadata.csv'

    #write metadata to csv
    with open(md_str, "wb") as f:
        writer = csv.writer(f)
        writer.writerows(md)

    ### END OF METADATA

    return False

if __name__ == "__main__":
    main()
