#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
IMAGE_TAG="samp-plugin-crashdetect/build:ubuntu-24.04"

# Available configs: Debug, RelWithDebInfo, [Release], MinSizeRel
config="${CONFIG:-Release}"

docker build \
    --platform linux/amd64 \
    -t "${IMAGE_TAG}" \
    "${SCRIPT_DIR}"

mkdir -p "${SCRIPT_DIR}/build"

docker run \
    --rm \
    -t \
    --platform linux/amd64 \
    --user "$(id -u):$(id -g)" \
    -w /code \
    -v "${SCRIPT_DIR}/..:/code" \
    -v "${SCRIPT_DIR}/build:/code/build" \
    -e CONFIG="${config}" \
    "${IMAGE_TAG}"
