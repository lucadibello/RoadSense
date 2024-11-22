#!/usr/bin/env bash

# Fail on error
set -euo pipefail

#
# Load environment variables from the .env file
#

ENV_FILE=.env
# Check if the .env file exists
if [ ! -f "$ENV_FILE" ]; then
  echo "Error: $ENV_FILE file not found. Please create one."
  exit 1
fi
# Load env file
set -a
# shellcheck disable=SC1091
source "$ENV_FILE"
set +a
unset ENV_FILE

# Ensure that the CERTIFICATE_DOMAINS is set
if [ -z "$CERTIFICATE_DOMAINS" ]; then
  echo "Please set CERTIFICATE_DOMAINS in the .env file."
  exit 1
fi

# Create the cert directory and CA directory if they don't exist
mkdir -p ./certs/CA

echo "Generating certificates for domains [$CERTIFICATE_DOMAINS]..."

# Define Docker volume mappings and environment variables
DOCKER_VOLUME_CERT="-v $(pwd)/certs:/certs"
DOCKER_VOLUME_CA="-v $(pwd)/certs/CA:/CA"
DOCKER_ENV_CAROOT="-e CAROOT=/CA"
DOCKER_WORKDIR="-w /certs"

# Install the local CA in the Docker container
docker run -ti --rm \
  $DOCKER_VOLUME_CERT \
  $DOCKER_VOLUME_CA \
  $DOCKER_ENV_CAROOT \
  $DOCKER_WORKDIR \
  alpine/mkcert -install

# Generate the certificates
docker run -ti --rm \
  $DOCKER_VOLUME_CERT \
  $DOCKER_VOLUME_CA \
  $DOCKER_ENV_CAROOT \
  $DOCKER_WORKDIR \
  alpine/mkcert $CERTIFICATE_DOMAINS
