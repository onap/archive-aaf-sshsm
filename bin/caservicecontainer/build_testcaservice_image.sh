#!/bin/bash
set -e
DIRNAME=`dirname $0`
DOCKER_BUILD_DIR=`cd $DIRNAME/; pwd`
echo "DOCKER_BUILD_DIR=${DOCKER_BUILD_DIR}"
cd ${DOCKER_BUILD_DIR}

BUILD_ARGS="--no-cache"
ORG="onap"
VERSION="4.0.0"
PROJECT="aaf"
IMAGE="testcaservice"
DOCKER_REPOSITORY="nexus3.onap.org:10003"
IMAGE_NAME="${DOCKER_REPOSITORY}/${ORG}/${PROJECT}/${IMAGE}"
TIMESTAMP=$(date +"%Y%m%dT%H%M%S")

if [ $HTTP_PROXY ]; then
    BUILD_ARGS+=" --build-arg HTTP_PROXY=${HTTP_PROXY}"
fi
if [ $HTTPS_PROXY ]; then
    BUILD_ARGS+=" --build-arg HTTPS_PROXY=${HTTPS_PROXY}"
fi
echo $BUILD_ARGS

function build_image {
    echo "Copying files for image"
    cp ../../test/integration/samplecaservicecontainer/applicationfiles/CaSign.java .
    cp ../../test/integration/samplecaservicecontainer/applicationfiles/ca.cert .
    cp ../../test/integration/samplecaservicecontainer/applicationfiles/test.csr .

    echo "Start build docker image: ${IMAGE_NAME}:latest"
    docker build ${BUILD_ARGS} -t ${IMAGE_NAME}:latest -f dockerfile .

    echo "Remove files after image is built"
    rm CaSign.java ca.cert test.csr
}

function push_image {
    echo "Start push ${IMAGE_NAME}:latest"
    docker push ${IMAGE_NAME}:latest
    echo "Start push ${IMAGE_NAME}:${VERSION}-SNAPSHOT-latest"
    docker tag ${IMAGE_NAME}:latest ${IMAGE_NAME}:${VERSION}-SNAPSHOT-latest
    docker push ${IMAGE_NAME}:${VERSION}-SNAPSHOT-latest
}

build_image
push_image
