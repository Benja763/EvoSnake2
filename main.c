#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
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
Segmento serpiente[100];

int tamanoSerpiente = 3;

int puntaje = 0;

int tiempo = 0;
int contadorTiempo = 0;

int comidasNecesarias = 5;

int tieneLlave = 0;

int Nivel = 1;

char mensaje[100] = "";
int tiempoMensaje = 0;

int desplazamientoX = 1;
int desplazamientoY = 0;

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

void generarComida();
int haySerpiente(int x, int y);

void cargarMapa(char nombreArchivo[])
{
    FILE *archivo = fopen(nombreArchivo, "r");

    if(archivo == NULL)
    {
        printf("No se pudo abrir el archivo %s\n", nombreArchivo);
        return;
    }

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
        {
            mapa[i][j] = fgetc(archivo);

            if(mapa[i][j] == 'S')
            {
                serpiente[0].x = j;
                serpiente[0].y = i;

                serpiente[1].x = j-1;
                serpiente[1].y = i;

                serpiente[2].x = j-2;
                serpiente[2].y = i;

                mapa[i][j] = ' ';
            }
        }

        fgetc(archivo); // Saltar el Enter al final de cada línea
    }

    fclose(archivo);
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

        sprintf(mensaje, "La puerta esta cerrada");
        tiempoMensaje = 24;

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
    tiempo = 0;
    contadorTiempo = 0;

    desplazamientoX = 1;
    desplazamientoY = 0;

    cargarMapa("nivel1.txt");
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
    al_init_image_addon();

    ALLEGRO_DISPLAY *display =
        al_create_display(WIDTH, HEIGHT);

    ALLEGRO_TIMER *timer =
        al_create_timer(1.0/8);

    ALLEGRO_EVENT_QUEUE *queue =
        al_create_event_queue();

    ALLEGRO_FONT *font =
        al_create_builtin_font();

    cabezaArriba = al_load_bitmap("CabezaSerpArriba.png");
    cabezaAbajo = al_load_bitmap("CabezaSerpAbajo.png");
    cabezaIzquierda = al_load_bitmap("CabezaSerpIzquierda.png");
    cabezaDerecha = al_load_bitmap("CabezaSerpDerecha.png");

    cuerpoHorizontal = al_load_bitmap("CuerpoSerpHorizontal.png");
    cuerpoVertical = al_load_bitmap("CuerpoSerpVertical.png");

    colaArriba = al_load_bitmap("ColaSerpArriba.png");
    colaAbajo = al_load_bitmap("ColaSerpAbajo.png");
    colaIzquierda = al_load_bitmap("ColaSerpIzquierda.png");
    colaDerecha = al_load_bitmap("ColaSerpDerecha.png");

    curva1 = al_load_bitmap("CurvaSerp1.png");
    curva2 = al_load_bitmap("CurvaSerp2.png");
    curva3 = al_load_bitmap("CurvaSerp3.png");
    curva4 = al_load_bitmap("CurvaSerp4.png");

    if(!cabezaArriba || !cabezaAbajo ||
   !cabezaIzquierda || !cabezaDerecha ||
   !cuerpoHorizontal || !cuerpoVertical ||
   !colaArriba || !colaAbajo ||
   !colaIzquierda || !colaDerecha ||
   !curva1 || !curva2 || !curva3 || !curva4)
{
    printf("Error cargando las imagenes\n");
    return -1;
}

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
    cargarMapa("nivel1.txt");
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
        if(desplazamientoY != 1)
        {
            desplazamientoX = 0;
            desplazamientoY = -1;
        }
        break;

    case ALLEGRO_KEY_DOWN:
        if(desplazamientoY != -1)
        {
            desplazamientoX = 0;
            desplazamientoY = 1;
        }
        break;

    case ALLEGRO_KEY_LEFT:
        if(desplazamientoX != 1)
        {
            desplazamientoX = -1;
            desplazamientoY = 0;
        }
        break;

    case ALLEGRO_KEY_RIGHT:
        if(desplazamientoX != -1)
        {
            desplazamientoX = 1;
            desplazamientoY = 0;
        }
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

                serpiente[tamanoSerpiente] = serpiente[tamanoSerpiente-1];

                tamanoSerpiente++;

                mapa[serpiente[0].y][serpiente[0].x] = ' ';

                generarComida();

                if(puntaje >= comidasNecesarias)
                {
                    sprintf(mensaje, "Ya puedes recoger la llave");
                    tiempoMensaje = 24;
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
                    sprintf(mensaje, "Necesitas %d comidas mas", comidasNecesarias - puntaje);
                    tiempoMensaje = 24;
                }
            }

            if(mapa[serpiente[0].y][serpiente[0].x] == 'P' && tieneLlave)
            {
                Nivel = 2;

                char nombreNivel[20];
                sprintf(nombreNivel, "nivel%d.txt", Nivel);

                cargarMapa(nombreNivel);

                tamanoSerpiente = 3;
                puntaje = 0;
                tieneLlave = 0;
                tiempo = 0;
                contadorTiempo = 0;

                desplazamientoX = 1;
                desplazamientoY = 0;

                sprintf(mensaje, "Bienvenido al Bosque");
                tiempoMensaje = 24;
            }

    al_clear_to_color(
        al_map_rgb(136,231,136));

