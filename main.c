#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
#define CELL 20
#define N 30
#define M 40
#define MAX_COMIDAS 10
#define MAX_FASES 3
#define MAX_ENEMIGOS 10
#define MAX_BALAS 5
#define FRAMES_GATO 12
#define GATO 0
#define PERRO 1
#define MONO 2
#define MAX_BANANAS 10
#define MAX_RANKING 10

typedef struct
{
    int x;
    int y;

} Segmento;

typedef struct
{
    Segmento segmentos[100];

    int tamano;

    int dx;
    int dy;

    //Aca poner el arreglo de balas
    //Que tenga una maxima cantidad de balas
    //Las balas de la serpienten deben tener un rango

} Serpiente;

typedef struct
{
    int nivel;
    int puntaje;
    int tiempo;
    int tieneLlave;
    int comidasNecesarias;
    int comidasFase;

    int contadorTiempo;

    int tiempoMensaje;

    char mensaje[100];
    char archivoNivel[30];
    char nombreNivel[30];
    

    int xLlave;
    int yLlave;

} Juego;

typedef struct
{
    char nombre[20];
    int dato;

} Ranking;

typedef struct
{
    int x;
    int y;
    bool activa;

} Comida;

typedef struct
{
    int numero;
    int comidasComidas;
    int comidas[MAX_FASES];

} Fase;

typedef struct
{
    float x;
    float y;

    float distanciaAnimacion;
    
    int dx;
    int dy;

    int frame;
    int contadorAnimacion;

    int respawn;

    int tiempoDisparo;

    int tipo;

    bool vivo;

} Enemigo;

//Modificar y generar una sola struct de balas, agregar un tipo bala
typedef struct
{
    bool activa;

    float x;
    float y;

    float destinoX;
    float destinoY;

    float velocidadX;
    float velocidadY;

} Banana;

typedef struct
{
    float x;
    float y;

    float dx;
    float dy;

    bool activa;

} Bala;

typedef struct
{
    //Niveles
    ALLEGRO_BITMAP *fondoPradera;
    ALLEGRO_BITMAP *arbusto;

    ALLEGRO_BITMAP *bosquePiso;
    ALLEGRO_BITMAP *bosqueMuro;

    //Comidas
    ALLEGRO_BITMAP *frutas[5];

    //Llave y Puerta
    ALLEGRO_BITMAP *llave;
    ALLEGRO_BITMAP *puerta;

    //Gato
    ALLEGRO_BITMAP *gatoSprite[FRAMES_GATO];

    //Perro
    ALLEGRO_BITMAP *perroSprite[2];

    //Mono
    ALLEGRO_BITMAP *monoDerecha;

    ALLEGRO_BITMAP *bananaMono;

    // Serpiente
    ALLEGRO_BITMAP *cabezaArriba;
    ALLEGRO_BITMAP *cabezaAbajo;
    ALLEGRO_BITMAP *cabezaIzquierda;
    ALLEGRO_BITMAP *cabezaDerecha;

    ALLEGRO_BITMAP *cuerpoHorizontal;
    ALLEGRO_BITMAP *cuerpoVertical;

    ALLEGRO_BITMAP *colaArriba;
    ALLEGRO_BITMAP *colaAbajo;
    ALLEGRO_BITMAP *colaIzquierda;
    ALLEGRO_BITMAP *colaDerecha;

    ALLEGRO_BITMAP *curva1;
    ALLEGRO_BITMAP *curva2;
    ALLEGRO_BITMAP *curva3;
    ALLEGRO_BITMAP *curva4;

} Sprites;

char mapa[N][M];

Serpiente serpiente;
Juego juego;
Fase fase;
Sprites sprites;

Enemigo enemigos[MAX_ENEMIGOS];
Banana bananas[MAX_BANANAS];
Bala balas[MAX_BALAS];
Comida comidas[MAX_COMIDAS];

Ranking rankingSegmentos[MAX_RANKING];
Ranking rankingTiempo[MAX_RANKING];

int cantidadRankingSegmentos = 0;
int cantidadRankingTiempo = 0;

int cantidadEnemigos = 0;

void cargarMapa(char nombreArchivo[]);
void cargarSprites();
void destruirSprites();
void cargarRankingSegmentos();
void guardarRankingSegmentos();
void cargarRankingTiempo();
void guardarRankingTiempo();
void reiniciarJuego();
void generarComidas(int cantidad);
void generarEnemigo(int i);
void lanzarBanana();
void disparar();
int haySerpiente(int x, int y);
int hayComida(int x, int y);
int hayEnemigo(int x, int y);
int verificarColisionMuro();
int verificarColisionSerpiente();
int verificarPuertaBloqueada();
int verificarLimites();

