#####################################################################
# 1.  Étape de construction (build)                                 #
#####################################################################
FROM debian:bookworm-slim AS build

# ----- Dépendances de compilation ---------------------------------
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        git \
        pkg-config \
        libasio-dev \
        libpqxx-dev \
        liboctomap-dev \
        zlib1g-dev \
        libboost-system-dev \
    && rm -rf /var/lib/apt/lists/*

# ----- Copie du code source ---------------------------------------
WORKDIR /src
COPY . .

# ----- Compilation ------------------------------------------------
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

#####################################################################
# 2.  Étape d’exécution (runtime)                                   #
#####################################################################
FROM debian:bookworm-slim

# ----- Librairies nécessaires à l’exécution -----------------------
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        libpqxx-7.7 \
        liboctomap1 \
        zlib1g \
        libboost-system1.74.0 \
        libasio1 \
    && rm -rf /var/lib/apt/lists/*

# ----- Installation binaire + config ------------------------------
WORKDIR /opt/map-server
COPY --from=build /src/build/server_udp           ./server_udp
COPY config/server.yaml                           ./config/server.yaml

# ----- Port UDP par défaut ----------------------------------------
EXPOSE 48484/udp

# ----- Variables (surchargables via docker‑compose ou kubernetes) --
ENV PGURI="postgresql://postgres:postgres@postgres/maps" \
    UDP_PORT=48484 \
    UDP_MTU=1300

# ----- Commande d’entrée -----------------------------------------
CMD ["./server_udp"]
