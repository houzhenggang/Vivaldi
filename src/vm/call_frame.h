#ifndef VV_VM_CALL_FRAME_H
#define VV_VM_CALL_FRAME_H

#include "instruction.h"

#include "symbol.h"
#include "value.h"
#include "utils/dumb_ptr.h"
#include "utils/hash_map.h"
#include "utils/vector_ref.h"

namespace vv {

namespace vm {

// Class representing a local execution environment.
//
// Each call frame has its own environment. If this is a closure (and all
// Vivaldi functions are closures) or a block, the enclosing environment is
// stored in environment::enclosing.
//
// Execution environments are, hackishly, actually Vivaldi objects (they
// inherit from value::object); this is because, because of closures, manual
// memory management is basically impossible, and even my simplistic garbage
// collector is significantly faster than reference counting via shared_ptr.
class environment : public value::object {
public:
  environment(gc::managed_ptr<environment> enclosing = nullptr,
              value::object_ptr self                 = nullptr);

  // Frame in which current function (i.e. closure) was defined.
  gc::managed_ptr<environment> enclosing;
  // self, if this is a method call.
  value::object_ptr self;

  void mark() override;
};

// A single call frame. The VM's call stack is implemented as a vector of these
// in vm::machine. Each one represents a single function call.
struct call_frame {
  call_frame(vector_ref<vm::command> instr_ptr = {},
             gc::managed_ptr<environment> env  = nullptr,
             size_t argc                       = 0,
             size_t frame_ptr                  = 0);

  // The numer of arguments the called function takes (used in stack
  // manipulation)
  size_t argc;
  // The position in the VM stack were local variables begin.
  size_t frame_ptr;

  // The outermost environment. Given lexical scoping, this will of course be
  // different for each call frame.
  gc::managed_ptr<environment> env;

  // The calling function, if there is one; stored here only for GC purposes.
  value::object_ptr caller;

  // The local catch function, if we're in a try...catch block.
  value::object_ptr catcher;

  // Pointer to the current VM instruction we're executing.
  vector_ref<vm::command> instr_ptr;
};

}

}

#endif
