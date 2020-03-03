from setuptools import Extension, setup
from glob import glob

sources = glob('openudm/*.cpp') + glob('openudm/*.i')

py_modules = ['CellularModel', 'RasterToolkit', 'MultiCriteriaEval', 'DevZones']

cxxflags = ['-Wall', '-std=c++11', '-Werror', '-pedantic']

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
            'openudm/DRaster.cpp',
            'openudm/IRaster.cpp',
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
            'openudm/DRaster.cpp',
            'openudm/IRaster.cpp',
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
            'openudm/DRaster.cpp',
            'openudm/IRaster.cpp',
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
            'openudm/DRaster.cpp',
            'openudm/IRaster.cpp',
        ],
        depends=headers,
        swig_opts=['-c++']
    )
]

setup(
    name='openudm',
    version='1.0.0',
    author='James Virgo',
    description='Urban Development Model',
    packages=['openudm'],
    ext_modules=ext_modules
)
