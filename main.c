#include <allegro.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include "datafile.h"
#include "dfSom.h"

#define NOVA 500
#define MAXIMO 10
#define TELAX 800
#define TELAY 600
#define PROPORCAO 0.003
#define CHANCEBOL 5

//Timer
volatile int tempo;

void aumentaTempo() {
    tempo++;
}
END_OF_FUNCTION(aumentaTempo);

volatile int fps;

void aumentaFps() {
	fps++;
}
END_OF_FUNCTION(aumentaFps);

volatile int escudo;
void tempoEscudo(){
    escudo++;
}
END_OF_FUNCTION(tempoEscudo);
//Structs das bolinhas
struct bolinhas {
	float posX, posY, raio, velocidade, distancia;
	float cosY, senoX;
	int existe, cor;
};

volatile int sairPrograma;
void fechaPrograma () {
    sairPrograma = 1;
}

void reinicia (struct bolinhas p[]) {
    int i;
    for (i =0; i < MAXIMO; i++){
        p[i].existe = 0;
        p[i].cor = 0;
        p[i].cosY = 0;
        p[i].distancia = 0;
        p[i].posX = 0;
        p[i].posY = 0;
        p[i].raio = 0;
        p[i].senoX = 0;
        p[i].velocidade = 0;
	}
	escudo = 0;
}

//Inicializa componentes
void inicializa(struct bolinhas p[]) {
	allegro_init();
	install_timer();
	install_keyboard();
	install_mouse();
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, TELAX, TELAY, 0,0);
	set_close_button_callback(fechaPrograma);
	set_window_title("STEEL SHIELD");

	reinicia(p);
}

void titlescreen (int *jogo, DATAFILE data[], DATAFILE fontLetra[]) {
    int exit = 0;
    int avancoY = SCREEN_H/2 - 50;
    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);

    while (!sairPrograma && !exit) {
        draw_sprite(buffer,(BITMAP*) data[TELAINICIAL].dat, 0, 0);
        textout_centre_ex(buffer, (FONT*) fontLetra[MARIO].dat, "STEEL SHIELD", SCREEN_W/2, 50, makecol(255, 255, 0), -1);
        textout_centre_ex(buffer, (FONT*) fontLetra[VERDANA].dat, "INICIAR JOGO", SCREEN_W/2, SCREEN_H/2 - 50, makecol(255, 255, 255), -1);
        textout_centre_ex(buffer, (FONT*) fontLetra[VERDANA].dat, "SAIR", SCREEN_W/2, SCREEN_H/2 + 50, makecol(255, 255, 255), -1);
        textout_right_ex(buffer, font, "v1.1", SCREEN_W, SCREEN_H - 100, makecol(255, 255, 255), -1);
        rect(buffer, 300, avancoY + 20, 500, avancoY, makecol(255, 0 ,0));
        if (key[KEY_UP]){
            avancoY = SCREEN_H/2 - 50;
        }else if (key[KEY_DOWN]){
            avancoY = SCREEN_H/2 + 50;
        }else if (key[KEY_ESC]){
            sairPrograma = 1;
        }else if (key[KEY_ENTER]){
            exit = 1;
            if (avancoY == SCREEN_H/2 + 50) {
                sairPrograma = 1;
            }else{
                (*jogo) = 1;
            }
        }
        draw_sprite(screen, buffer, 0, 0);
        clear(buffer);

    }
    destroy_bitmap(buffer);
}

