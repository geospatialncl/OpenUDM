from setuptools import Extension, setup

model = Extension(
    '_CellularModel',
    sources=[
        'CellularModel.i',
        'CellularModel.cpp',
        'CSVToolkit.cpp',
        'DRaster.cpp',
        'IRaster.cpp',
        'UDMCell.cpp',
        'UDMWard.cpp',
        'UDMZone.cpp'
    ],
    swig_opts=['-c++']
)

setup(
    name='openudm',
    version='1.0.0',
    author='James Virgo',
    description='Urban Development Model',
    ext_modules=[model],
    py_modules=["CellularModel"]
)
