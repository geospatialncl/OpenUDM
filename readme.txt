---Raster header.

first 6 rows from .asc format raster - (rasterHeader.hdr)

ncols val
nrows val
xllcorner val 
yllcorner val
cellsize val
NODATA_value -1

all input rasters must be definied by this shared header information.

---Multi Criteria Evaluation Raster Inputs.

in_mce_ras_dbl.csv

'double' type rasters - proximities etc.

table format: asc / csv / weight / convert

if convert is 'y' then named rasters are converted from asc to csv, set to 'n' to speed up when conversion is not required.

in_mce_ras_int.csv

'integer' type rasters - constraints etc.

table format: asc / csv / weight / convert

if convert is 'y' then named rasters are converted from asc to csv, set to 'n' to speed up when conversion is not required.

N.B. weight of -1 must be assigned to the constraint mask, all other weights should sum to 1 across both files.

---UDM Raster Inputs.

in_udm_ras.csv

'integer' type rasters.

table format: asc / csv / convert

if convert is 'y' then named rasters are converted from asc to csv, set to 'n' to speed up when conversion is not required.

N.B. Must contain the following rasters in this order: 
1 - constraint mask
2 - zone identity (starting at 0 - corresponding to in_zone_order.csv)
3 - current development

---Zonal Inputs.

in_zone_order.csv

table format: label

zones ordered by label corresponding to zone identity raster.

in_zonal_density.csv

table format: density (people per square metre)

zones ordered by ons_label corresponding to zone identity raster.

in_zonal_pop.csv

table format: initial_pop / final_pop

poplation for each zone - zones ordered by ons_label corresponding to zone identity raster.

---Parameters.

The following can be modified by editing in_params.csv

num_zones = () number of zones

density_provided = () set to 1 when providing zonal density values - otherwise values are calculated from current development raster

min_dev_area = () minimum number of cells to group into development areas within zones

cell_size = () from raster header

num_cols = () from raster header

num_rows = () from raster header

other parameters can be modified by editing udm_solo.py...

moore = () set neighbourhood type when grouping cells into development areas (0 = vonNeumann,1 = Moore). 

label_total = () number of columns for in_zone_order.csv.
label_col = () read column for in_zone_order.csv.
pop_total = () number of columns for in_zonal_pop.csv.
cur_pop_col = () current population read column for in_zonal_pop.csv.
fut_pop_col = () future population read column for in_zonal_pop.csv.

bin_ras = (0) leave as 0
unlog_ras = (0) leave as 0
is_driven = (0) leave as 0
reverse = (0) leave as 0 

---Outputs.

out_cell_dev.asc - raster output (0 = no dev, 1 = current dev, 2 = future dev)

out_cell_overflow.csv - zones ordered by ons_label corresponding to zone identity raster.

out_cell_metadata.csv - record of all inputs, outputs and parameters.

---Usage

run udm_solo.py script

--Additional tools

---Raster to Vector
Command line tool allowing conversion of rasters to vectors

command line call: raster_to_vector
expected arguments:
-i <path and name of input raster>
-o <path and name of output vector - supports geopackages only at the moment>
-c <the cell value to identify and convert to vectors, expects an integer>

---Generate urban fabric
Command line tool allowing for the generation of buildings and urban layouts using the outputs from a UDM run

command line call: generate_urban_fabric
expected arguments:
-i <path to directory with urban tiles in>

Outputs from UDM are expected in the data directory, and this is also where the output file will be written.
