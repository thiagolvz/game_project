#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_TENTATIVAS 6
#define MAX_NOME 100
#define MAX_ESTATISTICAS 100
#define ARQUIVO_ESTATS "estatisticas.bin"
#define NUM_FASES 5
#define MAX_PALAVRA 100
#define NUM_PROCEDURAL 5

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

            printf("\n=== Estatísticas de %s ===\n", jogador.nome);
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
int carregarPalavras(char palavras[NUM_FASES][MAX_PALAVRA]) {
    for (int i = 0; i < NUM_FASES; i++) {
        char nomeArquivo[MAX_PALAVRA];
        snprintf(nomeArquivo, sizeof(nomeArquivo), "fase%d.txt", i + 1); // Cria o nome do arquivo (fase1.txt, fase2.txt, etc.)

        FILE *file = fopen(nomeArquivo, "r");
        if (!file) {
            printf("Erro: Não foi possível abrir o arquivo %s.\n", nomeArquivo);
            return 0; // Retorna 0 se algum arquivo não puder ser aberto
        }

        if (fscanf(file, "%s", palavras[i]) != 1) {
            printf("Erro: Não foi possível carregar a palavra de %s.\n", nomeArquivo);
            fclose(file);
            return 0; // Retorna 0 se não conseguir ler uma palavra
        }

        fclose(file);
    }
    return NUM_FASES; // Retorna o número de fases carregadas com sucesso
}


// Função para gerar palavras aleatórias para as fases procedurais
void gerarPalavraAleatoria(char palavra[]) {
    const char *opcoes[] = {"banana", "abacaxi", "programa", "computador", "cachorro", 
                            "gato", "jogador", "ciencia", "musica", "desafio"};
    int totalOpcoes = sizeof(opcoes) / sizeof(opcoes[0]);
    strcpy(palavra, opcoes[rand() % totalOpcoes]);
}

// Função para executar uma fase do jogo
int jogarFase(char palavra[], int tempoLimite, Jogador *jogador) {
    int tamanhoPalavra = strlen(palavra);
    char letrasDescobertas[tamanhoPalavra];

    for (int i = 0; i < tamanhoPalavra; i++) {
        letrasDescobertas[i] = '_';
    }

    int tentativasRestantes = MAX_TENTATIVAS;
    time_t tempoInicio = time(NULL);

    while (tentativasRestantes > 0) {
        time_t tempoDecorrido = time(NULL) - tempoInicio;
        if (tempoLimite > 0 && tempoDecorrido > tempoLimite) {
            printf("\nTempo esgotado! Você perdeu esta fase.\n");
            return 0; // Jogador perdeu a fase
        }

        printf("\nFase: ");
        mostrarPalavra(letrasDescobertas, tamanhoPalavra);
        printf("Tentativas restantes: %d\n", tentativasRestantes);
        if (tempoLimite > 0) {
            printf("Tempo restante: %ld segundos\n", tempoLimite - tempoDecorrido);
        }
        printf("Digite uma letra: ");
        
        char letra;
        scanf(" %c", &letra);
        letra = tolower(letra);

        if (tentarLetra(letra, palavra, letrasDescobertas, tamanhoPalavra)) {
            printf("Você acertou a letra '%c'!\n", letra);
        } else {
            printf("A letra '%c' não está na palavra.\n", letra);
            tentativasRestantes--;
        }

        if (palavraCompleta(letrasDescobertas, tamanhoPalavra)) {
            printf("\nParabéns! Você adivinhou a palavra: %s\n", palavra);
            jogador->placar += 20; // 20 pontos por fase
            return 1; // Jogador venceu a fase
        }
    }

    printf("\nVocê perdeu! A palavra era: %s\n", palavra);
    return 0; // Jogador perdeu a fase
}

// Função principal do jogo
void iniciarJogo() {
    Jogador jogador;
    printf("Digite seu nome: ");
    scanf("%s", jogador.nome);
    jogador.placar = 0;

    char palavras[NUM_FASES][MAX_PALAVRA];
    int numPalavrasCarregadas = carregarPalavras(palavras);
    if (numPalavrasCarregadas < NUM_FASES) {
        printf("Erro: Não há palavras suficientes no arquivo para as fases.\n");
        return;
    }

    for (int i = 0; i < NUM_FASES; i++) {
        printf("\n=== Fase %d ===\n", i + 1);
        if (!jogarFase(palavras[i], 0, &jogador)) {
            salvarEstatisticas(jogador);
            return; // Jogador perdeu
        }
    }

    for (int i = 0; i < NUM_PROCEDURAL; i++) {
        printf("\n=== Fase Procedural %d ===\n", i + 1);
        char palavra[MAX_PALAVRA];
        gerarPalavraAleatoria(palavra);
        if (!jogarFase(palavra, 30, &jogador)) { // Tempo limite de 30 segundos
            salvarEstatisticas(jogador);
            return; // Jogador perdeu
        }
    }

    printf("\nParabéns, %s! Você completou todas as fases com %d pontos.\n", jogador.nome, jogador.placar);
    salvarEstatisticas(jogador);
}

// Função para exibir o menu principal
void exibirMenu() {
    int opcao;

    while (1) {
        printf("\n=== Menu Principal ===\n");
        printf("1. Iniciar Jogo\n");
        printf("2. Estatísticas\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                iniciarJogo();
                break;
            case 2: {
                Jogador jogadorAtual;
                printf("Digite seu nome para ver suas estatísticas: ");
                scanf("%s", jogadorAtual.nome);
                mostrarEstatisticas(jogadorAtual);
                break;
            }
            case 0:
                printf("Saindo do jogo...\n");
                return;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    }
}

int main() {
    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios
    exibirMenu();
    return 0;
}
