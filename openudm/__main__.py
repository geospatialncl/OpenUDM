#!/usr/bin/env  python
# -*- coding: utf-8 -*-
"""
udm_solo.py - Generic standalone model to carry out multi-criteria evaluation, zone id creation, zone avg suitability, and cellular urban development.

"""
from __future__ import print_function
__author__ = "James Virgo"

import csv
import os
import sys

from openudm import (
    CellularModel, DevZones as dz, MultiCriteriaEval as mce, RasterToolkit as rt)

def main(swap_path):

    ### GENERIC INTERFACE BEGIN----------------------------------------------------------------------------------------

    # NEW INTERFACE

    # HARDCODED CONTROL PARAMETERS - NOT NEEDED BY USER

    bin_ras = 0
    unlog_ras = 0
    is_driven = 0
    reverse = 0
    moore = 0
    zonal_density = 0    

    # RASTER HEADER AND HARDCODED NAMES

    #set the raster header name - including swap_path
    rast_hdr = os.path.join(swap_path, 'rasterHeader.hdr')

    #set the zone identity raster name - including swap_path
    zone_id_ras = os.path.join(swap_path, 'zone_identity.asc')
    
    #set name of combined constraint raster - including swap_path
    constraint_ras = os.path.join(swap_path, 'constraint.asc')

    #set name of current development raster - including swap_path 
    current_dev_ras = os.path.join(swap_path, 'current_development.asc')

    #set name of suitability raster (mce output) - including swap_path
    cell_suit_ras = os.path.join(swap_path, 'out_cell_suit.asc')

    #set name of development area identity raster - including swap_path
    dev_area_id_ras = os.path.join(swap_path, 'dev_area_id.asc')

    #set name of development area suitability raster - including swap_path
    dev_area_suit_ras = os.path.join(swap_path, 'dev_area_suit.asc')

    #set name of output development raster - including swap_path
    cell_dev_output_ras = os.path.join(swap_path, 'out_cell_dev.asc')

    #set name of OPTIONAL density raster - including swap_path
    density_ras = os.path.join(swap_path, 'density.asc')

    #set name of development density raster - including swap_path
    cell_dph_ras = os.path.join(swap_path, 'out_cell_dph.asc')

    #set name of development density raster - including swap_path
    cell_pph_ras = os.path.join(swap_path, 'out_cell_pph.asc')

    # TABLE NAMES

    #set name of constraints table - including swap_path    
    constraints_tbl = os.path.join(swap_path, 'constraints.csv')

    #set name of attractors table - including swap_path
    attractors_tbl = os.path.join(swap_path, 'attractors.csv')

    #set name of population table - including swap_path
    population_tbl = os.path.join(swap_path, 'population.csv')

    #set name of parameters table - including swap_path
    parameters_tbl = os.path.join(swap_path, 'parameters.csv')

    #set name of overflow data table - including swap_path
    overflow_tbl = os.path.join(swap_path, 'out_cell_overflow.csv')

    #set name of OPTIONAL zonal density table - including swap_path
    density_tbl = os.path.join(swap_path, 'density.csv')

    #set name of metadata output table
    metadata_tbl = os.path.join(swap_path, 'out_cell_metadata.csv')

    # PARAMETERS FROM TABLES - ATTRACTOR STANDARDISATION

    #density_from_raster
    #people_per_dwelling
    #coverage_threshold
    #minimum_plot_size

    #import parameters
    with open(parameters_tbl) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            density_from_raster = int(row['density_from_raster'])
            people_per_dwelling = float(row['people_per_dwelling'])
            coverage_threshold = float(row['coverage_threshold'])
            minimum_plot_size = int(row['minimum_plot_size'])            

    print( "Parameters file imported.")

    num_zones = 0
    num_constraints = 0
    num_attractors = 0

    #num_zones = number of rows in population_tbl
    with open(population_tbl) as csvfile:
        reader = csv.DictReader(csvfile)        
        for row in reader:
            num_zones += 1

    #num_constraints = number of rows in constraints_tbl
    with open(constraints_tbl) as csvfile:
        reader = csv.DictReader(csvfile)        
        for row in reader:
            num_constraints += 1

    #num_attractors = number of rows in attractors_tbl - apply standardisation    
    with open(attractors_tbl) as csvfile:
        reader = csv.DictReader(csvfile)        
        for row in reader:
            num_attractors += 1
            if row['reverse_polarity_flag'] == '1':            
                rt.RevPolarityStandardise(os.path.join(swap_path, row['layer_name']), zone_id_ras)
            elif row['reverse_polarity_flag'] == '0':
                rt.Standardise(os.path.join(swap_path, row['layer_name']), zone_id_ras)

    #test
    print('num_zones = ')
    print (num_zones)
    print('num_constraints = ')
    print (num_constraints)
    print('num_attractors = ')
    print (num_attractors)

    # PARAMETERS FROM RASTER HEADER

    ras_columns = 0
    ras_rows = 0
    ras_cellsize = 0

    #generate raster header from zone identity raster
    rt.IRasterToHeader(zone_id_ras, rast_hdr)

    #extract variables from raster header - cellsize and number of rows/columns
    #these variables are used in the cellular model setup function
    ras_columns = rt.ParameterFromHeader(rast_hdr, 'ncols')
    ras_rows = rt.ParameterFromHeader(rast_hdr, 'nrows')
    ras_cellsize = rt.ParameterFromHeader(rast_hdr, 'cellsize')

    #test
    print('ras_columns = ')
    print (ras_columns)
    print('ras_rows = ')
    print (ras_rows)
    print('ras_cellsize = ')
    print (ras_cellsize)

    # COVERAGE TO CONSTRAINT

    #generate combined constraint and current development rasters
    rt.RasteriseAreaThresholds(swap_path + "/", rast_hdr, constraint_ras, current_dev_ras, constraints_tbl, num_constraints, coverage_threshold)

    #flip constraint raster to form boolean suitability
    rt.IRasterNotBoolean(constraint_ras)
    
    #mask nodata for region using zone_id_ras
    rt.IRasterSetNoDataToRef(constraint_ras, zone_id_ras);

    #MULTI CRITERIA EVALUATION

    #bin_ras = 0    
    #reverse = 0    

    #set bval based upon boolean input (bin_ras) - it can then be tested in place as function argument
    bval = 0
    if bin_ras:
        bval = 1

    #set rval based upon boolean input (reverse) - it can then be tested in place as function argument
    rval = 0
    if reverse:
        rval = 1

    #generate suitability raster
    #mce.MaskedWeightedSum(binary, constraint_ras, num_attractors, attractors_tbl, cell_suit_ras, rast_hdr, swap_path + "/", reverse)
    mce.MaskedWeightedSum((bval>0), constraint_ras, num_attractors, attractors_tbl, cell_suit_ras, rast_hdr, swap_path + "/", (rval>0))
    print("mce.MaskedWeightedSum")

    #CREATE DEVELOPMENT AREAS

    #int minimum_plot_size = 4 

    #moore = 0

    #set mval based upon boolean input (moore) - it can then be tested in place as an argument to CreateDevZones()
    mval = 0
    if moore:
        mval = 1

    #generate development area identity raster
    #dz.CreateDevZones(binary, minimum_plot_size, moore, constraint_ras, dev_area_id_ras, rast_hdr, swap_path, zone_id_ras)
    dz.CreateDevZones((bval>0), minimum_plot_size, (mval>0), constraint_ras, dev_area_id_ras, rast_hdr, swap_path, zone_id_ras)    
    print("dz.CreateDevZones") 

    #COMPUTE DEVELOPMENT AREA SUITABILITY

    #generate development area average suitability raster
    #dz.DevZoneAVGSuit(binary, dev_area_id_ras, cell_suit_ras, dev_area_suit_ras, rast_hdr, swap_path)
    dz.DevZoneAVGSuit((bval>0), dev_area_id_ras, cell_suit_ras, dev_area_suit_ras, rast_hdr, swap_path)    
    print("dz.DevZoneAVGSuit")

    #URBAN DEVELOPMENT MODEL      

    #cellular model class    
    cm = CellularModel.CellularModel()  

    cm.Setup(num_zones, ras_cellsize, ras_columns, ras_rows)
    
    cm.UseBinaryRasters((bval>0))

    cm.SetRasterHeader(rast_hdr)

    cm.SetPathToBinaryConfigFiles(swap_path)    

    #set parameters to read zone codes from population table
    label_total = 4
    label_col = 1

    #read zone codes from population table
    cm.LoadWardLabels(population_tbl, label_col, label_total)

    #set parameters to read population data from population table
    pop_total = 4
    cur_pop_col = 2
    fut_pop_col = 3

    #read population data from population table
    cm.LoadCurrentPopulation(population_tbl, cur_pop_col, pop_total)
    cm.LoadFuturePopulation(population_tbl, fut_pop_col, pop_total)

    ##---

    #zonal_density = 0

    #table format: zone_id | zone_density
    print("if", zonal_density)
    if zonal_density:
        print("cm.LoadWardDensity", density_tbl,0,2)
        cm.LoadWardDensity(density_tbl,0,2)

    #density_from_raster = 0

    #set dval based upon boolean input (density_from_raster) - it can then be tested in place as function argument
    dval = 0
    if density_from_raster:
        dval = 1

    #variable density 
    if dval:
        cm.LoadDwellingsRaster(density_ras)

    ##---

    #load zone identity raster
    cm.LoadWardIDRaster(zone_id_ras)

    #/oad development area identity raster
    cm.LoadZoneIDRaster(dev_area_id_ras)

    #load development area average suitability raster
    cm.LoadZoneAVGRaster(dev_area_suit_ras)

    #load current development raster
    cm.LoadDevLandRaster(current_dev_ras)

    #load development suitability raster
    cm.LoadCellSuitRaster(cell_suit_ras)    

    cm.RunModel()

    cm.OutputRasterResult(cell_dev_output_ras)    
    cm.WriteOverflowWards(overflow_tbl)

    # GENERATE DWELLINGS PER HECTARE RASTER OUTPUT IF USING VARIABLE DENSITY 
    # otherwise set to a constant value for now
    # todo - derive fixed density output from zonal density and dwellings occupancy  

    if dval:        
        rt.IRasterDevToDPH(cell_dev_output_ras, density_ras, cell_dev_output_ras, cell_dph_ras)
    else:
        #rt.IRasterSetToValue(rast_hdr, 50, density_ras)
        #rt.IRasterDevToDPH(cell_dev_output_ras, density_ras, cell_dev_output_ras, cell_dph_ras) 
        cm.OutputDevelopmentDensity(cell_pph_ras, cell_dph_ras, people_per_dwelling)

    # generate coverage from dph
    tiles_path = os.path.abspath(os.path.join(os.path.dirname(__file__), 'Tiles'))
    rt.UFGCoverageFromDensity(swap_path, tiles_path)    

    # METADATA - basic for now
    # todo - full metadata for new interface

    name = []
    value = []

    name.append("model")
    value.append("urban development")

    # PARAMETERS
    name.append("parameters")
    value.append("parameters")

    name.append("num_zones")
    value.append(num_zones)

    name.append("num_constraints")
    value.append(num_constraints)

    name.append("num_attractors")
    value.append(num_attractors)

    name.append("ras_columns")
    value.append(ras_columns)

    name.append("ras_rows")
    value.append(ras_rows)

    name.append("ras_cellsize")
    value.append(ras_cellsize)

    #create metadata list of lists
    md = [name, value]

    #zip metadata into name-value rows
    md = zip(*md)    

    #write metadata to csv
    print("writing", metadata_tbl)
    with open(metadata_tbl, "w", newline='') as f:
        writer = csv.writer(f)
        writer.writerows(md)

    ### END OF METADATA
    print("Model run complete")

if __name__ == "__main__":
    data_path = 'Data'

    try:
        data_path = sys.argv[1]
        print(f"Using provided data path: {data_path}")
    except IndexError:
        print(f"Using default data path: {data_path}")

    main(data_path) 
        