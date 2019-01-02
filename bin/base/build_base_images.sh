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
IMAGE_NAME_XENIAL="${DOCKER_REPOSITORY}/${ORG}/${PROJECT}/${IMAGE}-xenial"
IMAGE_NAME_OPENSSL_110="${DOCKER_REPOSITORY}/${ORG}/${PROJECT}/${IMAGE}-openssl_1.1.0"
TIMESTAMP=$(date +"%Y%m%dT%H%M%S")

if [ $HTTP_PROXY ]; then
    BUILD_ARGS+=" --build-arg HTTP_PROXY=${HTTP_PROXY}"
fi
if [ $HTTPS_PROXY ]; then
    BUILD_ARGS+=" --build-arg HTTPS_PROXY=${HTTPS_PROXY}"
fi
echo $BUILD_ARGS

function build_image {
    echo "Start build docker image: ${IMAGE_NAME_XENIAL}:latest"
    docker build ${BUILD_ARGS} -t ${IMAGE_NAME_XENIAL}:latest -f xenialdockerfile .
    echo "Start build docker image: ${IMAGE_NAME_OPENSSL_110}:latest"
    docker build ${BUILD_ARGS} -t ${IMAGE_NAME_OPENSSL_110}:latest -f openssldockerfile .
}

function push_image {
    echo "Start push ${IMAGE_NAME_XENIAL}:latest"
    docker push ${IMAGE_NAME_XENIAL}:latest
    echo "Start push ${IMAGE_NAME_XENIAL}:${VERSION}-SNAPSHOT-latest"
    docker tag ${IMAGE_NAME_XENIAL}:latest ${IMAGE_NAME_XENIAL}:${VERSION}-SNAPSHOT-latest
    docker push ${IMAGE_NAME_XENIAL}:${VERSION}-SNAPSHOT-latest
    echo "Start push ${IMAGE_NAME_OPENSSL_110}:latest"
    docker push ${IMAGE_NAME_OPENSSL_110}:latest
    echo "Start push ${IMAGE_NAME_OPENSSL_110}:${VERSION}-SNAPSHOT-latest"
    docker tag ${IMAGE_NAME_OPENSSL_110}:latest ${IMAGE_NAME_OPENSSL_110}:${VERSION}-SNAPSHOT-latest
    docker push ${IMAGE_NAME_OPENSSL_110}:${VERSION}-SNAPSHOT-latest

}

build_image
push_image
