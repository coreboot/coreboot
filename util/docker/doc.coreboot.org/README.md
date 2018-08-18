# doc.coreboot.org
 Docker container for generating and developing documentation for doc.coreboot.org

**NOTE**: All paths are from the base of the coreboot git repo.

### Build

```sh
  docker build --force-rm -t "doc.coreboot.org" "$PWD/util/docker/doc.coreboot.org/"
```

### Generating production HTML

```sh
# To ensure the output directory is given the correct permissions, make sure to
# created it before running docker the first time.
mkdir -p "$PWD/Documentation/_build/"

docker run -it --rm \
           --user "$(id -u):$(id -g)" \
           -v "$PWD/:/data-in/:ro" \
           -v "$PWD/Documentation/_build/:/data-out/" \
           doc.coreboot.org
```

### live reloaded with web server
On the host machine, open a browser to the address http://0.0.0.0:8000
```sh
docker run -it --rm \
           --net=host -v "$PWD/:/data-in/:ro" \
           doc.coreboot.org livehtml
```
