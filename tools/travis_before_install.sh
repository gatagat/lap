#!/usr/bin/env bash
set -ex

export PIP_DEFAULT_TIMEOUT=60

export TEST_ARGS="-v"

section install_pip_venv
python -m pip install -U pip
if pyver_ge $TRAVIS_PYTHON_VERSION 3.3; then
	true  # There is the venv module.
else
	pip install -U virtualenv
fi
section_end install_pip_venv

section venv
if pyver_ge $TRAVIS_PYTHON_VERSION 3.3; then
	python -m venv -p python ~/venv
else
	virtualenv -p python ~/venv
fi
source ~/venv/bin/activate
python -m pip install -U pip
pip install --retries 3 -q wheel pytest pytest-timeout cython numpy scipy
pip list
section_end venv

set +ex
