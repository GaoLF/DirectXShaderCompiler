///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilTypeSystem.h                                                          //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// DXIL extension to LLVM type system.                                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "llvm/ADT/StringRef.h"
#include "dxc/HLSL/DxilCompType.h"
#include "dxc/HLSL/DxilInterpolationMode.h"

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace llvm {
class LLVMContext;
class Module;
class Function;
class Type;
class StructType;
class StringRef;
};


namespace hlsl {

enum class MatrixOrientation { Undefined = 0, RowMajor, ColumnMajor, LastEntry };

struct DxilMatrixAnnotation {
  unsigned Rows;
  unsigned Cols;
  MatrixOrientation Orientation;

  DxilMatrixAnnotation();
};

/// Use this class to represent type annotation for structure field.
class DxilFieldAnnotation {
public:
  DxilFieldAnnotation();
  
  bool IsPrecise() const;
  void SetPrecise(bool b = true);

  bool HasMatrixAnnotation() const;
  const DxilMatrixAnnotation &GetMatrixAnnotation() const;
  void SetMatrixAnnotation(const DxilMatrixAnnotation &MA);

  bool HasCBufferOffset() const;
  unsigned GetCBufferOffset() const;
  void SetCBufferOffset(unsigned Offset);

  bool HasCompType() const;
  const CompType &GetCompType() const;
  void SetCompType(CompType::Kind kind);

  bool HasSemanticString() const;
  const std::string &GetSemanticString() const;
  llvm::StringRef GetSemanticStringRef() const;
  void SetSemanticString(const std::string &SemString);

  bool HasInterpolationMode() const;
  const InterpolationMode &GetInterpolationMode() const;
  void SetInterpolationMode(const InterpolationMode &IM);

  bool HasFieldName() const;
  const std::string &GetFieldName() const;
  void SetFieldName(const std::string &FieldName);

private:
  bool m_bPrecise;
  CompType m_CompType;
  DxilMatrixAnnotation m_Matrix;
  unsigned m_CBufferOffset;
  std::string m_Semantic;
  InterpolationMode m_InterpMode;
  std::string m_FieldName;
};


/// Use this class to represent LLVM structure annotation.
class DxilStructAnnotation {
  friend class DxilTypeSystem;

public:
  unsigned GetNumFields() const;
  DxilFieldAnnotation &GetFieldAnnotation(unsigned FieldIdx);
  const DxilFieldAnnotation &GetFieldAnnotation(unsigned FieldIdx) const;
  const llvm::StructType *GetStructType() const;
  unsigned GetCBufferSize() const;
  void SetCBufferSize(unsigned size);
  void MarkEmptyStruct();
  bool IsEmptyStruct();
private:
  const llvm::StructType *m_pStructType;
  std::vector<DxilFieldAnnotation> m_FieldAnnotations;
  unsigned m_CBufferSize;  // The size of struct if inside constant buffer.
};


enum class DxilParamInputQual {
  In,
  Out,
  Inout,
  InputPatch,
  OutputPatch,
  OutStream0,
  OutStream1,
  OutStream2,
  OutStream3,
  InputPrimitive,
};

/// Use this class to represent type annotation for function parameter.
class DxilParameterAnnotation : public DxilFieldAnnotation {
public:
  DxilParameterAnnotation();
  const DxilParamInputQual GetParamInputQual() const;
  void SetParamInputQual(DxilParamInputQual qual);
  const std::vector<unsigned> &GetSemanticIndexVec() const;
  void SetSemanticIndexVec(const std::vector<unsigned> &Vec);
  void AppendSemanticIndex(unsigned SemIdx);
private:
  DxilParamInputQual m_inputQual;
  std::vector<unsigned> m_semanticIndex;
};

/// Use this class to represent LLVM function annotation.
class DxilFunctionAnnotation {
  friend class DxilTypeSystem;

public:
  unsigned GetNumParameters() const;
  DxilParameterAnnotation &GetParameterAnnotation(unsigned ParamIdx);
  const DxilParameterAnnotation &GetParameterAnnotation(unsigned ParamIdx) const;
  const llvm::Function *GetFunction() const;
  DxilParameterAnnotation &GetRetTypeAnnotation();
  const DxilParameterAnnotation &GetRetTypeAnnotation() const;
private:
  const llvm::Function *m_pFunction;
  std::vector<DxilParameterAnnotation> m_parameterAnnotations;
  DxilParameterAnnotation m_retTypeAnnotation;
};

/// Use this class to represent structure type annotations in HL and DXIL.
class DxilTypeSystem {
public:
  using StructAnnotationMap = std::map<const llvm::StructType *, std::unique_ptr<DxilStructAnnotation> >;
  using FunctionAnnotationMap = std::map<const llvm::Function *, std::unique_ptr<DxilFunctionAnnotation> >;

  DxilTypeSystem(llvm::Module *pModule);

  DxilStructAnnotation *AddStructAnnotation(const llvm::StructType *pStructType);
  DxilStructAnnotation *GetStructAnnotation(const llvm::StructType *pStructType);
  void EraseStructAnnotation(const llvm::StructType *pStructType);

  StructAnnotationMap &GetStructAnnotationMap();

  DxilFunctionAnnotation *AddFunctionAnnotation(const llvm::Function *pFunction);
  DxilFunctionAnnotation *GetFunctionAnnotation(const llvm::Function *pFunction);
  void EraseFunctionAnnotation(const llvm::Function *pFunction);

  FunctionAnnotationMap &GetFunctionAnnotationMap();

  // Utility methods to create stand-alone SNORM and UNORM.
  // We may want to move them to a more centralized place for most utilities.
  llvm::StructType *GetSNormF32Type(unsigned NumComps);
  llvm::StructType *GetUNormF32Type(unsigned NumComps);

private:
  llvm::Module *m_pModule;
  StructAnnotationMap m_StructAnnotations;
  FunctionAnnotationMap m_FunctionAnnotations;

  llvm::StructType *GetNormFloatType(CompType CT, unsigned NumComps);
};

DXIL::SigPointKind SigPointFromInputQual(DxilParamInputQual Q, DXIL::ShaderKind SK, bool isPC);

} // namespace hlsl
