FROM gcc:latest

COPY . /codigo-fonte

WORKDIR /codigo-fonte

RUN ["gcc", "-o", "/projeto-compilado", "main.c", "compress.c", "compressed_search.c", "huffman.c", "search.c"]

WORKDIR /