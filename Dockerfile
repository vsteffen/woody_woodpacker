FROM amd64/ubuntu:bionic

CMD ["/bin/bash"]

RUN rm /etc/dpkg/dpkg.cfg.d/excludes

RUN apt update \
	&& apt install -y git vim man gcc clang make nasm gdb \
	&& git clone https://github.com/longld/peda.git /root/peda \
	&& echo "source /root/peda/peda.py" >> ~/.gdbinit \
	&& mkdir /root/woody_woodpacker

WORKDIR /root/woody_woodpacker
