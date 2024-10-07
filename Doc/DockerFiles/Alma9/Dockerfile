#To run use:  docker build -t mach3 .
# KS: Get glorious container from Luke which will work as a base
FROM picker24/root_v6_26_10:alma9 AS mach3tutorial_build

# Declare the build argument
ARG MACH3_VERSION
ENV MACH3_VERSION=${MACH3_VERSION:-develop}
ENV MACH3_WORK_DIR=/opt/MaCh3Tutorial/
ENV MACH3_INSTALL_DIR=/opt/MaCh3Tutorial/build/

RUN mkdir -p ${MACH3_WORK_DIR}

WORKDIR /opt/
# KS: Let's clone MaCh3Tutorial
RUN --mount=type=ssh git clone https://github.com/mach3-software/MaCh3Validations ${MACH3_WORK_DIR}
WORKDIR ${MACH3_WORK_DIR}
RUN git checkout ${MACH3_VERSION}

RUN mkdir -p ${MACH3_INSTALL_DIR}
WORKDIR ${MACH3_INSTALL_DIR}
RUN cmake -DPYTHON_ENABLED=ON ${MACH3_WORK_DIR}
RUN make -j4 && make install

# KS: Need to set them here, otherwise container using this container will not be able to find MaCh3Tutorial
ENV MaCh3Validations_ROOT=${MACH3_INSTALL_DIR}
ENV PATH=${MaCh3Validations_ROOT}/bin:${PATH} \
    LD_LIBRARY_PATH=${MaCh3Validations_ROOT}/lib:${LD_LIBRARY_PATH}

# Start from MaCh3Tutorial install dir
WORKDIR ${MACH3_INSTALL_DIR}
