Create folder under /tmp/volume/host_sample on host. This will be mounted into the container as shared volume for now.

Build the container using 

  docker build --no-cache -t dist-center .

Run it mounting the volume

 docker run -v /tmp/volume:/volume dist-center

This will output the following files in /tmp/volume/host_sample

  ca.cert
  dupEncKey
  dupPriv
  dupPub
  dupSymseed
