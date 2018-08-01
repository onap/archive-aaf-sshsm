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
IMAGE="aaf-base"
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
    echo "Start build docker image: ${IMAGE_NAME}:xenial"
    docker build ${BUILD_ARGS} -t ${IMAGE_NAME}:xenial -f xenialdockerfile .
    echo "Start build docker image: ${IMAGE_NAME}:openssl_1.1.0"
    docker build ${BUILD_ARGS} -t ${IMAGE_NAME}:openssl_1.1.0 -f openssldockerfile .
}

function push_image {
    echo "Start push ${IMAGE_NAME}:xenial"
    docker push ${IMAGE_NAME}:xenial
    echo "Start push ${IMAGE_NAME}:openssl_1.1.0"
    docker push ${IMAGE_NAME}:openssl_1.1.0

}

build_image
push_image
