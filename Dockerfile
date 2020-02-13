FROM amd64/ubuntu:bionic

CMD ["/bin/bash"]

RUN apt update \
	&& apt install -y git vim man gcc clang make \
	&& mkdir /root/woody_woodpacker

WORKDIR /root/woody_woodpacker
