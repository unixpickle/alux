#!/bin/bash

if [ `uname` == 'Darwin' ]; then
  echo -n macho64
else
  echo -n elf64
fi
