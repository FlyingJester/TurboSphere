import os
import sys

Import("environment poll_api")

conf = Configure(environment)

fjnet_files = ["socket.c"]



if poll_api=='kqueue':
  fjnet_files += ["kqueue.c"]
elif poll_api=='poll':
  fjnet_files += ["poll.c"]
else:
  fjnet_files += ["select.c"]

conf.Finish()

libfjnet = environment.StaticLibrary("fjnet", fjnet_files)

Return("libfjnet")
