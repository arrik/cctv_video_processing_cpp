#!/usr/bin/env bash
set -e

echo "=================start running the service================="

BUILD_DIR="build"

LOG_DIR="log"
if [ -d "$LOG_DIR" ]; then
  if [ -L "$LOG_DIR" ]; then
    mkdir "$LOG_DIR"
  else
    rm -rf "$LOG_DIR"
    mkdir "$LOG_DIR"
  fi
fi


cd $BUILD_DIR
chmod +x cctv_video_processing_cpp
./cctv_video_processing_cpp >> ../$LOG_DIR/message.log 2>&1