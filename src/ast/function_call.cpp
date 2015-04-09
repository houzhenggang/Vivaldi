#include "function_call.h"

#include "vm/instruction.h"

using namespace vv;

ast::function_call::function_call(
    std::unique_ptr<ast::expression>&& name,
    std::vector<std::unique_ptr<ast::expression>>&& args)
  : m_function {move(name)},
    m_args     {move(args)}
{ }

std::vector<vm::command> ast::function_call::generate() const
{
  std::vector<vm::command> vec;

  //for (const auto& i : m_args) {
  for_each(rbegin(m_args), rend(m_args), [&](auto& i)
  {
    const auto arg = i->code();
    copy(begin(arg), end(arg), back_inserter(vec));
  });

  const auto fn = m_function->code();
  copy(begin(fn), end(fn), back_inserter(vec));

  vec.emplace_back(vm::instruction::call, static_cast<int>(m_args.size()));
  return vec;
}
