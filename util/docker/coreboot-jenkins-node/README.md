run with

    docker run --privileged --restart=always -d -p 49151:49151 -v $host/path/to/ccache:/home/coreboot/.ccache -v $host/path/to/data/cache:/data/cache coreboot/coreboot-jenkins-node
