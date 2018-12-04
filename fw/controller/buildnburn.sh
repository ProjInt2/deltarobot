#!/bin/sh

set -e

make
lm4flash gcc/project.bin
