#!/usr/bin/env bash

# Fail on error
set -euo pipefail

#
# Load environment variables from the .env file
#

ENV_FILE=.env.pub
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
mkdir -p ./certs

echo "Generating certificates for domains [$CERTIFICATE_DOMAINS]..."
cwd=$(pwd)
tmpDir=$(mktemp -d -t mkcert-XXXX)
cd $tmpDir

# install mkcert + generate certs
cd $tmpDir
mkcert -install
mkcert $CERTIFICATE_DOMAINS
cd $cwd

# Move back all generated files to the certs directory
mv $tmpDir/*.pem ./certs

# Delete the temporary directory
rm -rf $tmpDir
