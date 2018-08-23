#!/bin/bash
set -e

cd base
chmod 755 build_base_images.sh
./build_base_images.sh

cd ../distcenter
chmod 755 build_distcenter_image.sh
./build_distcenter_image.sh

cd ../abrmdcontainer
chmod 755 build_initabrmd_image.sh
./build_initabrmd_image.sh
