import os
import csv

# import swig-wrapped RasterToolkit as rt
from openudm import (RasterToolkit as rt)


def urban_fabric_generator(data_path = 'Path/To/TilesFolder'):
	"""

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
