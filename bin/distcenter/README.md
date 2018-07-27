Create folder under /tmp/volume/host_<host name> for each host ( eg host_sample where sample is the name of the tpm capable host). 
This folder will be mounted into the container as shared volume for now.
Expects the out_parent_public for each host under the corresponding host directory and passphrase file under /tmp/volume/ containing the passphrase to encrypt the key. 


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

Encrypted priviate key under /tmp/volume
  privkey.pem.gpg
