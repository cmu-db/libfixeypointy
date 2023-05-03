import random

def makeEqu(start, end):
  result = ""
  scale_a = random.randint(start, end)
  scale_b = random.randint(start, end)
  scale_b = scale_a
  
  val_a = random.random() * 10**4
  val_b = random.random() * 10**4
  
  neg_a = random.randint(0,1)
  if neg_a == 0:
    neg_a = -1
  neg_b = random.randint(0,1)
  if neg_b == 0:
    neg_b = -1

  op = random.randint(1,4)
  op_str = "+"
  if op == 1:
    op_str = "+"
  elif op == 2:
    op_str = "-"
  elif op == 3:
    op_str = "*"
  else:
    op_str = "/"

  result = "{} {} {}\n".format(val_a * neg_a, op_str, val_b * neg_b)

  return result

if __name__ == "__main__":
  repeat = 40000
  start = 1
  end = 38
  f = open("input.txt", 'w')
  
  for iter in range(repeat):
    result = makeEqu(start, end)
    f.write(result)

  f.close()
