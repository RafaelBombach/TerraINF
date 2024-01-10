#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(){
    char mapa[20][30];
    FILE *nivel;

    nivel = fopen("nivel.txt", "w");
    for(int i = 0; i <20; i++){
        for (int j = 0; j<30; j++){
            if (i==0 || i==19 || j==0 || j==29){ // contrutor de n�vel autom�tico
                mapa[i][j] = 'B';
            }
            else{
                int n = rand()%7;
                switch(n){
                    case 0:
                        mapa[i][j] = 'G' ;
                        break;
                    case 1:
                        mapa[i][j] ='T';
                        break;
                    case 2:
                        mapa[i][j] = 'S';
                        break;
                    case 3:
                        mapa[i][j] = 'U';
                        break;
                    case 4:
                        mapa[i][j] = 'C';
                        break;
                    case 5:
                        mapa[i][j] = 'X';
                        break;
                    case 6:
                        mapa[i][j] = 'f';
                        break;
                    }
            }
            if ( (i==1 || i == 2) && !(j==0 || j==29)){
                mapa[i][j] = 'f';
            }
            fprintf(nivel,"%c ", mapa[i][j]);
            if(j==29){
                fprintf(nivel, "\n");
            }
        }
    }
}
