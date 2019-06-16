#include "type.h"

#include <map>
#include <unordered_map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DIBuilder.h>

#include "ast_base.h"
#include "ast_types.h"
#include "context.h"

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

LLVMTypeRegistry::LLVMTypeRegistry(GlobalContext &gl_ctx)
  : gl_ctx(gl_ctx)
{
    voidPointerType = llvm::Type::getInt8PtrTy(gl_ctx.llvm_context); // is void in llvm
}

llvm::Type *LLVMTypeRegistry::getType(Type *t)
{
    if (auto *ct = dynamic_cast<ClassType*>(t)) {
        return getClassType(ct);
    } else {
        return getBuiltinType(t->getKind());
    }
}

llvm::PointerType *LLVMTypeRegistry::getClassType(ClassType *ct)
{
    auto pos = classTypeMap.find(ct);
    if (pos != classTypeMap.end()) {
        return pos->second;
    }
    auto type = createLLVMClassType(ct);
    classTypeMap.emplace(ct, type);
    return type;
}

llvm::Type *LLVMTypeRegistry::getBuiltinType(BuiltinTypeKind tk)
{
  switch (tk)
  {
    // int_t, flt_t, chr_t, boo_t, str_t, vac_t
    case BuiltinTypeKind::vac_t:
      return llvm::Type::getVoidTy(gl_ctx.llvm_context);
    case BuiltinTypeKind::int_t:
      return llvm::Type::getInt64Ty(gl_ctx.llvm_context);
    case BuiltinTypeKind::flt_t:
      return llvm::Type::getDoubleTy(gl_ctx.llvm_context);
    case BuiltinTypeKind::boo_t:
      return llvm::Type::getInt1Ty(gl_ctx.llvm_context);
    case BuiltinTypeKind::chr_t:
      return llvm::Type::getInt8Ty(gl_ctx.llvm_context); // no unicode
    case BuiltinTypeKind::str_t:
      return llvm::Type::getInt8PtrTy(gl_ctx.llvm_context);
    default:
      throw CodeGenError("Unknown type id " + getTypeName(tk));
  }
}

llvm::PointerType *LLVMTypeRegistry::createLLVMClassType(ClassType *ct)
{
    std::vector<llvm::Type*> members;
    members.reserve(ct->fields.size());
    for (auto &field : ct->fields) {
        members.push_back(getType(field.type));
    }
    auto structType = llvm::StructType::create(gl_ctx.llvm_context, members, ct->getName());
    return llvm::PointerType::get(structType, 0);
}

llvm::DIType *LLVMTypeRegistry::getDIType(Type *t)
{
    if (auto *ct = dynamic_cast<ClassType*>(t)) {
        throw CodeGenError("todo implement debug class types");
        // return getClassType(ct);
    } else {
        return getDIBuiltinType(t->getKind());
    }
}
llvm::DIType *LLVMTypeRegistry::getDIBuiltinType(BuiltinTypeKind tk)
{
  if (!diTypesInitialized) {
    createDIBuiltinTypes();
  }
  return diBuiltinTypes[(int)tk];
}

void LLVMTypeRegistry::createDIBuiltinTypes()
{
    diBuiltinTypes[(int)BuiltinTypeKind::vac_t] = nullptr;
    diBuiltinTypes[(int)BuiltinTypeKind::int_t] = gl_ctx.diBuilder.createBasicType("int", 64, llvm::dwarf::DW_ATE_signed);
    diBuiltinTypes[(int)BuiltinTypeKind::flt_t] = gl_ctx.diBuilder.createBasicType("flt", 64, llvm::dwarf::DW_ATE_float);
    diBuiltinTypes[(int)BuiltinTypeKind::boo_t] = gl_ctx.diBuilder.createBasicType("boo", 8, llvm::dwarf::DW_ATE_boolean);
    diBuiltinTypes[(int)BuiltinTypeKind::chr_t] = gl_ctx.diBuilder.createBasicType("chr", 8, llvm::dwarf::DW_ATE_signed_char);
    diBuiltinTypes[(int)BuiltinTypeKind::str_t] = gl_ctx.diBuilder.createPointerType(diBuiltinTypes[(int)BuiltinTypeKind::chr_t], 64, 1, {}, "str");
    diTypesInitialized = true;
}
