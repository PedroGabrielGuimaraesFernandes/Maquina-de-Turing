#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATES 50
#define MAX_TRANSITIONS 500
#define MAX_TAPE_LEN 200
#define MAX_WORD_LEN 101
#define MAX_WORDS 100
#define MAX_STEPS 10000

typedef struct {
    int current_state;
    char symbol_read;
    char symbol_write;
    char move;
    int next_state;
} Transition;

typedef struct {
    int num_states;
    Transition transitions[MAX_TRANSITIONS];
    int num_transitions;
    int acceptance_state;
} TuringMachine;

int find_transition(TuringMachine *tm, int state, char symbol, Transition *found) {
    for (int i = 0; i < tm->num_transitions; i++) {
        if (tm->transitions[i].current_state == state && tm->transitions[i].symbol_read == symbol) {
            *found = tm->transitions[i];
            return 1;
        }
    }
    return 0;
}

int process_word(TuringMachine *tm, char *word, char *final_tape) {
    char tape[MAX_TAPE_LEN];
    int tape_len = strlen(word);
    int head_pos = 0;
    int current_state = 0;
    int steps = 0;
    
    strcpy(tape, word);
    
    while (steps < MAX_STEPS) {
        if (head_pos < 0) {
            for (int i = tape_len; i >= 0; i--) {
                tape[i + 1] = tape[i];
            }
            tape[0] = '-';
            head_pos = 0;
            tape_len++;
        } else if (head_pos >= tape_len) {
            tape[tape_len] = '-';
            tape_len++;
            tape[tape_len] = '\0';
        }
        
        Transition trans;
        if (!find_transition(tm, current_state, tape[head_pos], &trans)) {
            strcpy(final_tape, tape);
            return (current_state == tm->acceptance_state);
        }
        
        tape[head_pos] = trans.symbol_write;
        
        if (trans.move == 'D') {
            head_pos++;
        } else if (trans.move == 'E') {
            head_pos--;
        }
        
        current_state = trans.next_state;
        
        if (current_state == tm->acceptance_state) {
            strcpy(final_tape, tape);
            return 1;
        }
        
        steps++;
    }
    
    strcpy(final_tape, tape);
    return 0;
}

int main() {
    FILE *file;
    char filename[100];
    char alphabet[100];
    TuringMachine tm;
    char words[MAX_WORDS][MAX_WORD_LEN];
    int num_words;
    char line[256];
    char final_tape[MAX_TAPE_LEN];
    
    printf("============================================================\n");
    printf("SIMULADOR DE MAQUINA DE TURING\n");
    printf("============================================================\n");
    
    printf("Digite o nome do arquivo de entrada: ");
    scanf("%s", filename);
    
    file = fopen(filename, "r");
    if (!file) {
        printf("Erro: Arquivo '%s' nao encontrado.\n", filename);
        return 1;
    }
    
    fscanf(file, "%s", alphabet);
    
    fscanf(file, "%d", &tm.num_states);
    tm.acceptance_state = tm.num_states - 1;
    
    fscanf(file, "%d", &tm.num_transitions);
    
    for (int i = 0; i < tm.num_transitions; i++) {
        fscanf(file, "%d %c %c %c %d", 
               &tm.transitions[i].current_state,
               &tm.transitions[i].symbol_read,
               &tm.transitions[i].symbol_write,
               &tm.transitions[i].move,
               &tm.transitions[i].next_state);
    }
    
    fscanf(file, "%d", &num_words);
    
    for (int i = 0; i < num_words; i++) {
        fscanf(file, "%s", words[i]);
    }
    
    fclose(file);
    
    printf("\nMaquina carregada com sucesso!\n");
    printf("Estados: %d\n", tm.num_states);
    printf("Estado inicial: 0\n");
    printf("Estado de aceitacao: %d\n", tm.acceptance_state);
    printf("Estado de rejeicao: %d\n", tm.acceptance_state - 1);
    printf("Transicoes definidas: %d\n", tm.num_transitions);
    printf("Palavras a processar: %d\n", num_words);
    printf("\n============================================================\n");
    printf("PROCESSANDO PALAVRAS...\n");
    printf("============================================================\n\n");
    
    for (int i = 0; i < num_words; i++) {
        int accepted = process_word(&tm, words[i], final_tape);
        
        char *result = accepted ? "OK" : "not OK";
        
        for (int j = strlen(final_tape) - 1; j >= 0; j--) {
            if (final_tape[j] == '-') {
                final_tape[j] = '\0';
            } else {
                break;
            }
        }
        
        printf("%d: %s %s\n", i + 1, words[i], result);
        
        if (strlen(final_tape) > 0) {
            printf("   Fita final: %s\n", final_tape);
        } else {
            printf("   Fita final: (fita vazia)\n");
        }
        printf("\n");
    }
    
    printf("============================================================\n");
    printf("FIM DO PROCESSAMENTO\n");
    printf("============================================================\n");
    
    return 0;
}