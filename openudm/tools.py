import rasterio
from rasterio.features import shapes
import geopandas as gp
import sys
import getopt
from os.path import join


def output_raster_to_vector():
    """
    Run raster to vector file conversion for UDM output dataset conversion.

    """
    # default values for required input variables - used if not passed as env
    value_of_interest = 22
    raster_file_path = '/data/output/data/out_uf.asc'
    output_vector_file = 'buildings.gpkg'
    feature_types = 'buildings','roads','greenspace'

    # set details for allowed feature types
    allowed_feature_types = ['buildings','roads','greenspace']
    feature_type_identifiers = {'buildings':22,
                              'roads':21,
                              'greenspace':20}

    # get args list
    args = sys.argv[1:]

    # parse passed command line arguments
    try:
        opts, args = getopt.getopt(args, "i:o:f:", ["input_path_file=", "output_path_file=", "feature_type="]) # e.g. i = input file (colon indicates input expected)
    except getopt.GetoptError as err:
        print(err)
        sys.exit(2)

    # check each passed argument and assign values to variables
    for opt, arg in opts:

        if opt in ("-i", "input_path_file="):
            raster_file_path = arg
        elif opt in ("-o", "output_path_file="):
            output_vector_file = arg
        elif opt in ("-f", "feature_type="):

            feature_types = arg.strip()
            feature_types = feature_types.split(',')

            for feature_type in feature_types:
                if feature_type in allowed_feature_types:
                    # get the cell value for the passed feature type
                    value_of_interest = feature_type_identifiers[feature_type]
                else:
                    print('Error! The passed feature_type (%s) does not exist! It should be one of %s' % (feature_type, allowed_feature_types))
                    sys.exit(2)
        else:
            print('Un-recognised argument %s' % opt)
            sys.exit(2)

    # loop through the feature types and process
    for feature_type in feature_types:
        value_of_interest = feature_type_identifiers[feature_type]

        output_vector_file = f'{feature_type}.gpkg'

        raster_to_vector(value_of_interest, raster_file_path, output_vector_file, feature_type)


def raster_to_vector(value_of_interest=0, raster_file_path='/data/output/data/out_uf.asc', output_vector_file='buildings.gpkg', feature_type='buildings'):
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

    if len(gpd_polygons) == 0:
        print('Could not export as no cells matched the expected cell value (%s)' %value_of_interest)
        exit(2)

    # export file
    gpd_polygons.to_file(join('/data/outputs/data',output_vector_file), layer=feature_type, driver="GPKG")

    return
