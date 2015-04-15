#ifndef VV_EXPRESSION_H
#define VV_EXPRESSION_H

#include "symbol.h"

#include <vector>

namespace vv {

namespace vm {

struct command;

}

namespace ast {

// Base class for all AST nodes.
class expression {
public:
  // Code generator (internal; should be made protected); override to implement
  // AST class.
  virtual std::vector<vm::command> generate() const = 0;
  // Returns finalized VM code for this AST subtree.
  std::vector<vm::command> code() const;
  virtual ~expression() { }
};

class assignment;
class array;
class block;
class cond_statement;
class dictionary;
class except;
class for_loop;
class function_call;
class function_definition;
class logical_and;
class logical_or;
class member;
class member_assignment;
class require;
class return_statement;
class try_catch;
class type_definition;
class while_loop;
class variable;
class variable_declaration;

namespace literal {

class boolean;
class character;
class floating_point;
class integer;
class nil;
class regex;
class string;
class symbol;

}

}

}

#endif
