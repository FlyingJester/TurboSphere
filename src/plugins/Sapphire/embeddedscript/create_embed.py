# Creates an IFF file containing the scripts.
# This file will be expanded when TurboSphere first runs with Sapphire.
# Sapphire will add the script data from V8 to the IFF on the first run.

import glob
import binascii
import os
import sys
from shutil import copyfile

dir = os.getcwd()

if len(sys.argv) > 1:
	dir = sys.argv[1]

print sys.argv

def StringifyKeepingValue(s):
	stringy = ""
	while s:
		digit = s %256
		s-= s%256
		s = s/256
		stringy+=chr(digit)
	return stringy

def PadString(val, s, l):
	if len(s) == l:
		return s
	if len(s) > l:
		return s [0:l]
	while len(s) < l:
		s+=val
	return s

def PadAndString(n, l):
	return PadString(chr(0), StringifyKeepingValue(n), l)

#numbers must be in hex, no 0x or slash-x appends
def WriteIffHeader(iff_file, signature1, signature2, map_offset):
	
	iff_file.seek(0)

	
	iff_file.write((signature1+chr(0)+signature2) [0:60])

	iff_file.write("     " [0:4])
	
	iff_file.seek(64)

#numbers must be in hex, no 0x or slash-x appends
def WriteIffResourceHeader(iff_file, size, type, ID, name):

	iff_file.write(	PadAndString(type, 4))
	iff_file.write( PadAndString(size, 4))
	iff_file.write( PadAndString(ID,   2))
	# Flags. Nobody cares.
	iff_file.write(chr(32))
	iff_file.write(chr(32))

	loc = iff_file.tell()
	iff_file.write(name [0:64])
	iff_file.seek(loc+64)


iff_file = open(os.path.join(dir, "scripts.tsiff"), "wb")

#Put this at the end.
WriteIffHeader(iff_file, "TURBOSPHERE SAPPHIRE EMBEDDED JS", "IFF VERSION 2.6:TYPE,SIZE\x00", 0)

iff_file.seek(64)

files = glob.glob(os.path.join(dir, "*.js"))

n = 0

for file in files:
	print 'Embedding file '+file
	n+=1
	js_file = open(file, "rb")
	contents = js_file.read()
	WriteIffResourceHeader(iff_file, len(contents), 23, n, file)
	iff_file.write(contents)

iff_file.close()

copyfile(os.path.join(dir, "scripts.tsiff"), "../../bin/Release/extra/Sapphire/scripts.tsiff")


