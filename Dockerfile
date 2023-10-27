# Use the GCC image to have make and gcc by default
FROM gcc:bookworm
LABEL org.opencontainers.image.authors="Killian RAIMBAUD <killian.rai@gmail.com>"

# Install the missing dependencies (libssl3 & libssl-dev)
RUN apt update
RUN apt install libssl-dev -y

# Copy the program sources and moved into the build directory
COPY src /build/src
COPY Makefile /build/Makefile
COPY diag.sh /usr/local/bin/diag.sh
RUN chmod a+x /usr/local/bin/diag.sh
WORKDIR /build

# Build the app
RUN make

# Make the executable available throughout the container
RUN mv /build/stormbreaker /usr/local/bin/stormbreaker

# Remove the sources from the image
RUN rm -rf /build