import os
import sys

yyymonitor_files = ["monitor.cpp"]

if sys.platform.startswith('win'):
  yyymonitor_files += ["monitor_win32.c"]
else:
  yyymonitor_files += ["monitor_pthread.c"]

libyyymonitor = StaticLibrary("yyymonitor", yyymonitor_files)

Return("libyyymonitor")
