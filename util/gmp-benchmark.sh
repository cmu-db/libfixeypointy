BUILD_DIR=../build
BENCH_DIR=benchmark
BENCH_INPUT=../util/bench_reg
BENCH_MAGIC_INPUT=../util/bench_magic
BENCH_ITER=10000 # repeat operation multiples times to bring it to microsecond level
GMP_PRECISION=128

# output file naming
GMP_PREFIX=gmp
LIB_PREFIX=lib
MAGIC_PREFIX=magic

cd $BUILD_DIR
make -j$(nproc) bench-gmp bench-lib
rm $BENCH_DIR/*.csv

RUNS=10 # run each benchmark several
SKIP=2 # skip stats of first n runs for warm cache

# GMP
./bench-gmp add $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX-add.csv
./bench-gmp sub $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX-sub.csv
./bench-gmp mlt $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX-mlt.csv
./bench-gmp div $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX-div.csv

# Libfixeypointy
./bench-lib add $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-add.csv
./bench-lib sub $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-sub.csv
./bench-lib mlt $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-mlt.csv
./bench-lib div $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-div.csv

# # Benchmark magic mult and div
./bench-gmp mlt $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$GMP_PREFIX-$MAGIC_PREFIX-mlt.csv
./bench-gmp div $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$GMP_PREFIX-$MAGIC_PREFIX-div.csv
./bench-lib mlt $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$LIB_PREFIX-$MAGIC_PREFIX-mlt.csv
./bench-lib div $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$LIB_PREFIX-$MAGIC_PREFIX-div.csv
