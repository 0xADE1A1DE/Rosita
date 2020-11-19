import sys
import random
import os
import struct
fout = None
if sys.argv[1] == '-f':
    fout = open(sys.argv[2],'w')
    C = int(sys.argv[3])
elif sys.argv[1] == '-n':
    D = ''
    C = int(sys.argv[2])
else:
    D = '\n'
    C = int(sys.argv[1])

def write_stdout(line):
    print(line, end=D)
def write_file(line):
    global fout
    fout.write(line)
    fout.write('\n')

writer = write_stdout
if fout:
    writer = write_file

for i in range(0,C):
    writer( "%02x" % (struct.unpack('I',os.urandom(4))[0] % 256))



