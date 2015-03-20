#include "value.h"

#include "builtins.h"
#include "gc.h"
#include "ast/function_definition.h"
#include "utils/lang.h"
#include "value/builtin_function.h"
#include "value/function.h"

using namespace vv;

value::object::object(gc::managed_ptr<struct type> new_type)
  : members  {},
    type     {new_type},
    m_marked {false}
{ }

value::object::object()
  : members  {},
    type     {&builtin::type::object},
    m_marked {false}
{ }

size_t value::object::hash() const
{
  const static std::hash<const void*> hasher{};
  return hasher(static_cast<const void*>(this));
}

bool value::object::equals(const value::object& other) const
{
  return this == &other;
}

void value::object::mark()
{
  m_marked = true;
  if (type && !type->marked())
    gc::mark(*type);
  for (auto& i : members)
    if (!i.second->marked())
      gc::mark(*i.second);
}

value::basic_function::basic_function(func_type type,
                                      int argc,
                                      gc::managed_ptr<vm::environment> enclosing,
                                      vector_ref<vm::command> body)
  : object    {&builtin::type::function},
    type      {type},
    argc      {argc},
    enclosing {enclosing},
    body      {body}
{ }

value::type::type(const std::function<gc::managed_ptr<object>()>& constructor,
                  const hash_map<vv::symbol, gc::managed_ptr<basic_function>>& methods,
                  gc::managed_ptr<type> parent,
                  vv::symbol name)
  : object      {&builtin::type::custom_type},
    methods     {methods},
    constructor {constructor},
    parent      {parent},
    name        {name}
{
  if (auto init = find_method(this, {"init"})) {
    init_shim.argc = init->argc;

    for (auto i = 0; i != init_shim.argc; ++i) {
      init_shim.body.emplace_back(vm::instruction::arg, i);
    }

    init_shim.body.emplace_back( vm::instruction::self );
    init_shim.body.emplace_back( vm::instruction::readm, vv::symbol{"init"} );
    init_shim.body.emplace_back( vm::instruction::call, init_shim.argc );
    init_shim.body.emplace_back( vm::instruction::self );
    init_shim.body.emplace_back( vm::instruction::ret, false );
  }
  else {
    init_shim.argc = 0;
    init_shim.body = {
      { vm::instruction::self },
      { vm::instruction::ret, false }
    };
  }
}

std::string value::type::value() const { return to_string(name); }

void value::type::mark()
{
  object::mark();
  for (const auto& i : methods)
    if (!i.second->marked())
      gc::mark(*i.second);
  if (!parent->marked())
    gc::mark(*parent);
}

size_t std::hash<vv::gc::managed_ptr<vv::value::object>>::operator()(
    const vv::gc::managed_ptr<vv::value::object> b) const
{
  return b->hash();
}

bool std::equal_to<vv::gc::managed_ptr<vv::value::object>>::operator()(
    const vv::gc::managed_ptr<vv::value::object> left,
    const vv::gc::managed_ptr<vv::value::object> right) const
{
  return left->equals(*right);
}
