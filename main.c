#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
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
#define MAX_RANKING 10
#define BALA_SERPIENTE 1
#define BALA_MONO 2
#define RANGO_BALA_SERPIENTE 6
#define RANGO_BANANA_MONO 6
#define CASCARA_BANANA 3

typedef struct
{
    float x;
    float y;

    float dx;
    float dy;

    float velocidadX;
    float velocidadY;

    float destinoX;
    float destinoY;

    float distancia;
    float rango;

    int tipo;

    bool activa;

} Bala;

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

    Bala balas[MAX_BALAS];

    int municion;

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

    char nombreJugador[20];
    int largoNombre;

    int pantalla;
    int opcionMenu;

    int invulnerable;
    int tiempoInvulnerable;

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

    Bala balas[MAX_BALAS];

    bool vivo;

} Enemigo;

typedef struct
{
    //Menu
    ALLEGRO_BITMAP *fondoMenu;

        //Niveles

    //Nivel 1
    ALLEGRO_BITMAP *fondoPradera;
    ALLEGRO_BITMAP *arbusto;

    //Nivel 2
    ALLEGRO_BITMAP *bosquePiso;
    ALLEGRO_BITMAP *bosqueMuro;

    //Nivel 3
    ALLEGRO_BITMAP *desiertoPiso;
    ALLEGRO_BITMAP *desiertoMuro;

    //Nivel 4
    ALLEGRO_BITMAP *icebergPiso;
    ALLEGRO_BITMAP *icebergMuro;

    //Nivel 5
    ALLEGRO_BITMAP *volcanPiso;
    ALLEGRO_BITMAP *volcanMuro;

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
    ALLEGRO_BITMAP *cascarabananaMono;

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

typedef struct
{
    ALLEGRO_SAMPLE *musicaMenu;
} Sonidos;

typedef struct
{
    char mapa[N][M];

    Serpiente serpiente;
    Juego juego;
    Fase fase;
    Sprites sprites;
    Sonidos sonidos;

    Enemigo enemigos[MAX_ENEMIGOS];
    Comida comidas[MAX_COMIDAS];

    Ranking rankingSegmentos[MAX_RANKING];
    Ranking rankingTiempo[MAX_RANKING];

    int cantidadRankingSegmentos;
    int cantidadRankingTiempo;
    int cantidadEnemigos;
    int cursorX;
    int cursorY;
    char bloqueSeleccionado;

} EstadoJuego;

void actualizarJuego(EstadoJuego *estado);
void dibujarMenu(EstadoJuego *estado, ALLEGRO_FONT *font);
void dibujarIngresoNombre(EstadoJuego *estado, ALLEGRO_FONT *font);
void dibujarRanking(EstadoJuego *estado, ALLEGRO_FONT *font);
void dibujarJuego(EstadoJuego *estado, ALLEGRO_FONT *font);
void dibujarEditor(EstadoJuego *estado, ALLEGRO_FONT *font);
void guardarMapaEditor(EstadoJuego *estado);
void limpiarMapaEditor(EstadoJuego *estado);
void cargarMapa(EstadoJuego *estado, char nombreArchivo[]);
void cargarSprites(EstadoJuego *estado);
void destruirSprites(EstadoJuego *estado);
void cargarSonidos(EstadoJuego *estado);
void destruirSonidos(EstadoJuego *estado);
void cargarRankingSegmentos(EstadoJuego *estado);
void guardarRankingSegmentos(EstadoJuego *estado);
void cargarRankingTiempo(EstadoJuego *estado);
void guardarRankingTiempo(EstadoJuego *estado);
void ordenarRankingSegmentos(EstadoJuego *estado);
void ordenarRankingTiempo(EstadoJuego *estado);
void registrarRanking(EstadoJuego *estado);
void reiniciarJuego(EstadoJuego *estado);
void generarComidas(EstadoJuego *estado, int cantidad);
void generarEnemigo(EstadoJuego *estado, int i);
void lanzarBanana(EstadoJuego *estado, int mono);
void disparar(EstadoJuego *estado);
int haySerpiente(EstadoJuego *estado, int x, int y);
int hayComida(EstadoJuego *estado, int x, int y);
int hayEnemigo(EstadoJuego *estado, int x, int y);
int verificarColisionMuro(EstadoJuego *estado);
int verificarColisionSerpiente(EstadoJuego *estado);
int verificarPuertaBloqueada(EstadoJuego *estado);
int verificarLimites(EstadoJuego *estado);

int main()
{
    EstadoJuego estado;
    estado.cantidadEnemigos = 0;
    estado.cantidadRankingSegmentos = 0;
    estado.cantidadRankingTiempo = 0;

    al_init();
    srand(time(NULL));
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_image_addon();

    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(16);

    ALLEGRO_DISPLAY *display =
        al_create_display(WIDTH, HEIGHT);

    ALLEGRO_TIMER *timer =
        al_create_timer(1.0/8);

    ALLEGRO_EVENT_QUEUE *queue =
        al_create_event_queue();

    ALLEGRO_FONT *font =
        al_create_builtin_font();

    cargarSprites(&estado);
    cargarSonidos(&estado);
    cargarRankingSegmentos(&estado);
    cargarRankingTiempo(&estado);

    estado.juego.pantalla = 0;
    estado.juego.opcionMenu = 0;
    estado.juego.largoNombre = 0;
    estado.juego.nombreJugador[0] = '\0';

    al_play_sample(
        estado.sonidos.musicaMenu,
        1.0,
        0.0,
        1.0,
        ALLEGRO_PLAYMODE_LOOP,
        NULL);

    al_install_keyboard();

    al_set_keyboard_leds(0);

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

    int running = 1;

    while(running)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue,&ev);

        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            running = 0;

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if(estado.juego.pantalla == 0)
            {
                switch(ev.keyboard.keycode)
                {
                    case ALLEGRO_KEY_UP:

                        estado.juego.opcionMenu--;
                        
                        if(estado.juego.opcionMenu < 0)
                            estado.juego.opcionMenu = 3;

                        break;

                    case ALLEGRO_KEY_DOWN:

                        estado.juego.opcionMenu++;

                        if(estado.juego.opcionMenu > 3)
                            estado.juego.opcionMenu = 0;

                        break;

                    case ALLEGRO_KEY_ENTER:

                        if(estado.juego.opcionMenu == 0)
                            estado.juego.pantalla = 1;

                        else if(estado.juego.opcionMenu == 1)
                        {
                            estado.juego.pantalla = 4;

                            estado.cursorX = 1;
                            estado.cursorY = 1;

                            estado.bloqueSeleccionado = '#';

                            limpiarMapaEditor(&estado);
                        }
                        else if(estado.juego.opcionMenu == 2)
                            estado.juego.pantalla = 2;

                        else
                            running = 0;

                        break;
                }

                continue;
            }

            if(estado.juego.pantalla == 2)
            {
                if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    estado.juego.pantalla = 0;
                }

                continue;
            }

            if(estado.juego.pantalla == 4)
            {
                switch(ev.keyboard.keycode)
                {
                    case ALLEGRO_KEY_F5:

                        guardarMapaEditor(&estado);
                        break;

                    case ALLEGRO_KEY_ESCAPE:

                        estado.juego.pantalla = 0;
                        break;

                    case ALLEGRO_KEY_LEFT:

                        if(estado.cursorX > 0)
                        {
                            estado.cursorX--;
                        }

                        break;

                    case ALLEGRO_KEY_RIGHT:

                        if(estado.cursorX < M - 1)
                        {
                            estado.cursorX++;
                        }

                        break;

                    case ALLEGRO_KEY_UP:

                        if(estado.cursorY > 0)
                        {
                            estado.cursorY--;
                        }

                        break;

                    case ALLEGRO_KEY_DOWN:

                        if(estado.cursorY < N - 1)
                        {
                            estado.cursorY++;
                        }

                        break;

                    case ALLEGRO_KEY_1:

                        estado.bloqueSeleccionado = '#';
                        break;

                    case ALLEGRO_KEY_2:

                        estado.bloqueSeleccionado = 'S';
                        break;

                    case ALLEGRO_KEY_3:

                        estado.bloqueSeleccionado = 'L';
                        break;

                    case ALLEGRO_KEY_4:

                        estado.bloqueSeleccionado = 'E';
                        break;

                    case ALLEGRO_KEY_5:

                        estado.bloqueSeleccionado = 'G';
                        break;

                    case ALLEGRO_KEY_6:

                        estado.bloqueSeleccionado = 'P';
                        break;

                    case ALLEGRO_KEY_7:

                        estado.bloqueSeleccionado = 'M';
                        break;

                    case ALLEGRO_KEY_8:

                        estado.bloqueSeleccionado = 'C';
                        break;

                    case ALLEGRO_KEY_0:

                        estado.bloqueSeleccionado = ' ';
                        break;

                    case ALLEGRO_KEY_SPACE:

                        estado.mapa[estado.cursorY][estado.cursorX] = estado.bloqueSeleccionado;
                        break;
                }

                continue;
            }

            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_UP:
                    if(estado.serpiente.dy != 1)
                    {
                        estado.serpiente.dx = 0;
                        estado.serpiente.dy = -1;
                    }
                    break;

                case ALLEGRO_KEY_DOWN:
                    if(estado.serpiente.dy != -1)
                    {
                        estado.serpiente.dx = 0;
                        estado.serpiente.dy = 1;
                    }
                    break;

                case ALLEGRO_KEY_LEFT:
                    if(estado.serpiente.dx != 1)
                    {
                        estado.serpiente.dx = -1;
                        estado.serpiente.dy = 0;
                    }
                    break;

                case ALLEGRO_KEY_RIGHT:
                    if(estado.serpiente.dx != -1)
                    {
                        estado.serpiente.dx = 1;
                        estado.serpiente.dy = 0;
                    }
                    break;

                case ALLEGRO_KEY_SPACE:

                    if(estado.serpiente.municion > 0)
                    {
                        disparar(&estado);
                        estado.serpiente.municion--;
                    }

                break;
            }
        }

        if(ev.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if(estado.juego.pantalla == 1)
            {
                int tecla = ev.keyboard.unichar;

                if(tecla >= 32 && tecla <= 126)
                {
                    if(estado.juego.largoNombre < 19)
                    {
                        estado.juego.nombreJugador[estado.juego.largoNombre] = tecla;
                        estado.juego.largoNombre++;
                        estado.juego.nombreJugador[estado.juego.largoNombre] = '\0';
                    }
                }
                else if(tecla == 8)
                {
                    if(estado.juego.largoNombre > 0)
                    {
                        estado.juego.largoNombre--;
                        estado.juego.nombreJugador[estado.juego.largoNombre] = '\0';
                    }
                }
                else if(tecla == 13)
                {
                    if(estado.juego.largoNombre > 0)
                    {
                        al_stop_samples();

                        reiniciarJuego(&estado);
                        estado.juego.pantalla = 3;
                    }
                }
            }
        }

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            actualizarJuego(&estado);

            if(estado.juego.pantalla == 0)
            {
                dibujarMenu(&estado, font);
            }
            else if(estado.juego.pantalla == 1)
            {
                dibujarIngresoNombre(&estado, font);
            }
            else if(estado.juego.pantalla == 2)
            {
                dibujarRanking(&estado, font);
            }
            else if(estado.juego.pantalla == 3)
            {
                dibujarJuego(&estado, font);
            }
            else if(estado.juego.pantalla == 4)
            {
                dibujarEditor(&estado, font);
            }

            al_flip_display();
        }
    }

    destruirSprites(&estado);
    destruirSonidos(&estado);

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}

