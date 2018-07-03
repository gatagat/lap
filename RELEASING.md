1. Set version in lap/__init__.py to X.Y.Z
2. Create a source distribution:

    python setup.py sdist

3. Upload it to the test server (this requires setting up ~/.pypirc):

    twine upload --repository testpypi dist/*

4. Check install from test pypi into a venv:

    virtualenv test
    . test/bin/activate
    pip install numpy
    pip install --index-url https://test.pypi.org/simple/ lap

5. Make sure stuff works there:

    pip install pytest
    pip install scipy
    pytest $VIRTUAL_ENV/lib/python*/site-packages/lap/

6. Tag and release on github:

    git tag vX.Y.Z
    git push origin vX.Y.Z

And follow https://help.github.com/articles/creating-releases/

7. Finally, upload to the live pypi server:

    twine upload dist/*
