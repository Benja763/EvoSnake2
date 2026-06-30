#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600
#define CELL 20

#define N 30
#define M 40

typedef struct{
    int x;
    int y;
} Segmento;

char mapa[N][M];
//={{' ',' ',' '},
//                 {' ',' ',' '}};
char mapaNivel2[N][M];
Segmento serpiente[100];

int tamanoSerpiente = 3;

int puntaje = 0;

int comidasNecesarias = 5;

int tieneLlave = 0;

int Nivel = 1;

int desplazamientoX = 1;
int desplazamientoY = 0;

void generarComida();
int haySerpiente(int x, int y);

void crearMapa()
{
    for(int i=0; i<N; i++)
    {
        for(int j=0; j<M; j++)
        {
            mapa[i][j] = ' ';
        }
    }

    for(int i=0; i<N; i++)
    {
        mapa[i][0] = '#';
        mapa[i][M-1] = '#';
    }

    for(int j=0; j<M; j++)
    {
        mapa[0][j] = '#';
        mapa[N-1][j] = '#';
    }

    //Paredes
    mapa[10][10] = '#';
    mapa[10][11] = '#';
    mapa[10][12] = '#';

    mapa[15][20] = '#';
    mapa[16][20] = '#';
    mapa[17][20] = '#';

    //Llave
    mapa[5][30] = 'L';

    //Puerta
    mapa[25][35] = 'P';

    generarComida();
}

void crearMapaNivel2()
{
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
        {
            mapaNivel2[i][j] = ' ';
        }
    }

    //Bordes

    for(int i = 0; i < N; i++)
    {
        mapaNivel2[i][0] = '#';
        mapaNivel2[i][M-1] = '#';
    }

    for(int j = 0; j < M; j++)
    {
        mapaNivel2[0][j] = '#';
        mapaNivel2[N-1][j] = '#';
    }

    //Muros internos

    for(int i = 3; i < 27; i++)
    {
        mapaNivel2[i][8] = '#';
        mapaNivel2[i][20] = '#';
        mapaNivel2[i][32] = '#';
    }

    mapaNivel2[6][12] = 'C';

    mapaNivel2[24][36] = 'L';

    mapaNivel2[2][37] = 'P';
}

void generarComida()
{
    int x, y;

    do
    {
        x = rand() % M;
        y = rand() % N;
    
    } while(mapa[y][x] != ' ' || haySerpiente(x, y));

    mapa[y][x] = 'C';
}

int verificarColisionMuro()
{
    if(mapa[serpiente[0].y][serpiente[0].x] == '#')
    {
        return 1;
    }

    return 0;
}

int verificarColisionSerpiente()
{
    for(int i=1; i<tamanoSerpiente; i++)
    {
        if(serpiente[0].x == serpiente[i].x &&
           serpiente[0].y == serpiente[i].y)
        {
            return 1;
        }
    }

    return 0;
}

int verificarPuertaBloqueada()
{
    if(mapa[serpiente[0].y][serpiente[0].x] == 'P' && !tieneLlave)
    {
        serpiente[0].x -= desplazamientoX;
        serpiente[0].y -= desplazamientoY;

        printf("No tienes la llave para abrir la puerta!\n");

        return 1;
    }

    return 0;
}

int verificarLimites()
{
    if(serpiente[0].x < 0 || serpiente[0].x >= M ||
       serpiente[0].y < 0 || serpiente[0].y >= N)
    {
        return 1;
    }

    return 0;
}

void reiniciarJuego()
{
    tamanoSerpiente = 3;
    puntaje = 0;
    tieneLlave = 0;

    desplazamientoX = 1;
    desplazamientoY = 0;

    serpiente[0].x = 5;
    serpiente[0].y = 5;

    serpiente[1].x = 4;
    serpiente[1].y = 5;

    serpiente[2].x = 3;
    serpiente[2].y = 5;

    crearMapa();
}

int haySerpiente(int x, int y)
{
    for(int i=0; i<tamanoSerpiente; i++)
    {
        if(serpiente[i].x == x && serpiente[i].y == y)
        {
            return 1;
        }
    }

    return 0;
}

