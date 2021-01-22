import rasterio
from rasterio.features import shapes
import geopandas as gp
import sys
import getopt

def output_raster_to_vector():
    """
    Run raster to vector file conversion for UDM output dataset conversion.

    """
    # default values for required input variables
    value_of_interest = 22
    raster_file_path = 'raster.asc'
    output_vector_file = 'buildings.gpkg'

    # get args list
    args = sys.argv[1:]

    # parse passed command line arguments
    opts, args = getopt.getopt(args, "i:o:cv:", ["input_path_file=", "output_path_file", "raster_cell_value"]) # e.g. i = input file (colon indicates input expected)

    for opt, arg in opts:
        if opt in ("-i"):
            raster_file_path = arg
        elif opt in ("-o"):
            output_vector_file = arg
        elif opt in ("cv"):
            value_of_interest = arg

    raster_to_vector(value_of_interest, raster_file_path, output_vector_file)


def raster_to_vector(value_of_interest = 0, raster_file_path = 'raster.asc', output_vector_file = 'buildings.gpkg'):
    """
    Converts a .asc file (or any raster format) to a vector geopackage creating polygons for cells with the given value (default value set to 0)

    param: value_of_interest:
        raster value assigned to cells which are to be converted to polygons
    param: raster_file_path:
        file path and file name of raster file to convert to a vector layer/file
    param: output_vector_file:
        file path and file name of vector file to be saved (as a geopackage)

    """

    # read in raster file
    mask = None
    with rasterio.Env():
        with rasterio.open(raster_file_path) as src:
            image = src.read(1) # first band
            results = (
            {'properties': {'raster_val': v}, 'geometry': s}
            for i, (s, v)
            in enumerate(
                shapes(image, mask=mask, transform=src.transform)))

    # save polygons to a geodataframe
    polygons = list(results)
    gpd_polygons  = gp.GeoDataFrame.from_features(polygons)

    # remove any polygons which are not of interest
    gpd_polygons = gpd_polygons[gpd_polygons.raster_val == value_of_interest]

    # export file
    gpd_polygons.to_file(output_vector_file, layer='buildings', driver="GPKG")

    return
