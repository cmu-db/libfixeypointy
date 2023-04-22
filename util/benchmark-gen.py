import random
import sys
import getopt

VALUE_MIN = 1.0
VALUE_MAX = 1000000000.0

def gen(outfile, length, fix_op1, fix_op2):
    benchFile = open(outfile, 'w')
    for _ in range(length):
        if fix_op1 == '':
            op1 = random.uniform(VALUE_MIN, VALUE_MAX)
        else:
            op1 = float(fix_op1)
        if fix_op2 == '':
            op2 = random.uniform(VALUE_MIN, VALUE_MAX)
        else:
            op2 = float(fix_op2)
        line = str(op1) + ' ' + str(op2) + '\n'
        benchFile.write(line)
    benchFile.close()

def main(argv):
    length = 0
    fix_op1 = ''
    fix_op2 = ''
    outfile = ''
    opts, args = getopt.getopt(argv,"hl:a:b:o:")
    for opt, arg in opts:
        if opt == '-h':
         print ('benchmark-gen.py -l <length> -a <fix op1> -b <fix op2>')
         sys.exit()
        elif opt in ("-l"):
            length = int(arg)
        elif opt in ("-a"):
            fix_op1 = arg
        elif opt in ("-b"):
            fix_op2 = arg
        elif opt in ("-o"):
            outfile = arg
    print('Generating bench sequence with len =', length, 'fixed ops [', fix_op1, ',', fix_op2,']')
    gen(outfile, length, fix_op1, fix_op2)


if __name__ == "__main__":
    main(sys.argv[1:])
