#!/usr/bin/env bash
set -ex

export PIP_DEFAULT_TIMEOUT=60

export TEST_ARGS="-v"

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	section setup_osx_venv
	git clone https://github.com/matthew-brett/multibuild ~/multibuild
	source ~/multibuild/osx_utils.sh
	get_macpython_environment $TRAVIS_PYTHON_VERSION ~/venv
	section_end setup_osx_venv
else
	section setup_linux_venv
	python -m pip install -U pip
	HAVE_VENV=$(
		python <<-EOL
		try:
		    import venv
		    print('1')
		except ImportError:
		    pass
		EOL
	)
	if [ "$HAVE_VENV" ]; then
		python -m venv ~/venv
	else
		python -m pip install -U virtualenv
		virtualenv -p python ~/venv
	fi
	section_end setup_linux_venv
fi

section install_requirements
source ~/venv/bin/activate
python -m pip install -U pip
python -m pip install --retries 3 wheel
python -m pip install --retries 3 pytest pytest-timeout cython numpy scipy
python -m pip list
section_end install_requirements

set +ex
