### OOM - Onap Operations Manager will deploy this as part of CA service
### two stages
### Stage 1 - Initialize TPM and exit (init.sh script is used for this case)
### Stage 2 - Starts the abrmd process and keeps it running to provide the
### means for application to interact wit TPM (run_abrmd.sh is used)

### Make  a directory /tmp/volume (mkdir -p /tmp/volume) on the Host
### This directory (Sample Shared volume ABRMD_DATA) is mounted to abrmd
### container and is mounted as /tmp/files/ inside container
### the host's name "TPM_NODE_NAME" environmental variable

### The input to this container is srkhandle(a file/environmental variable)
### tpm_status.yaml (TPM State) password (for TPM Primary key,
### password is encrypted using pgp and is extracted using the passphrase
### (provided in shared volume))
### Output of this container is the public
### portion of the TPM's Primary key (out_parent_public) and the status will
### be updated in the tpm_status.yaml file


### Input
### 1.srkhandle 2.password 3.passphrase 4.tpm_status.yaml

### Output
### 1. out_parent_public

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

##### Sanity Check
Run the following command in the tools container to see if everything is setup correctly:

```
tpm2_listpcrs
```
