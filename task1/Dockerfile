# QT TCP Server (Fedora-based)
FROM fedora:36

RUN dnf install -y qt5-qtbase-devel gcc-c++ make && \
    dnf clean all

WORKDIR /var/lib/server

COPY echoserver.h echoserver.cpp main.cpp echoServer.pro ./

RUN qmake-qt5 echoServer.pro && make && mv echoServer fedora_server

EXPOSE 65432

CMD ["./fedora_server"]
