
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

# 3. Copy the tpm Plugin's so file

cp /usr/local/lib/libtpm2-plugin.so /tmp/hwparent

# 4. Generate the TPM Primary key handle's file and give the required permission and update the file

touch /tmp/hwparent/activate/Afile1.id1
chmod 755 /tmp/hwparent/activate/Afile1.id1

#Manually create the handle
echo "0x8100011" >> /tmp/hwparent/activate/Afile1.id1
# Or Pass it as an environmental variable

#tpm_handle = "0x81000011"
#echo "$tpm_handle" >> /tmp/hwparent/activate/Afile1.id1

# Copy the private key which is to bve imported into TPM
cp /tmp/files/applicationfiles/pkcs11.cfg/privkey.pem /sshsm/tpm-util/initandverify/private.pem

# 5. Generate the pkcs11.cfg file required for the SoftHSM opeations

touch /tmp/hwparent/S01tpm/key01/pkcs11.cfg
# Create the config file for the application as well
touch /tmp/cfg.pkcs11
chmod 755 /tmp/hwparent/S01tpm/key01/pkcs11.cfg
chmod 755 /tmp/cfg.pkcs11

key_id="8738"
echo "${key_id}"
echo "key_id:${key_id}" >> /tmp/hwparent/S01tpm/key01/pkcs11.cfg

key_label="ABC"
echo "${key_label}"
echo "key_label:${key_label}" >> /tmp/hwparent/S01tpm/key01/pkcs11.cfg
echo "name = ${key_label}" >> /tmp/cfg.pkcs11

upin="123456789"
echo "upin:${upin}" >> /tmp/hwparent/S01tpm/key01/pkcs11.cfg
sopin="123456789"
echo "sopin is ${sopin}"

applicationlibrary="/usr/local/lib/libtpm2-plugin.so"
echo "${applicationlibrary}"
echo "library = ${applicationlibrary}" >> /tmp/cfg.pkcs11

cp /tmp/files/applicationfiles/test.csr /tmp/
cp /tmp/files/applicationfiles/CaSign.java /tmp

softhsm2-util --init-token --slot 0 --label "Token1" --pin ${upin} --so-pin ${sopin}

softhsm2-util --show-slots | grep 'Slot ' | cut -d\           -f2 | head -1 >> slotinfo.txt

SoftHSMv2SlotID="$cat(slotinfo.txt)"

echo "${SoftHSMv2SlotID}"

sed -i  '1i'slot:$SoftHSMv2SlotID'' /tmp/hwparent/S01tpm/key01/pkcs11.cfg

sed -i  '1i'slot = $SoftHSMv2SlotID'' /tmp/pkcs11.cfg

./sshsm/tpm-util/initandverify/ImportTpmKey.sh