int main()
{
    al_init();
    srand(time(NULL));
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_image_addon();

    ALLEGRO_DISPLAY *display =
        al_create_display(WIDTH, HEIGHT);

    ALLEGRO_TIMER *timer =
        al_create_timer(1.0/8);

    ALLEGRO_EVENT_QUEUE *queue =
        al_create_event_queue();

    ALLEGRO_FONT *font =
        al_create_builtin_font();

        cargarSprites();

    al_install_keyboard();

    al_register_event_source(
        queue,
        al_get_display_event_source(display));

    al_register_event_source(
        queue,
        al_get_keyboard_event_source());

    al_register_event_source(
        queue,
        al_get_timer_event_source(timer));

    al_start_timer(timer);
    reiniciarJuego();
    printf("Juego iniciado\n");

    int running = 1;

    while(running)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue,&ev);

        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            running = 0;

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_UP:
                    if(serpiente.dy != 1)
                    {
                        serpiente.dx = 0;
                        serpiente.dy = -1;
                    }
                    break;

                case ALLEGRO_KEY_DOWN:
                    if(serpiente.dy != -1)
                    {
                        serpiente.dx = 0;
                        serpiente.dy = 1;
                    }
                    break;

                case ALLEGRO_KEY_LEFT:
                    if(serpiente.dx != 1)
                    {
                        serpiente.dx = -1;
                        serpiente.dy = 0;
                    }
                    break;

                case ALLEGRO_KEY_RIGHT:
                    if(serpiente.dx != -1)
                    {
                        serpiente.dx = 1;
                        serpiente.dy = 0;
                    }
                    break;

                case ALLEGRO_KEY_SPACE:
                    disparar();
                    break;
            }
        }

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            for(int i = 0; i < cantidadEnemigos; i++)
            {
                //Respawn del Gato

                if(!enemigos[i].vivo)
                {
                    if(enemigos[i].respawn > 0)
                    {
                        enemigos[i]. respawn--;
                    }
                    else if(enemigos[i].respawn == 0)
                    {
                        if(juego.nivel == 1)
                        {
                            generarEnemigo(i);
                        }
                    }

                    continue;
                }

                if(enemigos[i].vivo)
                {
                    //Mono Lanza Bananas
                    if(enemigos[i].tipo == MONO)
                    {
                        enemigos[i].tiempoDisparo--;

                        if(enemigos[i].tiempoDisparo <=0)
                        {
                            lanzarBanana(i);

                            enemigos[i].tiempoDisparo = 120;
                        }
                    } 

                    //Movimiento
                 float nuevoX = enemigos[i].x + enemigos[i].dx;
                 float nuevoY = enemigos[i].y + enemigos[i].dy;

                 int columna = (nuevoX + CELL/2) / CELL;
                 int fila = (nuevoY + CELL/2) / CELL;

                 if(mapa[fila][columna] == '#')
                 {
                    if(enemigos[i].tipo == GATO)
                    {
                        enemigos[i].dx *= -1;
                    }
                    else
                    {
                        enemigos[i].dy *= -1;
                    }
                 }
                 else
                 {
                    enemigos[i].x = nuevoX;
                    enemigos[i].y = nuevoY;
                 }

                 int enemigoX = (enemigos[i].x + CELL / 2) / CELL;
                 int enemigoY = (enemigos[i].y + CELL / 2) / CELL;

                 //Enemigo Mata a la Serpiente
                 if(enemigoX == serpiente.segmentos[0].x &&
                    enemigoY == serpiente.segmentos[0].y)
                 {
                    reiniciarJuego();
                 }

                    //Animacion

                if(enemigos[i].tipo == GATO)
                {
                    enemigos[i].distanciaAnimacion +=
                        fabs(enemigos[i].dx) + fabs(enemigos[i].dy);

                    if(enemigos[i].distanciaAnimacion >= 16)
                    {
                        enemigos[i].distanciaAnimacion = 0;

                        if(enemigos[i].dx > 0)
                        {
                            enemigos[i].frame++;

                            if(enemigos[i].frame > 5)
                                enemigos[i].frame = 0;
                        }
                        else
                        {
                            enemigos[i].frame++;

                            if(enemigos[i].frame < 6 || enemigos[i].frame > 11)
                                enemigos[i].frame = 6;
                        }
                    }        
                }
            }
        }

        //Movimiento de bananas
                    for(int j = 0; j < MAX_BANANAS; j++)
                    {
                        if(bananas[j].activa)
                        {
                            bananas[j].x += bananas[j].velocidadX;
                            bananas[j].y += bananas[j].velocidadY;

                            if(fabs(bananas[j].x - bananas[j].destinoX) < 0.2 &&
                            fabs(bananas[j].y - bananas[j].destinoY) < 0.2)
                            {
                                bananas[j].x = bananas[j].destinoX;
                                bananas[j].y = bananas[j].destinoY;

                                bananas[j].velocidadX = 0;
                                bananas[j].velocidadY = 0;
                            }

                            if((int)bananas[j].x == serpiente.segmentos[0].x &&
                               (int)bananas[j].y == serpiente.segmentos[0].y)
                            {
                                reiniciarJuego();
                            }
                        }
                    }

            for(int i =0; i < MAX_BALAS; i++)
            {
                if(balas[i].activa)
                {
                    balas[i].x += balas[i].dx;
                    balas[i].y += balas[i].dy;

                    int columna = balas[i].x / CELL;
                    int fila = balas[i].y / CELL;

                    if(mapa[fila][columna] == '#')
                    {
                        balas[i].activa = false;
                    }

                    if(balas[i].x < 0 ||
                       balas[i].x > M * CELL ||
                       balas[i].y < 0 ||
                       balas[i].y > N * CELL)
                    {
                        balas[i].activa = false;
                    }
                }    
            }

            for(int i = 0; i < MAX_BALAS; i++)
            {
                if(balas[i].activa)
                {
                    for(int j = 0; j < cantidadEnemigos; j++)
                    {
                        if(enemigos[j].vivo)
                        {
                            float dx = balas[i].x - (enemigos[j].x + CELL/2);
                            float dy = balas[i].y - (enemigos[j].y + CELL/2);

                            if(dx*dx + dy*dy < (CELL/2)*(CELL/2))
                            {
                                balas[i].activa = false;
                                enemigos[j].vivo = false;
                                enemigos[j].respawn = 120;

                                break;
                            }
                        }
                    }
                }
            }

            for(int i=serpiente.tamano-1;i>0;i--)
            {
                serpiente.segmentos[i] = serpiente.segmentos[i-1];
            }

            serpiente.segmentos[0].x += serpiente.dx;
            serpiente.segmentos[0].y += serpiente.dy;

            if(verificarLimites())
            {
                printf("GAME OVER\n");

                reiniciarJuego();
            }
            if(verificarColisionMuro())
            {
                printf("GAME OVER\n");

                reiniciarJuego();
            }

            if(verificarColisionSerpiente())
            {
                printf("GAME OVER\n");

                reiniciarJuego();
            }

            verificarPuertaBloqueada();

            int comidaComida = 0;

            for(int i=0; i<MAX_COMIDAS; i++)
            {
                if(comidas[i].activa && serpiente.segmentos[0].x == comidas[i].x && serpiente.segmentos[0].y == comidas[i].y)
                {
                    comidas[i].activa = false;

                    serpiente.segmentos[serpiente.tamano] = serpiente.segmentos[serpiente.tamano-1];

                    serpiente.tamano++;

                    juego.puntaje++;

                    fase.comidasComidas++;

                    comidaComida = 1;

                    break;
                }
            }

            if(comidaComida)
            {
                int quedan = 0;

                for(int i=0; i<MAX_COMIDAS; i++)
                {
                    if(comidas[i].activa)
                    {
                        quedan++;
                    }
                }

                if(quedan == 0)
                {
                    if(fase.numero < 3)
                    {
                        fase.numero++;
                        fase.comidasComidas = 0;

                        sprintf(juego.mensaje, "Fase %d", fase.numero);
                        juego.tiempoMensaje = 24;

                        generarComidas(fase.comidas[fase.numero - 1]);
                    }
                    else
                    {
                        mapa[juego.yLlave][juego.xLlave] = 'L';
                        strcpy(juego.mensaje, "Ya puedes recoger la llave");
                        juego.tiempoMensaje = 24;
                    }
                }
            }

           if(mapa[serpiente.segmentos[0].y][serpiente.segmentos[0].x] == 'L')
            {
                juego.tieneLlave = 1;

                mapa[serpiente.segmentos[0].y][serpiente.segmentos[0].x] = ' ';

                strcpy(juego.mensaje, "Llave obtenida");

                juego.tiempoMensaje = 24;
            }

            if(mapa[serpiente.segmentos[0].y][serpiente.segmentos[0].x] == 'E' && juego.tieneLlave)
            {
                juego.nivel = 2;

                sprintf(juego.archivoNivel, "Niveles/nivel%d.txt", juego.nivel);

                cargarMapa(juego.archivoNivel);

                for(int i = 0; i < MAX_BANANAS; i++)
                {
                    bananas[i].activa = false;
                }

                switch(juego.nivel)
                {
                    case 1:
                        strcpy(juego.nombreNivel, "Pradera");
                        break;

                    case 2:
                        strcpy(juego.nombreNivel, "Bosque");
                        break;

                    case 3:
                        strcpy(juego.nombreNivel, "Desierto");
                        break;
                    case 4:
                        strcpy(juego.nombreNivel, "Iceberg");
                        break;
                    case 5:
                        strcpy(juego.nombreNivel, "Volcan");
                        break;
                }

                fase.numero = 1;
                fase.comidasComidas = 0;

                for(int i=0; i<MAX_COMIDAS; i++)
                {
                    comidas[i].activa = false;
                }

                generarComidas(fase.comidas[fase.numero - 1]);

                juego.tieneLlave = 0;

                serpiente.dx = 1;
                serpiente.dy = 0;

                sprintf(juego.mensaje, "Bienvenido a %s", juego.nombreNivel);
                juego.tiempoMensaje = 24;
            }

    al_clear_to_color(
        al_map_rgb(136,231,136));

        if(juego.nivel == 1)
        {
            for(int i=0; i<N; i++)
            {
                for(int j=0; j<M; j++)
                {
                    al_draw_scaled_bitmap(
                        sprites.fondoPradera,
                        0,
                        0,
                        64,
                        64,
                        j*CELL,
                        i*CELL,
                        CELL,
                        CELL,
                        0);
                }
            }
        }
        else if(juego.nivel == 2)
        {
            for(int i=0; i<N; i++)
            {
                for(int j=0; j<M; j++)
                {
                    al_draw_scaled_bitmap(
                        sprites.bosquePiso,
                        0,
                        0,
                        64,
                        64,
                        j*CELL,
                        i*CELL,
                        CELL,
                        CELL,
                        0);
                }
            }
        }
        else
        {
            al_clear_to_color(al_map_rgb(136,231,136));
        }              

