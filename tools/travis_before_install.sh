#!/usr/bin/env bash
set -ex

export PIP_DEFAULT_TIMEOUT=60

export TEST_ARGS="-v"

HAVE_VENV=$(python <<EOL
try:
	import venv
	print('1')
except ImportError:
	pass
EOL
)

section install_pip_venv
python -m pip install -U pip
if [ ! "$HAVE_VENV" ]; then
	pip install -U virtualenv
fi
section_end install_pip_venv

section venv
if [ "$HAVE_VENV" ]; then
	python -m venv ~/venv
else
	virtualenv -p python ~/venv
fi
source ~/venv/bin/activate
python -m pip install -U pip
pip install --retries 3 -q wheel pytest pytest-timeout cython numpy scipy
pip list
section_end venv

set +ex
