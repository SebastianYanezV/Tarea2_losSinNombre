#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "list.h"

typedef struct
{
    char *nombre;
    char *marca;
    char *tipo;
    int stock;
    int precio;
} tipoProducto;

typedef struct
{
    int totalProductos;
    List *listaProductos;
    Map *mapaTipo;
    Map *mapaMarca;
    Map *mapaNombre;
    Map *mapaCarritos;
} tipoCatalogo;

typedef struct
{
    int totalProducto;
    char *nombreProducto;
    char *nombreCarrito;
} tipoCarrito;

//Función para poder hacer los "createMap"
int is_equal_string(void * key1, void * key2) 
{
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

//Función para inicializar la variable que almacenará todos los mapas del programa
tipoCatalogo* crearCatalogo()
{
    tipoCatalogo *aux = (tipoCatalogo*) calloc (1, sizeof(tipoCatalogo));
    aux->listaProductos = createList();
    aux->mapaTipo = createMap(is_equal_string);
    aux->mapaMarca = createMap(is_equal_string);
    aux->mapaNombre = createMap(is_equal_string);
    aux->mapaCarritos = createMap(is_equal_string);
    aux->totalProductos = 0;
    return aux;
}

//Función que lee los datos ingresados por el usuario
void leerChar (char** nombreArchivo)
{
  char nombre[200];
  int largo;
  scanf("%[^\n]s", nombre);
  getchar();
  largo = strlen(nombre) + 1;
  (*nombreArchivo) = (char*) calloc (largo, sizeof(char));
  strcpy((*nombreArchivo), nombre);
}

//Función que lee el archivo 
char* get_csv_field (char * tmp, int k) 
{
    int open_mark = 0;
    char* ret = (char*) malloc (100*sizeof(char));
    int ini_i = 0, i = 0;
    int j = 0;

    while(tmp[i+1] !='\0')
    {
        if(tmp[i]== '\"')
        {
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }
        if(open_mark || tmp[i]!= ',')
        {
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }
        if(tmp[i]== ',')
        {
            if(k==j) 
            {
               ret[i-ini_i ] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }
    if(k==j) 
    {
       ret[i-ini_i] = 0;
       return ret;
    }

    return NULL;
}

char* reservarMemoria(int largo)
{
    char *aux = (char*) calloc(largo, sizeof(char));
    return aux;
}

int noEsta(tipoProducto *productoAux, List *listaProductos)
{
    tipoProducto *aux = (tipoProducto*) firstList(listaProductos);
    while (aux != NULL)
    {
        if (strcmp(aux->nombre, productoAux->nombre) == 0)
        {
            return (1); //Si está retorna 1
        }
        aux = (tipoProducto*) nextList(listaProductos);
    }
    return (0); //Si no está retorna 0
}

void importarProductos(tipoCatalogo *catalogo, char *nombreArchivo)
{
    FILE *archivoProductos = fopen(nombreArchivo, "r");
    if (archivoProductos == NULL)
    {
        printf("No se pudo importar el archivo.\n");
        return;
    } 

    char linea[1024];
    int largo;

    while (fgets (linea, 1023, archivoProductos) != NULL) 
    {
        tipoProducto *productoAux = (tipoProducto*) calloc(1, sizeof(tipoProducto));
        for (int i = 0 ; i < 5 ; i = i + 1) //Se itera 5 veces porque cada producto tiene 5 características
        {
            char *aux = get_csv_field(linea, i);
            largo = strlen(aux) + 1;

            switch (i)
            {
                case 0: //Se almacena el nombre
                {
                    productoAux->nombre = reservarMemoria(largo);
                    strcpy(productoAux->nombre, aux);
                    break;
                }
                case 1: //Se almacena la marca
                {
                    productoAux->marca = reservarMemoria(largo);
                    strcpy(productoAux->marca, aux);
                    break;
                }
                case 2: //Se almacena el tipo
                {
                    productoAux->tipo = reservarMemoria(largo); 
                    strcpy(productoAux->tipo, aux);
                    break;
                }
                case 3: //Se almacena el stock
                {
                    productoAux->stock = atoi(aux); 
                    break;
                }
                case 4: //Se almacena el precio
                {
                    productoAux->precio = atoi(aux);
                    break;
                }
            }
        }

        if (noEsta(productoAux, catalogo->listaProductos) == 0) 
        {
            pushBack(catalogo->listaProductos, productoAux);
            catalogo->totalProductos = catalogo->totalProductos + 1;  

            List *listaTipo = searchMap(catalogo->mapaTipo, productoAux->tipo); 
            //Se ingresan los productos a un mapa de listas, en donde cada par es un tipo de producto (key)
            //con una lista de productos de ese tipo (value)
            if (listaTipo == NULL)
            {
                listaTipo = createList(); 
                insertMap(catalogo->mapaTipo, productoAux->tipo, listaTipo);
            }
            pushBack(listaTipo, productoAux); //Si ya existe un producto del tipo ingresado, simplemente se agrega a su lista asociada
            
            List *listaMarca = searchMap(catalogo->mapaMarca, productoAux->marca); 
            //Se ingresan los productos a un mapa de listas, en donde cada par es una marca de producto (key)
            //con una lista de productos de esa marca (value)
            if (listaMarca == NULL)
            {
                listaMarca = createList(); 
                insertMap(catalogo->mapaMarca, productoAux->marca, listaMarca);
            }
            pushBack(listaMarca, productoAux); //Si ya existe un producto de la marca ingresada, simplemente se agrega a su lista asociada

            insertMap(catalogo->mapaNombre, productoAux->nombre, productoAux); 
            //Se ingresan los productos a un mapa de nombres, en el que cada nombre, al ser único, es la clave de cada producto

        }
        else
        {
            //Si el producto ya se leyó, se libera la memoria de la variable auxiliar y se continúa con la lectura
            free(productoAux->nombre);
            free(productoAux->marca);
            free(productoAux->tipo);
            free(productoAux);
        }
    }
    fclose(archivoProductos);
}

void exportarProductos(tipoCatalogo *catalogo, char *archivoAExportar)
{
  FILE *archivo = fopen (archivoAExportar, "a"); //Se abre en modo "append", el cual escribe al final del archivo si existe o crea uno si no existe.
  tipoProducto *aux = (tipoProducto*) firstList(catalogo->listaProductos);
  //Se recorre la lista de productos hasta que sea nula.
  while (aux != NULL)  
  {
    fprintf(archivo, "%s, %s, %s, %d, %d\n", aux->nombre, aux->marca, aux->tipo, aux->stock, aux->precio); //Se escriben los datos en el archivo.
    aux = (tipoProducto*) nextList(catalogo->listaProductos);
  }
  fclose(archivo);
}

void agregarProducto(tipoCatalogo *catalogo, tipoProducto *nuevoProducto)
{
    if (noEsta(nuevoProducto, catalogo->listaProductos) == 0)
    {
        pushBack(catalogo->listaProductos, nuevoProducto);
        catalogo->totalProductos = catalogo->totalProductos + 1;  

        List *listaTipo = searchMap(catalogo->mapaTipo, nuevoProducto->tipo); //Se agrega el producto al mapa con clave tipo
        if (listaTipo == NULL)
        {
            listaTipo = createList(); 
            insertMap(catalogo->mapaTipo, nuevoProducto->tipo, listaTipo);
        }
        pushBack(listaTipo, nuevoProducto);

        List *listaMarca = searchMap(catalogo->mapaMarca, nuevoProducto->marca); //Se agrega el producto al mapa con clave marca
        if (listaMarca == NULL)
        {
            listaMarca = createList(); 
            insertMap(catalogo->mapaMarca, nuevoProducto->marca, listaMarca);
        }
        pushBack(listaMarca, nuevoProducto);

        insertMap(catalogo->mapaNombre, nuevoProducto->nombre, nuevoProducto); //Se agrega el producto al mapa con clave nombre
    }
    else
    {
        tipoProducto *aux = (tipoProducto*) firstMap(catalogo->mapaNombre); 

        while (aux != NULL) 
        {
            if (strcmp(aux->nombre, nuevoProducto->nombre) == 0)
            {
                aux->stock = aux->stock + nuevoProducto->stock; //Si el producto ya había sido almacenado, solo se aumenta su stock
            }

            aux = (tipoProducto*) nextMap(catalogo->mapaNombre);
        }
    }
}

void buscarProductosTipo(tipoCatalogo *catalogo, char *tipoABuscar)
{
    List* aux = searchMap(catalogo->mapaTipo, tipoABuscar); //Se busca la lista asociada al tipo de producto ingresado

    if (aux == NULL)
    {
        printf("No existe ningún producto de este tipo.\n");
        return;
    }

    tipoProducto* recorrer = (tipoProducto*) calloc (1, sizeof(tipoProducto*));
    recorrer = (tipoProducto*) firstList(aux); //Se crea una variable para recorrer la lista

    while (recorrer != NULL) //Se recorre la lista
    {
        printf("%s, ", recorrer->nombre);
        printf("%s, ", recorrer->marca);
        printf("%s, ", recorrer->tipo);
        printf("%d, ", recorrer->stock);
        printf("%d\n", recorrer->precio);
        printf("\n");

        recorrer = (tipoProducto*) nextList(aux);
    }
}

void buscarProductosMarca(tipoCatalogo *catalogo, char *marcaABuscar)
{
    List* aux = searchMap(catalogo->mapaMarca, marcaABuscar); //Se busca la lista asociada a la marca de producto ingresada

    if (aux == NULL)
    {
        printf("No existe ningún producto de esta marca.\n");
        return;
    }

    tipoProducto* recorrer = (tipoProducto*) calloc (1, sizeof(tipoProducto*));
    recorrer = (tipoProducto*) firstList(aux); //Se crea una variable para recorrer la lista

    while (recorrer != NULL) //Se recorre la lista
    {
        printf("%s, ", recorrer->nombre);
        printf("%s, ", recorrer->marca);
        printf("%s, ", recorrer->tipo);
        printf("%d, ", recorrer->stock);
        printf("%d\n", recorrer->precio);
        printf("\n");

        recorrer = (tipoProducto*) nextList(aux);
    }
}

void buscarProductosNombre(tipoCatalogo *catalogo, char *nombreProducto)
{
    tipoProducto *aux = (tipoProducto*) searchMap(catalogo->mapaNombre, nombreProducto); //Se busca un producto con nombre igual al ingresado

    if (aux == NULL) printf("No existe ningún producto con este nombre.\n");
    else
    {
        printf("%s, ", aux->nombre);
        printf("%s, ", aux->marca);
        printf("%s, ", aux->tipo);
        printf("%d, ", aux->stock);
        printf("%d\n", aux->precio);
    }
}

void mostrarTodosLosProductos(tipoCatalogo *catalogo)
{
    tipoProducto *aux = (tipoProducto*) firstMap(catalogo->mapaNombre); 

    while (aux != NULL) 
    {
        printf("%s, ", aux->nombre);
        printf("%s, ", aux->marca);
        printf("%s, ", aux->tipo);
        printf("%d, ", aux->stock);
        printf("%d\n", aux->precio);

        aux = (tipoProducto*) nextMap(catalogo->mapaNombre);
    }
}

void agregarAlCarrito(tipoCatalogo *catalogo, char *nombreProducto, char cantidad, char* nombreCarrito)
{
    tipoCarrito* productoAIngresar = (tipoCarrito*) calloc(1, sizeof(tipoCarrito)); 
    int largo = strlen(nombreProducto) + 1;

    productoAIngresar->totalProducto = cantidad;
    productoAIngresar->nombreProducto = reservarMemoria(largo);
    strcpy(productoAIngresar->nombreProducto, nombreProducto);

    largo = strlen(nombreCarrito) + 1;
    productoAIngresar->nombreCarrito = reservarMemoria(largo);
    strcpy(productoAIngresar->nombreCarrito, nombreCarrito);

    //Se crea una variable tipoCarrito y en ella se guarda el nombre del producto que se quiere agregar al carrito, la cantidad 
    //que se quiere agregar y el nombre del carrito en el que se quiere agregar

    List* listaCarritos = searchMap(catalogo->mapaCarritos, nombreCarrito); //Se busca el carrito ingresado por el usuario

    if (listaCarritos == NULL) //Si el carrito no existe, se crea y se le agrega el producto
    {
        listaCarritos = createList();
        if (searchMap(catalogo->mapaNombre, nombreProducto) == NULL) printf("El producto no se pudo ingresar al carrito.\n");
        else 
        {
            pushBack(listaCarritos, productoAIngresar);
            insertMap(catalogo->mapaCarritos, nombreCarrito, listaCarritos);
            printf("Producto ingresado correctamente al carrito.\n");
        }
    }
    else //Si ya existe, simplemente se agrega el producto 
    {
        if (searchMap(catalogo->mapaNombre, nombreProducto) == NULL) printf("El producto no se pudo ingresar al carrito.\n");
        else pushBack(listaCarritos, productoAIngresar);
    }
}

void eliminarDelCarrito(tipoCatalogo *catalogo, char *nombreCarrito)
{
    List *listaCarritos = searchMap(catalogo->mapaCarritos, nombreCarrito); //Se busca el carrito ingresado por el usuario
    
    if (listaCarritos == NULL) 
    {
        printf("No existe este carrito. El producto no se pudo eliminar.\n");
        return;
    }
    else 
    {
        printf("Producto eliminado correctamente del carrito.\n");
        listaCarritos = popBack(listaCarritos); //Se elimina el último producto ingresado
    }
}

int totalAPagar(tipoCatalogo *catalogo, char *nombreCarrito)
{
    List *listaCarritos = searchMap(catalogo->mapaCarritos, nombreCarrito); //Se busca el carrito ingresado por el usuario

    tipoCarrito *recorrer = (tipoCarrito*) calloc (1, sizeof(tipoCarrito)); //Se crea una variable para recorrer el carrito
    recorrer = (tipoCarrito*) firstList(listaCarritos);

    tipoProducto *aux; //Se crea una variable para recorrer el "mapaNombre"

    int total = 0;

    while (recorrer != NULL) //Se recorre todo el carrito
    {
        aux = (tipoProducto*) firstMap(catalogo->mapaNombre);

        while (aux != NULL) //Se recorre todo el "mapaNombre"
        {
            if (strcmp(aux->nombre, recorrer->nombreProducto) == 0) //Si el nombre del producto del carrito coincide con el nombre del
                                                                    //producto que se está leyendo, se aumenta el total a pagar
            {
                total = total + (aux->precio * recorrer->totalProducto);
            }

            aux = (tipoProducto*) nextMap(catalogo->mapaNombre);
        }

        recorrer = (tipoCarrito*) nextList(listaCarritos);
    }

    return total;
}

void restarStock(tipoCatalogo *catalogo, tipoCarrito *recorrer)
{
    tipoProducto *aux = (tipoProducto*) firstMap(catalogo->mapaNombre);

    while (aux != NULL) //Se recorre todo el "mapaNombre"
    {
        if (strcmp(aux->nombre, recorrer->nombreProducto) == 0) //Si el nombre del producto que se quiere comprar coincide con el nombre
                                                                //del producto del mapa que se está leyendo, se resta su stock
        {
            aux->stock = aux->stock - recorrer->totalProducto;
        }

        aux = (tipoProducto*) nextMap(catalogo->mapaNombre);
    }
}

void completarCompra(tipoCatalogo *catalogo, char *nombreCarrito)
{
    List *aux = searchMap(catalogo->mapaCarritos, nombreCarrito); //Se busca el carrito ingresado por el usuario

    if (aux == NULL) 
    {
        printf("No existe este carrito. No se puede concretar la compra.\n");
        return;
    }

    int total = totalAPagar(catalogo, nombreCarrito);
    printf("Total a pagar por el carrito = $%d\n", total);

    tipoCarrito *recorrer = (tipoCarrito*) calloc (1, sizeof(tipoCarrito));
    recorrer = (tipoCarrito*) firstList(aux);

    printf("Productos del carrito:\n");

    while (recorrer != NULL)
    {
        printf("%s, ", recorrer->nombreProducto);
        printf("%d\n", recorrer->totalProducto);
        restarStock(catalogo, recorrer);

        recorrer = (tipoCarrito*) nextList(aux);
    }

    cleanList(aux); //Se vacía el carrito
    eraseMap(catalogo->mapaCarritos, nombreCarrito); //Se elimina el carrito del mapa
}

int cantidadDeProductos(tipoCatalogo *catalogo, char *nombreCarrito)
{
    List *listaCarritos = searchMap(catalogo->mapaCarritos, nombreCarrito); //Se busca el carrito que se está leyendo en el momento

    tipoCarrito *recorrer = (tipoCarrito*) calloc (1, sizeof(tipoCarrito));
    recorrer = (tipoCarrito*) firstList(listaCarritos);

    int total = 0;

    while (recorrer != NULL)
    {
        total = total + (recorrer->totalProducto); //Se va sumando el total de productos que hay en el carrito
        recorrer = (tipoCarrito*) nextList(listaCarritos);
    }

    return total;
}

void mostrarCarritosDeCompra(tipoCatalogo *catalogo)
{
    List *aux = firstMap(catalogo->mapaCarritos); //Se busca el primer carrito del mapa

    if (aux == NULL)
    {
        printf("No existe ningún carrito.\n");
        return;
    }

    tipoCarrito *recorrer = (tipoCarrito*) calloc (1, sizeof(tipoCarrito));
    recorrer = (tipoCarrito*) firstList(aux); //Se crea una variable para leer el primer producto del carrito 

    int cantidad;

    while (aux != NULL) //Se recorre hasta que no hayan más carritos
    {
        if (recorrer != NULL) //Solo se muestran los carritos que contienen productos
        {
            printf("%s, ", recorrer->nombreCarrito);
            cantidad = cantidadDeProductos(catalogo, recorrer->nombreCarrito);
            printf("%d productos.\n", cantidad);
        }

        aux = nextMap(catalogo->mapaCarritos); //Se mueve al siguiente carrito del mapa
        recorrer = (tipoCarrito*) firstList(aux); //Se lee el primer producto del siguiente carrito
    }
}

void mostrarOpcionesMenu()
{
    printf("\n1.- Importar productos.\n");
    printf("2.- Exportar productos.\n");
    printf("3.- Agregar producto.\n");
    printf("4.- Buscar productos por tipo.\n");
    printf("5.- Buscar productos por marca.\n");
    printf("6.- Buscar producto por nombre.\n");
    printf("7.- Mostrar todos los productos.\n");
    printf("8.- Agregar un producto al carrito.\n");
    printf("9.- Eliminar un producto del carrito.\n");
    printf("10.- Finalizar compra.\n");
    printf("11.- Mostrar carritos de compra\n");
    printf("12.- Salir del programa.\n");
    printf("Seleccione una opcion.\n");
}

int main() 
{
    tipoCatalogo *catalogo = crearCatalogo(); //Se crea la variable que almacenará todos los mapas del programa
    int opcion;

    while(1)
    {
        do
        {
            mostrarOpcionesMenu();
            scanf("%d", &opcion);
            getchar();
        } while (opcion < 1 || opcion > 12);

        switch (opcion)
        {
            case 1:
            {
                char *nombreArchivo = NULL;
                printf("\nIngrese el archivo a importar:\n");
                leerChar(&nombreArchivo);
                importarProductos(catalogo, nombreArchivo);
                break;
            }
            case 2:
            {
                printf("\nIngrese el nombre del archivo al que desea exportar:\n");
                char *archivoAExportar = NULL;
                leerChar(&archivoAExportar);
                exportarProductos(catalogo, archivoAExportar);
                break;
            }
            case 3:
            {
                printf("\nA continuación ingrese los datos del producto que desea agregar:\n");
                tipoProducto *nuevoProducto = (tipoProducto*) malloc(sizeof(tipoProducto));
                printf("Ingrese el nombre del producto:\n");
                leerChar(&nuevoProducto->nombre);
                printf("Ingrese la marca del producto:\n");
                leerChar(&nuevoProducto->marca);
                printf("Ingrese el tipo del producto:\n");
                leerChar(&nuevoProducto->tipo);
                printf("Ingrese el stock del producto:\n");
                scanf("%d", &nuevoProducto->stock);
                printf("Ingrese el precio del producto:\n");
                scanf("%d", &nuevoProducto->precio);
                agregarProducto(catalogo, nuevoProducto);
                break;
            }
            case 4:
            {
                printf("\nIngrese el tipo de producto que desea buscar:\n");
                char *tipoABuscar;
                leerChar(&tipoABuscar);
                buscarProductosTipo(catalogo, tipoABuscar);
                break;
            }
            case 5:
            {
                printf("\nIngrese la marca de producto que desea buscar:\n");
                char *marcaABuscar;
                leerChar(&marcaABuscar);
                buscarProductosMarca(catalogo, marcaABuscar);
                break;
            }
            case 6:
            {
                printf("\nIngrese el nombre del producto que desea buscar:\n");
                char *nombreProducto;
                leerChar(&nombreProducto);
                buscarProductosNombre(catalogo, nombreProducto);
                break;
            }
            case 7:
            {
                printf("\nA continuación se mostrarán todos los productos disponibles, y cada uno con su respectiva información:\n");
                mostrarTodosLosProductos(catalogo);
                break;
            }
            case 8:
            {
                printf("\nIngrese el nombre del carrito al que desea agregar su producto:\n");
                char *nombreCarrito;
                char *nombreProducto;
                int cantidad;
                leerChar(&nombreCarrito);
                printf("Ingrese el nombre del producto que desea agregar al carrito:\n");
                leerChar(&nombreProducto);
                printf("Ingrese la cantidad del producto que desea agregar al carrito:\n");
                scanf("%d", &cantidad);
                agregarAlCarrito(catalogo, nombreProducto, cantidad, nombreCarrito);
                break;
            }
            case 9:
            {
                printf("\nIngrese el nombre del carrito del que desea eliminar el último producto ingresado:\n");
                char *nombreCarrito;
                leerChar(&nombreCarrito);
                eliminarDelCarrito(catalogo, nombreCarrito);
                break;
            }
            case 10:
            {
                printf("\nIngrese el nombre del carrito del que desea concretar su compra:\n");
                char *nombreCarrito;
                leerChar(&nombreCarrito);
                completarCompra(catalogo, nombreCarrito);
                break;
            }
            case 11:
            {
                printf("\nA continuación se mostrarán todos los carritos existentes y la cantidad de productos que tiene cada uno:\n");
                mostrarCarritosDeCompra(catalogo);
                break;
            }
            case 12:
            {
                printf("\nGracias por utilizar el programa.\n");
                return 0;
            }
        }

        do
        {
            printf("\n¿Desea continuar?\n");
            printf("1- Si. / 2- NO.\n");
            scanf("%d", &opcion);
            getchar();
        } while (opcion < 1 || opcion > 2);

        if (opcion == 2)
        {
            printf("\nGracias por utilizar el programa\n");
            break;
        }
    }
    return 0;
}
