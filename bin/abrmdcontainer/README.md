### Make  a directory /tmp/volume (mkdir -p /tmp/volume) on the Host
### This directory (Sample Shared volume) will be used to mount into the abrmd
### container and is mounted as /tmp/files/ inside container

### The input to this container is the srkhandle
### (a file or environmental variable) and password(TPM Primary key operartion)
### of this container is the public portion of the TPM's out_parent_public
### (Public portion of TPM primary key) and tpm_status.yaml toindicate the status
### of the TPM initialization tool

### Building Docker Images

```
$ docker build -t <image name> -f dockerfile .
```

### Running ABRMD Container with Simulator

```
$ docker run -d --privileged -v /tmp/run/dbus:/var/run/dbus -v /tmp/volume:/tmp/files --name <container name> <image name>
```

### Running ABRMD Container with TPM Hardware

```
$ docker run -d --privileged -device=/dev/tpm0 -v /tmp/run/dbus:/var/run/dbus -v /tmp/volume:/tmp/files --name <container name> <image name>
```

### Running Tools Container
This command will drop you into the tools container with everything setup appropriately:
```
# Runs without any privileges.
# Requires that the dbus be mounted from the same host folder
# This is to enable communication between the tools and ABRMD
$ docker run -v /tmp/run/dbus:/var/run/dbus --name <container name> -it --entrypoint /bin/bash <image name>
```

##### Sanity Check
Run the following command in the tools container to see if everything is setup correctly:

```
tpm2_listpcrs
```
