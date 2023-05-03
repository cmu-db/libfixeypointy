import java.io.*;
import java.math.BigDecimal;

/*
 * Overflow behavior:
 * Multiply/Divide force a set-precision operation, if unable to complete:
 * 1. revert result as if test line has not been executed
 * 2. log overflow to output
 */
class BenchJava {
  public static void Bench(String mode, int trials, int skip, int iterations, String bench_file) {
    long t_op_start = System.nanoTime();
    long t_op_end = System.nanoTime();

    for (int trial = 0; trial < trials; ++trial) {
      try {
        BufferedReader br = new BufferedReader(new FileReader(bench_file));
        long init_time = 0;
        long op_time = 0;

        String line;
        while ((line = br.readLine()) != null) {
          String[] tok = line.split(" ");
          String op1_s = tok[0];
          String op2_s = tok[1];
          BigDecimal[] op1_arr = new BigDecimal[iterations];
          BigDecimal[] op2_arr = new BigDecimal[iterations];

          long t_init_start = System.nanoTime();
          for (int i = 0; i < iterations; ++i) {
            op1_arr[i] = new BigDecimal(op1_s);
            op2_arr[i] = new BigDecimal(op2_s);
          }
          long t_init_end = System.nanoTime();

          if (mode.equals("add")) {
            t_op_start = System.nanoTime();
            for (int i = 0; i < iterations; ++i) {
              op1_arr[i].add(op2_arr[i]);
            }
            t_op_end = System.nanoTime();
          } else if (mode.equals("sub")) {
            t_op_start = System.nanoTime();
            for (int i = 0; i < iterations; ++i) {
              op1_arr[i].subtract(op2_arr[i]);
            }
            t_op_end = System.nanoTime();
          } else if (mode.equals("mlt")) {
            t_op_start = System.nanoTime();
            for (int i = 0; i < iterations; ++i) {
              op1_arr[i].multiply(op2_arr[i]);
            }
            t_op_end = System.nanoTime();
          } else if (mode.equals("div")) {
            t_op_start = System.nanoTime();
            for (int i = 0; i < iterations; ++i) {
              op1_arr[i].divide(op2_arr[i]);
            }
            t_op_end = System.nanoTime();
          } else {
            System.out.println("Unsupported benchmark mode");
            System.exit(1);
          }

          op1_arr = null;
          op2_arr = null;

          long init_duration = t_init_end - t_init_start;
          long op_duration = t_op_end - t_op_start;
          init_time += init_duration / 1000;
          op_time += op_duration / 1000;
        }

        if (trial >= skip) {
          System.out.println(Long.toString(init_time) + "," + Long.toString(op_time));
        }
      } catch (Exception e) {
        System.out.println(e.getMessage());
      }
    }
  }

  public static void main(String[] args) {
    String mode = args[0];
    int trials = Integer.valueOf(args[1]);
    int skip = Integer.valueOf(args[2]);
    int iterations = Integer.valueOf(args[3]);
    String bench_file = args[4];

    Bench(mode, trials, skip, iterations, bench_file);
  }
}