#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- Definições Globais ---
#define TAM_HASH 31 // Tamanho primo para melhor distribuição

// --- 1. ESTRUTURA: Tabela Hash (Banco de Dados de Suspeitos) ---
typedef struct HashNode {
    char pista[50];
    char suspeito[50];
    struct HashNode* prox; // Encadeamento para colisões
} HashNode;

HashNode* tabelaSuspeitos[TAM_HASH];

// --- 2. ESTRUTURA: Árvore de Busca (Caderno de Pistas do Jogador) ---
typedef struct BSTNode {
    char pista[50];
    struct BSTNode *esq, *dir;
} BSTNode;

// --- 3. ESTRUTURA: Árvore Binária (Mapa da Mansão) ---
typedef struct Sala {
    char nome[50];
    char pistaEscondida[50]; // Se vazio, não tem pista
    struct Sala *esq, *dir;
} Sala;

// --- Protótipos ---
// Hash
int funcaoHash(char* chave);
void inicializarHash();
void cadastrarSuspeito(char* pista, char* suspeito);
char* consultarSuspeito(char* pista);

// BST (Pistas)
BSTNode* inserirPista(BSTNode* raiz, char* pista);
void exibirPistasOrdem(BSTNode* raiz);

// Mapa (Salas)
Sala* criarSala(char* nome, char* pista);
Sala* montarMansao();
void explorar(Sala* atual, BSTNode** cadernoPistas);

// Lógica Final
void analisarCulpado(BSTNode* pistasEncontradas);

// --- MAIN ---
int main() {
    // 1. Configuração Inicial
    inicializarHash();
    
    // Alimentando o "Banco de Dados" do jogo (O jogador não vê isso, é a lógica interna)
    cadastrarSuspeito("Faca", "Cozinheira");
    cadastrarSuspeito("Veneno", "Jardineiro");
    cadastrarSuspeito("Relogio", "Mordomo");
    cadastrarSuspeito("Luvas", "Jardineiro");
    cadastrarSuspeito("Castiçal", "Mordomo");
    
    // 2. Montando o Mapa
    Sala* entrada = montarMansao();
    BSTNode* cadernoPistas = NULL; // Inventário do jogador começa vazio

    printf("=== 🕵️ DETECTIVE QUEST: O MISTERIO DA MANSÃO ===\n");
    printf("Voce deve explorar a mansao, coletar pistas e descobrir o culpado.\n");
    
    // 3. Loop do Jogo (Navegação)
    explorar(entrada, &cadernoPistas);

    // 4. Conclusão e Dedução
    printf("\n--- FIM DA EXPLORACAO ---\n");
    printf("Pistas coletadas (Ordem Alfabetica):\n");
    if (cadernoPistas == NULL) {
        printf("(Nenhuma pista encontrada... Voce foi um pessimo detetive!)\n");
    } else {
        exibirPistasOrdem(cadernoPistas);
        printf("\n--- ANALISE DE SUSPEITOS ---\n");
        analisarCulpado(cadernoPistas);
    }

    return 0;
}

// --- IMPLEMENTAÇÃO: Tabela Hash --- 

[Image of hash table with chaining]


int funcaoHash(char* chave) {
    int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        soma += toupper(chave[i]); // Soma valores ASCII
    }
    return soma % TAM_HASH;
}

void inicializarHash() {
    for (int i = 0; i < TAM_HASH; i++) tabelaSuspeitos[i] = NULL;
}

void cadastrarSuspeito(char* pista, char* suspeito) {
    int idx = funcaoHash(pista);
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    
    // Inserção no início da lista encadeada (tratamento de colisão)
    novo->prox = tabelaSuspeitos[idx];
    tabelaSuspeitos[idx] = novo;
}

char* consultarSuspeito(char* pista) {
    int idx = funcaoHash(pista);
    HashNode* atual = tabelaSuspeitos[idx];
    
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->prox;
    }
    return "Desconhecido";
}

// --- IMPLEMENTAÇÃO: BST (Pistas) ---

