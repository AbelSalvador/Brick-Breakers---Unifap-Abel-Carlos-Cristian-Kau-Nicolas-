#include <stdlib.h>
#include <stdio.h>
#include <conio.h>  // Espec�fico do Windows
#include <windows.h>
#include <mmsystem.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")  // Link necess�rio para mciSendString

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

int vidas = 3;          // O jogador come�a com 3 vidas
int pontuacao = 0;      // Rastreia a pontua��o do jogador
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

    // Exibe o t�tulo do jogo
    setCor(10, 8);
    printf("\n\n");
    printf("========================================\n");
    printf("            BRICK BREAKERS              \n");
    printf("                                        \n");
    printf(" Pressione qualquer tecla para iniciar  \n");
    printf("========================================\n");
    
    // Aguardar a entrada do usu�rio
    _getch();
    setCor(2, 0); // Resetar para cores padr�o
}

void atualizar_particula(struct particula *pt) {
    pt->x += pt->dx;
    pt->y += pt->dy;

    // Verifica colis�es com as paredes
    if (pt->x <= 1 || pt->x >= LARGURA - 2) pt->dx *= -1; // Ajusta para as paredes laterais
    if (pt->y <= 1) pt->dy *= -1;  // Ajusta para a parede superior
}

int colisao_raquete(struct raquete *pd, struct particula *pt) {
    return pt->y >= ALTURA - pd->a - 1 && pt->y <= ALTURA - pd->a && pt->x >= pd->x && pt->x < pd->x + pd->l;
}

// Fun��o para reposicionar o cursor no console
void reposicionar_cursor(int x, int y) {
    COORD posicao = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), posicao);
}

void renderizar_jogo(struct particula *pt, struct raquete *pd, int blocos[ALTURA][LARGURA]) {
    // Exibir pontua��o e vidas
    printf("Pontuacao: %d  Vidas: %d\n\n", pontuacao, vidas);

    // Desenhar campo de jogo
    for (int y = 0; y < ALTURA; y++) {
        for (int x = 0; x < LARGURA; x++) {
            int desenhado = 0;

            // Desenhar raquete
            if (y == ALTURA - pd->a - 1 && x >= pd->x && x < pd->x + pd->l) {
                printf("=");
                desenhado = 1;
            }

            // Desenhar bola
            if (!desenhado && y == pt->y && x == pt->x) {
                printf("O");
                desenhado = 1;
            }

            // Desenhar blocos
            if (!desenhado && blocos[y][x] == 1) {
                printf("#");
                desenhado = 1;
            }

            // Espa�o vazio
            if (!desenhado) {
                // Desenhar bordas
                if (y == 0) {
                    printf("="); // Borda superior diferenciada
                } else if (y == ALTURA - 1 || x == 0 || x == LARGURA - 1) {
                    printf("*"); // Outras bordas
                } else {
                    printf(" ");
                }
            }
        }
        printf("\n");
    }
}

int main() {
	tocarMusicaDeFundo();  // Inicia a m�sica de fundo em loop
    tela_inicial();  // Adiciona a tela inicial

    struct raquete pd = {LARGURA / 2 - PADDLE_L / 2, PADDLE_L, PADDLE_A};
    struct particula pt;
    redefinir_bola(&pt); // Redefinir posi��o inicial da bola
    int blocos[ALTURA][LARGURA] = {0};

    // Inicializar blocos em uma �rea maior, sem ocupar a primeira linha
    for (int i = 2; i < LARGURA - 2; i += 4) {
        for (int j = 1; j < ALTURA / 2; j++) {
            blocos[j][i] = 1;
        }
    }

    int rodando = 1;
    while (rodando) {
        reposicionar_cursor(0, 0);  // Reposiciona o cursor no canto superior esquerdo

        // Mover raquete com entrada do teclado
        if (_kbhit()) {
            char tecla = _getch();
            if (tecla == 'a' && pd.x > 1) pd.x -= 2;
            if (tecla == 'd' && pd.x < LARGURA - pd.l - 1) pd.x += 2;
            if (tecla == 'q') rodando = 0;  // Sair do jogo
        }

        // Atualizar posi��o da bola
        atualizar_particula(&pt);

        // Verificar colis�o com a raquete
        if (colisao_raquete(&pd, &pt)) {
        pt.dy *= -1;
        pontuacao += 10; // Aumentar a pontua��o por acertar a raquete
        PlaySound(TEXT("sound3.wav"), NULL, SND_ASYNC); // Tocar som da colis�o com a raquete
        }
        

        // Verificar colis�o com bloco
        if (pt.y < ALTURA / 2 && blocos[pt.y][pt.x] == 1) { 
        blocos[pt.y][pt.x] = 0;
        pt.dy *= -1;
        pontuacao += 50; // Aumentar a pontua��o por destruir um bloco
        PlaySound(TEXT("sound2.wav"), NULL, SND_ASYNC); // Tocar som da colis�o com o bloco
        }
       

        // Verificar se a bola cai abaixo da raquete
        if (pt.y >= ALTURA - 1) {
            vidas--;
            if (vidas > 0) {
                redefinir_bola(&pt); // Redefinir posi��o da bola
            } else {
            	system("cls");
            	setCor(4, 0);
            printf("\n\n");
            printf("========================================\n");
            printf("        PERDEU IRMAO                    \n");
            printf("                                        \n");
            printf("========================================\n");
            printf("Pontuacao Final: %d\n", pontuacao);
            system("pause"); // Esperar o usu�rio antes de fechar
            rodando = 0;  // Terminar o jogo
            }
        }

        // Renderizar jogo
        renderizar_jogo(&pt, &pd, blocos);

        // Atraso para controlar a velocidade da bola
        Sleep(velocidade_bola);
    }

    return 0;  
}

