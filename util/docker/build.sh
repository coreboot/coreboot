#!/usr/bin/env sh

IMAGE_DIR="$1"
IMAGE_TYPE="$2"

if [ -z "${IMAGE_DIR}" ] || [ -z "${IMAGE_TYPE}" ]; then
	echo "Usage: $0 <directory> <type>"
	echo "Missing parameters. Exit."
	exit 1
fi

case "${IMAGE_TYPE}" in
base)
	;;
toolchain)
	;;
jenkins)
	;;
*)
	echo "No valid image type given. Exit."
	exit 1
	;;
esac

docker build \
	--no-cache \
	--tag coreboot/coreboot-sdk-${IMAGE_DIR}-${IMAGE_TYPE}:latest \
	-f ${IMAGE_DIR}/Dockerfile.${IMAGE_TYPE} \
	./${IMAGE_DIR}