for(int i=0; i<N; i++)
{
    for(int j=0; j<M; j++)
    {

        char casilla = mapa[i][j];

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

for(int i=0; i<tamanoSerpiente; i++)
{
    if(i == 0)
    {
        ALLEGRO_BITMAP *cabeza;

        if(desplazamientoX == 1)
            cabeza = cabezaDerecha;
        else if(desplazamientoX == -1)
            cabeza = cabezaIzquierda;
        else if(desplazamientoY == -1)
            cabeza = cabezaArriba;
        else
            cabeza = cabezaAbajo;

        al_draw_scaled_bitmap(
            cabeza,
            0,
            0,
            64,
            64,
            serpiente[i].x * CELL,
            serpiente[i].y * CELL,
            CELL,
            CELL,
            0);
    }
    else if(i == tamanoSerpiente - 1)
    {
        ALLEGRO_BITMAP *cola;

        if(tamanoSerpiente == 1)
            cola = colaAbajo;
        else
        {
        int dx = serpiente[i].x - serpiente[i-1].x;
        int dy = serpiente[i].y - serpiente[i-1].y;

        if(dx == 0 && dy == 0)
        {
            // La serpiente acaba de crecer
            if(desplazamientoX == 1)
                cola = colaDerecha;
            else if(desplazamientoX == -1)
                cola = colaIzquierda;
            else if(desplazamientoY == 1)
                cola = colaAbajo;
            else
                cola = colaArriba;
        }
        else if(dx == 1)
        {
            cola = colaDerecha;
        }
        else if(dx == -1)
        {
            cola = colaIzquierda;
        }
        else if(dy == 1)
        {
            cola = colaAbajo;
        }
        else
        {
            cola = colaArriba;
        }
        }

        al_draw_scaled_bitmap(
            cola,
            0,0,
            64,64,
            serpiente[i].x*CELL,
            serpiente[i].y*CELL,
            CELL,
            CELL,
            0);
    }
    else
    {
        int dx1 = serpiente[i].x - serpiente[i-1].x;
        int dy1 = serpiente[i].y - serpiente[i-1].y;

        int dx2 = serpiente[i+1].x - serpiente[i].x;
        int dy2 = serpiente[i+1].y - serpiente[i].y;

        ALLEGRO_BITMAP *imagen = cuerpoHorizontal;

        // Cuerpo recto

    if(dy1 == 0 && dy2 == 0)
    {
        // Se mueve de izquierda a derecha
        imagen = cuerpoHorizontal;
    }
    else if(dx1 == 0 && dx2 == 0)
    {
        // Se mueve de arriba hacia abajo
        imagen = cuerpoVertical;
    }

        // Curvas

        else if((dx1==1 && dy2==1) || (dy1==-1 && dx2==-1))
        {
            imagen = curva2;
        }

        else if((dx1==-1 && dy2==1) || (dy1==-1 && dx2==1))
        {
            imagen = curva1;
        }

        else if((dx1==-1 && dy2==-1) || (dy1==1 && dx2==1))
        {
            imagen = curva3;
        }

        else
        {
            imagen = curva4;
        }

        al_draw_scaled_bitmap(
            imagen,
            0,
            0,
            64,
            64,
            serpiente[i].x*CELL,
            serpiente[i].y*CELL,
            CELL,
            CELL,
            0);
    }
}           //Nivel en Pantalla
                char textoNivel[50];

                sprintf(textoNivel,
                        "Nivel: %d",
                        Nivel);

                al_draw_text(
                    font,
                    al_map_rgb(255,255,255),
                    10,
                    70,
                    0,
                    textoNivel);
            //Puntaje en Pantalla
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
            //Tiempo en Pantalla
                char textoTiempo[100];

                sprintf(textoTiempo,
                        "Tiempo: %d",
                        tiempo);

                al_draw_text(
                    font,
                    al_map_rgb(255,255,255),
                    10,
                    50,
                    0,
                    textoTiempo);
            //Comidas necesarias en Pantalla
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
                    90,
                    0,
                    "Llave obtenida");
            }

            if(tiempoMensaje > 0)
            {
                 tiempoMensaje--;
            }

            if(tiempoMensaje > 0)
            {
                al_draw_text(
                    font,
                    al_map_rgb(255,255,0),
                    WIDTH/2,
                    HEIGHT-30,
                    ALLEGRO_ALIGN_CENTER,
                    mensaje
                );
            }

            contadorTiempo++;

            if(contadorTiempo >= 8)
            {
                tiempo++;
                contadorTiempo = 0;
            }

            al_flip_display();
        }
    }

    al_destroy_bitmap(cabezaArriba);
    al_destroy_bitmap(cabezaAbajo);
    al_destroy_bitmap(cabezaIzquierda);
    al_destroy_bitmap(cabezaDerecha);

    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}