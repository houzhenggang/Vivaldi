#include "builtins.h"

#include "gc.h"
#include "utils/lang.h"
#include "value/array.h"
#include "value/array_iterator.h"
#include "value/builtin_function.h"
#include "value/opt_functions.h"

using namespace vv;
using namespace builtin;

namespace {

// Array {{{

value::base* fn_array_init(vm::machine& vm)
{
  vm.self();
  auto arr = static_cast<value::array*>(vm.top());
  vm.arg(0);
  auto arg = vm.top();
  if (arg->type != &type::array)
    return throw_exception("Arrays can only be constructed from other Arrays");
  arr->val = static_cast<value::array*>( arg )->val;
  return arr;
}

value::base* fn_array_size(value::base* self)
{
  auto sz = static_cast<value::array*>(self)->val.size();
  return gc::alloc<value::integer>( static_cast<int>(sz) );
}

value::base* fn_array_append(value::base* self, value::base* arg)
{
  static_cast<value::array*>(self)->val.push_back(arg);
  return self;
}

value::base* fn_array_pop(value::base* self)
{
  auto arr = static_cast<value::array*>(self);
  auto val = arr->val.back();
  arr->val.pop_back();
  return val;
}

value::base* fn_array_at(value::base* self, value::base* arg)
{
  if (arg->type != &type::integer)
    return throw_exception("Index must be an Integer");
  auto val = static_cast<value::integer*>(arg)->val;
  const auto& arr = static_cast<value::array*>(self)->val;
  if (arr.size() <= static_cast<unsigned>(val) || val < 0)
    return throw_exception("Out of range (expected 0-"
                           + std::to_string(arr.size()) + ", got "
                           + std::to_string(val) + ")");
  return arr[static_cast<unsigned>(val)];
}

value::base* fn_array_set_at(vm::machine& vm)
{
  vm.arg(0);
  auto arg = vm.top();
  if (arg->type != &type::integer)
    return throw_exception("Index must be an Integer");
  auto val = static_cast<value::integer*>(arg)->val;
  vm.self();
  auto& arr = static_cast<value::array&>(*vm.top()).val;
  if (arr.size() <= static_cast<unsigned>(val) || val < 0)
    return throw_exception("Out of range (expected 0-"
                           + std::to_string(arr.size()) + ", got "
                           + std::to_string(val) + ")");
  vm.arg(1);
  return arr[static_cast<unsigned>(val)] = vm.top();
}

value::base* fn_array_start(value::base* self)
{
  auto& arr = static_cast<value::array&>(*self);
  return gc::alloc<value::array_iterator>( arr );
}

value::base* fn_array_stop(value::base* self)
{
  auto& arr = static_cast<value::array&>(*self);
  auto iter = gc::alloc<value::array_iterator>( arr );
  iter->idx = arr.val.size();
  return iter;
}

value::base* fn_array_add(value::base* self, value::base* arg)
{
  auto arr = static_cast<value::array*>(self);
  if (arg->type != &type::array)
    return throw_exception("Only Arrays can be added to other Arrays");
  auto other = static_cast<value::array*>(arg);
  copy(begin(other->val), end(other->val), back_inserter(arr->val));
  return arr;
}

// }}}
// Iterator {{{

value::base* fn_array_iterator_at_start(value::base* self)
{
  auto iter = static_cast<value::array_iterator*>(self);
  return gc::alloc<value::boolean>( iter->idx == 0 );
}

value::base* fn_array_iterator_at_end(value::base* self)
{
  auto iter = static_cast<value::array_iterator*>(self);
  return gc::alloc<value::boolean>( iter->idx == iter->arr.val.size() );
}

value::base* fn_array_iterator_get(value::base* self)
{
  auto iter = static_cast<value::array_iterator*>(self);
  if (iter->idx == iter->arr.val.size())
    return throw_exception("ArrayIterator is at end of array");
  return iter->arr.val[iter->idx];
}

value::base* fn_array_iterator_increment(value::base* self)
{
  auto iter = static_cast<value::array_iterator*>(self);
  if (iter->idx == iter->arr.val.size())
    return throw_exception("ArrayIterators cannot be incremented past end");
  iter->idx += 1;
  return iter;
}

value::base* fn_array_iterator_decrement(value::base* self)
{
  auto iter = static_cast<value::array_iterator*>(self);
  if (iter->idx == 0)
    return throw_exception("ArrayIterators cannot be decremented past start");
  iter->idx -= 1;
  return iter;
}

value::base* fn_array_iterator_add(value::base* self, value::base* arg)
{
  auto iter = static_cast<value::array_iterator*>(self);

  if (arg->type != &builtin::type::integer)
    return throw_exception("Only Integers can be added to ArrayIterators");
  auto offset = static_cast<value::integer*>(arg)->val;

  if (static_cast<int>(iter->idx) + offset < 0)
    return throw_exception("ArrayIterators cannot be decremented past start");
  if (iter->idx + offset > iter->arr.val.size())
    return throw_exception("ArrayIterators cannot be incremented past end");

  auto other = gc::alloc<value::array_iterator>( *iter );
  static_cast<value::array_iterator*>(other)->idx = iter->idx + offset;
  return other;
}

value::base* fn_array_iterator_subtract(value::base* self, value::base* arg)
{
  auto iter = static_cast<value::array_iterator*>(self);

  if (arg->type != &builtin::type::integer)
    return throw_exception("Only Integers can be added to ArrayIterators");
  auto offset = static_cast<value::integer*>(arg)->val;

  if (!offset)
    return throw_exception("Only numeric types can be added to ArrayIterators");
  if (static_cast<int>(iter->idx) - offset < 0)
    return throw_exception("ArrayIterators cannot be decremented past start");
  if (iter->idx - offset > iter->arr.val.size())
    return throw_exception("ArrayIterators cannot be incremented past end");

  auto other = gc::alloc<value::array_iterator>( *iter );
  static_cast<value::array_iterator*>(other)->idx = iter->idx - offset;
  return other;
}

value::base* fn_array_iterator_equals(value::base* self, value::base* arg)
{
  auto iter = static_cast<value::array_iterator*>(self);
  auto other = static_cast<value::array_iterator*>(arg);
  return gc::alloc<value::boolean>( &iter->arr == &other->arr
                                  && iter->idx == other->idx );
}

value::base* fn_array_iterator_unequal(value::base* self, value::base* arg)
{
  auto iter = static_cast<value::array_iterator*>(self);
  auto other = static_cast<value::array_iterator*>(arg);
  return gc::alloc<value::boolean>( &iter->arr != &other->arr
                                  || iter->idx != other->idx );
}

value::base* fn_array_iterator_greater(value::base* self, value::base* arg)
{
  auto iter = static_cast<value::array_iterator*>(self);
  auto other = static_cast<value::array_iterator*>(arg);
  if (&iter->arr != &other->arr)
    return throw_exception("Only ArrayIterators from the same Array can be compared");
  return gc::alloc<value::boolean>(iter->idx > other->idx );
}

value::base* fn_array_iterator_less(value::base* self, value::base* arg)
{
  auto iter = static_cast<value::array_iterator*>(self);
  auto other = static_cast<value::array_iterator*>(arg);
  if (&iter->arr != &other->arr)
    return throw_exception("Only ArrayIterators from the same Array can be compared");
  return gc::alloc<value::boolean>(iter->idx < other->idx );
}

// }}}

value::builtin_function array_init   {fn_array_init,   1};
value::opt_monop        array_size   {fn_array_size     };
value::opt_binop        array_append {fn_array_append   };
value::opt_monop        array_pop    {fn_array_pop      };
value::opt_binop        array_at     {fn_array_at       };
value::builtin_function array_set_at {fn_array_set_at, 2};
value::opt_monop        array_start  {fn_array_start    };
value::opt_monop        array_stop   {fn_array_stop     };
value::opt_binop        array_add    {fn_array_add      };

value::opt_monop array_iterator_at_start  {fn_array_iterator_at_start };
value::opt_monop array_iterator_at_end    {fn_array_iterator_at_end   };
value::opt_monop array_iterator_get       {fn_array_iterator_get      };
value::opt_binop array_iterator_equals    {fn_array_iterator_equals   };
value::opt_binop array_iterator_unequal   {fn_array_iterator_unequal  };
value::opt_binop array_iterator_greater   {fn_array_iterator_greater  };
value::opt_binop array_iterator_less      {fn_array_iterator_less     };
value::opt_monop array_iterator_increment {fn_array_iterator_increment};
value::opt_monop array_iterator_decrement {fn_array_iterator_decrement};
value::opt_binop array_iterator_add       {fn_array_iterator_add      };
value::opt_binop array_iterator_subtract  {fn_array_iterator_subtract };
}

value::type type::array {gc::alloc<value::array>, {
  { {"init"},   &array_init },
  { {"size"},   &array_size },
  { {"append"}, &array_append },
  { {"pop"},    &array_pop },
  { {"at"},     &array_at },
  { {"set_at"}, &array_set_at },
  { {"start"},  &array_start },
  { {"stop"},   &array_stop },
  { {"add"},    &array_add }
}, builtin::type::object, {"Array"}};

value::type type::array_iterator {[]{ return nullptr; }, {
  { {"at_start"},  &array_iterator_at_start },
  { {"at_end"},    &array_iterator_at_end },
  { {"get"},       &array_iterator_get },
  { {"equals"},    &array_iterator_equals },
  { {"unequal"},   &array_iterator_unequal },
  { {"greater"},   &array_iterator_greater },
  { {"less"},      &array_iterator_less },
  { {"increment"}, &array_iterator_increment },
  { {"decrement"}, &array_iterator_decrement },
  { {"add"},       &array_iterator_add },
  { {"subtract"},  &array_iterator_subtract },
}, builtin::type::object, {"ArrayIterator"}};
