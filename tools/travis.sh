#!/usr/bin/env bash
set -ex

section test
pytest $TEST_ARGS lapjv
section_end test
