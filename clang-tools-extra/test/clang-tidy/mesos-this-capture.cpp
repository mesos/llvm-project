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

template <typename PID, typename Iterate, typename Body>
Future<Nothing> loop(const PID &pid, Iterate &&iterate, Body &&body) { return {}; }
} // namespace process  {

using process::Future;
using process::defer;
using process::loop;

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

struct P {
  Future<Nothing> future() const { return {}; }

  void f() {
    // CHECK-MESSAGES: :[[@LINE+1]]:16: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
    loop(this, [this]() { (void)this; }, []() {});
    // CHECK-MESSAGES: :[[@LINE+1]]:25: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
    loop(this, []() {}, [this]() { (void)this; });

    auto l = [this]() { (void)this; };
    // CHECK-MESSAGES: :[[@LINE+1]]:16: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
    loop(this, l, []() {});
    // CHECK-MESSAGES: :[[@LINE+1]]:25: warning: callback capturing this should be dispatched/deferred to a specific PID [mesos-this-capture]
    loop(this, []() {}, l);
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

void g() {
  K k;
  loop(k, []() {}, []() {});
  loop(k, [k]() {}, []() {});
  loop(k, []() {}, [k]() {});
}
