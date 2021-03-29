<img src="./logo/libfixeypointy.svg" alt="libfixeypointy" height="200">
*We couldn't use the name "libfixedpoint" because of SEO*

-----------------

[![CircleCI](https://circleci.com/gh/cmu-db/libfixeypointy/tree/main.svg?style=svg)](https://circleci.com/gh/cmu-db/libfixeypointy/tree/main)

This is a portable C++ library for fixed-point decimals. It was originally developed as part of the [NoisePage](https://noise.page) database project at Carnegie Mellon University.

This library implements decimals as 128-bit integers and stores them in scaled format. For example, it will store the decimal `12.23` with scale `5` `1223000`.
Addition and subtraction operations require two decimals of the same scale. Decimal multiplication accepts an argument of lower scale and returns a decimal in the higher scale. Decimal division accepts an argument of the denominator scale and returns the decimal in numerator scale. A rescale decimal function is also provided.

The following files are included:

* `decimal.cpp` - The core fixed decimal package supporting decimals with fixed precision(38) and a max scale of 38.
* `decimal_multiplication_generator.py` - Generates tests for the multiplication operations.
* `magic_number_generator.py` - To optimize multiplication and division with specific constants we can generate precompiled constats to speed it up.
