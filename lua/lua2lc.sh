#!/bin/sh
echo Convert lua to lc
luac -o ../release/.sys/preprocess.lc preprocess.lua

if [ $? -ne 0 ]; then
  echo failed!
fi