for(int i=0; i<N; i++)
{
    for(int j=0; j<M; j++)
    {

        char casilla = mapa[i][j];

        switch(casilla)
        {
            //Muros
            case '#':

            if(juego.nivel == 1)
            {
                al_draw_scaled_bitmap(
                    sprites.arbusto,
                    0,
                    0,
                    64,
                    64,
                    j*CELL,
                    i*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else if(juego.nivel == 2)
            {
                al_draw_scaled_bitmap(
                    sprites.bosqueMuro,
                    0,
                    0,
                    64,
                    64,
                    j*CELL,
                    i*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else
            {
                al_draw_filled_rectangle(
                    j*CELL,
                    i*CELL,
                    j*CELL+CELL,
                    i*CELL+CELL,
                    al_map_rgb(90,90,90));
            }
            break;

                //Llave
            case 'L':

                if(!juego.tieneLlave)
                {
                    al_draw_scaled_bitmap(
                        sprites.llave,
                        0,
                        0,
                        64,
                        64,
                        j*CELL,
                        i*CELL,
                        CELL,
                        CELL,
                        0);
                }

                break;
                //Puerta
            case 'E':

                al_draw_scaled_bitmap(
                    sprites.puerta,
                    0,
                    0,
                    64,
                    64,
                    j*CELL,
                    i*CELL,
                    CELL,
                    CELL,
                    0);

                break;
        }
    }
}


for(int i=0; i<MAX_COMIDAS; i++)
{
    if(comidas[i].activa)
    {
        ALLEGRO_BITMAP *fruta = sprites.frutas[juego.nivel - 1];

        al_draw_scaled_bitmap(
            fruta,
            0,
            0,
            64,
            64,
            comidas[i].x*CELL,
            comidas[i].y*CELL,
            CELL,
            CELL,
            0);
    }
}

for(int i = 0; i < cantidadEnemigos; i++)
{
    if(enemigos[i].vivo)
    {
        ALLEGRO_BITMAP *sprite;

        if(enemigos[i].tipo == GATO)
        {
            sprite = sprites.gatoSprite[enemigos[i].frame];
        }
        else if(enemigos[i].tipo == PERRO)
        {
            if(enemigos[i].dy < 0)
            {
                sprite = sprites.perroSprite[0];
            }
            else
            {
                sprite = sprites.perroSprite[1];
            }
        }
        
        if(enemigos[i].tipo == MONO)
        {
            sprite = sprites.monoDerecha;
        }

        al_draw_scaled_bitmap(
            sprite,
            0,
            0,
            64,
            64,
            enemigos[i].x,
            enemigos[i].y,
            CELL,
            CELL,
            0
        );
    }
}

for(int i=0;i<MAX_BANANAS;i++)
{
    if(bananas[i].activa)
    {
        al_draw_scaled_bitmap(
            sprites.bananaMono,
            0,
            0,
            64,
            64,
            bananas[i].x*CELL,
            bananas[i].y*CELL,
            CELL,
            CELL,
            0);

        int bananaX = bananas[i].x;
        int bananaY = bananas[i].y;

        for(int j = 0; j < serpiente.tamano; j++)
        {

            if(bananaX == serpiente.segmentos[j].x &&
               bananaY == serpiente.segmentos[j].y)
            {
                if(serpiente.tamano > 2)
                {
                    serpiente.tamano--;
                }
                else
                {
                reiniciarJuego();
                }

                bananas[i].activa = false;
            }
        }
    }
}

for(int i =0; i < MAX_BALAS; i++)
{
    if(balas[i].activa)
    {
        al_draw_filled_circle(
            balas[i].x,
            balas[i].y,
            5,
            al_map_rgb(0, 255, 0));
    }
}

for(int i=0; i<serpiente.tamano; i++)
{
    if(i == 0)
    {
        ALLEGRO_BITMAP *cabeza;

        if(serpiente.dx == 1)
            cabeza = sprites.cabezaDerecha;
        else if(serpiente.dx == -1)
            cabeza = sprites.cabezaIzquierda;
        else if(serpiente.dy == -1)
            cabeza = sprites.cabezaArriba;
        else
            cabeza = sprites.cabezaAbajo;

        al_draw_scaled_bitmap(
            cabeza,
            0,
            0,
            64,
            64,
            serpiente.segmentos[i].x * CELL,
            serpiente.segmentos[i].y * CELL,
            CELL,
            CELL,
            0);
    }
    else if(i == serpiente.tamano - 1)
    {
        ALLEGRO_BITMAP *cola;

        if(serpiente.tamano == 1)
            cola = sprites.colaAbajo;
        else
        {
        int dx = serpiente.segmentos[i].x - serpiente.segmentos[i-1].x;
        int dy = serpiente.segmentos[i].y - serpiente.segmentos[i-1].y;

        if(dx == 0 && dy == 0)
        {
            
            if(serpiente.dx == 1)
                cola = sprites.colaDerecha;
            else if(serpiente.dx == -1)
                cola = sprites.colaIzquierda;
            else if(serpiente.dy == 1)
                cola = sprites.colaAbajo;
            else
                cola = sprites.colaArriba;
        }
        else if(dx == 1)
        {
            cola = sprites.colaDerecha;
        }
        else if(dx == -1)
        {
            cola = sprites.colaIzquierda;
        }
        else if(dy == 1)
        {
            cola = sprites.colaAbajo;
        }
        else
        {
            cola = sprites.colaArriba;
        }
        }

        al_draw_scaled_bitmap(
            cola,
            0,0,
            64,64,
            serpiente.segmentos[i].x*CELL,
            serpiente.segmentos[i].y*CELL,
            CELL,
            CELL,
            0);
    }
    else
    {
        int dx1 = serpiente.segmentos[i].x - serpiente.segmentos[i-1].x;
        int dy1 = serpiente.segmentos[i].y - serpiente.segmentos[i-1].y;

        int dx2 = serpiente.segmentos[i+1].x - serpiente.segmentos[i].x;
        int dy2 = serpiente.segmentos[i+1].y - serpiente.segmentos[i].y;

        ALLEGRO_BITMAP *imagen = sprites.cuerpoHorizontal;

    // Cuerpo recto

    if(dy1 == 0 && dy2 == 0)
    {
        // Se mueve de izquierda a derecha
        imagen = sprites.cuerpoHorizontal;
    }
    else if(dx1 == 0 && dx2 == 0)
    {
        // Se mueve de arriba a abajo
        imagen = sprites.cuerpoVertical;
    }

    // Curvas

        else if((dx1==1 && dy2==1) || (dy1==-1 && dx2==-1))
        {
            imagen = sprites.curva2;
        }

        else if((dx1==-1 && dy2==1) || (dy1==-1 && dx2==1))
        {
            imagen = sprites.curva1;
        }

        else if((dx1==-1 && dy2==-1) || (dy1==1 && dx2==1))
        {
            imagen = sprites.curva3;
        }

        else
        {
            imagen = sprites.curva4;
        }

        al_draw_scaled_bitmap(
            imagen,
            0,
            0,
            64,
            64,
            serpiente.segmentos[i].x*CELL,
            serpiente.segmentos[i].y*CELL,
            CELL,
            CELL,
            0);
    }
}           
            //Nivel en Pantalla

                char textoNivel[50];

                sprintf(textoNivel,
                        "Nivel: %d",
                        juego.nivel);

                al_draw_text(
                    font,
                    al_map_rgb(255,255,255),
                    10,
                    30,
                    0,
                    textoNivel);

            //Puntaje en Pantalla

                char texto[100];

                sprintf(texto,
                        "Puntaje: %d",
                        juego.puntaje);

                al_draw_text(
                    font,
                    al_map_rgb(255,255,255),
                    10,
                    10,
                    0,
                    texto);

            //Tiempo en Pantalla

                char textoTiempo[100];

                int minutos = juego.tiempo / 60;
                int segundos = juego.tiempo % 60;

                sprintf(textoTiempo,
                        "Tiempo: %02d:%02d",
                        minutos,
                        segundos);

                al_draw_text(
                    font,
                    al_map_rgb(255,255,255),
                    10,
                    70,
                    0,
                    textoTiempo);

            //Llave Obtenida en Pantalla

                char textoLlave[50];
                char estadoLlave[5];

                if(juego.tieneLlave)
                {
                    strcpy(estadoLlave, "SI");
                }
                else
                {
                    strcpy(estadoLlave, "NO");
                }

                sprintf(textoLlave,
                        "Llave: %s",
                        estadoLlave);

                al_draw_text(
                    font,
                    al_map_rgb(255,255,255),
                    10,
                    90,
                    0,
                    textoLlave);

            //Fase y Comidas necesarias en Pantalla

                char textoComidas[100];

                sprintf(textoComidas,
                        "Fase: %d/3   Comidas: %d/%d",
                        fase.numero,
                        fase.comidasComidas,
                        fase.comidas[fase.numero - 1]);

                al_draw_text(
                     font,
                      al_map_rgb(255,255,255),
                      10,
                      50,
                      0,
                      textoComidas);

            //Tiempo en pantalla del mensaje

            if(juego.tiempoMensaje > 0)
            {
                 juego.tiempoMensaje--;
            }

            if(juego.tiempoMensaje > 0)
            {
                al_draw_text(
                    font,
                    al_map_rgb(255,255,0),
                    WIDTH/2,
                    HEIGHT-30,
                    ALLEGRO_ALIGN_CENTER,
                    juego.mensaje
                );
            }

            juego.contadorTiempo++;

            if(juego.contadorTiempo >= 8)
            {
                juego.tiempo++;
                juego.contadorTiempo = 0;
            }

            al_flip_display();
        }
    }

    destruirSprites();

    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}

void cargarMapa(char nombreArchivo[])
{
    cantidadEnemigos = 0;

    FILE *archivo = fopen(nombreArchivo, "r");

    if(archivo == NULL)
    {
        printf("No se pudo abrir el archivo %s\n", nombreArchivo);
        return;
    } 

    for(int i = 0; i<MAX_FASES; i++)
    {
        fscanf(archivo, "%d", &fase.comidas[i]);
    }

    fgetc(archivo);

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
        {
            mapa[i][j] = fgetc(archivo);

            if(mapa[i][j] == 'S')
            {
                serpiente.segmentos[0].x = j;
                serpiente.segmentos[0].y = i;

                serpiente.segmentos[1].x = j-1;
                serpiente.segmentos[1].y = i;

                serpiente.segmentos[2].x = j-2;
                serpiente.segmentos[2].y = i;

                mapa[i][j] = ' ';
            }

            if(mapa[i][j] == 'L')
            {
                juego.xLlave = j;
                juego.yLlave = i;

                mapa[i][j] = ' ';
            }

            if(mapa[i][j] == 'G')
            {
                if(cantidadEnemigos < MAX_ENEMIGOS)
                {
                    enemigos[cantidadEnemigos].x = j * CELL;
                    enemigos[cantidadEnemigos].y = i * CELL;

                    enemigos[cantidadEnemigos].dx = 4;
                    enemigos[cantidadEnemigos].dy = 0;

                    enemigos[cantidadEnemigos].frame = 0;
                    enemigos[cantidadEnemigos].contadorAnimacion = 0;

                    enemigos[cantidadEnemigos].vivo = true;
                    enemigos[cantidadEnemigos].respawn = 0;

                    enemigos[cantidadEnemigos].tipo = GATO;

                    cantidadEnemigos++;
                }

                mapa[i][j] = ' ';
            }

            if(mapa[i][j] == 'P')
            {
                if(cantidadEnemigos < MAX_ENEMIGOS)
                {
                    enemigos[cantidadEnemigos].x = j * CELL;
                    enemigos[cantidadEnemigos].y = i * CELL;

                    enemigos[cantidadEnemigos].dx = 0;
                    enemigos[cantidadEnemigos].dy = 4;

                    enemigos[cantidadEnemigos].frame = 0;
                    enemigos[cantidadEnemigos].contadorAnimacion = 0;

                    enemigos[cantidadEnemigos].vivo = true;
                    enemigos[cantidadEnemigos].respawn = 0;

                    enemigos[cantidadEnemigos].distanciaAnimacion = 0;

                    enemigos[cantidadEnemigos].tipo = PERRO;

                    cantidadEnemigos++;
                }

                mapa[i][j] = ' ';
            }

            if(mapa[i][j] == 'M')
            {
                if(cantidadEnemigos < MAX_ENEMIGOS)
                {
                    enemigos[cantidadEnemigos].x = j * CELL;
                    enemigos[cantidadEnemigos].y = i * CELL;

                    enemigos[cantidadEnemigos].dx = 0;
                    enemigos[cantidadEnemigos].dy = 0;

                    enemigos[cantidadEnemigos].frame = 0;
                    enemigos[cantidadEnemigos].contadorAnimacion = 0;

                    enemigos[cantidadEnemigos].vivo = true;
                    enemigos[cantidadEnemigos].respawn = 0;

                    enemigos[cantidadEnemigos].distanciaAnimacion = 0;

                    enemigos[cantidadEnemigos].tipo = MONO;

                    enemigos[cantidadEnemigos].tiempoDisparo = 120;

                    cantidadEnemigos++;
                }

                mapa[i][j] = ' ';
            }
        }

        fgetc(archivo);
    }

    fclose(archivo);
}

