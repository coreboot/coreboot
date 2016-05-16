This builds the coreboot tree in /dev/cb-build so that's a directory that uses
a tmpfs.  This helps to speed up the build and doesn't write the output to
the SSD.

The encapsulate tool that the coreboot build runs under for security requires
that docker be run using the --privileged command to work correctly.

Run with the command:

    docker run --privileged --restart=always -d -p 49151:49151 -v $host_path_to_ccache:/home/coreboot/.ccache -v $host_path_to_data_cache:/data/cache coreboot/coreboot-jenkins-node