BSTNode* inserirPista(BSTNode* raiz, char* pista) {
    if (raiz == NULL) {
        BSTNode* novo = (BSTNode*) malloc(sizeof(BSTNode));
        strcpy(novo->pista, pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }
    // Se a pista for alfabeticamente menor, vai pra esquerda
    if (strcmp(pista, raiz->pista) < 0) {
        raiz->esq = inserirPista(raiz->esq, pista);
    } else if (strcmp(pista, raiz->pista) > 0) {
        raiz->dir = inserirPista(raiz->dir, pista);
    }
    return raiz;
}

void exibirPistasOrdem(BSTNode* raiz) {
    if (raiz != NULL) {
        exibirPistasOrdem(raiz->esq);
        printf("- %s\n", raiz->pista);
        exibirPistasOrdem(raiz->dir);
    }
}

// --- IMPLEMENTAÇÃO: Mapa (Árvore Binária) --- 

[Image of binary tree data structure]


Sala* criarSala(char* nome, char* pista) {
    Sala* s = (Sala*) malloc(sizeof(Sala));
    strcpy(s->nome, nome);
    strcpy(s->pistaEscondida, pista);
    s->esq = s->dir = NULL;
    return s;
}

Sala* montarMansao() {
    // Criação manual da estrutura da árvore (Mapa)
    Sala* hall = criarSala("Hall de Entrada", "");
    Sala* biblioteca = criarSala("Biblioteca", "Relogio");
    Sala* cozinha = criarSala("Cozinha", "Faca");
    Sala* jardim = criarSala("Jardim de Inverno", "Luvas");
    Sala* porao = criarSala("Porao Escuro", "Veneno");
    Sala* salaJantar = criarSala("Sala de Jantar", "Castiçal");

    // Conexões (Topologia da Mansão)
    hall->esq = biblioteca;
    hall->dir = cozinha;
    
    biblioteca->esq = jardim; // Jardim fica à esquerda da biblioteca
    biblioteca->dir = salaJantar;
    
    cozinha->esq = porao; // Porão fica acessível pela cozinha
    
    return hall;
}

void explorar(Sala* atual, BSTNode** cadernoPistas) {
    char opcao;
    
    while (atual != NULL) {
        printf("\n📍 Voce esta em: [%s]\n", atual->nome);
        
        // Verifica se tem pista na sala
        if (strlen(atual->pistaEscondida) > 0) {
            printf("🔎 Voce encontrou uma pista: '%s'!\n", atual->pistaEscondida);
            *cadernoPistas = inserirPista(*cadernoPistas, atual->pistaEscondida);
            strcpy(atual->pistaEscondida, ""); // Remove a pista para não pegar de novo
        }

        if (atual->esq == NULL && atual->dir == NULL) {
            printf("Este cômodo é um beco sem saída. Fim da linha por aqui.\n");
            break; 
        }

        printf("Para onde deseja ir?\n");
        if (atual->esq) printf("[e] Esquerda: %s\n", atual->esq->nome);
        if (atual->dir) printf("[d] Direita: %s\n", atual->dir->nome);
        printf("[s] Sair da mansao\n");
        printf("Opcao: ");
        scanf(" %c", &opcao);
        opcao = tolower(opcao);

        if (opcao == 'e' && atual->esq) {
            atual = atual->esq;
        } else if (opcao == 'd' && atual->dir) {
            atual = atual->dir;
        } else if (opcao == 's') {
            break;
        } else {
            printf("Caminho invalido!\n");
        }
    }
}

// --- Lógica de Dedução ---

void auxiliarContagem(BSTNode* raiz, int* m, int* j, int* c) {
    if (raiz != NULL) {
        char* suspeito = consultarSuspeito(raiz->pista);
        printf("Evidencia '%s' aponta para: %s\n", raiz->pista, suspeito);
        
        if (strcmp(suspeito, "Mordomo") == 0) (*m)++;
        else if (strcmp(suspeito, "Jardineiro") == 0) (*j)++;
        else if (strcmp(suspeito, "Cozinheira") == 0) (*c)++;
        
        auxiliarContagem(raiz->esq, m, j, c);
        auxiliarContagem(raiz->dir, m, j, c);
    }
}

void analisarCulpado(BSTNode* pistasEncontradas) {
    int mordomo = 0, jardineiro = 0, cozinheira = 0;
    
    // Percorre a árvore de pistas e consulta a Hash para somar votos
    auxiliarContagem(pistasEncontradas, &mordomo, &jardineiro, &cozinheira);
    
    printf("\n--- RESULTADO DA INVESTIGACAO ---\n");
    printf("Pontos de suspeita:\n");
    printf("Mordomo: %d\nJardineiro: %d\nCozinheira: %d\n", mordomo, jardineiro, cozinheira);
    
    if (mordomo > jardineiro && mordomo > cozinheira) printf(">> O CULPADO É O MORDOMO! <<\n");
    else if (jardineiro > mordomo && jardineiro > cozinheira) printf(">> O CULPADO É O JARDINEIRO! <<\n");
    else if (cozinheira > mordomo && cozinheira > jardineiro) printf(">> A CULPADA É A COZINHEIRA! <<\n");
    else printf(">> As evidencias sao inconclusivas (Empate). Explore mais! <<\n");
}

