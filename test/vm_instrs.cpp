#include "output.h"

#include "builtins.h"
#include "value.h"
#include "vm.h"
#include "value/floating_point.h"

#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>

#include <numeric>

void check_pint(const int orig)
{
  vv::vm::machine vm{vv::vm::call_frame{}};

  vm.pint(orig);
  const auto val = vm.top();
  BOOST_CHECK_EQUAL(val.tag(), vv::tag::integer);
  BOOST_CHECK(val.type() == vv::builtin::type::integer);
  BOOST_CHECK_EQUAL(vv::value::get<vv::value::integer>(val), orig);
}

BOOST_AUTO_TEST_CASE(check_pbool)
{
  vv::vm::machine vm{vv::vm::call_frame{}};

  vm.pbool(false);
  const auto false_v = vm.top();
  BOOST_CHECK_EQUAL(false_v.tag(), vv::tag::boolean);
  BOOST_CHECK(false_v.type() == vv::builtin::type::boolean);
  BOOST_CHECK_EQUAL(vv::value::get<vv::value::boolean>(false_v), false);

  vm.pbool(true);
  const auto true_v = vm.top();
  BOOST_CHECK_EQUAL(true_v.tag(), vv::tag::boolean);
  BOOST_CHECK(true_v.type() == vv::builtin::type::boolean);
  BOOST_CHECK_EQUAL(vv::value::get<vv::value::boolean>(true_v), true);
}

BOOST_AUTO_TEST_CASE(check_pnil)
{
  vv::vm::machine vm{vv::vm::call_frame{}};

  vm.pnil();
  const auto nil = vm.top();
  BOOST_CHECK_EQUAL(nil.tag(), vv::tag::nil);
  BOOST_CHECK(nil.type() == vv::builtin::type::nil);
}

void check_pflt(const double orig)
{
  vv::vm::machine vm{vv::vm::call_frame{}};

  vm.pflt(orig);
  const auto val = vm.top();
  BOOST_CHECK_EQUAL(val.tag(), vv::tag::floating_point);
  BOOST_CHECK(val.type() == vv::builtin::type::floating_point);
  BOOST_CHECK_EQUAL(vv::value::get<vv::value::floating_point>(val), orig);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char** argv)
{
  vv::builtin::init();

  std::array<int32_t, 1002> ints;
  ints[0] = std::numeric_limits<int32_t>::min();
  ints[1] = std::numeric_limits<int32_t>::max();
  std::iota(begin(ints) + 2, end(ints), -500);

  boost::unit_test::framework::master_test_suite().add(
    BOOST_PARAM_TEST_CASE(&check_pint, begin(ints), end(ints)));

  std::array<double, 1003> doubles;
  ints[0] = std::numeric_limits<double>::min();
  ints[1] = std::numeric_limits<double>::max();
  ints[2] = 0.125;
  std::iota(begin(doubles) + 3, end(doubles), -500.0);

  boost::unit_test::framework::master_test_suite().add(
    BOOST_PARAM_TEST_CASE(&check_pflt, begin(doubles), end(doubles)));

  return nullptr;
}