void actualizarJuego(EstadoJuego *estado)
{
    if(estado->juego.pantalla != 3)
    {
        return;
    }

    for(int i = 0; i < estado->cantidadEnemigos; i++)
    {
        //Respawn del Gato

        if(!estado->enemigos[i].vivo)
        {
            if(estado->enemigos[i].respawn > 0)
            {
                estado->enemigos[i].respawn--;
            }
            else if(estado->enemigos[i].respawn == 0)
            {
                if(estado->juego.nivel == 1)
                {
                    generarEnemigo(estado, i);
                }
            }

            continue;
        }

        if(estado->enemigos[i].vivo)
        {
            //Mono Lanza Bananas
            if(estado->enemigos[i].tipo == MONO)
            {
                estado->enemigos[i].tiempoDisparo--;

                if(estado->enemigos[i].tiempoDisparo <=0)
                {
                    lanzarBanana(estado, i);

                    estado->enemigos[i].tiempoDisparo = 120;
                }
            } 

            //Movimiento
            float nuevoX = estado->enemigos[i].x + estado->enemigos[i].dx;
            float nuevoY = estado->enemigos[i].y + estado->enemigos[i].dy;

            int columna = (nuevoX + CELL/2) / CELL;
            int fila = (nuevoY + CELL/2) / CELL;

            if(estado->mapa[fila][columna] == '#')
            {
                if(estado->enemigos[i].tipo == GATO)
                {
                    estado->enemigos[i].dx *= -1;
                }
                else
                {
                    estado->enemigos[i].dy *= -1;
                }
            }
            else
            {
                estado->enemigos[i].x = nuevoX;
                estado->enemigos[i].y = nuevoY;
            }

            int enemigoX = (estado->enemigos[i].x + CELL / 2) / CELL;
            int enemigoY = (estado->enemigos[i].y + CELL / 2) / CELL;

            //Enemigo Mata a la Serpiente
            for(int k = 0; k < estado->serpiente.tamano; k++)
            {
                if(enemigoX == estado->serpiente.segmentos[k].x &&
                   enemigoY == estado->serpiente.segmentos[k].y)
                {
                    if(!estado->juego.invulnerable)
                    {
                        if(estado->serpiente.tamano > 2)
                        {
                            estado->serpiente.tamano--;

                            estado->juego.invulnerable = 1;
                            estado->juego.tiempoInvulnerable = 16;
                        }
                        else
                        {
                            registrarRanking(estado);
                            reiniciarJuego(estado);
                            return;
                        }
                    }
                }
            }

            //Animacion

            if(estado->enemigos[i].tipo == GATO)
            {
                estado->enemigos[i].distanciaAnimacion +=
                fabs(estado->enemigos[i].dx) + fabs(estado->enemigos[i].dy);

                if(estado->enemigos[i].distanciaAnimacion >= 16)
                {
                    estado->enemigos[i].distanciaAnimacion = 0;

                    if(estado->enemigos[i].dx > 0)
                    {
                        estado->enemigos[i].frame++;

                        if(estado->enemigos[i].frame > 5)
                           estado->enemigos[i].frame = 0;
                    }
                    else
                    {
                        estado->enemigos[i].frame++;

                        if(estado->enemigos[i].frame < 6 || estado->enemigos[i].frame > 11)
                           estado->enemigos[i].frame = 6;
                    }
                }        
            }
        }
    }

    //Movimiento de bananas
    for(int i = 0; i < estado->cantidadEnemigos; i++)
    {
        for(int j = 0; j < MAX_BALAS; j++)
        {
            if(estado->enemigos[i].balas[j].activa)
            {
                estado->enemigos[i].balas[j].x += estado->enemigos[i].balas[j].velocidadX;
                estado->enemigos[i].balas[j].y += estado->enemigos[i].balas[j].velocidadY;
                if(estado->enemigos[i].balas[j].velocidadX != 0 ||
                   estado->enemigos[i].balas[j].velocidadY != 0)
                {
                        estado->enemigos[i].balas[j].distancia +=
                            sqrt(estado->enemigos[i].balas[j].velocidadX * estado->enemigos[i].balas[j].velocidadX +
                            estado->enemigos[i].balas[j].velocidadY * estado->enemigos[i].balas[j].velocidadY);
                }

                if(estado->enemigos[i].balas[j].distancia >= estado->enemigos[i].balas[j].rango)
                {
                    estado->enemigos[i].balas[j].velocidadX = 0;
                    estado->enemigos[i].balas[j].velocidadY = 0;

                    estado->enemigos[i].balas[j].distancia =
                    estado->enemigos[i].balas[j].rango;
                }

                if(fabs(estado->enemigos[i].balas[j].x - estado->enemigos[i].balas[j].destinoX) < 0.2 &&
                   fabs(estado->enemigos[i].balas[j].y - estado->enemigos[i].balas[j].destinoY) < 0.2)
                {
                    estado->enemigos[i].balas[j].x = estado->enemigos[i].balas[j].destinoX;
                    estado->enemigos[i].balas[j].y = estado->enemigos[i].balas[j].destinoY;

                    estado->enemigos[i].balas[j].velocidadX = 0;
                    estado->enemigos[i].balas[j].velocidadY = 0;
                }

                int bananaX = estado->enemigos[i].balas[j].x / CELL;
                int bananaY = estado->enemigos[i].balas[j].y / CELL;

                for (int k = 0; k < estado->serpiente.tamano; k++)
                {
                    if(bananaX == estado->serpiente.segmentos[k].x &&
                       bananaY == estado->serpiente.segmentos[k].y)
                    {
                        if(!estado->juego.invulnerable)
                        {
                            if(estado->serpiente.tamano > 2)
                            {
                                estado->serpiente.tamano--;

                                estado->juego.invulnerable = 1;
                                estado->juego.tiempoInvulnerable = 16;
                            }
                            else
                            {
                                registrarRanking(estado);
                                reiniciarJuego(estado);
                                return;
                            }
                        }

                        estado->enemigos[i].balas[j].activa = false;
                        break;
                    }
                }
            }
        }
    }

    for(int i =0; i < MAX_BALAS; i++)
    {
        if(estado->serpiente.balas[i].activa)
        {
            estado->serpiente.balas[i].x += estado->serpiente.balas[i].dx;
            estado->serpiente.balas[i].y += estado->serpiente.balas[i].dy;

            estado->serpiente.balas[i].distancia +=
            abs((int)estado->serpiente.balas[i].dx) +
            abs((int)estado->serpiente.balas[i].dy);

            if(estado->serpiente.balas[i].distancia >= estado->serpiente.balas[i].rango)
            {
                estado->serpiente.balas[i].activa = false;
                continue;
            }

            int columna = estado->serpiente.balas[i].x / CELL;
            int fila = estado->serpiente.balas[i].y / CELL;

            if(columna < 0 || columna >= M ||
               fila < 0 || fila >= N)
            {
                estado->serpiente.balas[i].activa = false;
                continue;
            }

            if(estado->mapa[fila][columna] == '#')
            {
                estado->serpiente.balas[i].activa = false;
            }

            if(estado->serpiente.balas[i].x < 0 ||
               estado->serpiente.balas[i].x > M * CELL ||
               estado->serpiente.balas[i].y < 0 ||
               estado->serpiente.balas[i].y > N * CELL)
            {
                estado->serpiente.balas[i].activa = false;
            }
        }    
    }

    for(int i = 0; i < MAX_BALAS; i++)
    {
        if(estado->serpiente.balas[i].activa)
        {
            for(int j = 0; j < estado->cantidadEnemigos; j++)
            {
                if(estado->enemigos[j].vivo)
                {
                    float dx = estado->serpiente.balas[i].x - (estado->enemigos[j].x + CELL/2);
                    float dy = estado->serpiente.balas[i].y - (estado->enemigos[j].y + CELL/2);

                    if(dx*dx + dy*dy < (CELL/2)*(CELL/2))
                    {
                        estado->serpiente.balas[i].activa = false;
                        estado->enemigos[j].vivo = false;
                        estado->enemigos[j].respawn = 120;
                        break;
                    }
                }
            }
        }
    }

    for(int i = estado->serpiente.tamano - 1; i > 0; i--)
    {
        estado->serpiente.segmentos[i] = estado->serpiente.segmentos[i-1];
    }

    estado->serpiente.segmentos[0].x += estado->serpiente.dx;
    estado->serpiente.segmentos[0].y += estado->serpiente.dy;

    if(verificarLimites(estado))
    {
        printf("GAME OVER\n");

        registrarRanking(estado);
        reiniciarJuego(estado);
        return;
    }

    if(verificarColisionMuro(estado))
    {
        printf("GAME OVER\n");

        registrarRanking(estado);
        reiniciarJuego(estado);
        return;
    }

    if(verificarColisionSerpiente(estado))
    {
        printf("GAME OVER\n");

        registrarRanking(estado);
        reiniciarJuego(estado);
        return;
    }

    verificarPuertaBloqueada(estado);

    int comidaComida = 0;

    for(int i=0; i<MAX_COMIDAS; i++)
    {
        if(estado->comidas[i].activa && estado->serpiente.segmentos[0].x == estado->comidas[i].x && estado->serpiente.segmentos[0].y == estado->comidas[i].y)
        {
            estado->comidas[i].activa = false;

            estado->serpiente.segmentos[estado->serpiente.tamano] = estado->serpiente.segmentos[estado->serpiente.tamano-1];

            estado->serpiente.tamano++;
            estado->juego.puntaje++;

            if(estado->serpiente.municion < 5)
            {
                estado->serpiente.municion++;
            }

            estado->fase.comidasComidas++;
            comidaComida = 1;
            break;
        }
    }

    if(comidaComida)
    {
        int quedan = 0;

        for(int i=0; i<MAX_COMIDAS; i++)
        {
            if(estado->comidas[i].activa)
            {
                quedan++;
            }
        }

        if(quedan == 0)
        {
            if(estado->fase.numero < 3)
            {
                estado->fase.numero++;
                estado->fase.comidasComidas = 0;

                sprintf(estado->juego.mensaje, "Fase %d", estado->fase.numero);
                estado->juego.tiempoMensaje = 24;

                generarComidas(estado, estado->fase.comidas[estado->fase.numero - 1]);
            }
            else
            {
                estado->mapa[estado->juego.yLlave][estado->juego.xLlave] = 'L';
                strcpy(estado->juego.mensaje, "Ya puedes recoger la llave");
                estado->juego.tiempoMensaje = 24;
            }
        }
    }

    if(estado->mapa[estado->serpiente.segmentos[0].y][estado->serpiente.segmentos[0].x] == 'L')
    {
        estado->juego.tieneLlave = 1;

        estado->mapa[estado->serpiente.segmentos[0].y][estado->serpiente.segmentos[0].x] = ' ';

        strcpy(estado->juego.mensaje, "Llave obtenida");
        estado->juego.tiempoMensaje = 24;
    }

    if(estado->mapa[estado->serpiente.segmentos[0].y][estado->serpiente.segmentos[0].x] == 'E' && estado->juego.tieneLlave)
    {
        if(estado->juego.nivel < 5)
        {
            estado->juego.nivel++;
        }
        else
        {
            registrarRanking(estado);
            estado->juego.pantalla = 0;
            reiniciarJuego(estado);
            return;
        }
        sprintf(estado->juego.archivoNivel, "Niveles/nivel%d.txt", estado->juego.nivel);
        cargarMapa(estado, estado->juego.archivoNivel);

        for(int i = 0; i < estado->cantidadEnemigos; i++)
        {
            for(int j = 0; j < MAX_BALAS; j++)
            {
                estado->enemigos[i].balas[j].activa = false;
            }    
        }

        switch(estado->juego.nivel)
        {
            case 1:
            strcpy(estado->juego.nombreNivel, "Pradera");
            break;

            case 2:
            strcpy(estado->juego.nombreNivel, "Bosque");
            break;

            case 3:
            strcpy(estado->juego.nombreNivel, "Desierto");
            break;

            case 4:
            strcpy(estado->juego.nombreNivel, "Iceberg");
            break;

            case 5:
            strcpy(estado->juego.nombreNivel, "Volcan");
            break;
        }

        estado->fase.numero = 1;
        estado->fase.comidasComidas = 0;

        for(int i=0; i<MAX_COMIDAS; i++)
        {
            estado->comidas[i].activa = false;
        }

        generarComidas(estado, estado->fase.comidas[estado->fase.numero - 1]);

        estado->juego.tieneLlave = 0;

        estado->serpiente.dx = 1;
        estado->serpiente.dy = 0;

        sprintf(estado->juego.mensaje, "Bienvenido a %s", estado->juego.nombreNivel);
        estado->juego.tiempoMensaje = 24;
    }

    estado->juego.contadorTiempo++;

    if(estado->juego.contadorTiempo >= 8)
    {
        estado->juego.tiempo++;
        estado->juego.contadorTiempo = 0;
    }

    if(estado->juego.tiempoMensaje > 0)
    {
        estado->juego.tiempoMensaje--;
    }

    if(estado->juego.invulnerable)
    {
        estado->juego.tiempoInvulnerable--;

        if(estado->juego.tiempoInvulnerable <= 0)
        {
            estado->juego.invulnerable = 0;
        }
    }
}

