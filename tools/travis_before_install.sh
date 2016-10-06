#!/usr/bin/env bash
set -ex

export PIP_DEFAULT_TIMEOUT=60

export TEST_ARGS="-v --ignore-files=^setup.py$ --with-coverage"

section venv
virtualenv -p python ~/venv
source ~/venv/bin/activate
pip install --upgrade pip
pip install --retries 3 -q wheel codecov nose cython numpy
pip list
section_end venv

set +ex
