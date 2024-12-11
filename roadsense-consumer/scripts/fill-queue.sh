#!/usr/bin/env bash

ENV_FILE=.env

# ensure .env exists
if [ ! -f $ENV_FILE ]; then
  echo "No .env file found. Please create one."
  exit 1
fi

echo "Loading $(cat $ENV_FILE | wc -l) environment variables from $ENV_FILE"

# Load .env file
set -o allexport
source "$ENV_FILE"
set +o allexport

# Build the URI to connect to RabbitMQ
rabbitmq_uri="amqp://$RABBIT_USER:$RABBIT_PASSWORD@$RABBIT_HOST:$RABBIT_PORT"

# Run the perf-test Docker container to fill the queue
docker run -it --rm --network host pivotalrabbitmq/perf-test:latest \
  --auto-delete false \
  --uri "$rabbitmq_uri" \
  --queue "$RABBIT_QUEUE" \
  --producers 1 --consumers 0 -a \
  --id "roadsense-benchmark" \
  --body-content-type "application/json" \
  --json-body "{\"lat\": 51.5074, \"lon\": 0.1278, \"timestamp\": \"$(date +%s)\", \"device_id\": \"benchmark\"}"

# Print a message to the console
echo "Queue filled with messages."
