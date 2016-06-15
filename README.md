Welcome to OpenRAVE
-------------------

[Official OpenRAVE Homepage](<http://openrave.org>)

Installing dependencies from ppa
```bash
sudo apt-get install libpcre++-dev libopenscenegraph-dev openscenegraph libboost-python-dev python python-dev python-numpy ipython python-scipy python-sympy liblapack-dev liblapacke-dev libode-dev libbullet-dev libsoqt4-dev libgmp3-dev libmpfr-dev libmpfi-dev collada-dom2.4-dp* assimp-utils
```

Then go to the library folder
```bash
mkdir build
cd build
cmake ..
make install
```

After installation add the following to the .bashrc file
```bash
export PYTHONPATH=$PYTHONPATH:`openrave-config --python-dir`
source `openrave-config --share-dir`/openrave.bash
```

