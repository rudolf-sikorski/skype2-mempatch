#!/bin/sh -ex

PATCH_CMD="${PWD}/skype2-mempatch"
OLD_VERSION='4.3.0.37'
NEW_VERSION='8.10.0.9'
SKYPE_PID="$(pidof skype)"

"${PATCH_CMD}" "${SKYPE_PID}" "${OLD_VERSION}" "${NEW_VERSION}"