void dibujarMenu(EstadoJuego *estado, ALLEGRO_FONT *font)
{

    al_draw_scaled_bitmap(
        estado->sprites.fondoMenu,
        0, 0,
        al_get_bitmap_width(estado->sprites.fondoMenu),
        al_get_bitmap_height(estado->sprites.fondoMenu),
        0, 0,
        WIDTH,
        HEIGHT,
        0);

    ALLEGRO_COLOR color;

    if(estado->juego.opcionMenu == 0)
    {
        color = al_map_rgb(255,255,0);
    }
    else
    {
        color = al_map_rgb(255,255,255);
    }

    al_draw_text(
        font,
        color,
        WIDTH/2,
        200,
        ALLEGRO_ALIGN_CENTER,
        "Jugar");

    if(estado->juego.opcionMenu == 1)
    {
        color = al_map_rgb(255,255,0);
    }
    else
    {
        color = al_map_rgb(255,255,255);
    }

    al_draw_text(
        font,
        color,
        WIDTH/2,
        240,
        ALLEGRO_ALIGN_CENTER,
        "Editor de Mapas");

    if(estado->juego.opcionMenu == 2)
    {
        color = al_map_rgb(255,255,0);
    }
    else
    {
        color = al_map_rgb(255,255,255);
    }

    al_draw_text(
        font,
        color,
        WIDTH/2,
        280,
        ALLEGRO_ALIGN_CENTER,
        "Ranking");

    if(estado->juego.opcionMenu == 3)
    {
        color = al_map_rgb(255,255,0);
    }
    else
    {
        color = al_map_rgb(255,255,255);
    }

    al_draw_text(
        font,
        color,
        WIDTH/2,
        320,
        ALLEGRO_ALIGN_CENTER,
        "Salir");
}

void dibujarIngresoNombre(EstadoJuego *estado, ALLEGRO_FONT *font)
{
    al_draw_scaled_bitmap(
        estado->sprites.fondoMenu,
        0, 0,
        al_get_bitmap_width(estado->sprites.fondoMenu),
        al_get_bitmap_height(estado->sprites.fondoMenu),
        0, 0,
        WIDTH,
        HEIGHT,
        0);

    al_draw_text(
        font,
        al_map_rgb(255,255,255),
        WIDTH/2,
        200,
        ALLEGRO_ALIGN_CENTER,
        "Ingrese su nombre");

    al_draw_text(
        font,
        al_map_rgb(255,255,0),
        WIDTH/2,
        240,
        ALLEGRO_ALIGN_CENTER,
        estado->juego.nombreJugador);
}

