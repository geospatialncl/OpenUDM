import os
import sys
import csv
import getopt

# import swig-wrapped RasterToolkit as rt
from openudm import (RasterToolkit as rt)


def urban_fabric_generator_entrypoint():
    """
    Commandline entrypoint for urban fabric generator tool
    """
    # default values for required input variables
    data_path_tiles = ''

    # get args list
    args = sys.argv[1:]

    # parse passed command line arguments
    try:
        # e.g. i = input file (colon indicates input expected)
        opts, args = getopt.getopt(args, "i:t:o:", ["input_data_path=", 'tile_data_path=', 'output_data_path='])
    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    input_data_path = None
    tile_data_path = None
    output_data_path = None

    # check each passed argument and assign values to variables
    for opt, arg in opts:
        print(opt)
        if opt in ("-i", "input_data_path="):
            input_data_path = arg
        elif opt in ("-t", 'tile_data_path='):
            tile_data_path = arg
        elif opt in ("-o", 'output_data_path='):
            output_data_path = arg
        else:
            print('Un-recognised argument %s' %opt)
            sys.exit(2)

    if input_data_path is None:
        print('Input data path is required')
        sys.exit(2)

    urban_fabric_generator(dph_path=input_data_path, out_path=output_data_path, tiles_path=tile_data_path)


def urban_fabric_generator(dph_path, out_path=None, tiles_path=None):
    """
    Runs a function to generate a raster file which contains a urban fabric.
    Input data to be in the UDM data dir and output written to same directory.

    Inputs:
    - dph_path: the path to the dph raster
    - out_path: the path to save the output data at
    - tiles_path : accepts a file path to a folder which contains the tiles for forming a urban fabric.
    """

    if tiles_path is None:
        tiles_path = os.path.abspath(os.path.join(os.path.dirname(__file__), 'Tiles'))

    print(tiles_path)

    # tile table
    tile_table = os.path.join(tiles_path, 'in_tile_table.csv')

    with open(tile_table) as csvfile:
        reader = csv.DictReader(csvfile)
        num_tiles = 0
        for row in reader:
            num_tiles += 1

    print(num_tiles)

    if out_path is None:
        out_dph_path = os.path.join(os.path.dirname(dph_path), "out_uf.asc")
    else:
        out_dph_path = out_path

    # call wrapped C++ UFG function
    rt.UrbanFabricGenerator(dph_path, out_dph_path, os.path.join(tiles_path, ''), num_tiles)

    return