int main()
{
    al_init();
    srand(time(NULL));
    al_init_primitives_addon();
    al_init_font_addon();

    ALLEGRO_DISPLAY *display =
        al_create_display(WIDTH, HEIGHT);

    ALLEGRO_TIMER *timer =
        al_create_timer(1.0/8);

    ALLEGRO_EVENT_QUEUE *queue =
        al_create_event_queue();

    ALLEGRO_FONT *font =
        al_create_builtin_font();

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

    serpiente[0].x = 5;
    serpiente[0].y = 5;

    serpiente[1].x = 4;
    serpiente[1].y = 5;

    serpiente[2].x = 3;
    serpiente[2].y = 5;

    al_start_timer(timer);
    crearMapa();
    crearMapaNivel2();
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
                    desplazamientoX = 0;
                    desplazamientoY = -1;
                    break;

                case ALLEGRO_KEY_DOWN:
                    desplazamientoX = 0;
                    desplazamientoY = 1;
                    break;

                case ALLEGRO_KEY_LEFT:
                    desplazamientoX = -1;
                    desplazamientoY = 0;
                    break;

                case ALLEGRO_KEY_RIGHT:
                    desplazamientoX = 1;
                    desplazamientoY = 0;
                    break;
            }
        }

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            for(int i=tamanoSerpiente-1;i>0;i--)
            {
                serpiente[i] = serpiente[i-1];
            }

            serpiente[0].x += desplazamientoX;
            serpiente[0].y += desplazamientoY;

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

            if(mapa[serpiente[0].y][serpiente[0].x] == 'C')
            {
                puntaje++;
                tamanoSerpiente++;

                mapa[serpiente[0].y][serpiente[0].x] = ' ';

                generarComida();

                if(puntaje >= comidasNecesarias)
                {
                    printf("Has crecido lo suficiente para tomar la llave!\n");
                }
            }

           if(mapa[serpiente[0].y][serpiente[0].x] == 'L')
            {
                if(puntaje >= comidasNecesarias)
                {
                tieneLlave = 1;

                mapa[serpiente[0].y][serpiente[0].x] = ' ';
                }
                else
                {
                    printf("Necesitas crecer más para obtener la llave!\n");
                }
            }

           if(mapa[serpiente[0].y][serpiente[0].x] == 'P' && tieneLlave)
            {
                printf("Nivel 1 completado!\n");

                Nivel = 2;
            }

    al_clear_to_color(
        al_map_rgb(136,231,136));

            for(int i=0;i<tamanoSerpiente;i++)
            {
                al_draw_filled_rectangle(
                    serpiente[i].x*CELL,
                    serpiente[i].y*CELL,
                    serpiente[i].x*CELL+CELL,
                    serpiente[i].y*CELL+CELL,
                    al_map_rgb(0,120,0));
            }

for(int i=0; i<N; i++)
{
    for(int j=0; j<M; j++)
    {
        char casilla;

        if(Nivel == 1)
        {
            casilla = mapa[i][j];
        }
        else
        {
            casilla = mapaNivel2[i][j];
        }

        switch(casilla)
        {
            case '#':

                al_draw_filled_rectangle(
                    j*CELL,
                    i*CELL,
                    j*CELL+CELL,
                    i*CELL+CELL,
                    al_map_rgb(90,90,90));

                break;

            case 'C':

                al_draw_filled_circle(
                    j*CELL+10,
                    i*CELL+10,
                    8,
                    al_map_rgb(255,0,255));

                break;

            case 'L':

                if(!tieneLlave)
                {
                    al_draw_filled_circle(
                        j*CELL+10,
                        i*CELL+10,
                        8,
                        al_map_rgb(255,255,0));
                }

                break;

            case 'P':

                al_draw_filled_rectangle(
                    j*CELL,
                    i*CELL,
                    j*CELL+CELL,
                    i*CELL+CELL,
                    al_map_rgb(0,0,255));

                break;
        }
    }
}

            char texto[100];

            sprintf(texto,
                    "Puntaje: %d",
                    puntaje);

            al_draw_text(
                font,
                al_map_rgb(255,255,255),
                10,
                10,
                0,
                texto);

                char textoComidas[100];

                sprintf(textoComidas,
                        "Comidas: %d/%d",
                         puntaje,
                         comidasNecesarias);

                al_draw_text(
                     font,
                      al_map_rgb(255,255,255),
                      10,
                      30,
                      0,
                      textoComidas);

            if(tieneLlave)
            {
                al_draw_text(
                    font,
                    al_map_rgb(255,255,0),
                    10,
                    30,
                    0,
                    "Llave obtenida");
            }

            al_flip_display();
        }
    }

    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}