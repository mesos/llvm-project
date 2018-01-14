set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangTidyMesosModule
  FlagsInheritanceCheck.cpp
  MesosTidyModule.cpp
  RedundantGetCheck.cpp
  NamespaceCommentCheck.cpp
  ThisCaptureCheck.cpp

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangTidy
  clangTooling
  )
