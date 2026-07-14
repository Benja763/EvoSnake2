#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 600
#define CELL 20

#define N 30
#define M 40

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

} Serpiente;

typedef struct
{
    int nivel;
    int puntaje;
    int tiempo;
    int tieneLlave;

    int comidasNecesarias;
    int comidasFase;



    int tiempoMensaje;

    char mensaje[100];
    char archivoNivel[30];
    char nombreNivel[30];

    int xLlave;
    int yLlave;

} Juego;

typedef struct
{
    int x;
    int y;
    bool activa;

} Comida;

typedef struct
{
    int numero;
    int comidasPorFase;
    int comidasComidas;

} Fase;

#define MAX_COMIDAS 10

Comida comidas[MAX_COMIDAS];

char mapa[N][M];
Serpiente serpiente;

Juego juego;

Fase fase;

int contadorTiempo = 0;

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
int hayComida(int x, int y);
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
        }

        fgetc(archivo);
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

    comidas[0].x = x;
    comidas[0].y = y;
    comidas[0].activa = true;
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
    if(mapa[serpiente.segmentos[0].y][serpiente.segmentos[0].x] == 'P' && !juego.tieneLlave)
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
    contadorTiempo = 0;

    fase.numero = 1;
    fase.comidasPorFase = 3;
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

    cargarMapa(juego.archivoNivel);

    generarComidas(fase.comidasPorFase);
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

        //Sprites de la serpiente

    cabezaArriba = al_load_bitmap("Sprites/CabezaSerpArriba.png");
    cabezaAbajo = al_load_bitmap("Sprites/CabezaSerpAbajo.png");
    cabezaIzquierda = al_load_bitmap("Sprites/CabezaSerpIzquierda.png");
    cabezaDerecha = al_load_bitmap("Sprites/CabezaSerpDerecha.png");

    cuerpoHorizontal = al_load_bitmap("Sprites/CuerpoSerpHorizontal.png");
    cuerpoVertical = al_load_bitmap("Sprites/CuerpoSerpVertical.png");

    colaArriba = al_load_bitmap("Sprites/ColaSerpArriba.png");
    colaAbajo = al_load_bitmap("Sprites/ColaSerpAbajo.png");
    colaIzquierda = al_load_bitmap("Sprites/ColaSerpIzquierda.png");
    colaDerecha = al_load_bitmap("Sprites/ColaSerpDerecha.png");

    curva1 = al_load_bitmap("Sprites/CurvaSerp1.png");
    curva2 = al_load_bitmap("Sprites/CurvaSerp2.png");
    curva3 = al_load_bitmap("Sprites/CurvaSerp3.png");
    curva4 = al_load_bitmap("Sprites/CurvaSerp4.png");

    //Sprites de la comida

    ALLEGRO_BITMAP *manzana = al_load_bitmap("Sprites/ComidaManzana.png");
    ALLEGRO_BITMAP *naranja = al_load_bitmap("Sprites/ComidaNaranja.png");
    ALLEGRO_BITMAP *banana = al_load_bitmap("Sprites/ComidaBanana.png");
    ALLEGRO_BITMAP *arandano = al_load_bitmap("Sprites/ComidaArandanos.png");
    ALLEGRO_BITMAP *aji = al_load_bitmap("Sprites/ComidaAji.png");

    ALLEGRO_BITMAP *fondoPradera = al_load_bitmap("Sprites/FondoNivel1.png");
    ALLEGRO_BITMAP *arbusto = al_load_bitmap("Sprites/MurosArbustosNivel1.png");

    ALLEGRO_BITMAP *llaveSprite = al_load_bitmap("Sprites/LlaveSprite.png");
    ALLEGRO_BITMAP *puertaSprite = al_load_bitmap("Sprites/PuertaSprite.png");

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
}

        }

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
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

                    serpiente.tamano++;

                    serpiente.segmentos[serpiente.tamano] = serpiente.segmentos[serpiente.tamano-1];

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
                        fase.comidasPorFase++;
                        fase.comidasComidas = 0;

                        sprintf(juego.mensaje, "Fase %d", fase.numero);
                        juego.tiempoMensaje = 24;

                        generarComidas(fase.comidasPorFase);
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

            if(mapa[serpiente.segmentos[0].y][serpiente.segmentos[0].x] == 'P' && juego.tieneLlave)
            {
                juego.nivel = 2;

                char archivoNivel[20];
                sprintf(juego.archivoNivel, "Niveles/nivel%d.txt", juego.nivel);

                cargarMapa(juego.archivoNivel);

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
                fase.comidasPorFase = 3;
                fase.comidasComidas = 0;

                for(int i=0; i<MAX_COMIDAS; i++)
                {
                    comidas[i].activa = false;
                }

                generarComidas(fase.comidasPorFase);

                serpiente.tamano = 3;
                juego.puntaje = 0;
                juego.tieneLlave = 0;
                juego.tiempo = 0;
                contadorTiempo = 0;

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
                        fondoPradera,
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
                    arbusto,
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
                        llaveSprite,
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
            case 'P':

                al_draw_scaled_bitmap(
                    puertaSprite,
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
        ALLEGRO_BITMAP *fruta;

        switch(juego.nivel)
        {
            case 1: fruta = manzana; break;
            case 2: fruta = naranja; break;
            case 3: fruta = banana; break;
            case 4: fruta = arandano; break;
            default: fruta = aji; break;
        }

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

for(int i=0; i<serpiente.tamano; i++)
{
    if(i == 0)
    {
        ALLEGRO_BITMAP *cabeza;

        if(serpiente.dx == 1)
            cabeza = cabezaDerecha;
        else if(serpiente.dx == -1)
            cabeza = cabezaIzquierda;
        else if(serpiente.dy == -1)
            cabeza = cabezaArriba;
        else
            cabeza = cabezaAbajo;

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
            cola = colaAbajo;
        else
        {
        int dx = serpiente.segmentos[i].x - serpiente.segmentos[i-1].x;
        int dy = serpiente.segmentos[i].y - serpiente.segmentos[i-1].y;

        if(dx == 0 && dy == 0)
        {
            
            if(serpiente.dx == 1)
                cola = colaDerecha;
            else if(serpiente.dx == -1)
                cola = colaIzquierda;
            else if(serpiente.dy == 1)
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

        ALLEGRO_BITMAP *imagen = cuerpoHorizontal;

        // Cuerpo recto

    if(dy1 == 0 && dy2 == 0)
    {
        // Se mueve de izquierda a derecha
        imagen = cuerpoHorizontal;
    }
    else if(dx1 == 0 && dx2 == 0)
    {
        // Se mueve de arriba a abajo
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

                sprintf(textoLlave,
                        "Llave: %s",
                        juego.tieneLlave ? "SI" : "NO");

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
                        fase.comidasPorFase);

                al_draw_text(
                     font,
                      al_map_rgb(255,255,255),
                      10,
                      50,
                      0,
                      textoComidas);

            //Llave obtenida en Pantalla

            if(juego.tieneLlave)
            {
                al_draw_text(
                    font,
                    al_map_rgb(255,255,0),
                    10,
                    110,
                    0,
                    "Llave obtenida");
            }

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

            contadorTiempo++;

            if(contadorTiempo >= 8)
            {
                juego.tiempo++;
                contadorTiempo = 0;
            }

            al_flip_display();
        }
    }

    al_destroy_bitmap(cabezaArriba);
    al_destroy_bitmap(cabezaAbajo);
    al_destroy_bitmap(cabezaIzquierda);
    al_destroy_bitmap(cabezaDerecha);

    al_destroy_bitmap(manzana);
    al_destroy_bitmap(naranja);
    al_destroy_bitmap(banana);
    al_destroy_bitmap(arandano);
    al_destroy_bitmap(aji);

    al_destroy_bitmap(fondoPradera);
    al_destroy_bitmap(arbusto);

    al_destroy_bitmap(llaveSprite);
    al_destroy_bitmap(puertaSprite);

    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}