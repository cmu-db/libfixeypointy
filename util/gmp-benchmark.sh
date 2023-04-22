#!/bin/bash
# Run this script from util to generate and execute benchmarks

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

UTIL_DIR=$PROJECT_DIR/util
BUILD_DIR=$PROJECT_DIR/build
BENCH_DIR=$PROJECT_DIR/build/benchmark
BENCH_INPUT=$PROJECT_DIR/build/benchmark/bench_reg
BENCH_MAGIC_INPUT=$PROJECT_DIR/build/benchmark/bench_magic

# Build Benchmark Driver
cd $BUILD_DIR
rm $BENCH_DIR/*.csv
cmake -DCMAKE_BUILD_TYPE=Bench ..
make clean
make -j$(nproc) bench-gmp bench-lib

# Generate benchmarks
BENCH_LEN=1000 # length of random sequence of operations
python3 $SCRIPT_DIR/benchmark-gen.py -l $BENCH_LEN -o $BENCH_INPUT
python3 $SCRIPT_DIR/benchmark-gen.py -l $BENCH_LEN -b 7 -o $BENCH_MAGIC_INPUT

# Benchmark Driver Params
BENCH_ITER=10000 # repeat each operation multiples times to bring it to microsecond level
GMP_PRECISION=128
RUNS=10 # run each benchmark file several times
SKIP=2 # skip stats of first n runs for warm cache

# benchmark output file naming
GMP_PREFIX=gmp
LIB_PREFIX=lib
MAGIC_PREFIX=magic

# GMP
$BUILD_DIR/bench-gmp add $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX-add.csv
$BUILD_DIR/bench-gmp sub $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX-sub.csv
$BUILD_DIR/bench-gmp mlt $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX-mlt.csv
$BUILD_DIR/bench-gmp div $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX-div.csv

# Libfixeypointy
$BUILD_DIR/bench-lib add $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-add.csv
$BUILD_DIR/bench-lib sub $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-sub.csv
$BUILD_DIR/bench-lib mlt $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-mlt.csv
$BUILD_DIR/bench-lib div $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-div.csv

# Benchmark magic mult and div
$BUILD_DIR/bench-gmp mlt $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$GMP_PREFIX-$MAGIC_PREFIX-mlt.csv
$BUILD_DIR/bench-gmp div $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$GMP_PREFIX-$MAGIC_PREFIX-div.csv

$BUILD_DIR/bench-lib mlt $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$LIB_PREFIX-$MAGIC_PREFIX-mlt.csv
$BUILD_DIR/bench-lib div $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$LIB_PREFIX-$MAGIC_PREFIX-div.csv