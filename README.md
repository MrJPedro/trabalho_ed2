### **Especificação de Trabalho Prático: Sistema de Processamento de Arquivos Grandes**

**Objetivo Geral:** Projetar e implementar uma ferramenta de linha de comando
(CLI) para compressão e busca de substrings em arquivos de texto, com foco no
processamento de dados que excedem a memória RAM disponível. O projeto visa
aplicar conceitos de algoritmos, manipulação de arquivos em baixo nível e
gerenciamento de memória.

**Descrição Geral:** Os estudantes desenvolverão uma aplicação modular que será
construída em três etapas progressivas. A aplicação final deverá ser capaz de
compactar um arquivo de texto grande e, posteriormente, realizar buscas por
substrings diretamente no arquivo compactado, de forma eficiente e sem a
necessidade de descompressão **total**.

**Requisitos Gerais (Válidos para todas as etapas):**

* **Linguagem de Programação:** Livre, mas todos os recursos utilizados devem
ser implementados pela equipe.
* **Interface:** A interação com o programa deve ser via linha de comando (CLI).
* **Gerenciamento de Memória:** A restrição principal é que nenhuma etapa pode
carregar o arquivo de entrada inteiro na memória RAM. O uso de memória deve ser
baixo e constante, independentemente do tamanho do arquivo.
* **Entrega:** Código-fonte completo, bem documentado, e um breve relatório
técnico (em formato Markdown no README do projeto) explicando as decisões de projeto para cada etapa
concluída. A entrega deve ser realizada através de um fork deste projeto no gitlab.
* **Número de integrantes**: No mínimo 3 integrantes.
* **Uso de IA**: O uso de IA generativa é permitido no trabalho. Porém, este uso deve ser relatado no arquivo `AI_USAGE_LOG.md` seguindo as regras lá contidas. O preenchimento deste arquivo será contabilizado na nota de defesa e documentação.

---

### **Etapas do Projeto**

#### **Etapa 1: Compressão de Arquivos Grandes (Vale até 40 pontos)**

**Objetivo:** Implementar a funcionalidade de compressão de um arquivo de texto.
O algoritmo deve processar o arquivo de entrada garantindo baixo uso de memória.

* **Comando:** `meu_programa compactar <arquivo_original> <arquivo_compactado>`
* **Algoritmo Sugerido:** Implementação do LZW (Lempel-Ziv-Welch) ou Huffman. A
utilização de bibliotecas prontas para o núcleo do algoritmo de compressão é
vetada..

#### **Etapa 2: Busca de Substring em Arquivo Grande (Vale até 40 pontos adicionais)**

**Objetivo:** Implementar uma busca por substring em um arquivo de texto
**original (não comprimido)** que pode ser maior que a memória RAM disponível.

* **Comando:** `meu_programa buscar_simples <arquivo_original> "<substring>"`
* **Saída:** A lista de posições (offsets em bytes) onde a substring foi encontrada.
* **Algoritmo Sugerido:** Knuth-Morris-Pratt (KMP) ou Boyer-Moore.

#### **Etapa 3: Busca de Substring em Arquivo Comprimido (Vale até 40 pontos adicionais)**

**Objetivo:** Integrar e evoluir as etapas anteriores para permitir a busca por
substring diretamente no arquivo gerado pela Etapa 1, sem descompressão total.

* **Comando:** `meu_programa buscar_compactado <arquivo_compactado> "<substring>"`
* **Saída:** A lista de posições (offsets em bytes) relativas ao **arquivo original**.
* **Requisito de Arquitetura:** Para viabilizar esta etapa, o formato do arquivo
comprimido (criado na Etapa 1) deverá ser modificado. A solução recomendada é a
**compressão em blocos com um índice**. O arquivo comprimido deve conter uma
tabela (índice) que mapeia os blocos de dados do arquivo original para suas
versões comprimidas.
* **Processo de Busca:**
    1. Carregar apenas o índice em memória.
    2. Iterar pelos blocos usando o índice.
    3. Para cada bloco: ler do disco, descomprimir em memória e realizar a busca.
    4. Calcular a posição correta no arquivo original e lidar com ocorrências
       que cruzam as fronteiras dos blocos.
* **Desafio Principal:** Projetar uma estrutura de arquivo indexada eficiente e
implementar a lógica de busca que a utilize para descompressão seletiva.

---

### **Defesa do Trabalho**

