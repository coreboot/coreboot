#!/bin/sh
ls -d */*/*/*/ | `dirname $0`/bucketize.sh weekly | `dirname $0`/towiki.sh
