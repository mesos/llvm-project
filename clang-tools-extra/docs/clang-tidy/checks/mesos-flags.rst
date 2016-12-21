.. title:: clang-tidy - mesos-flags

mesos-flags
===========

Classes implementing flags parsing by inheriting stout's `flags::FlagBase`
class should always use `virtual` inheritance on any path leading to
`flags::FlagsBase`. This is required since the `FlagsBase` part of the flags
class contains a map of flag names to `Flag` instances; `virtual` inheritance
makes sure that only a single instance of the map is created so that all flags
from base classes are set once and found correctly.
