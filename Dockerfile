FROM debian:bookworm-slim AS toolchain

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        cmake \
        flex \
        g++ \
        libfl-dev \
        ninja-build \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /source
COPY . .

RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel

FROM toolchain AS test
RUN ctest --test-dir build --output-on-failure

FROM debian:bookworm-slim AS runtime

COPY --from=toolchain /source/build/r2d2c /usr/local/bin/r2d2c

ENTRYPOINT ["r2d2c"]
CMD ["--help"]
