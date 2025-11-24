FROM gcc:latest

COPY . .

RUN ["gcc", "-o", "projeto-compilado", "main.c", "compress.c", "compressed_search.c", "huffman.c", "search.c"]