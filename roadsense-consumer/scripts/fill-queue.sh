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
docker run -it --rm --network host \
  -v "$(pwd)/scripts/body.json:/body.json" \
  pivotalrabbitmq/perf-test:latest \
  --auto-delete false \
  --uri "$rabbitmq_uri" \
  --queue "$RABBIT_QUEUE" \
  --time 10 \
  --rate 5 \
  --producers 1 \
  -a \
  --consumers 0 \
  --id "roadsense-benchmark" \
  --body-content-type "application/json" \
  --body "/body.json"

# Print a message to the console
echo "Queue filled with messages."
