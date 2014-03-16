#!/bin/bash

export FTRACE=ftrace.fcf
LD_PRELOAD=../../Build/lib/libftracelib2.so Build/bin/ftracelib2-tv

exit 0

