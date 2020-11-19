#! /bin/bash

rm ./ELMO/test/output/*/*.txt
rm ./ELMO/test/output/traces/*.trc
rm ./ELMO/test/output/*.txt

cd TESTS/xoodoo
find . -name "*.c" -exec ../../testdel.sh {} \;
rm simple_tmpl.c final final.bin
cd -

cd TESTS/aes
find . -name "*.c" -exec ../../testdel.sh {} \;
rm simple_tmpl.c final final.bin
cd -

cd TESTS/chacha20
find . -name "*.c" -exec ../../testdel.sh {} \;
rm simple_tmpl.c final final.bin
cd -
