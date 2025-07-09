#!/usr/bin/env bash

if command -v python3 &>/dev/null; then
    PYTHON=python3
elif command -v python &>/dev/null; then
    PYTHON=python
else
    echo "Python is not installed. Please install Python before running this script."
    exit 1
fi

echo "Python found: $PYTHON"
echo "Running setup.py..."
$PYTHON setup.py
