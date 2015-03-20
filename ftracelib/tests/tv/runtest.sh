#!/bin/bash

export FTRACE=ftrace.fcf
LD_PRELOAD=../../../Build/lib/libftracelib.so ../../../Build/bin/libftrace-tv

exit 0

