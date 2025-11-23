FROM gcc:latest

COPY . .

CMD ["gcc", "-o", "main", "main.c", "compress.c", "compressed_search.c", "huffman.c", "search.c"]