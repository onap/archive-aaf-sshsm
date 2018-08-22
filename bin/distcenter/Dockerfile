FROM nexus3.onap.org:10001/onap/aaf/aaf-base-openssl_1.1.0:latest 

RUN git clone https://gerrit.onap.org/r/aaf/sshsm
RUN cd sshsm && \
  cd tpm-util && \
  cd duplicate && \
  make -f sampleMakefile
RUN mkdir -p /dup/bin && \
   cp sshsm/tpm-util/duplicate/ossl_tpm_duplicate /dup/bin && \
   rm -rf sshsm

RUN mkdir -p /distcenter/bin
COPY ./create_ca.sh /distcenter/bin

ADD entrypoint.sh /entrypoint.sh
ENTRYPOINT [ "/entrypoint.sh" ]
