#!/bin/bash
set -e
cd base
./build_base_images.sh
cd ../distcenter
./build_distcenter_image.sh
