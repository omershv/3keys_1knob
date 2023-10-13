FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    usbutils \
    sdcc \
    python3 \
    python3-pip \
    git \
    libusb-1.0-0-dev \
    && rm -rf /var/lib/apt/lists/*

RUN pip install pyusb pyYAML

WORKDIR /src

RUN git clone https://github.com/frank-zago/isp55e0.git isp55e0_git && cd isp55e0_git && make all && cp isp55e0 /usr/bin/ && cd .. && rm -rf isp55e0_git

CMD ["bash"]

