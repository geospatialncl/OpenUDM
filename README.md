# OpenUDM

Open Urban Development Model

[![Test](https://github.com/geospatialncl/OpenUDM/workflows/Test/badge.svg)](https://github.com/geospatialncl/OpenUDM/actions)
[![PyPI package](https://img.shields.io/pypi/v/openudm.svg)](https://pypi.python.org/pypi/openudm)

**OpenUDM** is a grid cell-based model which explores where future urban
development might take place, subject to attractors, constraints and variable
density of development.

The diagram below illustrates how a number of different potential attractors
and constraints can be combined to run the model.

[![OpenUDM Workflow](https://github.com/OpenCLIM/workflow-diagrams/raw/90b5aa935757742bf204862d574823e366536b76/udm/openclim-udm-workflow.png)](https://github.com/OpenCLIM/workflow-diagrams/raw/90b5aa935757742bf204862d574823e366536b76/udm/openclim-udm-workflow.png)

**OpenUDM** has been used in a number of contexts to explore urban development
scenarios. For example, this interactive article about
[urban development in the Oxford-Cambridge Arc](https://nismod.github.io/arc-udm-vis/)
introduces the modelling process and analyses the potential impacts of future
development on the natural environment.


## Citations

> Alistair Ford, Stuart Barr, Richard Dawson, James Virgo, Michael Batty, Jim
> Hall (2019) A multi-scale urban integrated assessment framework for climate
> change studies: A flooding application. Computers, Environment and Urban
> Systems. 75, 229-243. https://doi.org/10.1016/j.compenvurbsys.2019.02.005.


## Quickstart

OpenUDM is a Python package which is tested with Python 3.6 and above. To
install the package using pip, run:

    pip install openudm

If `pip install` fails, you may need to install [`swig`](http://swig.org/),
which is used to build the core model code. On Ubuntu, run:

    sudo apt install swig

Before running the model, set up the parameter files and grid data files for
attractors, constraints, current development, and zones. A small example is
included in the `Data` directory of this repository.

To run the model:

    python -m openudm /path/to/data


## Acknowledgements

OpenUDM has been developed by researchers at Newcastle University and the
University of Oxford in the UK. This work has been funded by EPSRC through the
ITRC MISTRAL project, the UK Climate Resilience Programme through the OpenCLIM
project, and the Alan Turing Institute.
