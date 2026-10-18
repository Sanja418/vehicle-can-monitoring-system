#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/../.."

python_path="build/renode-venv/Scripts/python.exe"
renode_dir="${RENODE_DIR:-C:/Program Files/Renode}"

if [[ ! -f "$python_path" ]]; then
    echo "Missing Python environment: build/renode-venv"
    exit 1
fi

# Rebuild firmware before testing.
bash firmware/can_demo/build.sh

"$python_path" "$renode_dir/tests/run_tests.py" \
    --css-file "$renode_dir/tests/robot.css" \
    --robot-framework-remote-server-full-directory "$renode_dir" \
    --robot-framework-remote-server-name renode.exe \
        -r "build/renode-results/$(date +%Y%m%d-%H%M%S)" \
    tests/renode/can_communication.robot
