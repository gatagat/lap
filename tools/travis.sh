#!/usr/bin/env bash
set -ex

section test
nosetests $TEST_ARGS lapjv
section_end test
