#include "type.h"

#include <map>
#include <unordered_map>

std::string getTypeName(Type t)
{
  static std::map<Type, std::string> names = {
      {Type::none, "none"}, // *
      {Type::inferred, "inferred"},
      {Type::int_t, "int_t"},
      {Type::flt_t, "flt_t"},
      {Type::chr_t, "chr_t"},
      {Type::boo_t, "boo_t"},
      {Type::str_t, "str_t"},
      {Type::vac_t, "vac_t"}};
  return names[t];
}

Type getTypeFromToken(int tok)
{
  static std::unordered_map<int, Type> types = {
      {Token::id_int, Type::int_t}, // *
      {Token::id_flt, Type::flt_t},
      {Token::id_chr, Type::chr_t},
      {Token::id_boo, Type::boo_t},
      {Token::id_str, Type::str_t},
      {Token::id_vac, Type::vac_t}};
  return types[tok];
}