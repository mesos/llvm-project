// RUN: %check_clang_tidy %s mesos-flags-inheritance %t

struct FlagsBase {};

struct A : FlagsBase {};
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: 'A' does not inherit virtually from 'FlagsBase' [mesos-flags-inheritance]
// CHECK-FIXES: {{^}}struct A : virtual FlagsBase {};{{$}}

struct B : A {};
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: 'B' does not inherit virtually from 'A' which inherits from 'FlagsBase' [mesos-flags-inheritance]
// CHECK-FIXES: {{^}}struct B : virtual A {};{{$}}

class C : public FlagsBase {};
// CHECK-MESSAGES: :[[@LINE-1]]:11: warning: 'C' does not inherit virtually from 'FlagsBase' [mesos-flags-inheritance]
// CHECK-FIXES: {{^}}class C : virtual public FlagsBase {};{{$}}

struct D : virtual FlagsBase {};
struct E : virtual C {};
