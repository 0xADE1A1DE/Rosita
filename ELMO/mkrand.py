import os
import struct 
import sys

f = open('/dev/urandom','rb')

for i in range(0,int(sys.argv[1])):
    print('%02x'%(struct.unpack('1B',f.read(1))[0]))
