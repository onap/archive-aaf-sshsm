
#!/bin/bash
set -e

mkdir -p /var/run/dbus

mkdir -p /tmp/files

stdbuf -oL -eL dbus-daemon --system --nofork 2>&1 1> /var/log/dbus-daemon.log &

# 1. Run Init_and_create_tpm_primary.sh script from /sshsm/tpm-util/initandverify directory
#    from Tabrmd (privileged) container

# 2. Create the directory as expected by the SoftHSM to read the files and
#    Copy the required files (keys and certificates) to be imported into TPM

mkdir -p /tmp/hwparent/S01tpm
mkdir -p /tmp/hwparent/S01tpm/activate
mkdir -p /tmp/hwparent/S01tpm/key01

# Copy the tpm Plugin's so file
cp /usr/local/lib/libtpm2-plugin.so /tmp/hwparent

cp /tmp/files/inittoolfiles/Afile1.id1 /tmp/hwparent/activate/

cp /tmp/files/applicationfiles/pkcs11.cfg/privkey.pem /sshsm/tpm-util/initandverify/private.pem

cp /tmp/files/Importtoolfiles/outPriv /tmp/hwparent/key01/Kfile1.priv

cp /tmp/files/Importtoolfiles/outPub /tmp/hwparent/key01/Kfile1.pub

cp /tmp/files/Importtoolfiles/pkcs11.cfg /tmp/hwparent/key01/

cp /tmp/files/applicationfiles/pkcs11.cfg /tmp/pkcs11.cfg

cp /tmp/files/applicationfiles/test.csr /tmp/

softhsm2-util --init-token --slot 0 --label "Token1" --pin 123456789 --so-pin 123456789

softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt

SoftHSMv2SlotID="$cat(slotinfo.txt)"

echo "${SoftHSMv2SlotID}"

sed -i  '1i'slot:$SoftHSMv2SlotID'' /tmp/hwparent/S01tpm/key01/pkcs11.cfg

sed -i  '1i'slot = $SoftHSMv2SlotID'' /tmp/pkcs11.cfg

# TODO - Script to generate the pkcs11.cfg for SoftHSM and Application
#cp /tmp/files/pkcs11.cfg /tmp/hwparent/S01tpm/key01/

#./sshsm/tpm-util/initandverify/ImportTpmKey.sh
