# how to use
# use docker build: docker build -t miniob .
# use docker compose: docker compose up -d --build
# make sure docker has been installed
FROM debian:latest

# ENV LANG=en_US.UTF-8
# locale
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y locales apt-utils && rm -rf /var/lib/apt/lists/* \
    && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
ENV LANG en_US.utf8
ENV TERM xterm-256color

# install packages
RUN apt-get update \
    && apt-get install -yq \
    ca-certificates wget curl python-is-python3 \
    openssh-server gpg-agent gnupg2 zsh \
    build-essential flex bison cmake \
    gdb valgrind clang-format \
    diffutils vim git ripgrep fd-find tree \
    libreadline-dev \
    --no-install-recommends \
    && rm -rf /var/lib/apt/lists/*
ENV PATH=/usr/lib/cargo/bin:$PATH

# init miniob dependencies
COPY --from=miniob . /tmp/miniob
RUN cd /tmp/miniob \
    && bash build.sh init \
    && rm -rf /tmp/miniob

# install openssh
RUN mkdir /var/run/sshd \
    && ssh-keygen -A \
    && service ssh start
EXPOSE 22
CMD ["/usr/sbin/sshd","-D"]

# configure user
RUN addgroup --gid 1000 docker && \
    adduser --uid 1000 --ingroup docker --home /home/docker --shell /bin/zsh --disabled-password --gecos "" docker
RUN USER=docker \
    && GROUP=docker \
    && curl -SsL https://github.com/boxboat/fixuid/releases/download/v0.6.0/fixuid-0.6.0-linux-amd64.tar.gz | tar -C /usr/local/bin -xzf - \
    && chown root:root /usr/local/bin/fixuid \
    && chmod 4755 /usr/local/bin/fixuid \
    && mkdir -p /etc/fixuid \
    && echo 'root:root' | chpasswd \
    && echo 'docker:docker' | chpasswd \
    && printf "user: $USER\ngroup: $GROUP\n" > /etc/fixuid/config.yml

# copy starter scripts
COPY bin/starter.sh /tmp
RUN chmod +x /tmp/starter.sh
ENTRYPOINT /tmp/starter.sh

# configure user shell
USER docker:docker

# install zinit plugins
ARG NO_INPUT=y
ARG NO_ANNEXES=y
RUN bash -c "$(curl --fail --show-error --silent --location https://raw.githubusercontent.com/zdharma-continuum/zinit/HEAD/scripts/install.sh)" \
    && echo "autoload -Uz compinit && compinit" >> ~/.zshrc \
    && echo "zinit ice depth=1; zinit light romkatv/powerlevel10k" >> ~/.zshrc \
    && echo "zinit light zsh-users/zsh-completions" >> ~/.zshrc \
    && echo "zinit light zsh-users/zsh-autosuggestions" >> ~/.zshrc \
    && echo "zinit light zdharma/fast-syntax-highlighting" >> ~/.zshrc \
    && echo "zinit snippet OMZ::lib/git.zsh" >> ~/.zshrc \
    && echo "zinit snippet OMZ::lib/history.zsh" >> ~/.zshrc \
    && echo "zinit snippet OMZ::lib/key-bindings.zsh" >> ~/.zshrc \
    && echo "zinit snippet OMZ::lib/clipboard.zsh" >> ~/.zshrc \
    && echo "zinit snippet OMZ::lib/completion.zsh" >> ~/.zshrc \
    && echo "zinit snippet OMZ::lib/theme-and-appearance.zsh" >> ~/.zshrc \
    && echo "zinit snippet OMZ::plugins/git/git.plugin.zsh" >> ~/.zshrc \
    && echo "zinit snippet OMZ::plugins/common-aliases/common-aliases.plugin.zsh" >> ~/.zshrc \
    && echo "zinit snippet OMZP::cp" >> ~/.zshrc \
    && echo "zinit snippet OMZP::gitignore" >> ~/.zshrc \
    && echo "zinit snippet OMZP::colored-man-pages" >> ~/.zshrc \
    && echo "zinit snippet OMZ::plugins/colored-man-pages/colored-man-pages.plugin.zsh" >> ~/.zshrc \
    && echo "export LD_LIBRARY_PATH=/usr/local/lib64:\$LD_LIBRARY_PATH" >> ~/.zshrc
SHELL ["/bin/zsh", "-c"]
RUN . /home/docker/.zshrc

WORKDIR /home/docker/miniob