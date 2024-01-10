#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct{
    char nome[50];
    int pontuacao;
} ranking_t;

void escrever_ranking(ranking_t ranking[5]) {
    FILE *arquivo;
    arquivo = fopen("ranking.bin", "wb");
    if (arquivo != NULL) {
        fwrite(ranking, sizeof(ranking_t), 5, arquivo);
        fclose(arquivo);
    }
}

void ler_ranking(ranking_t ranking[5]) {
    FILE *arquivo;
    arquivo = fopen("ranking.bin", "rb");
    if (arquivo != NULL) {
        fread(ranking, sizeof(ranking_t), 5, arquivo);
        fclose(arquivo);
    }
}

void escrever_ranking_txt(ranking_t ranking[5]) {
    FILE *arquivo;
    arquivo = fopen("ranking.txt", "wb");
    if (arquivo != NULL) {
        for (int i =0; i<5; i++){
            fprintf(arquivo, "%s: %d", ranking[i].nome, ranking[i].pontuacao);
        }

        fclose(arquivo);
    }
}



int main(void){
    ranking_t ranking[5] = {
        {"Fulano", 0},
        {"Ciclano", 0},
        {"Beltrano", 0},
        {"Altrano", 0},
        {"Luiz",0},
    };
    escrever_ranking(ranking);
    escrever_ranking_txt(ranking);


    printf("%s: %d", ranking[0].nome, ranking[0].pontuacao);

}
