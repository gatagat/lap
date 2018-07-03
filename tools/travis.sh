#!/usr/bin/env bash
set -ex

section test
python -m pytest $TEST_ARGS lap
section_end test
