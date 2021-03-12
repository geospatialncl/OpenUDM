import os
import csv

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
		opts, args = getopt.getopt(args, "i:", ["tile_data_path=",]) # e.g. i = input file (colon indicates input expected)
	except getopt.GetoptError as err:
		print(err)
		sys.exit(2)

	# check each passed argument and assign values to variables
	for opt, arg in opts:
		if opt in ("-i", "tile_data_path="):
			data_path_tiles = arg
		else:
			print('Un-recognised argument %s' %opt)
			sys.exit(2)

	urban_fabric_generator(data_path_tiles)


def urban_fabric_generator(data_path='Path/To/TilesFolder'):
	"""
	Runs a function to generate a raster file which contains a urban fabric.
	Input data to be in the UDM data dir and output written to same directory.

	Inputs:
	- data_path : accepts a file path to a folder which contains the tiles for forming a urban fabric.
	"""

	# data path - check exists
	if os.path.exists(data_path) is not True:
		return 'Error! Not data does not exist.'

	# tile table
	tile_table = os.path.join(data_path, 'in_tile_table.csv')

	with open(tile_table) as csvfile:
		reader = csv.DictReader(csvfile)
		num_tiles = 0
		for row in reader:
			num_tiles += 1

	print(num_tiles)

	# call wrapped C++ UFG function
	rt.UrbanFabricGenerator(data_path, num_tiles)

	return