void fimjogo(DATAFILE data[], DATAFILE font[], int pontuacao, int raio, int raioArco, int *reset) {
    set_uformat(U_ASCII);
    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    int exit = 0;
    int cor = makecol (0, 0, 0);
    int avanco = 305;
    while (!sairPrograma && !exit) {
        draw_sprite(buffer, (BITMAP*) data[TELAFUNDO].dat, 0, 0);
        if (raio < 2) {
            textout_centre_ex(buffer, (FONT*) font[MARIO].dat, "GAME OVER", SCREEN_W/2, 50, makecol(255, 0, 0), -1);
            textprintf_ex(buffer, (FONT*) font[MARIO].dat, 220, 150, makecol (255, 0, 0), -1, "SUA PONTUACAO: %d", pontuacao);
        }else if (raio >= raioArco){
            textout_centre_ex(buffer, (FONT*) font[MARIO].dat, "VOCE VENCEU", SCREEN_W/2, 50, cor, -1);
            textprintf_ex(buffer, (FONT*) font[MARIO].dat, 220, 150, makecol (255, 0, 0), -1, "SUA PONTUACAO: %d", pontuacao);
        }
        textout_centre_ex(buffer, (FONT*) font[VERDANA].dat, "Jogar novamente:", SCREEN_W/2, SCREEN_H/2, cor, -1);
        textout_right_ex(buffer, (FONT*) font[VERDANA].dat, "SIM", SCREEN_W/2 - 50, SCREEN_H/2 + 100, cor, -1);
        textout_right_ex(buffer, (FONT*) font[VERDANA].dat, "NAO", SCREEN_W/2 + 80, SCREEN_H/2 + 100, cor, -1);
        rect(buffer, avanco , 390, avanco + 60, 430, makecol(255, 0, 0));
        draw_sprite(screen, buffer, 0, 0);

        if (key[KEY_ESC]) {
            sairPrograma = 1;
        }else if (key[KEY_LEFT]){
            avanco = 305;
        }else if (key[KEY_RIGHT]){
            avanco = 430;
        }else if (key[KEY_ENTER]){
            if (avanco == 305) {
                exit = 1;
                (*reset) = 1;
            }else if (avanco == 430) {
                sairPrograma = 1;
            }
        }

    }
    destroy_bitmap(buffer);
}

int disponivel(struct bolinhas p[]) {
    int i = 0, indice = 1;

    for (i = 0; i < MAXIMO; i++){
        if (p[i].existe == 0){
            indice = i;
        }
    }

    return indice;
}

void iniciaBolinha (struct bolinhas *p) {
    int direcao = 0;

    direcao = rand()%4 + 1;
    if (p->existe == 0) {
        if (direcao == 1) {
            //POSICAO SUPERIOR
            p->posX = rand()%SCREEN_W;
            p->posY = 0 + p->raio;
        }

        if (direcao == 2){
            //LATERAL ESQUERDA
            p->posY = rand()%SCREEN_H + p->raio;
            p->posX = 0 + p->raio;
        }

        if (direcao == 3) {
            //POSICAO INFERIOR
            p->posX = rand()%SCREEN_W;
            p->posY = SCREEN_H - p->raio;
        }

        if (direcao == 4) {
            //LATERAL DIREITA
            p->posY = rand()%SCREEN_H;
            p->posX = SCREEN_W - p->raio;
        }
        p->velocidade = rand()%2 + 1;
        p->raio = rand()%10 + 2;
        p->existe = 1;
    }

    direcao = rand()%CHANCEBOL + 1;

    switch (direcao) {
        case 1:
            p->cor = makecol (0, 0, 155);
            break;
        default:
            p->cor = makecol(255, 0 ,0);
            break;

    }
}

float calculoAngulo(double *num){
    float x, y;
    float distancia;
    float angulo;

    x = mouse_x - SCREEN_W/2;
    y = mouse_y - SCREEN_H/2;

    if (mouse_x == SCREEN_W/2 && mouse_y == SCREEN_H/2){
        x = 1;
    }
    distancia = sqrt (pow(x, 2) + pow (y, 2));
    x /= distancia;
    y /= distancia;
    angulo = acos(x * 1 + y * 0);
    (*num) = angulo;

    if(y<0){
        angulo *= -1;
    }
    if (mouse_y > SCREEN_H/2) {
        (*num) *= -1;
    }
    angulo*=255/-(2*M_PI);

    return angulo;
}

//Criar arco
void arco (BITMAP *buffer, int raio, int cor, int posX, int posY, float angulo){
    arc(buffer, posX, posY, itofix(angulo - 64), itofix(angulo  + 64), raio, cor);
}

//Criar esfera
void novaEsfera (BITMAP *buffer, struct bolinhas *p) {
	circlefill(buffer, p->posX, p->posY, p->raio, p->cor);
}


//MOVIMENTAÇÃO
void movimenta(BITMAP *buffer, struct bolinhas *p, int playerX, int playerY, double *raio) {
    float hipotenusa, y, x;
    x = (p->posX)- (playerX);
    y = (p->posY) - (playerY);
    hipotenusa = sqrt((x * x) + (y * y));
    p->distancia = hipotenusa;

    if (p->distancia > ((*raio) + p->raio)){
        p->senoX = x/hipotenusa;
        p->cosY = y/hipotenusa;
        p->posX = p->posX - p->velocidade * (p->senoX);
        p->posY = p->posY - p->velocidade * (p->cosY);
    }
}

