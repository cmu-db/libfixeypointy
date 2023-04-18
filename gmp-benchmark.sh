cd build
make -j$(nproc) bench-gmp bench-lib

rm *.csv

RUNS=20 # run each benchmark several
SKIP=4 # skip stats of first n runs for warm cache

# EXAMPLE: add mode, 10 repeated runs, skip first 2 run stats (cache), 1000000 iterations, 128bit precision, 1.0000 + 1.00001
GMP_PREFIX=gmp
./bench-gmp add $RUNS $SKIP 1000000 128 1.000000000000000 0.000000000000001 > $GMP_PREFIX-add.csv
./bench-gmp sub $RUNS $SKIP 1000000 128 1.000000000000000 0.000000000000001 > $GMP_PREFIX-sub.csv
./bench-gmp mlt $RUNS $SKIP 1000000 128 1.000000000000000 1.00001 > $GMP_PREFIX-mlt.csv
./bench-gmp div $RUNS $SKIP 1000000 128 100.00000 1.00001 > $GMP_PREFIX-div.csv

# Benchmark script for Libfixeypointy
LIB_PREFIX=lib
./bench-lib add $RUNS $SKIP 1000000 15 1.000000000000000 0.000000000000001 > $LIB_PREFIX-add.csv
./bench-lib sub $RUNS $SKIP 1000000 15 1.000000000000000 0.000000000000001 > $LIB_PREFIX-sub.csv
./bench-lib mlt $RUNS $SKIP 1000000 15 1.000000000000000 1.00001 > $LIB_PREFIX-mlt.csv
./bench-lib div $RUNS $SKIP 1000000 15 100.00000 1.00001 > $LIB_PREFIX-div.csv
