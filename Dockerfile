FROM gcc:latest

COPY . /projeto/codigo-fonte

WORKDIR /projeto/codigo-fonte

RUN ["gcc", "-o", "/projeto/projeto-compilado", "main.c", "compress.c", "compressed_search.c", "search.c"]

WORKDIR /projeto