cd ..;
cd build;
cmake .. -DCMAKE_BUILD_TYPE=Release;
make;
cd ..;
cd benchmark;

export LD_LIBRARY_PATH=/home/ykerdcha/15721/libfixeypointy/build;
g++ benchmark.cpp -L../build/ -llibfixeypointy;
./a.out;