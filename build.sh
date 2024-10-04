#!/usr/bin/env bash
#set -e

echo "=================build service================="
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
  if [ -L "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
  else
    rm -rf "$BUILD_DIR"
    mkdir "$BUILD_DIR"
  fi
fi

# use the following command to build if you are in project root
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release

cd $BUILD_DIR
make

if [ $? -eq 0 ]
then
  echo "================= build success ================="
else
  echo ">>>>>>>>>>>>>>>>> build not success <<<<<<<<<<<<<<<<<<"
fi

