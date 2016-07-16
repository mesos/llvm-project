.. title:: clang-tidy - mesos-this-capture

mesos-this-capture
==================

`Futures` accessing internal object state through `this` should always be
executed on a dedicated actor to control lifetime and avoid races.
