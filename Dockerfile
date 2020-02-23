FROM amd64/ubuntu:bionic

CMD ["/bin/bash"]

RUN rm /etc/dpkg/dpkg.cfg.d/excludes

RUN apt update \
	&& apt install -y git vim man gcc clang make nasm \
	&& mkdir /root/woody_woodpacker

WORKDIR /root/woody_woodpacker
