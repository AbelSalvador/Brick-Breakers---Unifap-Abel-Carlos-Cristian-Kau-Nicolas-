#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") //pra funcao mcistring q faz a musica loopar

#define LARGURA 40
#define ALTURA 20
#define PADDLE_L 7
#define PADDLE_A 1

struct particula {
    int x, y, dx, dy;
};

struct raquete {
    int x, l, a;
};

int vidas = 3;          // O jogador começa com 3 vidas
int pontuacao = 0;      // Rastreia a pontuação do jogador
int velocidade_bola = 100; // Velocidade inicial da bola (atraso em ms)

void tocarMusicaDeFundo() {
    mciSendString("open \"sound1.wav\" type mpegvideo alias musica", NULL, 0, NULL);
    mciSendString("play musica repeat", NULL, 0, NULL);  // Toca em loop
}

void redefinir_bola(struct particula *pt) {
    pt->x = LARGURA / 2;
    pt->y = ALTURA / 2;
    pt->dx = 1;
    pt->dy = -1;
}

void setCor(int corTexto, int corFundo) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (corFundo << 4) | corTexto);
}

void tela_inicial() {
    system("cls");

    setCor(10, 8);
    printf("\n\n");
    printf("========================================\n");
    printf("            BRICK BREAKERS              \n");
    printf("                                        \n");
    printf(" Pressione qualquer tecla para iniciar  \n");
    printf("========================================\n");
    
    _getch();
    setCor(2, 0);
}

void atualizar_particula(struct particula *pt) {
    pt->x += pt->dx;
    pt->y += pt->dy;

    if (pt->x <= 1 || pt->x >= LARGURA - 2) pt->dx *= -1;
    if (pt->y <= 1) pt->dy *= -1;
}

int colisao_raquete(struct raquete *pd, struct particula *pt) {
    return pt->y >= ALTURA - pd->a - 1 && pt->y <= ALTURA - pd->a && pt->x >= pd->x && pt->x < pd->x + pd->l;
}

void reposicionar_cursor(int x, int y) {
    COORD posicao = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), posicao);
}

void renderizar_jogo(struct particula *pt, struct raquete *pd, int blocos[ALTURA][LARGURA]) {
    printf("Pontuacao: %d  Vidas: %d\n\n", pontuacao, vidas);

    for (int y = 0; y < ALTURA; y++) {
        for (int x = 0; x < LARGURA; x++) {
            int desenhado = 0;

            if (y == ALTURA - pd->a - 1 && x >= pd->x && x < pd->x + pd->l) {
                printf("=");
                desenhado = 1;
            }

            if (!desenhado && y == pt->y && x == pt->x) {
                printf("O");
                desenhado = 1;
            }

            if (!desenhado && blocos[y][x] == 1) {
                printf("#");
                desenhado = 1;
            }

            if (!desenhado) {
                if (y == 0) {
                    printf("=");
                } else if (y == ALTURA - 1 || x == 0 || x == LARGURA - 1) {
                    printf("*");
                } else {
                    printf(" ");
                }
            }
        }
        printf("\n");
    }
}

int main() {
    tocarMusicaDeFundo();  
    tela_inicial();

    struct raquete pd = {LARGURA / 2 - PADDLE_L / 2, PADDLE_L, PADDLE_A};
    struct particula pt;
    redefinir_bola(&pt);
    int blocos[ALTURA][LARGURA] = {0};

    for (int i = 2; i < LARGURA - 2; i += 4) {
        for (int j = 1; j < ALTURA / 2; j++) {
            blocos[j][i] = 1;
        }
    }

    int rodando = 1;
    while (rodando) {
        reposicionar_cursor(0, 0);

        // Movimenta a raquete com as teclas
        if (_kbhit()) {
            char tecla = _getch();
            if (tecla == 'a' && pd.x > 1) pd.x -= 2;
            if (tecla == 'd' && pd.x < LARGURA - pd.l - 1) pd.x += 2;
            if (tecla == 'q') rodando = 0; // Sair do jogo
        }

        atualizar_particula(&pt);

        if (colisao_raquete(&pd, &pt)) {
            pt.dy *= -1;

            int posRelativa = pt.x - pd.x;
            if (posRelativa < pd.l / 3) pt.dx = -1;
            else if (posRelativa >= 2 * pd.l / 3) pt.dx = 1;
            else pt.dx = (pt.dx == 1) ? -1 : 1;

            pontuacao += 10;
            PlaySound(TEXT("sound3.wav"), NULL, SND_ASYNC);
        }

        // Corrige a colisão com blocos para melhorar a detecção
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int bx = pt.x + i;
                int by = pt.y + j;
                if (by >= 0 && by < ALTURA / 2 && bx > 0 && bx < LARGURA - 1 && blocos[by][bx] == 1) {
                    blocos[by][bx] = 0;
                    pt.dy *= -1;
                    pontuacao += 50;
                    PlaySound(TEXT("sound2.wav"), NULL, SND_ASYNC);
                    break;
                }
            }
        }

        if (pt.y >= ALTURA - 1) {
            vidas--;
            if (vidas > 0) {
                redefinir_bola(&pt);
            } else {
                mciSendString("stop musica", NULL, 0, NULL); 
                PlaySound(TEXT("sound4.wav"), NULL, SND_ASYNC);

                system("cls");
                setCor(4, 0);
                printf("\n\n");
                printf("========================================\n");
                printf("        PERDEU IRMAO                    \n");
                printf("                                        \n");
                printf("========================================\n");
                printf("Pontuacao Final: %d\n", pontuacao);
                system("pause");
                rodando = 0;
            }
        }

        renderizar_jogo(&pt, &pd, blocos);
        Sleep(velocidade_bola);
    }

    return 0;  
}
