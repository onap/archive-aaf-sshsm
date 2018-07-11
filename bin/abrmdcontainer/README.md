### Building Docker Images

```
$ docker build -t <image name> -f dockerfile .
```

### Running ABRMD Container

```
$ docker run -d --privileged -v /tmp/run/dbus:/var/run/dbus --name <container name> <image name>
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
