# This script probably only works for Jack with his setup of source trees.
rm -r .venv
python3 -m venv .venv
. .venv/bin/activate
set -x
pip install ../../jack-git/bleak
pip install ../iotsa/extras/python
pip install -r ./extras/python/requirements.txt
pip install -e ./extras/python
