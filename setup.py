'''
Author: Vilewind
Date: 2022-01-08 21:49:41
LastEditors: Vilewind
LastEditTime: 2022-03-06 09:52:44
FilePath: /EoN/EvolutionaryVaccinationGame/setup.py
Version: 
Description: 
'''
"""
Author: Vilewind
Date: 2022-01-08 20:48:05
LastEditors: Vilewind
LastEditTime: 2022-01-08 21:46:47
FilePath: /event_driven-SIR/setup.py
Version: 
Description: 
"""
"""
Author: Vilewind
Date: 2021-12-30 10:58:16
LastEditors: Vilewind
LastEditTime: 2021-12-30 14:12:55
FilePath: /evolution_multi_process/setup.py
Version: 
Description: 
"""


# Available at setup time due to pyproject.toml

import sys
from glob import glob
from pybind11 import get_cmake_dir
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup
__version__ = "0.0.1"

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

ext_modules = [
    Pybind11Extension(
        "LVE",
        sorted(glob("src/*.cpp")),
        # Example: passing in the version to the compiled code
        define_macros=[("VERSION_INFO", __version__)],
    ),
]

setup(
    name="LVE",
    version=__version__,
    author="vilewind",
    author_email="luochengx2019@163.com",
    url="https://github.com/pybind/python_example",
    description="A test project using pybind11",
    long_description="",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.6",
)
