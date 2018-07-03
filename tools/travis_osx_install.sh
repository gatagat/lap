#!/usr/bin/env bash
#
# Adapted from the skimage project (https://github.com/scikit-image/scikit-image)

section osx_install
git clone https://github.com/MacPython/terryfy.git ~/terryfy
source ~/terryfy/travis_tools.sh
get_python_environment macpython $TRAVIS_PYTHON_VERSION ~/macpython_venv$TRAVIS_PYTHON_VERSION
source ~/macpython_venv$TRAVIS_PYTHON_VERSION/bin/activate
section_end osx_install
