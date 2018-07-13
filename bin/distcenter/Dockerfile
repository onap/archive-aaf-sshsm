FROM rmannfv/aaf-base:openssl_1.1.0

RUN git clone https://gerrit.onap.org/r/aaf/sshsm
RUN cd sshsm && \
  cd tpm-util && \
  cd duplicate && \
  make -f sampleMakefile

RUN mkdir /ca
COPY ./create_ca.sh /ca/
RUN mkdir /dup
RUN mkdir /dup/database
RUN mkdir /dup/database/host_sample
RUN mkdir /dup/bin

RUN cp sshsm/tpm-util/duplicate/ossl_tpm_duplicate /dup/bin
RUN cp sshsm/test/integration/samplecaservicecontainer/inittoolfiles/out_parent_public /dup/database/host_sample

ADD entrypoint.sh /entrypoint.sh
ENTRYPOINT [ "/entrypoint.sh" ]
