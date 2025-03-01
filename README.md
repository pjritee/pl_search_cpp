# PLSearchCPP

PLSearchCPP is a C++ library that approximates Prolog predicates using a continuation-passing style. It provides a framework for defining and executing Prolog-like predicates, including support for backtracking, choice points, and unification.

## Features

- Prolog-like predicates
- Continuation-passing style execution
- Backtracking and choice points
- Unification of terms
- Support for variables, integers, floats, atoms, and lists

## Installation

### Prerequisites

- C++11 or later
- CMake 3.10 or later

### Building the Library

1. Clone the repository:

```sh
git clone https://github.com/yourusername/PLSearchCPP.git
cd PLSearchCPP
```

2. Create a build directory and navigate to it:

```sh
mkdir build
cd build
```

3: Run CMake to configure the project:

```sh
cmake ..
```

4: Build the project:

```sh
make
```

This will build the pl_search library and place it in the lib directory.

## Examples

The examples directory contains the following examples of using the library.

- A solver for the SEND+MORE=MONEY puzzle (<code>send_more_money.cpp</code>).
- An example of a user defined Term type (<code>prolog_list.hpp</code>).
- An implementation of the Prolog append predicate using the above type (<code>append_pred.hpp</code>).
- A main program ( <code>prolog_list.cpp</code>) that exercises the definitions above.
- A Makefile to build the executables.

## License

This project is licensed under the MIT License - see the LICENSE file for details.