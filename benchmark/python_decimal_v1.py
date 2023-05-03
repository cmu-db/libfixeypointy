import subprocess
import sys
from decimal import Decimal, getcontext
import time

def make_input(args, scale, bscale):
  if bscale:
    result = args[0] + ":" + str(scale)
    result += " " + args[1] + " "
    result += args[2] + ":" + str(scale)
  else:
    result = args[0] + " " + args[1] + " " + args[2]
  return result

def check_func(args, scale, f):
  # check libfixeypointy
  # data = make_input(args, scale)

  data = args[0] + " " + args[1] + " " +args[2] 

  p = subprocess.Popen(args=["./demo"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
  out = p.communicate(input=data)
  # out = p.communicate(input=data)
  libfix = out[0].split()
  size = len(libfix)
  if size < 2:
    print("overflow {}\n".format(data))
    return
  elapse_lib = libfix[size - 1]
  result_lib = libfix[size - 2]

  a_val = args[0]
  b_val = args[2]

  if args[0].find(':') != -1:
    a_list = args[0].split(':')
    a_scale = a_list[1]

    b_list = args[2].split(':')
    
    a_val = a_list[0]
    b_val = b_list[0]

    getcontext().prec = int(a_scale)

  getcontext().prec = 50
    
  result = Decimal("0.0")
  a = Decimal(a_val)
  b = Decimal(b_val)

  # if args[1] == "+":
  #   result = a + b
  # elif args[1] == "/":
  #   result = a / b
  # elif args[1] == "*":
  #   result = a * b
  # elif args[1] == "-":
  #   result = a - b

  # if args[1] == "+":
  #   result = a + b
  # elif args[1] == "/":
  #   result = a / b
  # elif args[1] == "*":
  #   result = a * b
  # elif args[1] == "-":
  #   result = a - b

  start_time = time.perf_counter_ns()
  if args[1] == "+":
    result = a + b
  elif args[1] == "/":
    result = a / b
  elif args[1] == "*":
    result = a * b
  elif args[1] == "-":
    result = a - b
  end_time = time.perf_counter_ns()
  Decimal_res = (end_time - start_time)

  result_str = str(result)
  temp = result_lib.rstrip('0')
  new_lib = temp.rstrip('.')

  comp = result_str == new_lib
  gap = Decimal(result_lib) - Decimal(result_str)

  f.write("{} result libfixeypointy: {}, Decimal: {}, comp: {}, gap: {} ".format(data, result_lib, result, comp, gap))
  f.write("Elapse time libfixeypointy: {}, Decimal: {}\n\n".format(elapse_lib, Decimal_res))
  # print("Elapse time avg liibfixeypointy: {}, Decimal: {}".format(sum(lib_res) / len(lib_res), sum(Decimal_res) / len(Decimal_res)))
  # print("Elapse time min libfixeypointy: {}, Decimal: {}".format(min(lib_res), min(Decimal_res)))
  # print("Elapse time max libfixeypointy: {}, Decimal: {}".format(max(lib_res), max(Decimal_res)))


def main():
  f_out = open("output.txt", 'w')
  with open("input.txt", 'r') as f:
    line = f.readline()
    while line:
      args = line.split()

      # print(args)

      # f_out.write("{} {} {} : ".format(args[0], args[1], args[2]))
      check_func(args, 15, f_out)

      line = f.readline()
  f.close()

if __name__ == '__main__':
  main()

