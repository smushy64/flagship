#!/usr/bin/env bash

# Generate doxygen documentation for flagship.h

cd docs
doxygen DOXYFILE
xdg-open html/index.html
