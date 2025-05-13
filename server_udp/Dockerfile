#####################################################################
# 1 ─ Étape “build” (compile le serveur)                            #
#####################################################################
FROM debian:bookworm-slim AS build

# --- dépendances de compilation ------------------------------------
    RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git pkg-config \
    libasio-dev liboctomap-dev zlib1g-dev \
    libpqxx-dev libboost-system-dev \
&& rm -rf /var/lib/apt/lists/*


# --- workspace -----------------------------------------------------
WORKDIR /usr/src/map-server
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build -j$(nproc)

#####################################################################
# 2 ─ Étape “runtime” (image légère ~90 Mo)                         #
#####################################################################
# --- runtime stage ---
    FROM debian:bookworm-slim

    RUN apt-get update && apt-get install -y --no-install-recommends \
            liboctomap1.9 libboost-system1.74.0 zlib1g \
        && rm -rf /var/lib/apt/lists/*
    
    # récupère libpqxx.so.* depuis le stage build
    COPY --from=build /usr/lib/x86_64-linux-gnu/libpqxx*.so* /usr/lib/
    
    WORKDIR /opt/map-server
    COPY --from=build /usr/src/map-server/build/server_udp ./server_udp
    COPY config/server.yaml ./config/server.yaml
    

EXPOSE 9000/udp             

# variables (surchargées par docker‑compose ou k8s)
ENV PGURI=postgresql://user:password@host.docker.internal:5432/mydb \
    UDP_PORT=9000 \
    UDP_MTU=1300
