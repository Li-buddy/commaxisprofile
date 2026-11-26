#!/bin/bash
echo ---- call nject update ----
WORKING_DIR=$(realpath $(dirname "$0"))
ROOT_DIR="$WORKING_DIR/../.."
ROOT_DIR=$(realpath "$ROOT_DIR")

echo "Working Directory: $WORKING_DIR"
echo "Repository root Directory: $ROOT_DIR"

bash $WORKING_DIR/call_nject.sh update