void dibujarRanking(EstadoJuego *estado, ALLEGRO_FONT *font)
{
    al_draw_scaled_bitmap(
        estado->sprites.fondoMenu,
        0, 0,
        al_get_bitmap_width(estado->sprites.fondoMenu),
        al_get_bitmap_height(estado->sprites.fondoMenu),
        0, 0,
        WIDTH,
        HEIGHT,
        0);

    // Panel izquierdo
    al_draw_filled_rectangle(
        70, 190,
        340, 450,
        al_map_rgba(0, 0, 0, 180));

    // Panel derecho
    al_draw_filled_rectangle(
        420, 190,
        730, 450,
        al_map_rgba(0, 0, 0, 180));

    al_draw_text(
        font,
        al_map_rgb(255,255,255),
        WIDTH/2,
        170,
        ALLEGRO_ALIGN_CENTER,
        "RANKING");

    al_draw_text(
        font,
        al_map_rgb(255,255,0),
        100,
        210,
        0,
        "PUNTAJE");

    for(int i = 0; i < estado->cantidadRankingSegmentos && i < 10; i++)
    {
        char texto[50];

        sprintf(texto,
        "%d. %s - %d",
        i+1,
        estado->rankingSegmentos[i].nombre,
        estado->rankingSegmentos[i].dato);

        al_draw_text(
            font,
            al_map_rgb(255,255,255),
            100,
            240+i*20,
            0,
            texto);
    }

    al_draw_text(
        font,
        al_map_rgb(255,255,0),
        450,
        210,
        0,
        "TIEMPO");

    for(int i = 0; i < estado->cantidadRankingTiempo && i < 10; i++)
    {
        char texto[50];

        int minutos = estado->rankingTiempo[i].dato / 60;
        int segundos = estado->rankingTiempo[i].dato % 60;

        sprintf(texto,
        "%d. %s - %02d:%02d",
        i+1,
        estado->rankingTiempo[i].nombre,
        minutos,
        segundos);

        al_draw_text(
            font,
            al_map_rgb(255,255,255),
            450,
            240+i*20,
            0,
            texto);
    }
}

