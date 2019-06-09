#include "type.h"

#include <map>
#include <unordered_map>

#include "ast_base.h"

std::string getTypeName(BuiltinTypeKind t)
{
  static std::map<BuiltinTypeKind, std::string> names = {{BuiltinTypeKind::none, "none"}, // *
                                              {BuiltinTypeKind::inferred, "inferred"},
                                              {BuiltinTypeKind::int_t, "int_t"},
                                              {BuiltinTypeKind::flt_t, "flt_t"},
                                              {BuiltinTypeKind::chr_t, "chr_t"},
                                              {BuiltinTypeKind::boo_t, "boo_t"},
                                              {BuiltinTypeKind::str_t, "str_t"},
                                              {BuiltinTypeKind::vac_t, "vac_t"}};
  return names[t];
}

char getMangleName(BuiltinTypeKind t)
{
  static std::map<BuiltinTypeKind, char> names = {{BuiltinTypeKind::int_t, 'i'},
                                       {BuiltinTypeKind::flt_t, 'f'},
                                       {BuiltinTypeKind::chr_t, 'c'},
                                       {BuiltinTypeKind::boo_t, 'b'},
                                       {BuiltinTypeKind::str_t, 's'},
                                       {BuiltinTypeKind::vac_t, 'v'}};
  auto pos = names.find(t);
  if (pos == names.end())
    throw CodeGenError("invalid mangle name for type '" + getTypeName(t) +
                       '\'');
  return pos->second;
}

BuiltinTypeKind getTypeFromToken(int tok)
{
  static std::unordered_map<int, BuiltinTypeKind> types = {
    {Token::id_int, BuiltinTypeKind::int_t}, // *
    {Token::id_flt, BuiltinTypeKind::flt_t},
    {Token::id_chr, BuiltinTypeKind::chr_t},
    {Token::id_boo, BuiltinTypeKind::boo_t},
    {Token::id_str, BuiltinTypeKind::str_t},
    {Token::id_vac, BuiltinTypeKind::vac_t}};
  auto pos = types.find(tok);
  if (pos == types.end())
    throw CodeGenError("token '" + Lexer::getTokenName(tok) + "' is no type");
  return pos->second;
}
