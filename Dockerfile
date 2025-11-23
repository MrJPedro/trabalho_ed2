FROM gcc:latest

COPY . .

RUN ["gcc", "-o", "main", "main.c", "compress.c", "compressed_search.c", "huffman.c", "search.c"]