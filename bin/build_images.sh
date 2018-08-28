#!/bin/bash
set -e

cd base
chmod 755 build_base_images.sh
./build_base_images.sh

cd ../distcenter
chmod 755 build_distcenter_image.sh
./build_distcenter_image.sh

cd ../abrmdcontainer
chmod 755 build_abrmd_image.sh
./build_abrmd_image.sh

cd ../caservicecontainer
chmod 755 build_testcaservice_image.sh 
./build_testcaservice_image.sh