void cargarSprites()
{
    //Serpiente

    sprites.cabezaArriba = al_load_bitmap("Sprites/CabezaSerpArriba.png");
    sprites.cabezaAbajo = al_load_bitmap("Sprites/CabezaSerpAbajo.png");
    sprites.cabezaIzquierda = al_load_bitmap("Sprites/CabezaSerpIzquierda.png");
    sprites.cabezaDerecha = al_load_bitmap("Sprites/CabezaSerpDerecha.png");

    sprites.cuerpoHorizontal = al_load_bitmap("Sprites/CuerpoSerpHorizontal.png");
    sprites.cuerpoVertical = al_load_bitmap("Sprites/CuerpoSerpVertical.png");

    sprites.colaArriba = al_load_bitmap("Sprites/ColaSerpArriba.png");
    sprites.colaAbajo = al_load_bitmap("Sprites/ColaSerpAbajo.png");
    sprites.colaIzquierda = al_load_bitmap("Sprites/ColaSerpIzquierda.png");
    sprites.colaDerecha = al_load_bitmap("Sprites/ColaSerpDerecha.png");

    sprites.curva1 = al_load_bitmap("Sprites/CurvaSerp1.png");
    sprites.curva2 = al_load_bitmap("Sprites/CurvaSerp2.png");
    sprites.curva3 = al_load_bitmap("Sprites/CurvaSerp3.png");
    sprites.curva4 = al_load_bitmap("Sprites/CurvaSerp4.png");

    //Mono

    sprites.monoDerecha = al_load_bitmap("Sprites/monoDerecha.png");
    sprites.bananaMono = al_load_bitmap("Sprites/bananaMono.png");

    //Perro

    sprites.perroSprite[0] = al_load_bitmap("Sprites/PerroArriba.png");
    sprites.perroSprite[1] = al_load_bitmap("Sprites/PerroAbajo.png");

    //Gato

    sprites.gatoSprite[0] = al_load_bitmap("Sprites/gatoderecha1.png");
    sprites.gatoSprite[1] = al_load_bitmap("Sprites/gatoderecha2.png");
    sprites.gatoSprite[2] = al_load_bitmap("Sprites/gatoderecha3.png");
    sprites.gatoSprite[3] = al_load_bitmap("Sprites/gatoderecha4.png");
    sprites.gatoSprite[4] = al_load_bitmap("Sprites/gatoderecha5.png");
    sprites.gatoSprite[5] = al_load_bitmap("Sprites/gatoderecha6.png");
    sprites.gatoSprite[6] = al_load_bitmap("Sprites/gatoizq1.png");
    sprites.gatoSprite[7] = al_load_bitmap("Sprites/gatoizq2.png");
    sprites.gatoSprite[8] = al_load_bitmap("Sprites/gatoizq3.png");
    sprites.gatoSprite[9] = al_load_bitmap("Sprites/gatoizq4.png");
    sprites.gatoSprite[10] = al_load_bitmap("Sprites/gatoizq5.png");
    sprites.gatoSprite[11] = al_load_bitmap("Sprites/gatoizq6.png");

    //Llave y Puerta
    sprites.llave = al_load_bitmap("Sprites/LlaveSprite.png");
    sprites.puerta = al_load_bitmap("Sprites/PuertaSprite.png");

    //Frutas

    sprites.frutas[0] = al_load_bitmap("Sprites/ComidaManzana.png");
    sprites.frutas[1] = al_load_bitmap("Sprites/ComidaNaranja.png");
    sprites.frutas[2] = al_load_bitmap("Sprites/ComidaBanana.png");
    sprites.frutas[3] = al_load_bitmap("Sprites/ComidaArandanos.png");
    sprites.frutas[4] = al_load_bitmap("Sprites/ComidaAji.png");

    //Niveles

    sprites.bosquePiso = al_load_bitmap("Sprites/bosquePiso.png");
    sprites.bosqueMuro = al_load_bitmap("Sprites/bosqueMuro.png");

    sprites.fondoPradera = al_load_bitmap("Sprites/FondoNivel1.png");
    sprites.arbusto = al_load_bitmap("Sprites/MurosArbustosNivel1.png");

    //Comprobacion

    if(!sprites.fondoPradera ||
       !sprites.arbusto ||
       !sprites.bosquePiso ||
       !sprites.bosqueMuro ||
       !sprites.llave ||
       !sprites.puerta ||
       !sprites.monoDerecha ||
       !sprites.bananaMono ||
       !sprites.cabezaArriba ||
       !sprites.cabezaAbajo ||
       !sprites.cabezaIzquierda ||
       !sprites.cabezaDerecha ||
       !sprites.cuerpoHorizontal ||
       !sprites.cuerpoVertical ||
       !sprites.colaArriba ||
       !sprites.colaAbajo ||
       !sprites.colaIzquierda ||
       !sprites.colaDerecha ||
       !sprites.curva1 ||
       !sprites.curva2 ||
       !sprites.curva3 ||
       !sprites.curva4)
    {
        printf("Error cargando los sprites.\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < FRAMES_GATO; i++)
    {
        if(!sprites.gatoSprite[i])
        {
            printf("Error cargando el sprite del gato %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < 2; i++)
    {
        if(!sprites.perroSprite[i])
        {
            printf("Error cargando el sprite del perro %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < 5; i++)
    {
        if(!sprites.frutas[i])
        {
            printf("Error cargando la fruta %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

}

void destruirSprites()
{
    al_destroy_bitmap(sprites.cabezaArriba);
    al_destroy_bitmap(sprites.cabezaAbajo);
    al_destroy_bitmap(sprites.cabezaIzquierda);
    al_destroy_bitmap(sprites.cabezaDerecha);

    al_destroy_bitmap(sprites.cuerpoHorizontal);
    al_destroy_bitmap(sprites.cuerpoVertical);

    al_destroy_bitmap(sprites.colaArriba);
    al_destroy_bitmap(sprites.colaAbajo);
    al_destroy_bitmap(sprites.colaIzquierda);
    al_destroy_bitmap(sprites.colaDerecha);

    al_destroy_bitmap(sprites.curva1);
    al_destroy_bitmap(sprites.curva2);
    al_destroy_bitmap(sprites.curva3);
    al_destroy_bitmap(sprites.curva4);

    al_destroy_bitmap(sprites.monoDerecha);
    al_destroy_bitmap(sprites.bananaMono);

    al_destroy_bitmap(sprites.llave);
    al_destroy_bitmap(sprites.puerta);

    for(int i = 0; i < 2; i++)
        al_destroy_bitmap(sprites.perroSprite[i]);

    for(int i = 0; i < FRAMES_GATO; i++)
        al_destroy_bitmap(sprites.gatoSprite[i]);

    for(int i = 0; i < 5; i++)
        al_destroy_bitmap(sprites.frutas[i]);

    al_destroy_bitmap(sprites.bosquePiso);
    al_destroy_bitmap(sprites.bosqueMuro);

    al_destroy_bitmap(sprites.fondoPradera);
    al_destroy_bitmap(sprites.arbusto);
}

void cargarRankingSegmentos()
{
    FILE *archivo = fopen("rankingSegmentos.txt","r");

    if(archivo == NULL)
        return;

    cantidadRankingSegmentos = 0;

    while(fscanf(archivo,"%s %d",
        rankingSegmentos[cantidadRankingSegmentos].nombre,
        &rankingSegmentos[cantidadRankingSegmentos].dato) == 2)
    {
        cantidadRankingSegmentos++;

        if(cantidadRankingSegmentos >= MAX_RANKING)
            break;
    }

    fclose(archivo);
}

void cargarRankingTiempo()
{
    FILE *archivo = fopen("rankingTiempo.txt","r");

    if(archivo == NULL)
        return;

    cantidadRankingTiempo = 0;

    while(fscanf(archivo,"%s %d",
        rankingTiempo[cantidadRankingTiempo].nombre,
        &rankingTiempo[cantidadRankingTiempo].dato) == 2)
    {
        cantidadRankingTiempo++;

        if(cantidadRankingTiempo >= MAX_RANKING)
            break;
    }

    fclose(archivo);
}

void guardarRankingSegmentos()
{
    FILE *archivo = fopen("rankingSegmentos.txt","w");

    if(archivo == NULL)
        return;

    for(int i=0;i<cantidadRankingSegmentos;i++)
    {
        fprintf(archivo,"%s %d\n",
                rankingSegmentos[i].nombre,
                rankingSegmentos[i].dato);
    }

    fclose(archivo);
}

void guardarRankingTiempo()
{
    FILE *archivo = fopen("rankingTiempo.txt", "w");

    if(archivo == NULL)
    {
        return;
    }

    for(int i = 0; i < cantidadRankingTiempo; i++)
    {
        fprintf(archivo,
                "%s %d\n",
                rankingTiempo[i].nombre,
                rankingTiempo[i].dato);
    }

    fclose(archivo);
}

void ordenarRankingSegmentos()
{
    Ranking aux;

    for(int i = 0; i < cantidadRankingSegmentos - 1; i++)
    {
        for(int j = i + 1; j < cantidadRankingSegmentos; j++)
        {
            if(rankingSegmentos[j].dato > rankingSegmentos[i].dato)
            {
                aux = rankingSegmentos[i];
                rankingSegmentos[i] = rankingSegmentos[j];
                rankingSegmentos[j] = aux;
            }
        }
    }
}

void ordenarRankingTiempo()
{
    Ranking aux;

    for(int i = 0; i < cantidadRankingTiempo - 1; i++)
    {
        for(int j = i + 1; j < cantidadRankingTiempo; j++)
        {
            if(rankingTiempo[j].dato < rankingTiempo[i].dato)
            {
                aux = rankingTiempo[i];
                rankingTiempo[i] = rankingTiempo[j];
                rankingTiempo[j] = aux;
            }
        }
    }
}

void generarComidas(int cantidad)
{
    for(int i=0; i < cantidad && i < MAX_COMIDAS; i++)
    {
        int x, y;
    
        do
        {
            x = rand() % M;
            y = rand() % N;
        
        } while(mapa[y][x] != ' ' || haySerpiente(x, y) || hayComida(x, y));

        comidas[i].x = x;
        comidas[i].y = y;
        comidas[i].activa = true;
    }
}

void generarEnemigo(int i)
{
    int x, y;

    do
    {
        x = rand() % M;
        y = rand() % N;

    } while(mapa[y][x] != ' ' ||
            haySerpiente(x, y) ||
            hayComida(x, y) ||
            hayEnemigo(x, y));

    enemigos[i].x = x * CELL;
    enemigos[i].y = y * CELL;

    int dir = rand() % 2;

    if(enemigos[i].tipo == GATO)
    {
        if(dir == 0)
        {
            enemigos[i].dx = 4;
            enemigos[i].dy = 0;
        }
        else
        {
            enemigos[i].dx = -4;
            enemigos[i].dy = 0;
        }
    }
    else if(enemigos[i].tipo == PERRO)
    {
        if(dir == 0)
        {
            enemigos[i].dx = 0;
            enemigos[i].dy = 4;
        }
        else
        {
            enemigos[i].dx = 0;
            enemigos[i].dy = -4;
        }
    }
    else if(enemigos[i].tipo == MONO)
    {
        enemigos[i].dx = 0;
        enemigos[i].dy = 0;

        enemigos[i].tiempoDisparo = 120;
    }

    enemigos[i].frame = 0;
    enemigos[i].contadorAnimacion = 0;
    enemigos[i].vivo = true;
    enemigos[i].respawn = -1;
    enemigos[i].distanciaAnimacion = 0;
}

int hayEnemigo(int x, int y)
{
    for(int i = 0; i < cantidadEnemigos; i++)
    {
        if(enemigos[i].vivo)
        {
            if((int)(enemigos[i].x / CELL) == x &&
               (int)(enemigos[i].y / CELL) == y)
            {
                return 1;
            }
        }
    }

    return 0;
}

void lanzarBanana(int mono)
{
    for(int i = 0; i < MAX_BANANAS; i++)
    {
        if(!bananas[i].activa)
        {
            bananas[i].activa = true;

            //Banana sale del mono
            bananas[i].x = enemigos[mono].x / CELL;
            bananas[i].y = enemigos[mono].y / CELL;

            //Destino cabeza de la serpiente
            bananas[i].destinoX = serpiente.segmentos[0].x;
            bananas[i].destinoY = serpiente.segmentos[0].y;

            if(serpiente.dx == 1)
                bananas[i].destinoX += 4;

            if(serpiente.dx == -1)
                bananas[i].destinoX -= 4;

            if(serpiente.dy == 1)
                bananas[i].destinoY += 4;

            if(serpiente.dy == -1)
                bananas[i].destinoY -= 4;

            //Velocidad de la banana
            bananas[i].velocidadX =
                (bananas[i].destinoX - bananas[i].x) / 10.0;

            bananas[i].velocidadY =
                (bananas[i].destinoY - bananas[i].y) / 10.0;

            break;
        }
    }
}

int verificarColisionMuro()
{
    if(mapa[serpiente.segmentos[0].y][serpiente.segmentos[0].x] == '#')
    {
        return 1;
    }

    return 0;
}

int verificarColisionSerpiente()
{
    for(int i=1; i<serpiente.tamano; i++)
    {
        if(serpiente.segmentos[0].x == serpiente.segmentos[i].x &&
           serpiente.segmentos[0].y == serpiente.segmentos[i].y)
        {
            return 1;
        }
    }

    return 0;
}

int verificarPuertaBloqueada()
{
    if(mapa[serpiente.segmentos[0].y][serpiente.segmentos[0].x] == 'E' && !juego.tieneLlave)
    {
        serpiente.segmentos[0].x -= serpiente.dx;
        serpiente.segmentos[0].y -= serpiente.dy;

        sprintf(juego.mensaje, "La puerta esta cerrada");
        juego.tiempoMensaje = 24;

        return 1;
    }

    return 0;
}

int verificarLimites()
{
    if(serpiente.segmentos[0].x < 0 || serpiente.segmentos[0].x >= M ||
       serpiente.segmentos[0].y < 0 || serpiente.segmentos[0].y >= N)
    {
        return 1;
    }

    return 0;
}

void reiniciarJuego()
{
    serpiente.tamano = 3;
    juego.puntaje = 0;
    juego.tieneLlave = 0;
    juego.tiempo = 0;
    juego.contadorTiempo = 0;

    fase.numero = 1;
    fase.comidasComidas = 0;

    serpiente.dx = 1;
    serpiente.dy = 0;

    juego.nivel = 1;

    sprintf(juego.archivoNivel, "Niveles/nivel%d.txt", juego.nivel);

    strcpy(juego.nombreNivel, "Pradera");

    for(int i = 0; i < MAX_COMIDAS; i++)
    {
        comidas[i].activa = false;
    }

    for(int i=0; i<MAX_BALAS; i++)
    {
        balas[i].activa = false;
    }

    cargarMapa(juego.archivoNivel);

    generarComidas(fase.comidas[fase.numero - 1]);

    for(int i = 0; i < MAX_BANANAS; i++)
    {
        bananas[i].activa = false;
    }
}

void disparar()
{
    for(int i=0; i<MAX_BALAS; i++)
    {
        if(!balas[i].activa)
        {
            balas[i].activa = true;

            balas[i].x = serpiente.segmentos[0].x * CELL + CELL/2;
            balas[i].y = serpiente.segmentos[0].y * CELL + CELL/2;

            if(serpiente.dx == 1)
            {
                balas[i].dx = 8;
                balas[i].dy = 0;

                balas[i].x += CELL/2;
            }
            else if (serpiente.dx == -1)
            {
                balas[i].dx = -8;
                balas[i].dy = 0;

                balas[i].x -= CELL/2;
            }
            else if(serpiente.dy == 1)
            {
                balas[i].dx = 0;
                balas[i].dy = 8;

                balas[i].y += CELL/2;
            }
            else
            {
                balas[i].dx = 0;
                balas[i].dy = -8;

                balas[i].y -= CELL/2;
            }

            break;
        }
    }
}

int haySerpiente(int x, int y)
{
    for(int i=0; i<serpiente.tamano; i++)
    {
        if(serpiente.segmentos[i].x == x && serpiente.segmentos[i].y == y)
        {
            return 1;
        }
    }

    return 0;
}

int hayComida(int x, int y)
{
    for(int i=0; i<MAX_COMIDAS; i++)
    {
        if(comidas[i].activa && comidas[i].x == x && comidas[i].y == y)
        {
            return 1;
        }
    }

    return 0;
}