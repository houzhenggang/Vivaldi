#ifndef VV_VALUE_REGEX_H
#define VV_VALUE_REGEX_H

#include "value.h"

#include <regex>

namespace vv {

namespace value {

struct regex : public object {
public:
  regex(const std::regex& val = {}, const std::string& str = {});

  std::string value() const override;

  std::regex val;
  // Regex in string form (stored for pretty-printing)
  std::string str;
};

struct regex_result : public object {
public:
  regex_result(gc::managed_ptr<value::string> str, std::smatch&& res);

  std::string value() const override;

  void mark() override;

  std::smatch val;

  gc::managed_ptr<value::string> owning_str;
private:
};

}

}

#endif
