import random

def gen_regular():
    benchFile = open("bench_reg", 'w')
    length = 1000
    for _ in range(length):
        op1 = random.uniform(1.0, 1000000000.0)
        op2 = random.uniform(1.0, 1000000000.0)
        line = str(op1) + ' ' + str(op2) + '\n'
        benchFile.write(line)
    benchFile.close()

def gen_magic():
    benchFile = open("bench_magic", 'w')
    length = 1000
    for _ in range(length):
        op1 = random.uniform(1.0, 1000000000.0)
        # op1 = int(op1)
        op2 = 7
        line = str(op1) + ' ' + str(op2) + '\n'
        benchFile.write(line)
    benchFile.close()

def main():
    gen_regular()
    gen_magic()

if __name__ == "__main__":
    main()
