Create folder under /tmp/volume/host_<host name> for each host (example
host_sample where sample is the name of the tpm capable host).
This folder will be mounted into the container as shared volume for now.

Expects the input SRK pulic key "out_parent_public" for each host under
the corresponding host directory and file "passphrase" under /tmp/volume/
containing the passphrase to encrypt the key.

example

  /tmp/volume/host_sample/out_parent_public
  /tmp/volume/passphrase

Build the container using

  docker build --no-cache -t dist-center .

Run it mounting the volume

 docker run -v /tmp/volume:/volume dist-center

This will output the following files in /tmp/volume/host_<host name>

  ca.cert
  dupEncKey
  dupPriv
  dupPub
  dupSymseed

Encrypted private key and certificate under /tmp/volume

  ca.cert
  privkey.pem.gpg
