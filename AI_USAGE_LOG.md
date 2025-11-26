# Relatório de Uso de Inteligência Artificial Generativa

Este documento registra todas as interações significativas com ferramentas de IA generativa (como Gemini, ChatGPT, Copilot, etc.) durante o desenvolvimento deste projeto. O objetivo é promover o uso ético e transparente da IA como ferramenta de apoio, e não como substituta para a compreensão dos conceitos fundamentais.

## Política de Uso

O uso de IA foi permitido para as seguintes finalidades:

- Geração de ideias e brainstorming de algoritmos.
- Explicação de conceitos complexos.
- Geração de código boilerplate (ex: estrutura de classes, leitura de arquivos).
- Sugestões de refatoração e otimização de código.
- Debugging e identificação de causas de erros.
- Geração de casos de teste.

É proibido submeter código gerado por IA sem compreendê-lo completamente e sem adaptá-lo ao projeto. Todo trecho de código influenciado pela IA deve ser referenciado neste log.

## Registro de Interações

_Copie e preencha o template abaixo para cada interação relevante._

### Interação 1

- **Data:** 20/10/2025
- **Etapa do Projeto:** 1 - Compressão de Arquivos
- **Ferramenta de IA Utilizada:** Gemini Advanced
- **Objetivo da Consulta:** Eu estava com dificuldades para entender como gerenciar o dicionário do algoritmo LZW quando ele atinge o tamanho máximo. Precisava de uma estratégia para lidar com isso.

- **Prompt(s) Utilizado(s):**

  1. "No algoritmo de compressão LZW, o que acontece quando o dicionário atinge o tamanho máximo? Quais são as estratégias mais comuns para lidar com isso?"
  2. "Pode me dar um exemplo em Python de como implementar a estratégia de 'resetar o dicionário' no LZW?"

- **Resumo da Resposta da IA:**
  A IA explicou três estratégias: 1) parar de adicionar novas entradas, 2) resetar o dicionário para o estado inicial, e 3) usar uma política de descarte, como LRU (Least Recently Used), que é mais complexa. A IA forneceu um pseudocódigo para a estratégia de reset, que parecia a mais simples e eficaz para este projeto.

- **Análise e Aplicação:**
  A resposta da IA foi extremamente útil para clarear as opções. Optei por implementar a estratégia de resetar o dicionário. O código fornecido pela IA não foi usado diretamente, pois estava muito simplificado e não se encaixava na minha arquitetura de classes. No entanto, a lógica de verificar o tamanho do dicionário e invocar uma função `reset_dictionary()` foi a base para a minha implementação. Isso me poupou tempo de pesquisa em artigos e livros.

- **Referência no Código:**
  A lógica inspirada por esta interação foi implementada no arquivo `compressor/lzw.py`, especificamente na função `compress()`, por volta da linha 85.

---

### Interação 2

- **Data:** 24/11/2025
- **Etapa do Projeto:** Documentação
- **Ferramenta de IA Utilizada:** ChatGPT
- **Objetivo da Consulta:** Melhorar formatção do MARKDOWN
- **Prompt(s) Utilizado(s):** "Melhore esse texto e faça no formato markdown: Estou fazendo uma CLI em C para comprimir e buscar em arquivos. Ela deve:
- compactar <orig> <cmp> [block_size], descompactar <cmp> <saida>, busca <arquivo> "<padrao>", busca_compactada <cmp> "<padrao>" [offset_hint] [raio].
- Compressão por blocos usando Huffman; grava frequência (256 u32) por bloco, cabeçalho "CMP1", block_size, block_count, index_offset e índice com orig_offset/orig_size/comp_offset/comp_size; offsets 64 bits.
- Busca no texto original com KMP em streaming.
- Busca no compactado: lê índice, descompacta só blocos relevantes, aplica KMP com overlap; se houver hint_offset e raio, limita a janela e reseta estado fora da janela.
- Limites block_size: 1 KiB a 8 MiB, padrão 4 KiB; clamp ou rejeita fora do intervalo.
- Escreva em português com seções claras e bullets.
  "
- **Resumo da Resposta da IA:** A resposta da IA é o nosso atual README
- **Análise e Aplicação:** O uso da IA foi benefico para nós pois nos auxiliou na geração do markdown com base em um texto que fomos preenchendo ao longo do desenvolvimento
- **Referência no Código:** Utilizamos a IA no nosso arquivo README.md