Parte da nota de cada etapa será atribuída durante uma defesa presencial. Os
alunos deverão apresentar o código-fonte funcionando, explicar as escolhas de
algoritmos e estruturas de dados, e realizar pequenas alterações/correções no
código em tempo real, conforme solicitado pelo professor, para demonstrar
domínio sobre a solução desenvolvida.

---

### **Rubricas de Avaliação**

#### **Rubrica - Etapa 1 (Nota Máxima: 40)**

| Critério | Excelente (31-40 pts) | Bom (21-30 pts) | Satisfatório (15-20 pts) | Insuficiente (<15 pts) |
| :--- | :--- | :--- | :--- | :--- |
| **Funcionalidade (15 pts)** | Comprime e descomprime arquivos grandes perfeitamente, com integridade total dos dados. | Funciona para a maioria dos casos, mas pode ter bugs em arquivos muito específicos. | A compressão funciona, mas a descompressão falha ou corrompe dados. | Não funciona ou não foi entregue. |
| **Gerenciamento de Memória (10 pts)** | Uso de memória é baixo e constante, comprovadamente independente do tamanho do arquivo. | Uso de memória é baixo na maior parte do tempo, mas pode ter picos. | Tenta gerenciar memória, mas ainda carrega porções grandes demais do arquivo. | Carrega o arquivo inteiro na memória. |
| **Defesa Oral e Modificação (15 pts)** | Explica o algoritmo e o código com clareza. Realiza modificações solicitadas rapidamente. | Explica o funcionamento geral, mas tem dificuldade com detalhes. Realiza modificações com ajuda. | Explicação superficial. Não consegue realizar modificações práticas no código. | Não consegue explicar o próprio código. |

#### **Rubrica - Etapa 2 (Nota Máxima: 40)**

| Critério | Excelente (31-40 pts) | Bom (21-30 pts) | Satisfatório (15-20 pts) | Insuficiente (<15 pts) |
| :--- | :--- | :--- | :--- | :--- |
| **Funcionalidade (15 pts)** | Encontra todas as ocorrências, incluindo as que cruzam as fronteiras dos blocos de leitura. | Encontra a maioria das ocorrências, mas falha em alguns casos de fronteira. | Encontra apenas ocorrências que estão inteiramente dentro de um bloco. | Não encontra as ocorrências corretamente. |
| **Gerenciamento de Memória (10 pts)** | Uso de memória é baixo e constante. | Uso de memória aceitável, mas não otimizado. | Tenta gerenciar memória, mas ainda carrega porções grandes demais do arquivo. | Carrega o arquivo inteiro na memória. |
| **Defesa Oral e Modificação (15 pts)** | Explica o algoritmo de busca e a lógica para tratar fronteiras de blocos. Modifica a formatação da saída com facilidade. | Explica o algoritmo, mas a lógica de fronteira é confusa. Modifica a saída com dificuldade. | Explicação genérica do algoritmo de busca, sem abordar o desafio principal. | Não consegue explicar o próprio código. |

#### **Rubrica - Etapa 3 (Nota Máxima: 40)**

| Critério | Excelente (31-40 pts) | Bom (21-30 pts) | Satisfatório (15-20 pts) | Insuficiente (<15 pts) |
| :--- | :--- | :--- | :--- | :--- |
| **Design da Estrutura Indexada (10 pts)** | O formato do arquivo é eficiente, bem documentado e robusto. O índice é compacto. | O formato funciona, mas poderia ser mais otimizado ou está pouco documentado. | O formato é funcionalmente correto, mas ingênuo e/ou ineficiente. | Estrutura mal projetada ou inexistente. |
| **Funcionalidade da Busca (15 pts)** | Encontra todas as ocorrências no arquivo comprimido, reportando as posições originais corretamente. | Funciona na maioria dos casos, mas pode falhar em casos complexos (ex: múltiplas ocorrências na fronteira). | A busca funciona, mas os offsets reportados estão incorretos ou não trata fronteiras. | A busca não funciona. |
| **Defesa Oral e Modificação (15 pts)** | Demonstra domínio completo do sistema integrado. Justifica as decisões de design do índice e é capaz de debater trade-offs. Realiza alterações complexas na lógica de busca. | Explica bem o sistema, mas não consegue justificar profundamente as escolhas de design. Realiza alterações com ajuda. | Explicação superficial. Consegue apenas descrever o que o código faz, sem explicar o "porquê". | Não demonstra ter autoria sobre o projeto. |
