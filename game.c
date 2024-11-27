#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_TENTATIVAS 6
#define MAX_NOME 100
#define ARQUIVO_ESTATS "estatisticas.bin"
#define NUM_FASES 5

// Estrutura para armazenar as informações do jogador
typedef struct {
    char nome[MAX_NOME];
    int placar;
    time_t data;
} Jogador;

// Função para exibir o estado atual da palavra e das tentativas
void mostrarPalavra(char letrasDescobertas[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%c ", letrasDescobertas[i]);
    }
    printf("\n");
}

// Função para verificar se o jogador acertou a letra
int tentarLetra(char letra, char palavra[], char letrasDescobertas[], int tamanho) {
    int acertou = 0;
    for (int i = 0; i < tamanho; i++) {
        if (palavra[i] == letra && letrasDescobertas[i] == '_') {
            letrasDescobertas[i] = letra;
            acertou = 1;
        }
    }
    return acertou;
}

// Função para verificar se o jogador adivinhou toda a palavra
int palavraCompleta(char letrasDescobertas[], int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (letrasDescobertas[i] == '_') {
            return 0; // Ainda há letras não descobertas
        }
    }
    return 1; // Palavra completa
}

// Função para mostrar as estatísticas do jogador atual
void mostrarEstatisticas(Jogador jogadorAtual) {
    FILE *file = fopen(ARQUIVO_ESTATS, "rb");
    if (!file) {
        printf("Nenhuma estatística encontrada.\n");
        return;
    }

    Jogador jogador;
    int encontrou = 0;

    while (fread(&jogador, sizeof(Jogador), 1, file)) {
        if (strcmp(jogador.nome, jogadorAtual.nome) == 0) {
            char dataStr[20];
            struct tm *tm_info = localtime(&jogador.data);
            strftime(dataStr, 20, "%d/%m/%Y %H:%M:%S", tm_info);

            printf("\n=== Estatisticas de %s ===\n", jogador.nome);
            printf("Placar: %d | Data: %s\n", jogador.placar, dataStr);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Nenhum registro encontrado para o jogador %s.\n", jogadorAtual.nome);
    }

    fclose(file);
}

// Função para salvar o placar do jogador nas estatísticas
void salvarEstatisticas(Jogador jogador) {
    FILE *file = fopen(ARQUIVO_ESTATS, "ab");
    if (file) {
        jogador.data = time(NULL); // Adiciona a data e hora do jogo
        fwrite(&jogador, sizeof(Jogador), 1, file);
        fclose(file);
    }
}

// Função para carregar palavras de arquivos específicos para cada fase
int carregarPalavras(char ***palavras) {
    *palavras = malloc(NUM_FASES * sizeof(char *)); // Aloca espaço para os ponteiros das palavras

    if (*palavras == NULL) {
        printf("Erro de alocacao de memoria para as palavras.\n");
        return 0;
    }

    for (int i = 0; i < NUM_FASES; i++) {
        char nomeArquivo[20];
        snprintf(nomeArquivo, sizeof(nomeArquivo), "fase%d.txt", i + 1); // Cria o nome do arquivo

        FILE *file = fopen(nomeArquivo, "r");
        if (!file) {
            printf("Erro: Nao foi possivel abrir o arquivo %s.\n", nomeArquivo);
            return 0; // Retorna 0 se algum arquivo não puder ser aberto
        }

        (*palavras)[i] = malloc(100 * sizeof(char)); // Aloca espaço para cada palavra
        if ((*palavras)[i] == NULL) {
            printf("Erro de alocacao de memoria para a palavra da fase %d.\n", i + 1);
            return 0;
        }

        if (fscanf(file, "%s", (*palavras)[i]) != 1) {
            printf("Erro: Nao foi possivel carregar a palavra de %s.\n", nomeArquivo);
            fclose(file);
            return 0;
        }

        fclose(file);
    }

    return NUM_FASES;
}

// Função para liberar memória das palavras
void liberarPalavras(char **palavras, int numFases) {
    for (int i = 0; i < numFases; i++) {
        free(palavras[i]);
    }
    free(palavras);
}

// Função para executar uma fase do jogo
int jogarFase(char palavra[], Jogador *jogador) {
    int tamanhoPalavra = strlen(palavra);
    char *letrasDescobertas = malloc(tamanhoPalavra * sizeof(char)); // Aloca dinamicamente

    if (letrasDescobertas == NULL) {
        printf("Erro de alocacao de memoria para letras descobertas.\n");
        return 0;
    }

    for (int i = 0; i < tamanhoPalavra; i++) {
        letrasDescobertas[i] = '_';
    }

    int tentativasRestantes = MAX_TENTATIVAS;

    while (tentativasRestantes > 0) {
        printf("\nFase: ");
        mostrarPalavra(letrasDescobertas, tamanhoPalavra);
        printf("Tentativas restantes: %d\n", tentativasRestantes);

        printf("Digite uma letra: ");
        
        char letra;
        scanf(" %c", &letra);
        letra = tolower(letra);

        if (tentarLetra(letra, palavra, letrasDescobertas, tamanhoPalavra)) {
            printf("Voce acertou a letra '%c'!\n", letra);
        } else {
            printf("A letra '%c' nao esta na palavra.\n", letra);
            tentativasRestantes--;
        }

        if (palavraCompleta(letrasDescobertas, tamanhoPalavra)) {
            printf("\nParabens! Voce adivinhou a palavra: %s\n", palavra);
            jogador->placar += 20; // 20 pontos por fase
            free(letrasDescobertas);
            return 1;
        }
    }

    printf("\nVoce perdeu! A palavra era: %s\n", palavra);
    free(letrasDescobertas);
    return 0;
}

// Função principal do jogo
void iniciarJogo() {
    Jogador jogador;
    printf("Digite seu nome: ");
    scanf("%s", jogador.nome);
    jogador.placar = 0;

    char **palavras;
    int numPalavrasCarregadas = carregarPalavras(&palavras);
    if (numPalavrasCarregadas < NUM_FASES) {
        printf("Erro: Não ha palavras suficientes no arquivo para as fases.\n");
        liberarPalavras(palavras, numPalavrasCarregadas);
        return;
    }

    for (int i = 0; i < NUM_FASES; i++) {
        printf("\n=== Fase %d ===\n", i + 1);
        if (!jogarFase(palavras[i], &jogador)) {
            salvarEstatisticas(jogador);
            liberarPalavras(palavras, numPalavrasCarregadas);
            return;
        }
    }

    printf("\nParabens, %s! Voce completou todas as fases com %d pontos.\n", jogador.nome, jogador.placar);
    salvarEstatisticas(jogador);
    liberarPalavras(palavras, numPalavrasCarregadas);
}

// Função para exibir o menu principal
void exibirMenu() {
    int opcao;

    while (1) {
        printf("\n=== Menu Principal ===\n");
        printf("1. Iniciar Jogo\n");
        printf("2. Estatisticas\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                iniciarJogo();
                break;
            case 2: {
                Jogador jogadorAtual;
                printf("Digite seu nome para ver suas estatisticas: ");
                scanf("%s", jogadorAtual.nome);
                mostrarEstatisticas(jogadorAtual);
                break;
            }
            case 0:
                printf("Saindo do jogo...\n");
                return;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    }
}

int main() {
    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios
    exibirMenu();
    return 0;
}
