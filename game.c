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
            return 0;  // Ainda há letras não descobertas
        }
    }
    return 1;  // Palavra completa
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

    // Exibir as estatísticas apenas para o jogador atual
    while (fread(&jogador, sizeof(Jogador), 1, file)) {
        if (strcmp(jogador.nome, jogadorAtual.nome) == 0) {
            // Se o nome do jogador atual coincidir com o nome salvo
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
        jogador.data = time(NULL);  // Adiciona a data e hora do jogo
        fwrite(&jogador, sizeof(Jogador), 1, file);
        fclose(file);
    }
}

// Função para iniciar o jogo
void iniciarJogo() {
    Jogador jogador;
    printf("Digite seu nome: ");
    scanf("%s", jogador.nome);

    jogador.placar = 0;

    // Lista de tempos para as fases
    int tempos[NUM_FASES] = {90, 60, 30, 30, 30};  // Fase 3: 90s, Fase 4: 60s, Fase 5: 30s

    // Loop para as 5 fases
    for (int fase = 0; fase < NUM_FASES; fase++) {
        // O jogador pode digitar a palavra para cada fase
        char palavra[MAX_NOME];
        printf("\nDigite a palavra para a Fase %d (a palavra será oculta): ", fase + 1);
        scanf("%s", palavra);

        int tamanhoPalavra = strlen(palavra);

        // Criar um array para armazenar as letras descobertas (inicialmente com '_')
        char letrasDescobertas[tamanhoPalavra];
        for (int i = 0; i < tamanhoPalavra; i++) {
            letrasDescobertas[i] = '_';
        }

        int tentativasRestantes = MAX_TENTATIVAS;

        // Informar ao jogador sobre o limite de tempo
        if (fase >= 2) {
            printf("\nA partir desta fase, você tem %d segundos para adivinhar a palavra.\n", tempos[fase]);
        }

        // Verificar se o limite de tempo deve ser ativado
        time_t tempoInicio = 0, tempoDecorrido;

        // Limpa o buffer para a próxima entrada
        getchar();

        // Loop do jogo para a fase
        while (tentativasRestantes > 0) {
            // A partir da fase 3, começa a contar o tempo
            if (fase >= 2) {
                if (tempoInicio == 0) {
                    tempoInicio = time(NULL); // Inicia o tempo apenas na primeira tentativa
                }

                tempoDecorrido = time(NULL) - tempoInicio;

                // Verifica se o tempo passou
                if (tempoDecorrido > tempos[fase]) {
                    printf("\nTempo esgotado! Você perdeu a fase.\n");
                    salvarEstatisticas(jogador);
                    return;
                }
            }

            printf("\nFase %d: ", fase + 1);
            mostrarPalavra(letrasDescobertas, tamanhoPalavra);
            printf("Tentativas restantes: %d\n", tentativasRestantes);
            if (fase >= 2) {
                printf("Tempo restante: %ld segundos\n", tempos[fase] - tempoDecorrido);
            }
            printf("Digite uma letra: ");
            
            char letra;
            scanf(" %c", &letra); // O espaço antes de %c é para consumir o caractere de nova linha

            // Converte a letra para minúscula para comparar de forma consistente
            letra = tolower(letra);

            // Verificar se a letra foi correta
            if (tentarLetra(letra, palavra, letrasDescobertas, tamanhoPalavra)) {
                printf("Você acertou a letra '%c'!\n", letra);
            } else {
                tentativasRestantes--;
                printf("A letra '%c' não está na palavra.\n", letra);
            }

            // Verificar se o jogador adivinhou toda a palavra
            if (palavraCompleta(letrasDescobertas, tamanhoPalavra)) {
                printf("\nParabéns, %s! Você adivinhou a palavra: %s\n", jogador.nome, palavra);
                jogador.placar += 20;  // Exemplo de placar (20 pontos por fase)
                break;
            }
        }

        // Se o jogador não adivinhou a palavra, ele perdeu
        if (tentativasRestantes == 0) {
            printf("\nVocê perdeu na Fase %d! A palavra era: %s\n", fase + 1, palavra);
            salvarEstatisticas(jogador);
            return;
        }
    }

    // Se o jogador completou todas as fases
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
    exibirMenu();
    return 0;
}
