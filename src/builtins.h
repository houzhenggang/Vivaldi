#ifndef VV_BUILTINS_H
#define VV_BUILTINS_H

#include "value.h"
#include "vm/call_frame.h"

namespace vv {

namespace builtin {

// Commonly used symbols; defined for convenience and to prevent repeated
// constructions, since constructing a symbol is relatively expensive.
namespace sym {

// Defined in builtins.cpp

const extern vv::symbol self;
const extern vv::symbol call;

const extern vv::symbol start;
const extern vv::symbol at_end;
const extern vv::symbol get;
const extern vv::symbol increment;

const extern vv::symbol add;
const extern vv::symbol subtract;
const extern vv::symbol times;
const extern vv::symbol divides;

const extern vv::symbol equals;
const extern vv::symbol unequal;

const extern vv::symbol greater;
const extern vv::symbol less;
const extern vv::symbol greater_equals;
const extern vv::symbol less_equals;
}

// Standalone functions in the standard library.
namespace function {

// Defined in builtins.cpp

extern value::builtin_function print;
extern value::builtin_function puts;
extern value::builtin_function gets;

extern value::builtin_function filter;
extern value::builtin_function map;
extern value::builtin_function reduce;
extern value::builtin_function sort;
extern value::builtin_function all;
extern value::builtin_function any;
extern value::builtin_function count;

extern value::builtin_function quit;
extern value::builtin_function reverse;

}

// Types in the standard library.
namespace type {

// Each class has its own file in the builtins/ directory

extern value::type array;
extern value::type array_iterator;
extern value::type boolean;
extern value::type dictionary;
extern value::type custom_type;
extern value::type file;
extern value::type floating_point;
extern value::type function;
extern value::type integer;
extern value::type object;
extern value::type nil;
extern value::type range;
extern value::type regex;
extern value::type regex_result;
extern value::type string;
extern value::type string_iterator;
extern value::type symbol;

}

// Populate the provided environment with standard library types and functions.
void make_base_env(vm::environment& base);

}

}

#endif