//COLISAO BOLINHA
void colisaoBolinha (BITMAP *buffer, struct bolinhas *p, double *raio, int cor){
    if (p->distancia <= ((*raio) + p->raio)){
        if (p->cor == cor){
            (*raio) += p->raio/3;
        } else {
            (*raio) -= p->raio/2;
        }
        p->existe = 0;
    }
}

//COLISAO ESCUDO
void colisaoEscudo(struct bolinhas *p, double angulo, int raio, double anguloRadiano, int *pontuacao){
    double x, y, i, xT, yT, distancia;
    if (p->distancia < (raio + p->raio)){
        for (i = anguloRadiano; i<= anguloRadiano + M_PI; i= i + 0.01) {
            x = SCREEN_W/2 + raio * sin(i);
            y = SCREEN_H/2 + raio * cos(i);
            xT = x - p->posX;
            yT = y - p->posY;
            distancia = sqrt((xT * xT) + (yT * yT));
            if (distancia <= p->raio) {
                p->existe = 0;
                (*pontuacao)++;
                break;
            }
        }
    }
}

//ATUALIZA POSIÇÕES DA BOLINHA
void atualiza (BITMAP *buffer, struct bolinhas p[]){
    int i = 0;
    for (i = 0; i < MAXIMO; i++){
        if ((p[i]).existe == 1){
            circlefill(buffer, p[i].posX, p[i].posY, p[i].raio, p[i].cor);
        }
    }
}

