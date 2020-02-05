//===--- SILDifferentiabilityWitness.cpp - Differentiability witnesses ----===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2019 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "sil-differentiability-witness"

// SWIFT_ENABLE_TENSORFLOW
#include "swift/AST/ASTMangler.h"
// SWIFT_ENABLE_TENSORFLOW_END
#include "swift/SIL/SILDifferentiabilityWitness.h"
#include "swift/SIL/SILModule.h"

using namespace swift;

SILDifferentiabilityWitness *SILDifferentiabilityWitness::createDeclaration(
    SILModule &module, SILLinkage linkage, SILFunction *originalFunction,
    IndexSubset *parameterIndices, IndexSubset *resultIndices,
    GenericSignature derivativeGenSig, const DeclAttribute *attribute) {
  auto *diffWitness = new (module) SILDifferentiabilityWitness(
      module, linkage, originalFunction, parameterIndices, resultIndices,
      derivativeGenSig, /*jvp*/ nullptr, /*vjp*/ nullptr,
      /*isDeclaration*/ true, /*isSerialized*/ false, attribute);
  // Register the differentiability witness in the module.
  Mangle::ASTMangler mangler;
  auto mangledKey =
      mangler.mangleSILDifferentiabilityWitnessKey(diffWitness->getKey());
  assert(!module.DifferentiabilityWitnessMap.count(mangledKey) &&
         "Cannot create duplicate differentiability witness in a module");
  module.DifferentiabilityWitnessMap[mangledKey] = diffWitness;
  module.DifferentiabilityWitnessesByFunction[originalFunction->getName()]
      .push_back(diffWitness);
  module.getDifferentiabilityWitnessList().push_back(diffWitness);
  return diffWitness;
}

SILDifferentiabilityWitness *SILDifferentiabilityWitness::createDefinition(
    SILModule &module, SILLinkage linkage, SILFunction *originalFunction,
    IndexSubset *parameterIndices, IndexSubset *resultIndices,
    GenericSignature derivativeGenSig, SILFunction *jvp, SILFunction *vjp,
    bool isSerialized, const DeclAttribute *attribute) {
  auto *diffWitness = new (module) SILDifferentiabilityWitness(
      module, linkage, originalFunction, parameterIndices, resultIndices,
      derivativeGenSig, jvp, vjp, /*isDeclaration*/ false, isSerialized,
      attribute);
  // Register the differentiability witness in the module.
  // Register the differentiability witness in the module.
  Mangle::ASTMangler mangler;
  auto mangledKey =
      mangler.mangleSILDifferentiabilityWitnessKey(diffWitness->getKey());
  assert(!module.DifferentiabilityWitnessMap.count(mangledKey) &&
         "Cannot create duplicate differentiability witness in a module");
  module.DifferentiabilityWitnessMap[mangledKey] = diffWitness;
  module.DifferentiabilityWitnessesByFunction[originalFunction->getName()]
      .push_back(diffWitness);
  module.getDifferentiabilityWitnessList().push_back(diffWitness);
  return diffWitness;
}

void SILDifferentiabilityWitness::convertToDefinition(SILFunction *jvp,
                                                      SILFunction *vjp,
                                                      bool isSerialized) {
  assert(IsDeclaration);
  IsDeclaration = false;
  JVP = jvp;
  VJP = vjp;
  IsSerialized = isSerialized;
}

SILDifferentiabilityWitnessKey SILDifferentiabilityWitness::getKey() const {
  return std::make_pair(getOriginalFunction()->getName(), getConfig());
}
