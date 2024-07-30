FROM debian:sid

COPY sources.list /etc/apt/sources.list

RUN apt update && \
    yes Y | apt install cmake && \
    yes Y | apt install clang-18 clang-tools-18 libc++-18-dev libc++abi-18-dev g++ gdb ninja-build vim antlr4 && \
    yes Y | apt install libstartup-notification0-dev libxcb-cursor-dev libxcb-keysyms1-dev libxcb-ewmh-dev libxcb-util-dev libxcb-xrm-dev libxcb-randr0-dev libxcb-shape0-dev libxcb-icccm4-dev libxcb-xrm-dev libxkbcommon-dev libxkbcommon-x11-dev python3 pip python3-pytest && \
    pip install --no-input --break-system-packages conan && \
    rm -rf /var/lib/apt/lists/* && \
    ln -s /usr/bin/clang-scan-deps-18 /usr/bin/clang-scan-deps && \
    ln -s /usr/bin/clang-18 /usr/bin/clang && \
    ln -s /usr/bin/clang++-18 /usr/bin/clang++

RUN useradd -ms /bin/bash robert.knutsson

USER robert.knutsson

RUN conan profile detect
COPY clang /home/robert.knutsson/.conan2/profiles/default
COPY clang /home/robert.knutsson/.conan2/profiles/clang

#RUN rm -rf /etc/ssh/ssh_host_rsa_key /etc/ssh/ssh_host_dsa_key && ssh-keygen -A 

#RUN adduser -D -h src user && yes password | passwd user

#EXPOSE 22

#CMD ["/usr/sbin/sshd","-D"]

