#include "type.h"

#include <map>
#include <unordered_map>

#include "ast_base.h"
#include "ast_types.h"

const std::string &getTypeName(BuiltinTypeKind t)
{
  static std::map<BuiltinTypeKind, std::string> names = {{BuiltinTypeKind::none, "none"}, // *
                                              //{BuiltinTypeKind::inferred, "inferred"},
                                              {BuiltinTypeKind::int_t, "int_t"},
                                              {BuiltinTypeKind::flt_t, "flt_t"},
                                              {BuiltinTypeKind::chr_t, "chr_t"},
                                              {BuiltinTypeKind::boo_t, "boo_t"},
                                              {BuiltinTypeKind::str_t, "str_t"},
                                              {BuiltinTypeKind::vac_t, "vac_t"}};
  return names[t];
}

const std::string &getMangleName(BuiltinTypeKind t)
{
  static std::map<BuiltinTypeKind, std::string> names = {{BuiltinTypeKind::int_t, "i"},
                                       {BuiltinTypeKind::flt_t, "f"},
                                       {BuiltinTypeKind::chr_t, "c"},
                                       {BuiltinTypeKind::boo_t, "'b"},
                                       {BuiltinTypeKind::str_t, "s"},
                                       {BuiltinTypeKind::vac_t, "v"}};
  auto pos = names.find(t);
  if (pos == names.end())
    throw CodeGenError("invalid mangle name for type '" + getTypeName(t) +
                       '\'');
  return pos->second;
}

std::string getMangleName(Type *t)
{
  if (auto *classType = dynamic_cast<ClassType*>(t)) {
    const auto &name = classType->getName();
    return std::to_string(name.length()) + name;
  } else {
      return getMangleName(t->getKind());
  }
}
