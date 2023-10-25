# Use the GCC image to have make and gcc by default
FROM gcc:bookworm
LABEL org.opencontainers.image.authors="Killian RAIMBAUD <killian.rai@gmail.com"

# Install the missing dependencies (libssl3 & libssl-dev)
RUN apt update
RUN apt install libssl-dev -y

# Copy the program sources and moved into the build directory
COPY src /build/src
COPY include /build/include
COPY Makefile /build/Makefile
WORKDIR /build

# Build the app
RUN make

# Make the executable available throughout the container
RUN mv /build/bforcesha /usr/local/bin/bforcesha

# Remove the sources from the image
RUN rm -rf /build