#!/usr/bin/env bash
set -ex

export PIP_DEFAULT_TIMEOUT=60

export TEST_ARGS="-v"

section venv
virtualenv -p python ~/venv
source ~/venv/bin/activate
python -m pip install --upgrade pip
pip install --retries 3 -q wheel pytest pytest-timeout cython numpy scipy
pip list
section_end venv

set +ex
