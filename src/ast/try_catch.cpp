#include "try_catch.h"

#include "gc.h"

using namespace vv;

ast::try_catch::try_catch(std::unique_ptr<expression>&& body,
                          symbol exception_name,
                          std::unique_ptr<expression>&& catcher)
  : m_body           {move(body)},
    m_exception_name {exception_name},
    m_catcher        {move(catcher)}
{ }

std::vector<vm::command> ast::try_catch::generate() const
{
  std::vector<vm::command> catcher;
  catcher.emplace_back(vm::instruction::arg, 0);
  catcher.emplace_back(vm::instruction::let, m_exception_name);
  const auto catcher_body = m_catcher->code();
  copy(begin(catcher_body), end(catcher_body), back_inserter(catcher));
  catcher.emplace_back(vm::instruction::ret, false);

  std::vector<vm::command> vec;
  vec.emplace_back(vm::instruction::pfn, vm::function_t{1, move(catcher)});
  vec.emplace_back(vm::instruction::pushc);

  auto body = m_body->code();
  body.emplace_back(vm::instruction::ret, false);
  vec.emplace_back(vm::instruction::pfn, vm::function_t{0, move(body)});
  vec.emplace_back(vm::instruction::call, 0);

  vec.emplace_back(vm::instruction::popc);
  return vec;
}
