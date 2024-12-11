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

# Connect to postgres via psql and check if connection is successful
echo $DATABASE_URL
result=$(psql -d "$DATABASE_URL" -c "SELECT 1" 2>&1)

echo "Connecting to database..."
echo "$result"
