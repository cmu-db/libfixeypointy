#!/usr/bin/env python3

import sys
import os
import argparse
import datetime
import string

HEADER_TEMPLATE = """// custom_magic_numbers.h
//
// *WARNING*
// This header file was auto-generated on $DATE. You probably do not want 
// to edit this file by hand! If you need to generate new constant numbers to 
// the mappings, you can execute the following command to recreate this file:
// 
//    $NAME $INPUTS
//
#pragma once

#include "common.h"

namespace libfixeypointy {

/**
 * Magic numbers for 128-bit division with custom constants.
 */
std::unordered_map<uint128_t, MagicNumber128, Unsigned128BitHash> MAGIC_CUSTOM_128BIT_CONSTANT_DIVISION = {
    // clang-format off
$MAP128
    // clang-format on
};

/**
 * Magic numbers for 256-bit division with custom constants.
 */
std::unordered_map<uint128_t, MagicNumber256, Unsigned128BitHash> MAGIC_CUSTOM_256BIT_CONSTANT_DIVISION = {
    // clang-format off
$MAP256
    // clang-format on
};

}  // namespace libfixeypointy
"""

# Hacker's Delight [2E Chapter 10 Integer Division By Constants] describes
# an algorithm for fast unsigned integer division through the use of
# "magic numbers" that satisfy nice mathematical properties.
#
# Given divisors and word sizes, this script generates their corresponding
# magic numbers by implementing the Hacker's Delight algorithm.

def get_magic_number(divisor, word_size):
    """
    Given word size W >= 1 and divisor d, where 1 <= d < 2**W,
    finds the least integer m and integer p such that
        floor(mn // 2**p) == floor(n // d) for 0 <= n < 2**W
    with 0 <= m < 2**(W+1) and p >= W.

    Implements the algorithm described by Hacker's Delight [2E], specifically,
    section 10-9 Unsigned Division by Divisors >= 1.

    Parameters
    ----------
    divisor : int
        The divisor d in the problem statement.
    word_size : int
        The word size W in the problem statement. The number of bits.

    Returns
    -------
    M : hex
        The magic number.
    p : int
        The exponent p.
    algorithm_type : See common.h
    """
    d, W = divisor, word_size

    nc = (2**W // d) * d - 1                                # Eqn (24a)
    for p in range(2 * W + 1):                              # Eqn (29)
        if 2 ** p > nc * (d - 1 - (2 ** p - 1) % d):        # Eqn (27)
            m = (2 ** p + d - 1 - (2 ** p - 1) % d) // d    # Eqn (26)
            # Unsigned case, the magic number M is given by:
            #   m             if 0 <= m < 2**W
            #   m - 2**W      if 2**W <= m < 2**(W+1)
            if 0 <= m < 2**W:
                return d, W, m, p, "AlgorithmType::OVERFLOW_SMALL"
            elif 2**W <= m < 2**(W+1):
                return d, W, m - 2**W, p, "AlgorithmType::OVERFLOW_LARGE"
            else:
                raise RuntimeError("Invalid case. Not enough bits?")


def format_entry(magic_number_output):
    d, word_size, m, p, alg = magic_number_output

    # Convert the magic number into a bitstring padded with zeros
    # Then break it up into 64-bit chunks from highest to lowest
    m_bits = format(m, "0{0}b".format(word_size))
    chunks = []
    for i in range(int(word_size / 64)):
        idx = i * 64
        chunks.append(hex(int(m_bits[idx:idx+64], 2)))
    return " "*4 + "{{ {0}, {{{1}, {2}, {3} }} }}".format(d, ", ".join(chunks), p, alg)


if __name__ == "__main__":
    aparser = argparse.ArgumentParser()
    aparser.add_argument('divisor', type=int, nargs='+', help='Divisors')
    args = vars(aparser.parse_args())

    magic128 = []
    magic256 = []
    for d in args['divisor']:
        magic128.append(format_entry(get_magic_number(d, 128)))
        magic256.append(format_entry(get_magic_number(d, 256)))

    t = string.Template(HEADER_TEMPLATE)
    values = {
        "NAME":     os.path.basename(sys.argv[0]),
        "DATE":     datetime.datetime.now().strftime("%Y-%m-%d %H:%M"),
        "INPUTS":   " ".join(map(str, args['divisor'])),
        "MAP128":   ",\n".join(magic128),
        "MAP256":   ",\n".join(magic256),
    }
    print(t.substitute(values))
# MAIN
