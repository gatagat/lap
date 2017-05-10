#!/bin/sh

export PYTHONPATH=$LAPJV_OLD:$PYTHONPATH 

PYTEST_OPTS="-v -s --benchmark-sort=mean --benchmark-columns=mean,min,max,median,rounds,iterations"

if [ ! -e matrix_dense_hard.json ]; then
	pytest $PYTEST_OPTS --benchmark-json=matrix_dense_hard.json matrix_dense_hard.py
fi
if [ ! -e matrix_sparse.json ]; then
	pytest $PYTEST_OPTS --benchmark-json=matrix_sparse.json matrix_sparse.py
fi
if [ ! -e matrix_nnz.json ]; then
	pytest $PYTEST_OPTS --benchmark-json=matrix_nnz.json matrix_nnz.py
fi
if [ ! -e overview_dense.json ]; then
	pytest $PYTEST_OPTS --benchmark-json=overview_dense.json overview_dense.py
fi
if [ ! -e overview_sparse.json ]; then
	pytest $PYTEST_OPTS --benchmark-json=overview_sparse.json overview_sparse.py
fi
