#!/bin/bash
set -e
DIRNAME=`dirname $0`
DOCKER_BUILD_DIR=`cd $DIRNAME/; pwd`
echo "DOCKER_BUILD_DIR=${DOCKER_BUILD_DIR}"
cd ${DOCKER_BUILD_DIR}

BUILD_ARGS="--no-cache"
ORG="onap"
VERSION="3.0.0"
PROJECT="aaf"
IMAGE="abrmd"
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
    echo "Start build docker image: ${IMAGE_NAME}:latest"
    docker build ${BUILD_ARGS} -t ${IMAGE_NAME}:latest -f dockerfile .
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
