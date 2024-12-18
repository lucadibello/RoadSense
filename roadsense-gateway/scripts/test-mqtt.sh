#!/usr/bin/env bash

docker run hivemq/mqtt-cli publish \
  -h 192.168.58.246 -p 1883 -u roadsense \
  -pw roadsense -t roadsense \
  -m '{"key1": "value1", "key2": "value2"}' \
  -q 1 \
  --verbose
