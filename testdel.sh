#! /bin/bash

if [ -f ${1/.c/.o} ]; then
  rm "${1/.c/.o}"
  rm "${1/.c/.s}"
  rm "${1/.c/.d}"
  rm "${1/.c/.i}"
fi
