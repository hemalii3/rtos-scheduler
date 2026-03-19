FROM fedora:39
RUN dnf install -y cmake gcc-c++ make git && dnf clean all
WORKDIR /app
COPY . .
RUN mkdir -p build && cd build && cmake .. -Wno-dev && make -j$(nproc)
EXPOSE 9090
CMD ["./build/scheduler_server"]
