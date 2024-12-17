#!/usr/bin/env bash

ENV_FILE=.env

# Ensure .env exists
if [ ! -f $ENV_FILE ]; then
  echo "No .env file found. Please create one."
  exit 1
fi

echo "Loading $(wc -l <"$ENV_FILE") environment variables from $ENV_FILE"

# Load .env file
set -o allexport
source "$ENV_FILE"
set +o allexport

# Number of runs (how many times to run the container)
RUNS=100

# Function to generate random coordinates within a square over Switzerland
generate_random_body() {
  # Latitude (range: 45.8 - 47.8) and Longitude (range: 5.9 - 10.5) generation
  LAT=$(awk -v min=45.8 -v max=47.8 'BEGIN { srand(); printf "%.5f", min + (max - min) * rand() }')
  LON=$(awk -v min=5.9 -v max=10.5 'BEGIN { srand(); printf "%.5f", min + (max - min) * rand() }')
  BUMPINESS=$(awk -v min=0 -v max=255 'BEGIN { srand(); printf "%.0f", min + (max - min) * rand() }')
  TIMESTAMP=$(date +%s)

  # Create output directory if it doesn't exist
  OUTPUT_DIR="$(pwd)/scripts"
  mkdir -p "$OUTPUT_DIR"

  # JSON output file
  BODY_FILE="$OUTPUT_DIR/body.json"

  # Write the JSON to the file
  cat <<EOF >"$BODY_FILE"
{
  "lat": $LAT,
  "lon": $LON,
  "timestamp": $TIMESTAMP,
  "bumpiness": $BUMPINESS,
  "device_id": "benchmark"
}
EOF
}

# Handle cleanup on Ctrl+C
trap 'echo "\nStopping the script..."; exit 1' INT

# Build the URI to connect to RabbitMQ
rabbitmq_uri="amqp://$RABBIT_USER:$RABBIT_PASSWORD@$RABBIT_HOST:$RABBIT_PORT"

# Run the Docker container multiple times
for i in $(seq 1 $RUNS); do
  echo "Run #$i: Generating random coordinates in Switzerland..."
  generate_random_body

  # Run the Docker container
  docker run -it --rm --network host \
    -v "$(pwd)/scripts/body.json:/body.json" \
    pivotalrabbitmq/perf-test:latest \
    --auto-delete false \
    --uri "$rabbitmq_uri" \
    --queue "$RABBIT_QUEUE" \
    --time 1 \
    --rate 2 \
    --producers 1 \
    -a \
    --consumers 0 \
    --id "roadsense-benchmark-$i" \
    --body-content-type "application/json" \
    --body "/body.json"

  echo "Run #$i completed."
done

# Final message
echo "Completed $RUNS runs of the Docker container with coordinates in Switzerland."
