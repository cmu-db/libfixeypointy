import subprocess
import argparse
import sys
from decimal import Decimal, getcontext
import time

def Bench(mode, trials, skip, iterations, bench_file, check_float):
  t_op_start = time.perf_counter_ns()
  t_op_end = time.perf_counter_ns()

  if check_float:
    print("check float point in Decimal")

  for trial in range(trials):
    f = open(bench_file, 'r')
    init_time = 0
    op_time = 0
    
    line = f.readline()
    while line:
      op_list = line.split()
      
      op1_arr = []
      op2_arr = []
      
      if check_float == False:
        op1_s = op_list[0]
        op2_s = op_list[1]

        t_init_start = time.perf_counter_ns()
        for i in range(iterations):
          op1_arr.append(Decimal(op1_s))
          op2_arr.append(Decimal(op2_s))
        t_init_end = time.perf_counter_ns()
      else:
        op1_f = float(op_list[0])
      
      if mode == "add":
        t_op_start = time.perf_counter_ns()
        for i in range(iterations):
          op1_arr[i] += op2_arr[i]
        t_op_end = time.perf_counter_ns()
      elif mode == "sub":
        t_op_start = time.perf_counter_ns()
        for i in range(iterations):
          op1_arr[i] -= op2_arr[i]
        t_op_end = time.perf_counter_ns()
      elif mode == "mlt":
        t_op_start = time.perf_counter_ns()
        for i in range(iterations):
          op1_arr[i] *= op2_arr[i]
        t_op_end = time.perf_counter_ns()
      elif mode == "div":
        t_op_start = time.perf_counter_ns()
        for i in range(iterations):
          op1_arr[i] /= op2_arr[i]
        t_op_end = time.perf_counter_ns()
      else:
        print("Unsupported benchmark mode \n")
      
      init_duration = t_init_end - t_init_start
      op_duration = t_op_end - t_op_start
      
      init_time += init_duration
      op_time += op_duration
      
      line = f.readline()
      
    if trial >= skip:
      print("{}, {}\n".format(init_time/1000, op_time/1000))

def setup_argparse():
  parser = argparse.ArgumentParser(description='Decimal parser')
  parser.add_argument('-v', '--verbose', action='store_true', help='Print extra information to stdout', default=False)

  parser.add_argument('-m', '--mode', type=str, default="add", help='op mode (add, sub, mlt, div)', required=True)
  parser.add_argument('-t', '--trials', type=int, default=20, help='number of trials', required=True)
  parser.add_argument('-s', '--skip', type=int, default=20, help='number of skip', required=True)
  parser.add_argument('-i', '--iterations', type=int, default=20, help='number of iterations', required=True)
  parser.add_argument('-b', '--bench-file', type=str, default="./input.txt", help='bench data input file path', required=True)
  parser.add_argument('-c', '--check-float', default=False, help='check float point Decimal', required=False)
  parser.add_argument('-p', '--precision', type=int, default=128, help='Decimal precision', required=False)

  return parser

def main():
  global VERBOSE
  parser = setup_argparse()
  args = parser.parse_args()
  VERBOSE = args.verbose
  # if not validate_args(args):
  #   print('An error occurred during parameter validation, exiting without doing anything...')
  #   sys.exit(-1)
  Bench(args.mode, args.trials, args.skip, args.iterations, args.bench_file, args.check_float)
  sys.exit(0)

if __name__ == '__main__':
  main()
