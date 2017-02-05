// RUN: %check_clang_tidy %s mesos-this-capture %t

struct Nothing {};

namespace process {
template <typename>
struct Future {
  template <typename AnyCallback>
  Future onAny(AnyCallback &&) { return {}; };
};

template <typename F>
Future<Nothing> defer(int /*pid*/, F) { return {}; }

} // namespace process  {

using process::Future;
using process::defer;

struct S {
  Future<Nothing> future() const { return {}; }

  void f() {
    future()
        // CHECK-MESSAGES: :[[@LINE+1]]:16: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
        .onAny([this]() { (void)this; });
    }

    void g() {
      future()
          // CHECK-MESSAGES: :[[@LINE+1]]:18: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
          .onAny([=]() { (void)this; });
    }

    void h() {
      auto l = [=]() { (void)this; };
      // CHECK-MESSAGES: :[[@LINE+1]]:22: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
      future().onAny(l);
    }
};

// Negatives.
void f() {
  Future<Nothing>().onAny([]() {});
};

struct K {
    void f() {
      auto future = Future<Nothing>();

      future
          .onAny(defer(0, [=]() { (void)this; }));

      auto l = []() {};
      future.onAny(l);
    }
};
