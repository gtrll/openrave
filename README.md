Welcome to OpenRAVE
-------------------

[Official OpenRAVE Homepage](<http://openrave.org>)

If installing dependencies from debian does not work do the following
```
sudo apt-get install libpcre++-dev
sudo apt-get install libopenscenegraph-dev openscenegraph
sudo apt-get install libboost-python-dev python python-dev python-numpy ipython python-scipy python-sympy
sudo apt-get install liblapack-dev liblapacke-dev libode-dev libbullet-dev libsoqt4-dev libgmp3-dev libmpfr-dev libmpfi-dev collada-dom2.4-dp* assimp-utils
```

After installation add the following to the bashrc file
```
export PYTHONPATH=$PYTHONPATH:`openrave-config --python-dir`
source `openrave-config --share-dir`/openravebash
```