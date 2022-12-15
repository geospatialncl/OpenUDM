from setuptools import Extension, setup
from glob import glob
import os

sources = glob('openudm/*.cpp') + glob('openudm/*.i')

py_modules = ['CellularModel', 'RasterToolkit', 'MultiCriteriaEval', 'DevZones']

if os.name == 'nt':
    cxxflags = {'gcc': ['/Qstd=c99']}
else:
    cxxflags = ['-Wall', '-std=c++11', '-pedantic']

headers = glob('openudm/*.h')

ext_modules = [
    Extension(
        'openudm._CellularModel',
        extra_compile_args = cxxflags,
        sources=[
            'openudm/CellularModel.i',
            'openudm/CellularModel.cpp',
            # list anything included (may be a better way?)
            'openudm/CSVToolkit.cpp',
            'openudm/Raster.cpp',
            'openudm/UDMCell.cpp',
            'openudm/UDMWard.cpp',
            'openudm/UDMZone.cpp',
        ],
        depends=headers,
        swig_opts=['-c++']
    ),
    Extension(
        'openudm._DevZones',
        extra_compile_args = cxxflags,
        sources=[
            'openudm/DevZones.i',
            'openudm/DevZones.cpp',
            # list includes
            'openudm/CSVToolkit.cpp',
            'openudm/Raster.cpp',
        ],
        depends=headers,
        swig_opts=['-c++']
    ),
    Extension(
        'openudm._MultiCriteriaEval',
        extra_compile_args = cxxflags,
        sources=[
            'openudm/MultiCriteriaEval.i',
            'openudm/MultiCriteriaEval.cpp',
            # list includes
            'openudm/CSVToolkit.cpp',
            'openudm/Raster.cpp',
        ],
        depends=headers,
        swig_opts=['-c++']
    ),
    Extension(
        'openudm._RasterToolkit',
        extra_compile_args = cxxflags,
        sources=[
            'openudm/RasterToolkit.i',
            'openudm/RasterToolkit.cpp',
            # list includes
            'openudm/CSVToolkit.cpp',
            'openudm/Raster.cpp',
        ],
        depends=headers,
        swig_opts=['-c++']
    )
]


def readme():
  with open('README.md') as f:
    return f.read()


setup(
    name='openudm',
    version='5.0.7',
    author='James Virgo',
    description='Urban Development Model',
    long_description=readme(),
    long_description_content_type="text/markdown",
    packages=['openudm'],
    ext_modules=ext_modules,
    entry_points={'console_scripts':
                      ['raster_to_vector=openudm.tools:output_raster_to_vector',
                       'generate_urban_fabric=openudm.UrbanFabricGenerator:urban_fabric_generator_entrypoint',
                       'ufg_coverage=openudm.UFGCoverageFromDensity:ufg_coverage_from_density_entrypoint',
                       'ufg_fabric=openudm.UFGFabricFromCoverage:ufg_fabric_from_coverage_entrypoint'
                       ]},
    package_data={
            "openudm": ["Tiles/*"]
        }
)
