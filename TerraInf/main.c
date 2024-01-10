#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
const int screenWidth = 1200;
const int screenHeight = 800;
typedef struct {
    char nome[20];
    int posx;
    int posy;
    int pontuacao;
    int vidas;
    int energia;
    int escadas;
    char ultimo_item_mineirado;
    int fase;
}jogador_t;

typedef struct{
    char nome[50];
    int pontuacao;
}ranking_t;

bool IsAnyKeyPressed()
{
    bool keyPressed = false;
    int key = GetKeyPressed();

    if ((key >= 32) && (key <= 126)) keyPressed = true;

    return keyPressed;
}

size_t strlcat(char *dst, const char *src, size_t size) {
    size_t dstlen = strlen(dst);
    size_t srclen = strlen(src);

    if (dstlen + 1 < size) {
        strncat(dst, src, size - dstlen - 1);
        dst[size - 1] = '\0';
    }

    return dstlen + srclen;
}

void escrever_ranking(ranking_t ranking[5]) {
    FILE *arquivo;
    arquivo = fopen("ranking.bin", "wb");
    if (arquivo != NULL) {
        fwrite(ranking, sizeof(ranking_t), 5, arquivo);
        fclose(arquivo);
    }
}

void escrever_ranking_txt(ranking_t ranking[5]) {
    FILE *arquivo;
    arquivo = fopen("ranking.txt", "wb");
    if (arquivo != NULL) {
        for (int i =0; i<5; i++){
            fprintf(arquivo, "%s: %d \n", ranking[i].nome, ranking[i].pontuacao);
        }

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


int jogo(void);
void Ranking(void);



void ler_fase(int fase, char mapa[20][30]){
    char str[20];
    sprintf(str,"%d", fase);
    char arquivo_fase[50] = "nivel";
    strlcat(arquivo_fase,str, sizeof(arquivo_fase));
    strlcat(arquivo_fase, ".txt", sizeof(arquivo_fase));

    FILE *nivel = fopen(arquivo_fase, "r");
    if (nivel == NULL){
        printf("Erro ao abrir arquivo do nivel");
    }
    for (int i = 0; i <20; i++){//Contrói a matriz baseado no nivel_template.txt
        for (int j = 0; j<30; j++){
            fscanf(nivel, " %c", &mapa[i][j]);
        }   
    }
    // coloca o J(ogador) na posição inicial dele
    fclose(nivel);
}

int Menu(void){
		int opcao = 1;
        InitWindow(screenWidth, screenHeight, "Menu");
        Texture2D fundo = LoadTexture("./imagens/capa.png");
        InitAudioDevice();
        Music music = LoadMusicStream("./resources/bo2.mp3");
        PlayMusicStream(music);
        float timePlayed = 0.0f; 
        SetMasterVolume(0.5);
        while(!(IsKeyPressed(KEY_ENTER))){
            UpdateMusicStream(music);   
            timePlayed = GetMusicTimePlayed(music)/GetMusicTimeLength(music);
            if (timePlayed >=1.0f){
                StopMusicStream(music);
                PlayMusicStream(music);
            }
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(fundo, 0, 0, RAYWHITE);
            DrawText("Novo Jogo", 480, 400, 40, WHITE);
            DrawText("Ranking de Pontos", 420, 470, 40, WHITE);
            DrawText("Sair", 550, 540, 40, WHITE);

            if (IsKeyPressed(KEY_DOWN)){
                opcao++;
            }
            else if (IsKeyPressed(KEY_UP)){
                opcao--;
            }
            if (opcao >3){
                opcao = 1;
            }
            else if (opcao <1){
                opcao = 3;
            }
            switch(opcao){
                case 1:
                    DrawRectangleLines(470,400,230,40,WHITE);
                    break;
                case 2:
                    DrawRectangleLines(410, 470, 400, 40,WHITE);
                    break;
                case 3:
                    DrawRectangleLines(540, 540, 100,40,WHITE);
                    break;
            }
            EndDrawing();
        }
        UnloadMusicStream(music);
        CloseAudioDevice();
        switch(opcao){
            case 1:
                CloseWindow();
                jogo();
                // colocar ranking
                break;
            case 2:
                CloseWindow();
                Ranking();
                break;
            case 3:
                CloseWindow();
                return 0;
                
        }
    return 0;
}

int jogo(void){
    //Variáveis:
    jogador_t J;
    J.posx= 1;
    J.posy = 2;
    J.pontuacao = 0;
    J.vidas = 3;
    J.energia = 100;
    J.escadas = 20;
    J.ultimo_item_mineirado = ' ';
    J.fase = 1;
    char mapa[20][30];
    int GameOver = 0;
    int mineirado = 0;
    int queda = 0;

    char pontuacao_str[50];
    char energia_str[50];
    char vidas_str[50];
    char escadas_str[50];
    char fase_str[50];
    char posx_str[50];
    char posy_str[50];
    
    InitWindow(screenWidth, screenHeight, "Jogo");  
    int agulha;
    int size_pontuacao = MeasureText("Pontuação:", 20);
    int size_energia = MeasureText("Energia:", 20);
    int size_vidas = MeasureText("Vidas:", 20);
    int size_escadas = MeasureText("Escadas:", 20);
    int size_fase = MeasureText("Fase:", 20);
    int size_item = MeasureText("Último item:", 20);
    Texture2D borda = LoadTexture("./imagens/borda.png");
    Texture2D fundo_terra = LoadTexture("./imagens/fundo_terra.png");
    Texture2D minerio = LoadTexture("./imagens/minerio.png");
    Texture2D terra = LoadTexture("./imagens/terra.png");
    Texture2D jogador = LoadTexture("./imagens/jogador.png");
    Texture2D escada = LoadTexture("./imagens/escada.png");
    Texture2D jogador_escada = LoadTexture("./imagens/jogador_escada.png");
    Texture2D jogador_picareta = LoadTexture("./imagens/jogador_picareta.png");
    Texture2D jogador_picareta_escada = LoadTexture("./imagens/jogador_picareta_escada.png");
    Texture2D preto = LoadTexture("./imagens/preto.png");

    //começo
    ler_fase(J.fase, mapa);
    mapa[J.posy][J.posx]= 'J';
    InitAudioDevice();
    Music music = LoadMusicStream("./resources/kahoot.mp3");
    PlayMusicStream(music);
    float timePlayed = 0.0f; 
    SetMasterVolume(0.5);
    while(GameOver==0){// loop do jogo
        UpdateMusicStream(music);
        timePlayed = GetMusicTimePlayed(music)/GetMusicTimeLength(music);
        if (timePlayed >=1.0f){
            StopMusicStream(music);
            PlayMusicStream(music);
        }
        BeginDrawing();
        for (int i =0; i<20; i++){// gráfico da matriz
            for (int j = 0; j<30; j++){
                switch(mapa[i][j]){
                    case 'B':  
                        DrawTexture(borda, j*40,i*40,WHITE);
                        break;
                    case 'f':
                        DrawTexture(fundo_terra, j*40,i*40,WHITE);
                        break;
                    case ' ':
                        DrawTexture(fundo_terra, j*40,i*40,WHITE);
                        break;
                    case 'X':
                        DrawTexture(terra, j*40,i*40,WHITE);
                        break;
                    case 'J':
                        DrawTexture(jogador, j*40,i*40,WHITE);
                        J.posx = j;
                        J.posy = i;
                        break;
                        case 'H':
                        DrawTexture(escada, j*40,i*40,WHITE);
                        break;
                    case 'e':
                        DrawTexture(jogador_escada, j*40,i*40,WHITE);
                        break;
                    default:
                        DrawTexture(minerio, j*40,i*40,WHITE);
                        break;
                }
            }
        }
        //Letreiro da parte de cima:
        DrawRectangle(0,0,screenWidth, 40, BLACK);
        sprintf(pontuacao_str, "%d", J.pontuacao);
        sprintf(energia_str, "%d", J.energia);
        sprintf(vidas_str, "%d", J.vidas);
        sprintf(escadas_str, "%d", J.escadas);
        sprintf(fase_str, "%d", J.fase);
        sprintf(posx_str, "%d", J.posx);
        sprintf(posy_str, "%d", J.posy);
        
        DrawText("Pontuação:",0,10,20,WHITE);
        DrawText(pontuacao_str,size_pontuacao+20,10,20,WHITE);
        agulha = size_pontuacao + 100;

        DrawText("Energia:",agulha + 10,10,20,WHITE);
        DrawText(energia_str,agulha+size_energia+20,10,20,WHITE);
        agulha += 100 + size_energia;

        DrawText("Vidas:",agulha+ 10,10,20,WHITE);
        DrawText(vidas_str,agulha+size_vidas+20,10,20,WHITE);
        agulha += 100 + size_vidas;

        DrawText("Escadas:",agulha+10,10,20,WHITE);
        DrawText(escadas_str,agulha+size_escadas+20,10,20,WHITE);
        agulha += 100 + size_escadas;

        DrawText("Fase:",agulha+20,10,20,WHITE);
        DrawText(fase_str,agulha+size_fase+20,10,20,WHITE);
        agulha += 100 + size_fase;

        DrawText("Item:",agulha + 20,10,20,WHITE);
        switch (J.ultimo_item_mineirado)
        {
            case 'T':
                DrawText("Titânio",agulha+size_item, 10,20,WHITE);
                break;
            case 'G':
                DrawText("Ouro",agulha+size_item, 10,20,WHITE);
                break;
            case 'S':
                DrawText("Prata",agulha+size_item, 10,20,WHITE);
                break;
            case 'C':
                DrawText("Césio",agulha+size_item, 10,20,WHITE);
                break;
            case 'U':
                DrawText("Urânio",agulha+size_item, 10,20,WHITE);
                break;
            case 'X':
                DrawText("Terra",agulha+size_item, 10,20,WHITE);
                break;
        }
        agulha += 80 + size_item;
        DrawText("X:", agulha+20, 10, 20, WHITE );
        DrawText(posx_str, agulha+40, 10 ,20 ,WHITE);
        agulha += 60;
        DrawText("Y:", agulha+20, 10, 20, WHITE );
        DrawText(posy_str, agulha+40, 10 ,20 ,WHITE);

        //interações com o teclado e movimentação
        if (IsKeyDown(KEY_SPACE)){// ativa o modo mineiração e desenha o jogador com picareta
            if(mapa[J.posy][J.posx] == 'e') 
                DrawTexture(jogador_picareta_escada, J.posx*40, J.posy*40, WHITE );
            else    
                DrawTexture(jogador_picareta, J.posx*40, J.posy*40, WHITE );
        }

        if (IsKeyPressed(KEY_D)){
            // mineiração
            if ((IsKeyDown(KEY_SPACE)) && (mapa[J.posy][J.posx+1]!='B') && (mapa[J.posy][J.posx+1] != 'f') && (mapa[J.posy][J.posx+1] != 'H')){
                switch(mapa[J.posy][J.posx+1]){
                    case 'G':
                        J.ultimo_item_mineirado = 'G';
                        break;
                    case 'T':
                        J.ultimo_item_mineirado = 'T';
                        break;
                    case 'S':
                        J.ultimo_item_mineirado = 'S';
                        break;
                    case 'U':
                        J.ultimo_item_mineirado = 'U';
                        break;
                    case 'C':
                        J.ultimo_item_mineirado = 'C';
                        break;
                    case 'X':
                        J.ultimo_item_mineirado = 'X';
                        break;
                }
                mapa[J.posy][J.posx+1] = 'f';
                mineirado = 1;
            }
            //movimentação normal
            else if ((mapa[J.posy][J.posx+1] != 'B') && ((mapa[J.posy][J.posx+1] == 'f') || (mapa[J.posy][J.posx+1] == 'H'))){
                if (mapa[J.posy][J.posx] == 'e') mapa[J.posy][J.posx] = 'H';
                else mapa[J.posy][J.posx] = 'f';
                    
                if (mapa[J.posy][J.posx+1] == 'H') mapa[J.posy][J.posx+1] = 'e';
                else mapa[J.posy][J.posx+1] = 'J';
                    
                J.posx++;
            }
            //checagem de queda
            while( mapa[J.posy+1][J.posx] == 'f'){
                    queda++;                    
                    mapa[J.posy+1][J.posx] = 'J';
                    if(mapa[J.posy][J.posx] == 'e') mapa[J.posy][J.posx] = 'H';
                    else mapa[J.posy][J.posx] = 'f';
                    J.posy++;
                }
        }

        if (IsKeyPressed(KEY_A)){
            //mineiração
            if ((IsKeyDown(KEY_SPACE)) && (mapa[J.posy][J.posx-1]!='B') && (mapa[J.posy][J.posx-1] != 'f') && (mapa[J.posy][J.posx-1] != 'H')){
                switch(mapa[J.posy][J.posx-1]){
                    case 'G':
                        J.ultimo_item_mineirado = 'G';
                        break;
                    case 'T':
                        J.ultimo_item_mineirado = 'T';
                        break;
                    case 'S':
                        J.ultimo_item_mineirado = 'S';
                        break;
                    case 'U':
                        J.ultimo_item_mineirado = 'U';
                        break;
                    case 'C':
                        J.ultimo_item_mineirado = 'C';
                        break;
                    case 'X':
                        J.ultimo_item_mineirado = 'X';
                        break;
                }
                mapa[J.posy][J.posx-1] = 'f'; 
                mineirado = 1;
            }
            //movimentação
            else if ((mapa[J.posy][J.posx-1] != 'B') && ((mapa[J.posy][J.posx-1] == 'f') || (mapa[J.posy][J.posx-1] == 'H'))){
                if (mapa[J.posy][J.posx] == 'e') mapa[J.posy][J.posx] = 'H';
                else mapa[J.posy][J.posx] = 'f';

                if (mapa[J.posy][J.posx-1] == 'H') mapa[J.posy][J.posx-1] = 'e';
                else mapa[J.posy][J.posx-1] = 'J';
                    
                J.posx--;
            }
            //queda
            while( mapa[J.posy+1][J.posx] == 'f'){
                    queda++;
                    //checagens se tem escada
                    mapa[J.posy+1][J.posx] = 'J';
                    if(mapa[J.posy][J.posx] == 'e') mapa[J.posy][J.posx] = 'H';
                    else mapa[J.posy][J.posx] = 'f';
                    J.posy++;
                }
        }

        if (IsKeyPressed(KEY_W)) {
            //mineiração
            if ((IsKeyDown(KEY_SPACE)) && (mapa[J.posy-1][J.posx]!='B') && (mapa[J.posy-1][J.posx] != 'f') && (mapa[J.posy-1][J.posx] != 'H')){
                switch(mapa[J.posy-1][J.posx]){
                    case 'G':
                        J.ultimo_item_mineirado = 'G';
                        break;
                    case 'T':
                        J.ultimo_item_mineirado = 'T';
                        break;
                    case 'S':
                        J.ultimo_item_mineirado = 'S';
                        break;
                    case 'U':
                        J.ultimo_item_mineirado = 'U';
                        break;
                    case 'C':
                        J.ultimo_item_mineirado = 'C';
                        break;
                    case 'X':
                        J.ultimo_item_mineirado = 'X';
                        break;
                }
                mapa[J.posy-1][J.posx] = 'f';
                mineirado = 1;
            }
            //movimentação
            else if ((mapa[J.posy-1][J.posx] != 'B') && (mapa[J.posy][J.posx] == 'e') && ((mapa[J.posy-1][J.posx] == 'f') || (mapa[J.posy-1][J.posx] == 'H'))){//só se move se tiver escada nele
                mapa[J.posy][J.posx] = 'H';
                if (mapa[J.posy-1][J.posx] == 'H') mapa[J.posy-1][J.posx] = 'e';
                else if(mapa[J.posy-1][J.posx] == 'f') mapa[J.posy-1][J.posx] = 'J';
                
                J.posy--;
            }
        }

        if (IsKeyPressed(KEY_S)) {
            //mineiração
            if ((IsKeyDown(KEY_SPACE)) && (mapa[J.posy+1][J.posx]!='B') && (mapa[J.posy+1][J.posx] != 'f') && (mapa[J.posy+1][J.posx] != 'H')){
                switch(mapa[J.posy+1][J.posx]){
                    case 'G':
                        J.ultimo_item_mineirado = 'G';
                        break;
                    case 'T':
                        J.ultimo_item_mineirado = 'T';
                        break;
                    case 'S':
                        J.ultimo_item_mineirado = 'S';
                        break;
                    case 'U':
                        J.ultimo_item_mineirado = 'U';
                        break;
                    case 'C':
                        J.ultimo_item_mineirado = 'C';
                        break;
                    case 'X':
                        J.ultimo_item_mineirado = 'X';
                        break;
                }
                mapa[J.posy+1][J.posx] = 'f';
                mineirado = 1;
            }
            //movimentação
            else if ((mapa[J.posy+1][J.posx] != 'B') && (mapa[J.posy+1][J.posx] == 'H')){
                mapa[J.posy+1][J.posx] = 'e';
                if (mapa[J.posy][J.posx] == 'e') mapa[J.posy][J.posx] = 'H';
                else if (mapa[J.posy][J.posx] == 'J') mapa[J.posy][J.posx] = 'f';  
                
                J.posy++;
            }
            //queda
            while( mapa[J.posy+1][J.posx] == 'f'){
                    queda++;
                    mapa[J.posy+1][J.posx] = 'J';
                    if (mapa[J.posy][J.posx] == 'e') mapa[J.posy][J.posx] = 'H';
                    else mapa[J.posy][J.posx] = 'f';
                    J.posy++;
                }
        }
    
        if (IsKeyPressed(KEY_LEFT_SHIFT)) {//escadas
            if (J.escadas > 0){
                if (mapa[J.posy][J.posx] == 'e');
                else {
                    mapa[J.posy][J.posx] = 'e';
                    J.escadas --;
                } 
            }
        }
        
        EndDrawing();// para a parte gráfica pro backend tomar conta  

        if (IsKeyPressed(KEY_ESCAPE)){//pausar jogo
            int op = 1;
            while (!IsKeyPressed(KEY_ENTER))
            {
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("|Jogo pausado|", (screenWidth/2) - MeasureText("|Jogo pausado|", 60)/2 , screenHeight/5 ,60 , WHITE);
                DrawText("Voltar ao jogo", (screenWidth/2)-MeasureText("Voltar ao jogo",30)/2, screenHeight/3, 30, WHITE);
                DrawText("Sair", (screenWidth/2)-MeasureText("Sair",30)/2, screenHeight/3+80, 30, WHITE);

                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
                    op++;
                }
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
                    op--;
                }
                if (op>2) op =1;
                if (op<1) op =2;
                switch (op)
                {
                case 1:
                    DrawRectangleLines((screenWidth/2)-MeasureText("Voltar ao jogo",30)/2, screenHeight/3, MeasureText("Voltar ao jogo",30), 30, WHITE);
                    break;
                case 2:
                    DrawRectangleLines((screenWidth/2)-MeasureText("Sair",30)/2, screenHeight/3 +80,MeasureText("Sair",30), 30, WHITE);
                    break;
                }
                EndDrawing();
            }
            switch (op)
                {
                case 1:
                    break;
                case 2:
                    CloseWindow();
                    return 0;
                    break;
                }
        }
        //post processing de cada interação
        if (queda>3){
            J.vidas --;
        }
        queda = 0;
    
        if(mineirado == 1){ 
            switch (J.ultimo_item_mineirado)
            {
            case 'T':
                J.energia+=30;
                J.pontuacao += 150;
                break;
            case 'G':
                J.energia+=20;
                J.pontuacao += 100;
                break;
            case 'S':
                J.energia+=10;
                J.pontuacao += 50;
                break;
            case 'C':
                J.energia-=20;
                break;
            case 'U':
                J.energia-=30;
                break;
            case 'X':
                J.energia-=3;
                break;
            }
            mineirado = 0;
        }

        if(J.energia<=20){
            J.vidas--;
            J.energia = 100;
        }

        if (J.vidas<1){// game over
            GameOver =1;
        }         
        //passar de nível
        if (J.pontuacao >= (1000*(pow(2,J.fase-1)))){
            J.fase++;
            ler_fase(J.fase,mapa);
            mapa[2][1]= 'J';
        }
    }
    //post do game over 
    UnloadMusicStream(music);
    CloseAudioDevice();
    Rectangle textBox = { screenWidth/2 - 250, (screenHeight/2)+60, 500, 50 };
    int letterCount = 0;     
    char name[50];
    while(!IsKeyPressed(KEY_ENTER)){
        DrawTexture(preto, 0, 0, CLITERAL(Color){ 255, 255, 255, 128 });
        DrawText("GAME OVER!", (screenWidth/2) - MeasureText("GAME OVER!", 40)/2,(screenHeight/4),40,WHITE);
        DrawText("Pontuação:", (screenWidth/2)- MeasureText("Pontuação:", 40)/2,(screenHeight/4) + 40 ,40,WHITE);
        DrawText(pontuacao_str, (screenWidth/2) - MeasureText(pontuacao_str, 40)/2, (screenHeight/4)+80,40,RED);
        DrawText("Insira seu nome:", (screenWidth/2) - MeasureText("Insira seu nome:", 40)/2, (screenHeight/2), 40, WHITE);
        
        int key = GetCharPressed();
        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (letterCount < 20))
            {
                name[letterCount] = (char)key;
                name[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                letterCount++;
            }
            key = GetCharPressed();  // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            name[letterCount] = '\0';
        }
        DrawRectangleRec(textBox, LIGHTGRAY);
        DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
        DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);
        if (letterCount < 20){}
        else DrawText("Limite de caracteres atingido", (screenWidth/2) - MeasureText("Limite de caracteres atingido", 20)/2, (screenHeight/2)+40, 20, GRAY);
        EndDrawing(); 
    }  
    
    //reescrevendo ranking
    ranking_t ranking[5];
    ler_ranking(ranking);
    ranking_t novo_jogador; 
    ranking_t aux;
    novo_jogador.pontuacao = J.pontuacao;
    strncpy(novo_jogador.nome, name, sizeof(name)); 
    int i = 4;
    while ((novo_jogador.pontuacao > ranking[i].pontuacao) && (i>=0)){
        aux = ranking[i];
        ranking[i] = novo_jogador;
        if (!(i == 4)){
            ranking[i+1] = aux;
        }
        i--;
    }
    
    escrever_ranking(ranking);
    escrever_ranking_txt(ranking);
       

    CloseWindow();
    Ranking();
    return 0;
}

void Ranking(void){
    char temp[50];
    InitWindow(screenWidth, screenHeight, "Ranking");
    ranking_t ranking[5];
    ler_ranking(ranking);

    while (!IsKeyPressed(KEY_ENTER)){
        BeginDrawing();
        DrawText("Melhores pontuações:", (screenWidth/2)- MeasureText("Melhores pontuações:", 40)/2, screenHeight/4, 40, WHITE);
        DrawText("Aperte Enter para voltar ao menu", (screenWidth/2)- MeasureText("Aperte Enter para voltar ao menu", 20)/2, screenHeight-40, 20, WHITE);
        for (int i = 0; i<5; i++){
            sprintf(temp, "%d", ranking[i].pontuacao);
            DrawText(ranking[i].nome, (screenWidth/3)- MeasureText(ranking[i].nome, 40)/2, (screenHeight/3)+40*(i+1), 40, WHITE);
            DrawText(temp, (2*screenWidth/3)- MeasureText(temp, 40)/2, (screenHeight/3)+40*(i+1), 40, WHITE);
        }
        EndDrawing();
    }
    CloseWindow();
    Menu();
}

int main(void)
{	
	Menu();
    return 0;
}