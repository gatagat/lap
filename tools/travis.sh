#!/usr/bin/env bash
set -ex

section test
pytest $TEST_ARGS lap
section_end test
