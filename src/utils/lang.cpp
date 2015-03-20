#include "lang.h"

#include "builtins.h"
#include "gc/alloc.h"
#include "utils/error.h"
#include "value/boolean.h"
#include "value/string.h"

bool vv::truthy(gc::managed_ptr<const value::object> val)
{
  if (val->type == &builtin::type::nil)
    return false;
  else if (val->type == &builtin::type::boolean)
    return static_cast<const value::boolean&>(*val).val;
  return true;
}

[[noreturn]]
vv::value::object_ptr vv::throw_exception(const std::string& value)
{
  throw vm_error{gc::alloc<value::string>( value )};
}

[[noreturn]]
vv::value::object_ptr vv::throw_exception(value::object_ptr value)
{
  throw vm_error{value};
}

vv::gc::managed_ptr<vv::value::basic_function> vv::find_method(
    gc::managed_ptr<value::type> t,
    symbol name)
{
  auto i = std::begin(t->methods);
  while ((i = t->methods.find(name)) == std::end(t->methods) && t->parent != t)
    t = t->parent;

  if (i != std::end(t->methods))
    return i->second;

  return nullptr;
}

std::string vv::pretty_print(value::object_ptr object, vm::machine& vm)
{
  if (object->members.contains({"str"}) || find_method(object->type, {"str"})) {
    vm.push(object);
    vm.readm({"str"});
    vm.call(0);
    vm.run_cur_scope();
    auto str = vm.top();
    vm.pop(1);
    return str->value();
  }

  return object->value();
}
