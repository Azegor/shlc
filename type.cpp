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
                                       {BuiltinTypeKind::boo_t, "b"},
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
  if (auto *structType = dynamic_cast<StructureType*>(t)) {
    const auto &name = structType->getName();
    return std::to_string(name.length()) + name;
  } else {
      return getMangleName(t->getKind());
  }
}

LLVMTypeRegistry::LLVMTypeRegistry(GlobalContext &gl_ctx)
  : gl_ctx(gl_ctx)
{
    voidPointerType = llvm::Type::getInt8PtrTy(gl_ctx.llvm_context); // is void in llvm
    refCounterPtrType = getRefCounterType()->llvm::Type::getPointerTo();
}

llvm::Type *LLVMTypeRegistry::getType(Type *t)
{
    switch(t->getKind()) {
      case BuiltinTypeKind::cls_t:
        return getClassType(static_cast<ClassType*>(t));
        break;
      case BuiltinTypeKind::opq_t:
        return getOpaqueType(static_cast<OpaqueType*>(t));
        break;
      default:
        return getBuiltinType(t->getKind());
        break;
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

llvm::PointerType *LLVMTypeRegistry::getOpaqueType(OpaqueType *ot)
{
    auto pos = opaqueTypeMap.find(ot);
    if (pos != opaqueTypeMap.end()) {
        return pos->second;
    }
    auto type = createLLVMOpaqueType(ot);
    opaqueTypeMap.emplace(ot, type);
    return type;
}

llvm::Type *LLVMTypeRegistry::getBuiltinType(BuiltinTypeKind tk) const
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
    members.reserve(ct->fields.size() + 1);
    members.push_back(getRefCounterType());
    for (auto &field : ct->fields) {
        members.push_back(getType(field.type));
    }
    auto structType = llvm::StructType::create(gl_ctx.llvm_context, members, ct->getName());
    return llvm::PointerType::get(structType, 0);
}


llvm::PointerType *LLVMTypeRegistry::createLLVMOpaqueType(OpaqueType *ot) {
  auto structType = llvm::StructType::create(gl_ctx.llvm_context, ot->getName());
  return llvm::PointerType::get(structType, 0);
}

llvm::DIType *LLVMTypeRegistry::getDIType(Type *t)
{
  switch(t->getKind()) {
    case BuiltinTypeKind::cls_t:
      return getDIClassType(static_cast<ClassType*>(t));
      break;
    case BuiltinTypeKind::opq_t:
      return getDIOpaqueType(static_cast<OpaqueType*>(t));
      break;
    default:
      return getDIBuiltinType(t->getKind());
      break;
  }
}
llvm::DIType *LLVMTypeRegistry::getDIBuiltinType(BuiltinTypeKind tk)
{
  if (!diTypesInitialized) {
    createDIBuiltinTypes();
  }
  return diBuiltinTypes[(int)tk];
}

llvm::DIType *LLVMTypeRegistry::getDIClassType(ClassType *ct) {
  auto pos = diClassTypes.find(ct);
  if (pos != diClassTypes.end()) {
    return pos->second;
  }
  auto clsType = createDIClassType(ct);
  diClassTypes.insert({ct, clsType});
  return clsType;
}

llvm::DIType *LLVMTypeRegistry::getDIOpaqueType(OpaqueType *ot) {
  auto pos = diOpaqueTypes.find(ot);
  if (pos != diOpaqueTypes.end()) {
    return pos->second;
  }
  auto opaqueType = createDIOpaqueType(ot);
  diOpaqueTypes.insert({ot, opaqueType});
  return opaqueType;
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

llvm::DIType *LLVMTypeRegistry::createDIClassType(ClassType *ct)
{
  std::vector<llvm::Metadata*> members;
  members.reserve(ct->fields.size()/* + 1*/);
  // members.push_back(getDIBuiltinType(BuiltinTypeKind::int_t)); // refcount
  for (auto &field : ct->fields) {
      members.push_back(getDIType(field.type));
  }
  auto llvmType = getClassType(ct)->getPointerElementType();
  auto size = gl_ctx.module->getDataLayout().getTypeAllocSize(llvmType) - 8;
  auto align = gl_ctx.module->getDataLayout().getABITypeAlignment(llvmType); // TODO
  return gl_ctx.diBuilder.createClassType(gl_ctx.getCurrentDILexicalScope(), ct->name, gl_ctx.allDIFiles[ct->srcLoc.lexerNr], ct->srcLoc.startToken.line,
    size, align, 8, llvm::DINode::FlagZero, nullptr, gl_ctx.diBuilder.getOrCreateArray(members));
}

llvm::DIType *LLVMTypeRegistry::createDIOpaqueType(OpaqueType *ot)
{
  return gl_ctx.diBuilder.createForwardDecl(llvm::dwarf::DW_TAG_structure_type, ot->name, gl_ctx.getCurrentDILexicalScope(),
                                            gl_ctx.allDIFiles[ot->srcLoc.lexerNr], ot->srcLoc.startToken.line);
}
