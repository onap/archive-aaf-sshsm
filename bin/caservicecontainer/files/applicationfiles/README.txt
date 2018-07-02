1. downaload the tar file and untar it
   
   $tar -xvf casign.tar

2. Install the packages by running install.sh

    $chmod 755 install.sh
    $install.sh

    The script will copy pkcs11.cfg and test.csr files to /tmp

3. Make sure the pkcs11.cfg and test.csr files are copied to /tmp

4. Create a slot with pin

    $ sudo softhsm2-util --init-token --slot 0 --label "My token 1" --pin 123456789 --so-pin 123456789
    
    Note down the pin and slot number


5. Edit the /tmp/pkcs11.cfg file and update the slot and library path if required

6. Import the keys into softhsm/tpm and note the alias

7. Compile the java file

    $javac CaSign.java

8 Run the app

   $sudo java CaSign <pin> <alias>

9 The signed certificate will be generated at /tmp/test.cert