//INICIO DO PROGRAMA
int main() {
	//Inicialização de componentes
	struct bolinhas Bolinha[MAXIMO];
    set_uformat(U_ASCII);
	inicializa(Bolinha);

        //Variaveis do player e arco
	double playerX = SCREEN_W/2, playerY = SCREEN_H/2;
	int playerCor = makecol (0, 0, 155);
	double  playerRaio = 15.0;
	double arcoPosX= SCREEN_W/2, arcoPosY = SCREEN_H/2, arcoRaio = 55;
	int arcoCor = makecol(0, 66, 255);

    //Timers
    LOCK_FUNCTION(tempoEscudo);
    install_int_ex(tempoEscudo, SECS_TO_TIMER(1));

	LOCK_FUNCTION(aumentaTempo);
    install_int_ex(aumentaTempo, MSEC_TO_TIMER(1));

	LOCK_FUNCTION(aumentaFps);
	LOCK_VARIABLE(fps);
	install_int_ex(aumentaFps, BPS_TO_TIMER(60));

        //Variaveis das bolinhas e tempo
    double angulo = 0, anguloRadiano = 0;
    int tempoEspaco = tempo, tempoAgora = 0, tempoBolinha = tempo, verifica = 0;
	int novaBolinha = 0, pontuacao = 0;
	int i, espaco = 0;
	int fim = 0, jogo = 0, reset = 0;
	char protecao[15] = "";
	srand((unsigned)time(NULL));
	//BITMAP PRINCIPAl
	BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);

	//DATAFILES
	DATAFILE *data = NULL;
	packfile_password("2015");
	data = load_datafile("dataimg.dat");
	packfile_password(NULL);

    DATAFILE *dataFont = NULL;
    packfile_password("2015");
    dataFont = load_datafile("datafont.dat");
    packfile_password(NULL);

	if (!data || !dataFont){
        allegro_message("Erro ao carregar datafile.");
        sairPrograma = 1;
	} else {
	    allegro_message("INSTRUCOES:\n\n\t\tOBJETIVO:\nAcumular pontos com as bolinhas verdes ate o raio maximo\n\n\n\t\tTECLAS DO JOGO:\n\nMovimento: Utilizar o mouse para movimentar o escudo.\n\nEspecial: Utilize a tecla ESPACO para ativar o escudo\n\nPause: Pressione a tecla P para pausar\n\nSair: Pressione ESC para sair do jogo");
	}
    //INICIA O JOGO
	while (!sairPrograma) {
        if (jogo == 0) {
            titlescreen(&jogo, data, dataFont);
        }

        if (jogo == 1) {
            draw_sprite(buffer, (BITMAP*)data[TELAFUNDO].dat, 0, 0);
            angulo = calculoAngulo(&anguloRadiano);
            arco(buffer, arcoRaio, arcoCor, arcoPosX, arcoPosY, angulo);
            circlefill(buffer, playerX, playerY, playerRaio, playerCor);

            //NOVA BOLINHA
            if ((tempo - tempoBolinha) >= NOVA){
                novaBolinha = disponivel(Bolinha);
                if (novaBolinha != 0){
                    iniciaBolinha(&Bolinha[novaBolinha]);
                    novaEsfera(buffer, &Bolinha[novaBolinha]);
                    novaBolinha = 0;
                }
                tempoBolinha = tempo;
            }
            //MOVIMENTO
            for (i = 0; i < MAXIMO; i++){
                if (Bolinha[i].existe == 1) {
                    movimenta(buffer, &Bolinha[i], playerX, playerY, &playerRaio);
                    if(espaco != 0) {
                        if (Bolinha[i].distancia < (arcoRaio + Bolinha[i].raio)) {
                            Bolinha[i].existe = 0;
                            pontuacao++;
                        }
                    }
                    colisaoEscudo(&Bolinha[i], itofix(angulo - 64), arcoRaio, anguloRadiano, &pontuacao);
                    colisaoBolinha(buffer, &Bolinha[i], &playerRaio, playerCor);
                }
            }
            //COLISAO COM A BARREIRA
            if (espaco != 0 && escudo < 5){
                circlefill(buffer, SCREEN_W/2, SCREEN_H/2, arcoRaio, makecol(0, 0, 255));
                playerRaio -= PROPORCAO;
                tempoEspaco = tempo;
            }

            if (escudo > 5) {
                strcpy(protecao, "Disponivel");
                espaco = 0;
            }else {
                strcpy(protecao, "Indisponivel");
            }

            if (key[KEY_ESC]) {
                sairPrograma = 1;
            }

            if (key[KEY_F1]) {
                allegro_message("INSTRUCOES:\n\n\t\tOBJETIVO:\nAcumular pontos com as bolinhas verdes até o raio máximo\n\n\n\t\tTECLAS DO JOGO:\n\nMovimento: Utilizar o mouse para movimentar o escudo.\n\nEspecial: Utilize a tecla ESPACO para ativar o escudo(*Voce perde energia ao usar o escudo)\n\nPause: Pressione a tecla P para pausar\n\nSair: Pressione ESC para sair do jogo");
            }

            if (key[KEY_SPACE] && !strcmp(protecao, "Disponivel")){
                if (espaco != 1 && playerRaio > 10){
                    espaco = 1;
                    escudo = 0;
                }
            }

            if (key[KEY_P]) {
                allegro_message("PAUSE");
            }

            if (playerRaio < 2){
                fimjogo(data, dataFont, pontuacao, playerRaio, arcoRaio, &reset);
            }

            if (playerRaio >= arcoRaio) {
                fimjogo(data, dataFont, pontuacao, playerRaio, arcoRaio, &reset);;
            }

            tempoAgora = 0;

            //ATUALIZA BOLINHAS
            atualiza(buffer, Bolinha);

            textprintf_ex(buffer, (FONT*)dataFont[VERDANA].dat, 50, 10, makecol (0, 0, 0), -1, "Tempo: %d", tempo/1000);
            textprintf_ex(buffer, (FONT*)dataFont[VERDANA].dat, 220, 10, makecol (0, 0, 0), -1, "Score: %d", pontuacao);
            textprintf_ex(buffer, (FONT*)dataFont[VERDANA].dat, 400, 10, makecol (0, 0, 0), -1, "Escudo: %s", protecao);
            textprintf_right_ex(buffer, (FONT*)dataFont[VERDANA].dat, 700, 10, makecol(0, 0, 0), -1, "Ajuda: F1");

            circlefill(buffer, playerX, playerY, playerRaio, playerCor);

            if (reset == 1){
                reinicia(Bolinha);
                playerRaio = 15;
                tempo = 0;
                pontuacao = 0;
                reset = 0;
                escudo = 0;
                tempoBolinha = 0;
                tempoEspaco = 0;
            }
            //Desenha em tela e limpa o buffer
            draw_sprite(buffer, (BITMAP*)data[CURSOR].dat, mouse_x, mouse_y);
            draw_sprite(screen, buffer, 0,0);
            clear (buffer);
        }

	}

	if (!data) {
        unload_datafile(data);
        unload_datafile(dataFont);
	}
    destroy_bitmap (buffer);
	return 0;
}
END_OF_MAIN();
