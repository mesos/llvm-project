//===--- RedundantGetCheck.h - clang-tidy------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MESOS_REDUNDANT_GET_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MESOS_REDUNDANT_GET_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace mesos {

// Find and remove redundant calls to `.get()` on stout and libprocess wrapper
// types.

// Examples:

// \code
//   Option<vector<int>> option;

//   option.get().empty() ==> option->empty()
// \endcode
class RedundantGetCheck : public ClangTidyCheck {
public:
  RedundantGetCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace mesos
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MESOS_REDUNDANT_GET_H
