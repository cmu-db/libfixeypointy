#!/bin/bash
# Run this script from util to generate and execute benchmarks

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

UTIL_DIR=$PROJECT_DIR/util
BUILD_DIR=$PROJECT_DIR/build
BENCH_DIR=$PROJECT_DIR/build/benchmark
BENCH_INPUT=$PROJECT_DIR/build/benchmark/bench_reg
BENCH_MAGIC_INPUT=$PROJECT_DIR/build/benchmark/bench_magic

# Build Benchmark Drivers
cd $BUILD_DIR
mkdir $BENCH_DIR
rm $BENCH_DIR/*.csv
cmake -DCMAKE_BUILD_TYPE=Bench ..
cd $PROJECT_DIR
make clean
make -j$(nproc) bench-lib bench-native bench-gmp

# Generate benchmarks
BENCH_LEN=10 # length of random sequence of operations
python3 $SCRIPT_DIR/benchmark-gen.py -l $BENCH_LEN -o $BENCH_INPUT
python3 $SCRIPT_DIR/benchmark-gen.py -l $BENCH_LEN -b 7 -o $BENCH_MAGIC_INPUT

# Benchmark Driver Params
BENCH_ITER=1000 # repeat each operation multiples times to bring it to microsecond level
RUNS=10 # run each benchmark file several times
SKIP=2 # skip stats of first n runs for warm cache

# benchmark output file naming
MAGIC_PREFIX=magic
LIB_PREFIX=lib
NATIVE_PREVIX=native
GMP_PREFIX=gmp
PY_PREFIX=pyDecimal
JAVA_PREFIX=java

# Libfixeypointy
$BUILD_DIR/bench-lib add $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-add.csv
$BUILD_DIR/bench-lib sub $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-sub.csv
$BUILD_DIR/bench-lib mlt $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-mlt.csv
$BUILD_DIR/bench-lib div $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$LIB_PREFIX-div.csv
$BUILD_DIR/bench-lib mlt $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$LIB_PREFIX-$MAGIC_PREFIX-mlt.csv
$BUILD_DIR/bench-lib div $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$LIB_PREFIX-$MAGIC_PREFIX-div.csv

# Native Double
$BUILD_DIR/bench-native add $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$NATIVE_PREVIX-add.csv
$BUILD_DIR/bench-native sub $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$NATIVE_PREVIX-sub.csv
$BUILD_DIR/bench-native mlt $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$NATIVE_PREVIX-mlt.csv
$BUILD_DIR/bench-native div $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$NATIVE_PREVIX-div.csv
$BUILD_DIR/bench-native mlt $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$NATIVE_PREVIX-$MAGIC_PREFIX-mlt.csv
$BUILD_DIR/bench-native div $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$NATIVE_PREVIX-$MAGIC_PREFIX-div.csv

# GMP
for GMP_PRECISION in 64 128
do
$BUILD_DIR/bench-gmp add $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX$GMP_PRECISION-add.csv
$BUILD_DIR/bench-gmp sub $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX$GMP_PRECISION-sub.csv
$BUILD_DIR/bench-gmp mlt $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX$GMP_PRECISION-mlt.csv
$BUILD_DIR/bench-gmp div $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$GMP_PREFIX$GMP_PRECISION-div.csv
$BUILD_DIR/bench-gmp mlt $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$GMP_PREFIX$GMP_PRECISION-$MAGIC_PREFIX-mlt.csv
$BUILD_DIR/bench-gmp div $RUNS $SKIP $GMP_PRECISION $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$GMP_PREFIX$GMP_PRECISION-$MAGIC_PREFIX-div.csv
done

# Python Decimal
python3 $UTIL_DIR/benchmark-python.py -m add -t $RUNS -s $SKIP -i $BENCH_ITER -b $BENCH_INPUT > $BENCH_DIR/$PY_PREFIX-add.csv
python3 $UTIL_DIR/benchmark-python.py -m sub -t $RUNS -s $SKIP -i $BENCH_ITER -b $BENCH_INPUT > $BENCH_DIR/$PY_PREFIX-sub.csv
python3 $UTIL_DIR/benchmark-python.py -m mlt -t $RUNS -s $SKIP -i $BENCH_ITER -b $BENCH_INPUT > $BENCH_DIR/$PY_PREFIX-mlt.csv
python3 $UTIL_DIR/benchmark-python.py -m div -t $RUNS -s $SKIP -i $BENCH_ITER -b $BENCH_INPUT > $BENCH_DIR/$PY_PREFIX-div.csv
python3 $UTIL_DIR/benchmark-python.py -m mlt -t $RUNS -s $SKIP -i $BENCH_ITER -b $BENCH_MAGIC_INPUT > $BENCH_DIR/$PY_PREFIX-$MAGIC_PREFIX-mlt.csv
python3 $UTIL_DIR/benchmark-python.py -m div -t $RUNS -s $SKIP -i $BENCH_ITER -b $BENCH_MAGIC_INPUT > $BENCH_DIR/$PY_PREFIX-$MAGIC_PREFIX-div.csv

# Java BigDecimal
cd $UTIL_DIR
javac $UTIL_DIR/benchmark-java.java
java BenchJava add $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$JAVA_PREFIX-add.csv
java BenchJava sub $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$JAVA_PREFIX-sub.csv
java BenchJava mlt $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$JAVA_PREFIX-mlt.csv
java BenchJava div $RUNS $SKIP $BENCH_ITER $BENCH_INPUT > $BENCH_DIR/$JAVA_PREFIX-div.csv
java BenchJava mlt $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$JAVA_PREFIX-mlt.csv
java BenchJava div $RUNS $SKIP $BENCH_ITER $BENCH_MAGIC_INPUT > $BENCH_DIR/$JAVA_PREFIX-div.csv