void dibujarJuego(EstadoJuego *estado, ALLEGRO_FONT *font)
{
    al_clear_to_color(
    al_map_rgb(136,231,136));

    if(estado->juego.nivel == 1)
    {
        for(int i=0;i<N;i++)
        {
            for(int j=0;j<M;j++)
            {
                al_draw_scaled_bitmap(
                    estado->sprites.fondoPradera,
                    0,0,64,64,
                    j*CELL,i*CELL,
                    CELL,CELL,0);
            }
        }
    }
    else if(estado->juego.nivel == 2)
    {
        for(int i=0;i<N;i++)
        {
            for(int j=0;j<M;j++)
            {
                al_draw_scaled_bitmap(
                    estado->sprites.bosquePiso,
                    0,0,64,64,
                    j*CELL,i*CELL,
                    CELL,CELL,0);
            }
        }
    }
    else if(estado->juego.nivel == 3)
    {
        for(int i=0;i<N;i++)
        {
            for(int j=0;j<M;j++)
            {
                al_draw_scaled_bitmap(
                    estado->sprites.desiertoPiso,
                    0,0,64,64,
                    j*CELL,i*CELL,
                    CELL,CELL,0);
            }
        }
    }
    else if(estado->juego.nivel == 4)
    {
        for(int i=0;i<N;i++)
        {
            for(int j=0;j<M;j++)
            {
                al_draw_scaled_bitmap(
                    estado->sprites.icebergPiso,
                    0,0,64,64,
                    j*CELL,i*CELL,
                    CELL,CELL,0);
            }
        }
    }
    else if(estado->juego.nivel == 5)
    {
        for(int i=0;i<N;i++)
        {
            for(int j=0;j<M;j++)
            {
                al_draw_scaled_bitmap(
                    estado->sprites.volcanPiso,
                    0,0,64,64,
                    j*CELL,i*CELL,
                    CELL,CELL,0);
            }
        }
    }            

    for(int i=0; i<N; i++)
    {
        for(int j=0; j<M; j++)
        {

            char casilla = estado->mapa[i][j];

            switch(casilla)
            {
                //Muros
                case '#':

                if(estado->juego.nivel == 1)
                {
                    al_draw_scaled_bitmap(
                        estado->sprites.arbusto,
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
                else if(estado->juego.nivel == 2)
                {
                    al_draw_scaled_bitmap(
                        estado->sprites.bosqueMuro,
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
                else if(estado->juego.nivel == 3)
                {
                    al_draw_scaled_bitmap(
                        estado->sprites.desiertoMuro,
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
                else if(estado->juego.nivel == 4)
                {
                    al_draw_scaled_bitmap(
                        estado->sprites.icebergMuro,
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
                else if(estado->juego.nivel == 5)
                {
                    al_draw_scaled_bitmap(
                        estado->sprites.volcanMuro,
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

                //Llave
                case 'L':

                if(!estado->juego.tieneLlave)
                {
                    al_draw_scaled_bitmap(
                        estado->sprites.llave,
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
                    estado->sprites.puerta,
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
        if(estado->comidas[i].activa)
        {
            ALLEGRO_BITMAP *fruta = estado->sprites.frutas[estado->juego.nivel - 1];

            al_draw_scaled_bitmap(
                fruta,
                0,
                0,
                64,
                64,
                estado->comidas[i].x*CELL,
                estado->comidas[i].y*CELL,
                CELL,
                CELL,
                0);
        }
    }

    for(int i = 0; i < estado->cantidadEnemigos; i++)
    {
        if(estado->enemigos[i].vivo)
        {
            ALLEGRO_BITMAP *sprite;

            if(estado->enemigos[i].tipo == GATO)
            {
                sprite = estado->sprites.gatoSprite[estado->enemigos[i].frame];
            }
            else if(estado->enemigos[i].tipo == PERRO)
            {
                if(estado->enemigos[i].dy < 0)
                {
                    sprite = estado->sprites.perroSprite[0];
                }
                else
                {
                    sprite = estado->sprites.perroSprite[1];
                }
            }
        
            if(estado->enemigos[i].tipo == MONO)
            {
                sprite = estado->sprites.monoDerecha;
            }

            al_draw_scaled_bitmap(
                sprite,
                0,
                0,
                64,
                64,
                estado->enemigos[i].x,
                estado->enemigos[i].y,
                CELL,
                CELL,
                0);
        }
    }

    for(int i = 0; i < estado->cantidadEnemigos; i++)
    {
        for(int j = 0; j < MAX_BALAS; j++)
        {
            if(estado->enemigos[i].balas[j].activa)
            {

                    al_draw_scaled_bitmap(
                        estado->sprites.bananaMono,
                        0,0,64,64,
                        estado->enemigos[i].balas[j].x - CELL/2,
                        estado->enemigos[i].balas[j].y - CELL/2,
                        CELL,CELL,0);
            }
        }
    }

    for(int i =0; i < MAX_BALAS; i++)
    {
        if(estado->serpiente.balas[i].activa)
        {
            al_draw_filled_circle(
                estado->serpiente.balas[i].x,
                estado->serpiente.balas[i].y,
                5,
                al_map_rgb(0, 255, 0));
        }
    }

    bool dibujarSerpiente = true;

    if(estado->juego.invulnerable)
    {
        dibujarSerpiente =
        (estado->juego.tiempoInvulnerable % 2 == 0);
    }

    if(dibujarSerpiente)
    {
        for(int i = 0; i < estado->serpiente.tamano; i++)
        {
            if(i == 0)
            {
                ALLEGRO_BITMAP *cabeza;

                if(estado->serpiente.dx == 1)
                    cabeza = estado->sprites.cabezaDerecha;
                else if(estado->serpiente.dx == -1)
                    cabeza = estado->sprites.cabezaIzquierda;
                else if(estado->serpiente.dy == -1)
                    cabeza = estado->sprites.cabezaArriba;
                else
                    cabeza = estado->sprites.cabezaAbajo;

                al_draw_scaled_bitmap(
                    cabeza,
                    0,
                    0,
                    64,
                    64,
                    estado->serpiente.segmentos[i].x * CELL,
                    estado->serpiente.segmentos[i].y * CELL,
                    CELL,
                    CELL,
                    0);
            }
            else if(i == estado->serpiente.tamano - 1)
            {
                ALLEGRO_BITMAP *cola;

                if(estado->serpiente.tamano == 1)
                    cola = estado->sprites.colaAbajo;
                else
                {
                    int dx = estado->serpiente.segmentos[i].x - estado->serpiente.segmentos[i-1].x;
                    int dy = estado->serpiente.segmentos[i].y - estado->serpiente.segmentos[i-1].y;

                    if(dx == 0 && dy == 0)
                    {            
                        if(estado->serpiente.dx == 1)
                            cola = estado->sprites.colaDerecha;
                        else if(estado->serpiente.dx == -1)
                            cola = estado->sprites.colaIzquierda;
                        else if(estado->serpiente.dy == 1)
                            cola = estado->sprites.colaAbajo;
                        else
                            cola = estado->sprites.colaArriba;
                    }
                    else if(dx == 1)
                    {
                        cola = estado->sprites.colaDerecha;
                    }
                    else if(dx == -1)
                    {
                        cola = estado->sprites.colaIzquierda;
                    }
                    else if(dy == 1)
                    {
                        cola = estado->sprites.colaAbajo;
                    }
                    else
                    {
                        cola = estado->sprites.colaArriba;
                    }
                }

                al_draw_scaled_bitmap(
                    cola,
                    0,
                    0,
                    64,
                    64,
                    estado->serpiente.segmentos[i].x*CELL,
                    estado->serpiente.segmentos[i].y*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else
            {
                int dx1 = estado->serpiente.segmentos[i].x - estado->serpiente.segmentos[i-1].x;
                int dy1 = estado->serpiente.segmentos[i].y - estado->serpiente.segmentos[i-1].y;

                int dx2 = estado->serpiente.segmentos[i+1].x - estado->serpiente.segmentos[i].x;
                int dy2 = estado->serpiente.segmentos[i+1].y - estado->serpiente.segmentos[i].y;

                ALLEGRO_BITMAP *imagen = estado->sprites.cuerpoHorizontal;

                // Cuerpo recto

                if(dy1 == 0 && dy2 == 0)
                {
                    // Se mueve de izquierda a derecha
                    imagen = estado->sprites.cuerpoHorizontal;
                }
                else if(dx1 == 0 && dx2 == 0)
                {
                    // Se mueve de arriba a abajo
                    imagen = estado->sprites.cuerpoVertical;
                }

                // Curvas

                else if((dx1==1 && dy2==1) || (dy1==-1 && dx2==-1))
                {
                    imagen = estado->sprites.curva2;
                }
                else if((dx1==-1 && dy2==1) || (dy1==-1 && dx2==1))
                {
                    imagen = estado->sprites.curva1;
                }
                else if((dx1==-1 && dy2==-1) || (dy1==1 && dx2==1))
                {
                    imagen = estado->sprites.curva3;
                }
                else
                {
                    imagen = estado->sprites.curva4;
                }

                al_draw_scaled_bitmap(
                    imagen,
                    0,
                    0,
                    64,
                    64,
                    estado->serpiente.segmentos[i].x*CELL,
                    estado->serpiente.segmentos[i].y*CELL,
                    CELL,
                    CELL,
                    0);
            }
        }
    }           
    //Nivel en Pantalla

    char textoNivel[50];

    sprintf(textoNivel,
    "Nivel: %d",
    estado->juego.nivel);

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
    estado->juego.puntaje);

    al_draw_text(
        font,
        al_map_rgb(255,255,255),
        10,
        10,
        0,
        texto);

    //Tiempo en Pantalla

    char textoTiempo[100];

    int minutos = estado->juego.tiempo / 60;
    int segundos = estado->juego.tiempo % 60;

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

    if(estado->juego.tieneLlave)
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
    estado->fase.numero,
    estado->fase.comidasComidas,
    estado->fase.comidas[estado->fase.numero - 1]);

    al_draw_text(
        font,
        al_map_rgb(255,255,255),
        10,
        50,
        0,
        textoComidas);

    //Tiempo en pantalla del mensaje

    if(estado->juego.tiempoMensaje > 0)
    {
        al_draw_text(
            font,
            al_map_rgb(255,255,0),
            WIDTH/2,
            HEIGHT-30,
            ALLEGRO_ALIGN_CENTER,
            estado->juego.mensaje);
    }

    //Municion en Pantalla

    char textoBalas[50];

    sprintf(textoBalas,
    "Balas: %d",
    estado->serpiente.municion);

    al_draw_text(
        font,
        al_map_rgb(255,255,255),
        10,
        110,
        0,
        textoBalas);
}

void cargarMapa(EstadoJuego *estado, char nombreArchivo[])
{
    estado->cantidadEnemigos = 0;

    FILE *archivo = fopen(nombreArchivo, "r");

    if(archivo == NULL)
    {
        printf("No se pudo abrir el archivo %s\n", nombreArchivo);
        return;
    } 

    for(int i = 0; i<MAX_FASES; i++)
    {
        fscanf(archivo, "%d", &estado->fase.comidas[i]);
    }

    fgetc(archivo);

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
        {
            estado->mapa[i][j] = fgetc(archivo);

            if(estado->mapa[i][j] == 'S')
            {
                for(int k = 0; k < estado->serpiente.tamano; k++)
                {
                    estado->serpiente.segmentos[k].x = j - k;
                    estado->serpiente.segmentos[k].y = i;
                }

                estado->mapa[i][j] = ' ';
            }

            if(estado->mapa[i][j] == 'L')
            {
                estado->juego.xLlave = j;
                estado->juego.yLlave = i;

                estado->mapa[i][j] = ' ';
            }

            if(estado->mapa[i][j] == 'G')
            {
                if(estado->cantidadEnemigos < MAX_ENEMIGOS)
                {
                    estado->enemigos[estado->cantidadEnemigos].x = j * CELL;
                    estado->enemigos[estado->cantidadEnemigos].y = i * CELL;

                    estado->enemigos[estado->cantidadEnemigos].dx = 4;
                    estado->enemigos[estado->cantidadEnemigos].dy = 0;

                    estado->enemigos[estado->cantidadEnemigos].frame = 0;
                    estado->enemigos[estado->cantidadEnemigos].contadorAnimacion = 0;

                    estado->enemigos[estado->cantidadEnemigos].vivo = true;
                    estado->enemigos[estado->cantidadEnemigos].respawn = 0;

                    estado->enemigos[estado->cantidadEnemigos].tipo = GATO;

                    estado->cantidadEnemigos++;
                }

                estado->mapa[i][j] = ' ';
            }

            if(estado->mapa[i][j] == 'P')
            {
                if(estado->cantidadEnemigos < MAX_ENEMIGOS)
                {
                    estado->enemigos[estado->cantidadEnemigos].x = j * CELL;
                    estado->enemigos[estado->cantidadEnemigos].y = i * CELL;

                    estado->enemigos[estado->cantidadEnemigos].dx = 0;
                    estado->enemigos[estado->cantidadEnemigos].dy = 4;

                    estado->enemigos[estado->cantidadEnemigos].frame = 0;
                    estado->enemigos[estado->cantidadEnemigos].contadorAnimacion = 0;

                    estado->enemigos[estado->cantidadEnemigos].vivo = true;
                    estado->enemigos[estado->cantidadEnemigos].respawn = 0;

                    estado->enemigos[estado->cantidadEnemigos].distanciaAnimacion = 0;

                    estado->enemigos[estado->cantidadEnemigos].tipo = PERRO;

                    estado->cantidadEnemigos++;
                }

                estado->mapa[i][j] = ' ';
            }

            if(estado->mapa[i][j] == 'M')
            {
                if(estado->cantidadEnemigos < MAX_ENEMIGOS)
                {
                    estado->enemigos[estado->cantidadEnemigos].x = j * CELL;
                    estado->enemigos[estado->cantidadEnemigos].y = i * CELL;

                    estado->enemigos[estado->cantidadEnemigos].dx = 0;
                    estado->enemigos[estado->cantidadEnemigos].dy = 0;

                    estado->enemigos[estado->cantidadEnemigos].frame = 0;
                    estado->enemigos[estado->cantidadEnemigos].contadorAnimacion = 0;

                    estado->enemigos[estado->cantidadEnemigos].vivo = true;
                    estado->enemigos[estado->cantidadEnemigos].respawn = 0;

                    estado->enemigos[estado->cantidadEnemigos].distanciaAnimacion = 0;

                    estado->enemigos[estado->cantidadEnemigos].tipo = MONO;

                    estado->enemigos[estado->cantidadEnemigos].tiempoDisparo = 120;

                    estado->cantidadEnemigos++;
                }

                estado->mapa[i][j] = ' ';
            }
        }

        fgetc(archivo);
    }

    fclose(archivo);
}

void cargarSprites(EstadoJuego *estado)
{
    //Menu
    estado->sprites.fondoMenu = al_load_bitmap("Sprites/fondoMenu.png");

    //Serpiente

    estado->sprites.cabezaArriba = al_load_bitmap("Sprites/CabezaSerpArriba.png");
    estado->sprites.cabezaAbajo = al_load_bitmap("Sprites/CabezaSerpAbajo.png");
    estado->sprites.cabezaIzquierda = al_load_bitmap("Sprites/CabezaSerpIzquierda.png");
    estado->sprites.cabezaDerecha = al_load_bitmap("Sprites/CabezaSerpDerecha.png");

    estado->sprites.cuerpoHorizontal = al_load_bitmap("Sprites/CuerpoSerpHorizontal.png");
    estado->sprites.cuerpoVertical = al_load_bitmap("Sprites/CuerpoSerpVertical.png");

    estado->sprites.colaArriba = al_load_bitmap("Sprites/ColaSerpArriba.png");
    estado->sprites.colaAbajo = al_load_bitmap("Sprites/ColaSerpAbajo.png");
    estado->sprites.colaIzquierda = al_load_bitmap("Sprites/ColaSerpIzquierda.png");
    estado->sprites.colaDerecha = al_load_bitmap("Sprites/ColaSerpDerecha.png");

    estado->sprites.curva1 = al_load_bitmap("Sprites/CurvaSerp1.png");
    estado->sprites.curva2 = al_load_bitmap("Sprites/CurvaSerp2.png");
    estado->sprites.curva3 = al_load_bitmap("Sprites/CurvaSerp3.png");
    estado->sprites.curva4 = al_load_bitmap("Sprites/CurvaSerp4.png");

    //Mono

    estado->sprites.monoDerecha = al_load_bitmap("Sprites/monoDerecha.png");
    estado->sprites.bananaMono = al_load_bitmap("Sprites/bananaMono.png");
    estado->sprites.cascarabananaMono = al_load_bitmap("Sprites/cascarabananaMono.png");

    //Perro

    estado->sprites.perroSprite[0] = al_load_bitmap("Sprites/PerroArriba.png");
    estado->sprites.perroSprite[1] = al_load_bitmap("Sprites/PerroAbajo.png");

    //Gato

    estado->sprites.gatoSprite[0] = al_load_bitmap("Sprites/gatoderecha1.png");
    estado->sprites.gatoSprite[1] = al_load_bitmap("Sprites/gatoderecha2.png");
    estado->sprites.gatoSprite[2] = al_load_bitmap("Sprites/gatoderecha3.png");
    estado->sprites.gatoSprite[3] = al_load_bitmap("Sprites/gatoderecha4.png");
    estado->sprites.gatoSprite[4] = al_load_bitmap("Sprites/gatoderecha5.png");
    estado->sprites.gatoSprite[5] = al_load_bitmap("Sprites/gatoderecha6.png");
    estado->sprites.gatoSprite[6] = al_load_bitmap("Sprites/gatoizq1.png");
    estado->sprites.gatoSprite[7] = al_load_bitmap("Sprites/gatoizq2.png");
    estado->sprites.gatoSprite[8] = al_load_bitmap("Sprites/gatoizq3.png");
    estado->sprites.gatoSprite[9] = al_load_bitmap("Sprites/gatoizq4.png");
    estado->sprites.gatoSprite[10] = al_load_bitmap("Sprites/gatoizq5.png");
    estado->sprites.gatoSprite[11] = al_load_bitmap("Sprites/gatoizq6.png");

    //Llave y Puerta
    estado->sprites.llave = al_load_bitmap("Sprites/LlaveSprite.png");
    estado->sprites.puerta = al_load_bitmap("Sprites/PuertaSprite.png");

    //Frutas

    estado->sprites.frutas[0] = al_load_bitmap("Sprites/ComidaManzana.png");
    estado->sprites.frutas[1] = al_load_bitmap("Sprites/ComidaNaranja.png");
    estado->sprites.frutas[2] = al_load_bitmap("Sprites/ComidaBanana.png");
    estado->sprites.frutas[3] = al_load_bitmap("Sprites/ComidaArandanos.png");
    estado->sprites.frutas[4] = al_load_bitmap("Sprites/ComidaAji.png");

        //Niveles

    //Nivel 1
    estado->sprites.fondoPradera = al_load_bitmap("Sprites/FondoNivel1.png");
    estado->sprites.arbusto = al_load_bitmap("Sprites/MurosArbustosNivel1.png");

    //Nivel 2
    estado->sprites.bosquePiso = al_load_bitmap("Sprites/bosquePiso.png");
    estado->sprites.bosqueMuro = al_load_bitmap("Sprites/bosqueMuro.png");

    //Nivel 3
    estado->sprites.desiertoPiso = al_load_bitmap("Sprites/desiertoPiso.png");
    estado->sprites.desiertoMuro = al_load_bitmap("Sprites/desiertoMuro.png");

    //Nivel 4
    estado->sprites.icebergPiso = al_load_bitmap("Sprites/icebergPiso.png");
    estado->sprites.icebergMuro = al_load_bitmap("Sprites/icebergMuro.png");

    //Nivel 5
    estado->sprites.volcanPiso = al_load_bitmap("Sprites/volcanPiso.png");
    estado->sprites.volcanMuro = al_load_bitmap("Sprites/volcanMuro.png");

    //Comprobacion

    if(!estado->sprites.fondoPradera ||
       !estado->sprites.arbusto ||
       !estado->sprites.bosquePiso ||
       !estado->sprites.bosqueMuro ||
       !estado->sprites.llave ||
       !estado->sprites.puerta ||
       !estado->sprites.monoDerecha ||
       !estado->sprites.bananaMono ||
       !estado->sprites.cabezaArriba ||
       !estado->sprites.cabezaAbajo ||
       !estado->sprites.cabezaIzquierda ||
       !estado->sprites.cabezaDerecha ||
       !estado->sprites.cuerpoHorizontal ||
       !estado->sprites.cuerpoVertical ||
       !estado->sprites.colaArriba ||
       !estado->sprites.colaAbajo ||
       !estado->sprites.colaIzquierda ||
       !estado->sprites.colaDerecha ||
       !estado->sprites.curva1 ||
       !estado->sprites.curva2 ||
       !estado->sprites.curva3 ||
       !estado->sprites.curva4 ||
       !estado->sprites.desiertoPiso ||
       !estado->sprites.desiertoMuro ||
       !estado->sprites.icebergPiso ||
       !estado->sprites.icebergMuro ||
       !estado->sprites.volcanPiso ||
       !estado->sprites.volcanMuro)
    {
        printf("Error cargando los sprites.\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < FRAMES_GATO; i++)
    {
        if(!estado->sprites.gatoSprite[i])
        {
            printf("Error cargando el sprite del gato %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < 2; i++)
    {
        if(!estado->sprites.perroSprite[i])
        {
            printf("Error cargando el sprite del perro %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < 5; i++)
    {
        if(!estado->sprites.frutas[i])
        {
            printf("Error cargando la fruta %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

}

void destruirSprites(EstadoJuego *estado)
{
    al_destroy_bitmap(estado->sprites.fondoMenu);

    al_destroy_bitmap(estado->sprites.cabezaArriba);
    al_destroy_bitmap(estado->sprites.cabezaAbajo);
    al_destroy_bitmap(estado->sprites.cabezaIzquierda);
    al_destroy_bitmap(estado->sprites.cabezaDerecha);

    al_destroy_bitmap(estado->sprites.cuerpoHorizontal);
    al_destroy_bitmap(estado->sprites.cuerpoVertical);

    al_destroy_bitmap(estado->sprites.colaArriba);
    al_destroy_bitmap(estado->sprites.colaAbajo);
    al_destroy_bitmap(estado->sprites.colaIzquierda);
    al_destroy_bitmap(estado->sprites.colaDerecha);

    al_destroy_bitmap(estado->sprites.curva1);
    al_destroy_bitmap(estado->sprites.curva2);
    al_destroy_bitmap(estado->sprites.curva3);
    al_destroy_bitmap(estado->sprites.curva4);

    al_destroy_bitmap(estado->sprites.monoDerecha);
    al_destroy_bitmap(estado->sprites.bananaMono);
    al_destroy_bitmap(estado->sprites.cascarabananaMono);

    al_destroy_bitmap(estado->sprites.llave);
    al_destroy_bitmap(estado->sprites.puerta);

    for(int i = 0; i < 2; i++)
        al_destroy_bitmap(estado->sprites.perroSprite[i]);

    for(int i = 0; i < FRAMES_GATO; i++)
        al_destroy_bitmap(estado->sprites.gatoSprite[i]);

    for(int i = 0; i < 5; i++)
        al_destroy_bitmap(estado->sprites.frutas[i]);

    al_destroy_bitmap(estado->sprites.fondoPradera);
    al_destroy_bitmap(estado->sprites.arbusto);

    al_destroy_bitmap(estado->sprites.bosquePiso);
    al_destroy_bitmap(estado->sprites.bosqueMuro);

    al_destroy_bitmap(estado->sprites.desiertoPiso);
    al_destroy_bitmap(estado->sprites.desiertoMuro);

    al_destroy_bitmap(estado->sprites.icebergPiso);
    al_destroy_bitmap(estado->sprites.icebergMuro);

    al_destroy_bitmap(estado->sprites.volcanPiso);
    al_destroy_bitmap(estado->sprites.volcanMuro);
}

void cargarSonidos(EstadoJuego *estado)
{
    estado->sonidos.musicaMenu =
        al_load_sample("Sonidos/musicaMenu.ogg");

    if(!estado->sonidos.musicaMenu)
    {
        printf("Error cargando musicaMenu.ogg\n");
        exit(EXIT_FAILURE);
    }
}

void destruirSonidos(EstadoJuego *estado)
{
    al_destroy_sample(estado->sonidos.musicaMenu);
}

void cargarRankingSegmentos(EstadoJuego *estado)
{
    FILE *archivo = fopen("rankingSegmentos.txt","r");

    if(archivo == NULL)
        return;

    estado->cantidadRankingSegmentos = 0;

    while(fscanf(archivo,"%s %d",
        estado->rankingSegmentos[estado->cantidadRankingSegmentos].nombre,
        &estado->rankingSegmentos[estado->cantidadRankingSegmentos].dato) == 2)
    {
        estado->cantidadRankingSegmentos++;

        if(estado->cantidadRankingSegmentos >= MAX_RANKING)
            break;
    }

    fclose(archivo);
}

void cargarRankingTiempo(EstadoJuego *estado)
{
    FILE *archivo = fopen("rankingTiempo.txt","r");

    if(archivo == NULL)
        return;

    estado->cantidadRankingTiempo = 0;

    while(fscanf(archivo,"%s %d",
        estado->rankingTiempo[estado->cantidadRankingTiempo].nombre,
        &estado->rankingTiempo[estado->cantidadRankingTiempo].dato) == 2)
    {
        estado->cantidadRankingTiempo++;

        if(estado->cantidadRankingTiempo >= MAX_RANKING)
        break;
    }

    fclose(archivo);
}

void guardarRankingSegmentos(EstadoJuego *estado)
{
    FILE *archivo = fopen("rankingSegmentos.txt","w");

    if(archivo == NULL)
        return;

    for(int i=0;i<estado->cantidadRankingSegmentos;i++)
    {
        fprintf(archivo,"%s %d\n",
                estado->rankingSegmentos[i].nombre,
                estado->rankingSegmentos[i].dato);
    }

    fclose(archivo);
}

void guardarRankingTiempo(EstadoJuego *estado)
{
    FILE *archivo = fopen("rankingTiempo.txt", "w");

    if(archivo == NULL)
    {
        return;
    }

    for(int i = 0; i < estado->cantidadRankingTiempo; i++)
    {
        fprintf(archivo,
                "%s %d\n",
                estado->rankingTiempo[i].nombre,
                estado->rankingTiempo[i].dato);
    }

    fclose(archivo);
}

void registrarRanking(EstadoJuego *estado)
{
    if(estado->cantidadRankingSegmentos < MAX_RANKING ||
       estado->juego.puntaje > estado->rankingSegmentos[MAX_RANKING - 1].dato)
    {
        if(estado->cantidadRankingSegmentos < MAX_RANKING)
        {
            strcpy(
                estado->rankingSegmentos[estado->cantidadRankingSegmentos].nombre,
                estado->juego.nombreJugador);

                estado->rankingSegmentos[estado->cantidadRankingSegmentos].dato =
                estado->juego.puntaje;

                estado->cantidadRankingSegmentos++;
        }
        else
        {
            strcpy(
                estado->rankingSegmentos[MAX_RANKING - 1].nombre,
                estado->juego.nombreJugador);

                estado->rankingSegmentos[MAX_RANKING - 1].dato =
                estado->juego.puntaje;
        }
    }

    if(estado->cantidadRankingTiempo < MAX_RANKING ||
       estado->juego.tiempo < estado->rankingTiempo[MAX_RANKING - 1].dato)
    {
        if(estado->cantidadRankingTiempo < MAX_RANKING)
        {
            strcpy(
                estado->rankingTiempo[estado->cantidadRankingTiempo].nombre,
                estado->juego.nombreJugador);

                estado->rankingTiempo[estado->cantidadRankingTiempo].dato =
                estado->juego.tiempo;

                estado->cantidadRankingTiempo++;
        }
        else
        {
            strcpy(
                estado->rankingTiempo[MAX_RANKING - 1].nombre,
                estado->juego.nombreJugador);

                estado->rankingTiempo[MAX_RANKING - 1].dato =
                estado->juego.tiempo;
        }
    }

    ordenarRankingSegmentos(estado);
    ordenarRankingTiempo(estado);

    if(estado->cantidadRankingSegmentos > MAX_RANKING)
        estado->cantidadRankingSegmentos = MAX_RANKING;

    if(estado->cantidadRankingTiempo > MAX_RANKING)
        estado->cantidadRankingTiempo = MAX_RANKING;

    guardarRankingSegmentos(estado);
    guardarRankingTiempo(estado);
}

void ordenarRankingSegmentos(EstadoJuego *estado)
{
    Ranking aux;

    for(int i = 0; i < estado->cantidadRankingSegmentos - 1; i++)
    {
        for(int j = i + 1; j < estado->cantidadRankingSegmentos; j++)
        {
            if(estado->rankingSegmentos[j].dato > estado->rankingSegmentos[i].dato)
            {
                aux = estado->rankingSegmentos[i];
                estado->rankingSegmentos[i] = estado->rankingSegmentos[j];
                estado->rankingSegmentos[j] = aux;
            }
        }
    }
}

void ordenarRankingTiempo(EstadoJuego *estado)
{
    Ranking aux;

    for(int i = 0; i < estado->cantidadRankingTiempo - 1; i++)
    {
        for(int j = i + 1; j < estado->cantidadRankingTiempo; j++)
        {
            if(estado->rankingTiempo[j].dato < estado->rankingTiempo[i].dato)
            {
                aux = estado->rankingTiempo[i];
                estado->rankingTiempo[i] = estado->rankingTiempo[j];
                estado->rankingTiempo[j] = aux;
            }
        }
    }
}

void generarComidas(EstadoJuego *estado, int cantidad)
{
    for(int i=0; i < cantidad && i < MAX_COMIDAS; i++)
    {
        int x, y;
    
        do
        {
            x = rand() % M;
            y = rand() % N;
        
        } while(estado->mapa[y][x] != ' ' || haySerpiente(estado, x, y) || hayComida(estado, x, y));

        estado->comidas[i].x = x;
        estado->comidas[i].y = y;
        estado->comidas[i].activa = true;
    }
}

void generarEnemigo(EstadoJuego *estado, int i)
{
    int x, y;

    do
    {
        x = rand() % M;
        y = rand() % N;

    } while(estado->mapa[y][x] != ' ' ||
            haySerpiente(estado, x, y) ||
            hayComida(estado, x, y) ||
            hayEnemigo(estado, x, y));

    estado->enemigos[i].x = x * CELL;
    estado->enemigos[i].y = y * CELL;

    int dir = rand() % 2;

    if(estado->enemigos[i].tipo == GATO)
    {
        if(dir == 0)
        {
            estado->enemigos[i].dx = 4;
            estado->enemigos[i].dy = 0;
        }
        else
        {
            estado->enemigos[i].dx = -4;
            estado->enemigos[i].dy = 0;
        }
    }
    else if(estado->enemigos[i].tipo == PERRO)
    {
        if(dir == 0)
        {
            estado->enemigos[i].dx = 0;
            estado->enemigos[i].dy = 4;
        }
        else
        {
            estado->enemigos[i].dx = 0;
            estado->enemigos[i].dy = -4;
        }
    }
    else if(estado->enemigos[i].tipo == MONO)
    {
        estado->enemigos[i].dx = 0;
        estado->enemigos[i].dy = 0;

        estado->enemigos[i].tiempoDisparo = 120;
    }

    estado->enemigos[i].frame = 0;
    estado->enemigos[i].contadorAnimacion = 0;
    estado->enemigos[i].vivo = true;
    estado->enemigos[i].respawn = -1;
    estado->enemigos[i].distanciaAnimacion = 0;
}

int hayEnemigo(EstadoJuego *estado, int x, int y)
{
    for(int i = 0; i < estado->cantidadEnemigos; i++)
    {
        if(estado->enemigos[i].vivo)
        {
            if((int)(estado->enemigos[i].x / CELL) == x &&
               (int)(estado->enemigos[i].y / CELL) == y)
            {
                return 1;
            }
        }
    }

    return 0;
}

void lanzarBanana(EstadoJuego *estado, int mono)
{
    for(int i = 0; i < MAX_BALAS; i++)
    {
        if(!estado->enemigos[mono].balas[i].activa)
        {
            estado->enemigos[mono].balas[i].activa = true;

            //Banana sale del mono
            estado->enemigos[mono].balas[i].x = estado->enemigos[mono].x + CELL/2;
            estado->enemigos[mono].balas[i].y = estado->enemigos[mono].y + CELL/2;
            estado->enemigos[mono].balas[i].rango = RANGO_BANANA_MONO * CELL;
            estado->enemigos[mono].balas[i].distancia = 0;

            //Destino cabeza de la serpiente
            estado->enemigos[mono].balas[i].destinoX = estado->serpiente.segmentos[0].x * CELL + CELL/2;
            estado->enemigos[mono].balas[i].destinoY = estado->serpiente.segmentos[0].y * CELL + CELL/2;

            if(estado->serpiente.dx == 1)
                estado->enemigos[mono].balas[i].destinoX += 4;

            if(estado->serpiente.dx == -1)
                estado->enemigos[mono].balas[i].destinoX -= 4;

            if(estado->serpiente.dy == 1)
                estado->enemigos[mono].balas[i].destinoY += 4;

            if(estado->serpiente.dy == -1)
                estado->enemigos[mono].balas[i].destinoY -= 4;

            //Velocidad de la banana
            estado->enemigos[mono].balas[i].velocidadX =
                (estado->enemigos[mono].balas[i].destinoX - estado->enemigos[mono].balas[i].x) / 10.0;

            estado->enemigos[mono].balas[i].velocidadY =
                (estado->enemigos[mono].balas[i].destinoY - estado->enemigos[mono].balas[i].y) / 10.0;

            break;
        }
    }
}

int verificarColisionMuro(EstadoJuego *estado)
{
    if(estado->mapa[estado->serpiente.segmentos[0].y][estado->serpiente.segmentos[0].x] == '#')
    {
        return 1;
    }

    return 0;
}

int verificarColisionSerpiente(EstadoJuego *estado)
{
    for(int i=1; i<estado->serpiente.tamano; i++)
    {
        if(estado->serpiente.segmentos[0].x == estado->serpiente.segmentos[i].x &&
           estado->serpiente.segmentos[0].y == estado->serpiente.segmentos[i].y)
        {
            return 1;
        }
    }

    return 0;
}

int verificarPuertaBloqueada(EstadoJuego *estado)
{
    if(estado->mapa[estado->serpiente.segmentos[0].y][estado->serpiente.segmentos[0].x] == 'E' && !estado->juego.tieneLlave)
    {
        estado->serpiente.segmentos[0].x -= estado->serpiente.dx;
        estado->serpiente.segmentos[0].y -= estado->serpiente.dy;

        sprintf(estado->juego.mensaje, "La puerta esta cerrada");
        estado->juego.tiempoMensaje = 24;

        return 1;
    }

    return 0;
}

int verificarLimites(EstadoJuego *estado)
{
    if(estado->serpiente.segmentos[0].x < 0 || estado->serpiente.segmentos[0].x >= M ||
       estado->serpiente.segmentos[0].y < 0 || estado->serpiente.segmentos[0].y >= N)
    {
        return 1;
    }

    return 0;
}

void reiniciarJuego(EstadoJuego *estado)
{
    estado->serpiente.tamano = 3;
    estado->juego.puntaje = 0;
    estado->juego.tieneLlave = 0;
    estado->juego.tiempo = 0;
    estado->juego.contadorTiempo = 0;

    estado->juego.invulnerable = 0;
    estado->juego.tiempoInvulnerable = 0;

    estado->fase.numero = 1;
    estado->fase.comidasComidas = 0;

    estado->serpiente.dx = 1;
    estado->serpiente.dy = 0;

    estado->juego.nivel = 1;

    estado->serpiente.municion = 5;

    sprintf(estado->juego.archivoNivel, "Niveles/nivel%d.txt", estado->juego.nivel);

    strcpy(estado->juego.nombreNivel, "Pradera");

    for(int i = 0; i < MAX_COMIDAS; i++)
    {
        estado->comidas[i].activa = false;
    }

    for(int i=0; i<MAX_BALAS; i++)
    {
        estado->serpiente.balas[i].activa = false;
    }

    cargarMapa(estado, estado->juego.archivoNivel);

    generarComidas(estado, estado->fase.comidas[estado->fase.numero - 1]);

    for(int i = 0; i < estado->cantidadEnemigos; i++)
    {
        for(int j = 0; j < MAX_BALAS; j++)
        {
            estado->enemigos[i].balas[j].activa = false;
        }
    }
}

void disparar(EstadoJuego *estado)
{
    for(int i=0; i<MAX_BALAS; i++)
    {
        if(!estado->serpiente.balas[i].activa)
        {
            estado->serpiente.balas[i].activa = true;
            estado->serpiente.balas[i].tipo = BALA_SERPIENTE;
            estado->serpiente.balas[i].distancia = 0;
            estado->serpiente.balas[i].rango = RANGO_BALA_SERPIENTE * CELL;

            estado->serpiente.balas[i].x = estado->serpiente.segmentos[0].x * CELL + CELL/2;
            estado->serpiente.balas[i].y = estado->serpiente.segmentos[0].y * CELL + CELL/2;

            if(estado->serpiente.dx == 1)
            {
                estado->serpiente.balas[i].dx = 8;
                estado->serpiente.balas[i].dy = 0;

                estado->serpiente.balas[i].x += CELL/2;
            }
            else if (estado->serpiente.dx == -1)
            {
                estado->serpiente.balas[i].dx = -8;
                estado->serpiente.balas[i].dy = 0;

                estado->serpiente.balas[i].x -= CELL/2;
            }
            else if(estado->serpiente.dy == 1)
            {
                estado->serpiente.balas[i].dx = 0;
                estado->serpiente.balas[i].dy = 8;

                estado->serpiente.balas[i].y += CELL/2;
            }
            else
            {
                estado->serpiente.balas[i].dx = 0;
                estado->serpiente.balas[i].dy = -8;

                estado->serpiente.balas[i].y -= CELL/2;
            }

            break;
        }
    }
}

int haySerpiente(EstadoJuego *estado, int x, int y)
{
    for(int i=0; i<estado->serpiente.tamano; i++)
    {
        if(estado->serpiente.segmentos[i].x == x && estado->serpiente.segmentos[i].y == y)
        {
            return 1;
        }
    }

    return 0;
}

int hayComida(EstadoJuego *estado, int x, int y)
{
    for(int i=0; i<MAX_COMIDAS; i++)
    {
        if(estado->comidas[i].activa && estado->comidas[i].x == x && estado->comidas[i].y == y)
        {
            return 1;
        }
    }

    return 0;
}

void dibujarEditor(EstadoJuego *estado, ALLEGRO_FONT *font)
{
    al_clear_to_color(al_map_rgb(30,30,30));

    for(int y=0;y<N;y++)
    {
        for(int x=0;x<M;x++)
        {
            al_draw_filled_rectangle(
                x*CELL,
                y*CELL,
                x*CELL+CELL,
                y*CELL+CELL,
                al_map_rgb(60,60,60));

            char bloque = estado->mapa[y][x];

            if(bloque == '#')
            {
                al_draw_scaled_bitmap(
                    estado->sprites.arbusto,
                    0, 0, 64, 64,
                    x*CELL,
                    y*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else if(bloque == 'L')
            {
                al_draw_scaled_bitmap(
                    estado->sprites.llave,
                    0, 0, 64, 64,
                    x*CELL,
                    y*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else if(bloque == 'E')
            {
                al_draw_scaled_bitmap(
                    estado->sprites.puerta,
                    0, 0, 64, 64,
                    x*CELL,
                    y*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else if(bloque == 'G')
            {
                al_draw_scaled_bitmap(
                    estado->sprites.gatoSprite[0],
                    0, 0, 64, 64,
                    x*CELL,
                    y*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else if(bloque == 'P')
            {
                al_draw_scaled_bitmap(
                    estado->sprites.perroSprite[0],
                    0, 0, 64, 64,
                    x*CELL,
                    y*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else if(bloque == 'M')
            {
                al_draw_scaled_bitmap(
                    estado->sprites.monoDerecha,
                    0, 0, 64, 64,
                    x*CELL,
                    y*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else if(bloque == 'C')
            {
                al_draw_scaled_bitmap(
                    estado->sprites.frutas[0],
                    0, 0, 64, 64,
                    x*CELL,
                    y*CELL,
                    CELL,
                    CELL,
                    0);
            }
            else if(bloque == 'S')
            {
                al_draw_scaled_bitmap(
                    estado->sprites.cabezaDerecha,
                    0, 0, 64, 64,
                    x*CELL,
                    y*CELL,
                    CELL,
                    CELL,
                    0);
            }
        }
    }

    al_draw_rectangle(
        estado->cursorX*CELL,
        estado->cursorY*CELL,
        estado->cursorX*CELL+CELL,
        estado->cursorY*CELL+CELL,
        al_map_rgb(255,0,0),
        2);

    for(int y = 0; y < N; y++)
    {
        for(int x = 0; x < M; x++)
        {
            al_draw_rectangle(
                x * CELL,
                y * CELL,
                x * CELL + CELL,
                y * CELL + CELL,
                al_map_rgb(80,80,80),
                1);
        }
    }

    al_draw_rectangle(
        estado->cursorX * CELL,
        estado->cursorY * CELL,
        estado->cursorX * CELL + CELL,
        estado->cursorY * CELL + CELL,
        al_map_rgb(255,255,0),
        3);

    char texto[50];

    if(estado->bloqueSeleccionado == '#')
    {    
        sprintf(texto,"Bloque: MURO");
    }
    else if(estado->bloqueSeleccionado == 'L')
    {
        sprintf(texto,"Bloque: LLAVE");
    }
    else if(estado->bloqueSeleccionado == 'E')
    {
        sprintf(texto,"Bloque: PUERTA");
    }
    else if(estado->bloqueSeleccionado == 'S')
    {
        sprintf(texto,"Bloque: SERPIENTE");
    }
    else if(estado->bloqueSeleccionado == 'G')
    {
        sprintf(texto,"Bloque: GATO");
    }
    else if(estado->bloqueSeleccionado == 'P')
    {
        sprintf(texto,"Bloque: PERRO");
    }
    else if(estado->bloqueSeleccionado == 'M')
    {
        sprintf(texto,"Bloque: MONO");
    }
    else if(estado->bloqueSeleccionado == 'C')
    {
        sprintf(texto,"Bloque: COMIDA");
    }
    else
    {
        sprintf(texto,"Bloque: VACIO");
    }

    al_draw_text(
        font,
        al_map_rgb(255,255,255),
        10,
        HEIGHT - 20,
        0,
        texto);
}

void guardarMapaEditor(EstadoJuego *estado)
{
    FILE *archivo = fopen("NivelesCreados/editor.txt", "w");

    if(archivo == NULL)
        return;

    fprintf(archivo, "5 8 10\n");
    
    for(int y=0; y<N; y++)
    {
        for(int x=0; x<M; x++)
        {
            fputc(estado->mapa[y][x], archivo);
        }

        fputc('\n', archivo);
    }

    fclose(archivo);
}

void limpiarMapaEditor(EstadoJuego *estado)
{
    for(int y = 0; y < N; y++)
    {
        for(int x = 0; x < M; x++)
        {
            estado->mapa[y][x] = ' ';
        }
    }

    estado->cursorX = 0;
    estado->cursorY = 0;

    estado->bloqueSeleccionado = '#';
}