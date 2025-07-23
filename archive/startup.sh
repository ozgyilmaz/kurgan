#!/bin/bash

PORT=${1:-4000}
LOG_DIR="../log"
SHUTDOWN_FILE="shutdown.txt"

# Go to area directory
cd area || { echo "Cannot cd into area/"; exit 1; }

# Cleanup shutdown file if present
[ -e "$SHUTDOWN_FILE" ] && rm -f "$SHUTDOWN_FILE"

while true; do
    index=1000
    while [ -e "$LOG_DIR/$index.log" ]; do
        ((index++))
    done
    LOGFILE="$LOG_DIR/$index.log"

    echo "Starting Kurgan on port $PORT (log: $LOGFILE)..."
    ./kurgan "$PORT" &> "$LOGFILE"

    if [ -e "$SHUTDOWN_FILE" ]; then
        echo "Shutdown file found. Exiting."
        rm -f "$SHUTDOWN_FILE"
        break
    fi

    echo "Restarting in 10 seconds..."
    sleep 10
done
