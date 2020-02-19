from setuptools import Extension, setup
from glob import glob

sources = glob('*.cpp') + glob('*.i')

py_modules = ['CellularModel', 'RasterToolkit', 'MultiCriteriaEval', 'DevZones']

ext_modules = [
    Extension(
        f"_{name}",
        sources=sources,
        swig_opts=['-c++']
    )
    for name in py_modules
]

setup(
    name='openudm',
    version='1.0.0',
    author='James Virgo',
    description='Urban Development Model',
    ext_modules=ext_modules,
    py_modules=py_modules
)
