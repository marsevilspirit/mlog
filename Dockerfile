FROM archlinux:latest

# 修改 pacman 的镜像源
RUN sed -i 's/^#Server/Server/g' /etc/pacman.d/mirrorlist && \
    echo "Server = https://mirrors.tuna.tsinghua.edu.cn/archlinux/\$repo/os/\$arch" > /etc/pacman.d/mirrorlist && \
    echo "Server = https://mirrors.aliyun.com/archlinux/\$repo/os/\$arch" >> /etc/pacman.d/mirrorlist && \
    echo "Server = https://mirrors.ustc.edu.cn/archlinux/\$repo/os/\$arch" >> /etc/pacman.d/mirrorlist && \
    pacman -Syyu --noconfirm && \
    pacman -S --noconfirm base-devel gcc fmt jsoncpp

WORKDIR /mlog

COPY . .

RUN chmod +x setup.sh && ./setup.sh

CMD ["./logger.out"]
