### Sistema de Processamento de Arquivos

Ferramenta de linha de comando para **compactar** arquivos de texto em blocos, **descompactar** e **buscar substrings** tanto no texto original quanto diretamente no arquivo compactado.

### Como compilar e executar

- Local (requer gcc):  
  `gcc -Wall -Wextra -std=c11 -D_FILE_OFFSET_BITS=64 -o projeto main.c compress.c compressed_search.c search.c`

Comandos da CLI:

- `compactar <orig> <cmp> [block_size]` — compacta em blocos Huffman (tamanho padrão 4096 bytes, limitado a 1024..8MiB).
- `descompactar <cmp> <saida>` — reconstrói o arquivo original.
- `busca <arquivo> "<padrao>"` — busca KMP em streaming no texto original.
- `busca_compactada <cmp> "<padrao>" [offset_hint] [raio]` — descompacta apenas blocos relevantes e busca com KMP; `hint`/`raio` delimitam a janela opcional de busca.

### Formato CMP1 (arquivo compactado)

- Cabeçalho: `"CMP1"` + `block_size` (u32) + `block_count` (u32) + `index_offset` (u64).
- Dados: blocos compactados sequenciais. Cada bloco armazena a tabela de frequência (256 × u32 = 1024 bytes) seguida do bitstream Huffman.
- Índice: array de `BlockIndex {orig_offset, orig_size, comp_offset, comp_size}` gravado em `index_offset`.
- Offsets em 64 bits (`fseeko`/`ftello`), permitindo arquivos grandes.

### Algoritmos e fluxo

- **Compressão**: Huffman por bloco. O bloco é lido, frequências calculadas, árvore construída, tabela gravada e bitstream emitido. O índice captura offsets e tamanhos originais/compactados.
- **Descompressão**: lê cabeçalho e índice, valida limites e offsets, reconstrói cada bloco pelo Huffman do próprio bloco.
- **Busca em texto**: KMP em streaming com sobreposição de `m-1` bytes para capturar ocorrências que cruzam blocos de leitura.
- **Busca em compactado**: carrega apenas o índice, descompacta blocos sob demanda, aplica KMP com sobreposição; se `hint_offset` é usado, blocos/bytes fora da janela são ignorados e o estado do KMP é reinicializado para evitar falsos positivos.

### Decisões de projeto

- **Blocos com índice**: necessário para busca seletiva; o índice fica no fim para escrita única e leitura direta.
- **Limite de bloco**: clamp entre 1 KiB e 8 MiB para equilibrar compressão e memória. Padrão 4 KiB.
- **Streaming constante**: buffers de tamanho do bloco (+ sobreposição) e tabelas de 256 u32; não há carregamento integral do arquivo.
- **Validação defensiva**: checagem de cabeçalho, limites de bloco, offsets e tamanhos de índice/blocos antes de ler/descompactar.
- **KMP compartilhado**: rotina de LPS exposta para reutilização em buscas em texto e em compactados, reduzindo duplicação.

### Casos de borda tratados

- Arquivos vazios ou blocos com um único símbolo (código Huffman força 1 bit).
- Valores de `block_size` fora do limite são ajustados ou rejeitados conforme a operação.
- Ocorrências que cruzam fronteiras de blocos na leitura e na busca de compactados.
- Índices corrompidos ou offsets fora do arquivo são detectados antes da leitura.
