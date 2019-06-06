//===--- FlagsInheritanceCheck.cpp - clang-tidy----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "FlagsInheritanceCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace mesos {

void FlagsInheritanceCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      cxxRecordDecl(
          isDerivedFrom(namedDecl(hasName("FlagsBase"),
                                  hasParent(namespaceDecl(hasName("flags"))))
                            .bind("flags_decl")))
          .bind("flags-inheritance"),
      this);
}

namespace {
bool isDerivedFromFlagsBase(const CXXRecordDecl *decl,
                            const CXXRecordDecl *flagsDecl) {
  assert(decl);
  assert(flagsDecl);

  return llvm::any_of(decl->bases(), [flagsDecl](const CXXBaseSpecifier &base) {
    const auto *decl = base.getType()->getAsCXXRecordDecl();
    return decl->getName() == "FlagsBase" ||
           isDerivedFromFlagsBase(decl, flagsDecl);
  });
}
}

void FlagsInheritanceCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *derivedDecl = Result.Nodes.getNodeAs<CXXRecordDecl>("flags-inheritance");
  const auto *flagsDecl = Result.Nodes.getNodeAs<CXXRecordDecl>("flags_decl");

  // We explicitly iterate the bases here so we can emit diagnostic pointing to
  // the exact inheritance introducing the issue.
  for (auto &&base : derivedDecl->bases()) {
    const auto *baseDecl = base.getType()->getAsCXXRecordDecl();

    // First check if this inheritance is from 'FlagsBase' or any of its
    // descendants.
    if (baseDecl != flagsDecl && !isDerivedFromFlagsBase(baseDecl, flagsDecl)) {
      continue;
    }

    // If the inheritance was 'virtual' there's nothing we need to do here.
    if (base.isVirtual()) {
      continue;
    }

    diag(base.getBaseTypeLoc(), "'%0' does not inherit virtually from '%1'%2")
        << derivedDecl->getName() << baseDecl->getName()
        << FixItHint::CreateInsertion(base.getBaseTypeLoc(), "virtual ")
        << (baseDecl->getName() != "FlagsBase"
                ? " which inherits from 'FlagsBase'"
                : "");

    // Break if we found a match so any FixIts are in the correct location.
    // Rechecking after fixing the first issue might also find new issues as the
    // hierarchy is changed.
    break;
  }
}
} // namespace mesos
} // namespace tidy
} // namespace clang
