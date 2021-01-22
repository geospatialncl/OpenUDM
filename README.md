# OpenUDM
![Test](https://github.com/geospatialncl/OpenUDM/workflows/Test/badge.svg)

Open repository for the Urban Development Model

##  Usage
`python -m openudm /path/to/data`

##  Additional tools

### Raster to Vector
Command line tool allowing conversion of a raster file to a vector file

#### Usage 
`raster_to_vector -i input_file -o output_file -c cell_value`

##### Expected arguments:
`-i` - path and name of input raster. Default = 'raster.asc'  
`-o` - path and name of output vector - supports geopackages only at the moment. Default = 'buildings.gpkg'  
`-c` - the cell value to identify and convert to vectors, expects an integer. Default = 1