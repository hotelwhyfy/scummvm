set -e
./configure --enable-debug --disable-all-engines --enable-engine=scream
make -j8 2>&1 | grep "engines/scream"
./scummvm -d1