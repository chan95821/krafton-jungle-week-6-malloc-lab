FROM ubuntu:latest

ARG USER_UID=1000
ARG USER_GID=1000

ENV TZ=Asia/Seoul LANG=ko_KR.UTF-8 LANGUAGE=ko_KR.UTF-8

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    locales tzdata build-essential gcc-multilib sudo gdb vim curl git wget \
    python3 cmake make gcc sudo valgrind telnet net-tools iproute2\
    && locale-gen ko_KR.UTF-8 && update-locale LANG=ko_KR.UTF-8

RUN if ! getent group "${USER_GID}" >/dev/null; then groupadd --gid "${USER_GID}" jungle; fi \
    && if getent passwd "${USER_UID}" >/dev/null; then \
        existing_user="$(getent passwd "${USER_UID}" | cut -d: -f1)"; \
        usermod --login jungle --home /home/jungle --move-home --gid "${USER_GID}" "${existing_user}"; \
    else \
        useradd --uid "${USER_UID}" --gid "${USER_GID}" -m -s /bin/bash jungle; \
    fi \
    && usermod -aG sudo jungle
RUN echo "jungle ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/jungle
# 암호 설정
# RUN echo 'jungle:jungle' | chpasswd

USER jungle
WORKDIR /home/jungle
