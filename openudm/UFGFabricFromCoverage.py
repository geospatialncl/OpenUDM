import os
import sys
import csv
import getopt

# import swig-wrapped RasterToolkit as rt
from openudm import (RasterToolkit as rt)


#def urban_fabric_generator_entrypoint():
def ufg_fabric_from_coverage_entrypoint():
    """
    Commandline entrypoint
    """
    # default values for required input variables
    data_path_tiles = ''

    # get args list
    args = sys.argv[1:]

    # parse passed command line arguments
    try:
        # e.g. i = input file (colon indicates input expected)        
        #opts, args = getopt.getopt(args, "i:t:", ["input_data_path=", 'tile_data_path='])
        opts, args = getopt.getopt(args, "b:t:f:p:", ["input_build_ras=", "input_tile_ras=", "output_fabric_ras=", 'tile_data_path='])
    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    #input_data_path = None
    input_build_ras = None
    input_tile_ras = None
    output_fabric_ras = None
    tile_data_path = None    

    # check each passed argument and assign values to variables
    for opt, arg in opts:
        print(opt)
        if opt in ("-b", "input_build_ras="):
            input_build_ras = arg
        elif opt in ("-t", 'input_tile_ras='):
            input_tile_ras = arg 
        elif opt in ("-f", 'output_fabric_ras='):
            output_fabric_ras = arg 
        elif opt in ("-p", 'tile_data_path='):
            tile_data_path = arg        
        else:
            print('Un-recognised argument %s' %opt)
            sys.exit(2)

    #if input_data_path is None:
    #    print('Input data path is required')
    #    sys.exit(2)
    
    #ufg_fabric_from_coverage(dph_path=input_data_path, tiles_path=tile_data_path)
    ufg_fabric_from_coverage(build_ras=input_build_ras, tile_ras=input_tile_ras, output_ras=output_fabric_ras, tiles_path=tile_data_path)


#def urban_fabric_generator(dph_path, out_path=None, tiles_path=None):
#def ufg_fabric_from_coverage(dph_path, tiles_path=None):
def ufg_fabric_from_coverage(build_ras, tile_ras, output_ras, tiles_path=None):
    """
    Runs a function to generate a raster file containing urban fabric.
    Input data to be in the UDM data dir and output written to same directory.

    Inputs:
    - dph_path: the path to the folder containing the dph raster    
    - tiles_path : accepts a file path to a folder which contains the tiles to generate urban coverage.
    """

    if tiles_path is None:
        tiles_path = os.path.abspath(os.path.join(os.path.dirname(__file__), 'Tiles'))

    print(tiles_path)    

    # call wrapped C++ UFG function    
    #rt.UFGFabricFromCoverage(dph_path, tiles_path)
    rt.UFGFabricFromCoverage(build_ras, tile_ras, output_ras, tiles_path)

    return
