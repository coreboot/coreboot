#!/bin/sh
set -e
curl -O https://qa.coreboot.org/jnlpJars/agent.jar
java -jar agent.jar \
  -url https://qa.coreboot.org/ \
  -secret "$(cat /data/secret)" \
  -name "$(cat /data/nodename)" \
  -webSocket \
  -workDir "/home/coreboot/node-root"
