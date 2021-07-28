/*=============================================================================
 * Date: 2021/07/08
 * Version: v1.0
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "main.h"
#include "capa1_impresion.h"
#include "capa2_proceso.h"
#include "capa3_aplicacion.h"

#include "qmpool.h"
/*==================[definiciones de datos internos]=========================*/

tPool poolPtr_200, poolPtr_100, poolPtr_50, poolPtr_25; //puntero al segmento de memoria que albergara el pool

QMPool poolMem_200, poolMem_100, poolMem_50, poolMem_25; //memory pool (contienen la informacion que necesita la biblioteca qmpool.h)

QueueHandle_t queue_c2_to_c3;
QueueHandle_t queue_c3_to_c2;
QueueHandle_t queue_c2_to_c1;

/*==================[definiciones de datos externos]=========================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int
main (void)
{
  // ---------- CONFIGURACIONES ------------------------------
  boardConfig ();					// Inicializar y configurar la plataforma

  //	Reservo memoria para el memory pool
  poolPtr_200 = (tPool) pvPortMalloc (POOL_SIZE * sizeof(char));
  configASSERT(poolPtr_200!=NULL);
  poolPtr_100 = (tPool) pvPortMalloc (POOL_SIZE * sizeof(char));
  configASSERT(poolPtr_100!=NULL);
  poolPtr_50 = (tPool) pvPortMalloc (POOL_SIZE * sizeof(char));
  configASSERT(poolPtr_50!=NULL);
  poolPtr_25 = (tPool) pvPortMalloc (POOL_SIZE * sizeof(char));
  configASSERT(poolPtr_25!=NULL);

  //	Creo los pools de memoria
  QMPool_init (&poolMem_200, (tPool) poolPtr_200, POOL_SIZE * sizeof(tPool),
  PACKET_SIZE_200); //Tamanio del segmento de memoria reservado
  QMPool_init (&poolMem_100, (tPool) poolPtr_100, POOL_SIZE * sizeof(tPool),
  PACKET_SIZE_100);
  QMPool_init (&poolMem_50, (tPool) poolPtr_50, POOL_SIZE * sizeof(tPool),
  PACKET_SIZE_50);
  QMPool_init (&poolMem_25, (tPool) poolPtr_25, POOL_SIZE * sizeof(tPool),
  PACKET_SIZE_25);

  // Creo las COLAS para cominicar las diferentes capas

  queue_c2_to_c1 = xQueueCreate(N_QUEUE, sizeof(_sFrame));
  configASSERT(queue_c2_to_c1 != NULL); // Gestion de errores de colas

  queue_c2_to_c3 = xQueueCreate(N_QUEUE, sizeof(_sFrame));
  configASSERT(queue_c2_to_c3 != NULL); // Gestion de errores de colas

  queue_c3_to_c2 = xQueueCreate(N_QUEUE, sizeof(_sFrame));
  configASSERT(queue_c3_to_c2 != NULL); // Gestion de errores de colas

  //inicialización de capas
  init_capa1_impresion (UART_USB);
  init_capa2_proceso (UART_USB);
  init_capa3_aplicacion ();
  // Iniciar scheduler
  vTaskStartScheduler (); // Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

  // ---------- REPETIR POR SIEMPRE --------------------------
  configASSERT(0);

  // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
  // directamenteno sobre un microcontroladore y no es llamado por ningun
  // Sistema Operativo, como en el caso de un programa para PC.
  return TRUE;
}

/* hook que se ejecuta si al necesitar un objeto dinamico, no hay memoria disponible */
void
vApplicationMallocFailedHook ()
{
  printf ("Malloc Failed Hook!\n");
  configASSERT(0);
}
/*==================[fin del archivo]========================================*/
