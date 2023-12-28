FROM debian:sid

COPY sources.list /etc/apt/sources.list

RUN apt update && \
    yes Y | apt install -t experimental cmake && \
    yes Y | apt install clang-17 clang-tools-17 libc++-17-dev libc++abi-17-dev g++ gdb ninja-build vim antlr4 && \
    yes Y | apt install libstartup-notification0-dev libxcb-cursor-dev libxcb-keysyms1-dev libxcb-ewmh-dev libxcb-util-dev libxcb-xrm-dev libxcb-randr0-dev libxcb-shape0-dev libxcb-icccm4-dev libxcb-xrm-dev libxkbcommon-dev libxkbcommon-x11-dev libpcre2-dev libcairo2-dev libpango1.0-dev libglib2.0-dev libfmt-dev nlohmann-json3-dev libev-dev perl make python3 pip python3-pytest && \
    rm -rf /var/lib/apt/lists/* && \
    pip install --no-input --break-system-packages conan

RUN ln -s /usr/bin/clang-scan-deps-17 /usr/bin/clang-scan-deps
RUN ln -s /usr/bin/clang-17 /usr/bin/clang
RUN ln -s /usr/bin/clang++-17 /usr/bin/clang++

RUN useradd -ms /bin/bash robert.knutsson

USER robert.knutsson

RUN conan profile detect
COPY conan-default /home/robert.knutsson/.conan2/profiles/default
COPY conan-default /home/robert.knutsson/.conan2/profiles/clang

#RUN rm -rf /etc/ssh/ssh_host_rsa_key /etc/ssh/ssh_host_dsa_key && ssh-keygen -A 

#RUN adduser -D -h src user && yes password | passwd user

#EXPOSE 22

#CMD ["/usr/sbin/sshd","-D"]

