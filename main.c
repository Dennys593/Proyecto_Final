#include <stdio.h>
#include "funciones.h"

int main() {
    Zona zonas[MAX_ZONAS];
    int opcion;

    inicializarZonas(zonas);

    if (cargarDatos(zonas)) {
        printf("Datos cargados desde archivo.\n");
    } else {
        printf("No hay datos guardados. Ingrese datos desde la opcion 6.\n");
    }

    do {
        mostrarMenu();
        opcion = leerEntero("Seleccione una opcion: ", 0, 6);

        switch (opcion) {
            case 1:
                mostrarMonitoreoActual(zonas);
                break;
            case 2:
                mostrarPredicciones(zonas);
                break;
            case 3:
                mostrarAlertas(zonas);
                break;
            case 4:
                mostrarPromedios(zonas);
                break;
            case 5:
                mostrarRecomendaciones(zonas);
                break;
            case 6:
                ingresarDatosSistema(zonas);
                break;
            case 0:
                guardarDatos(zonas);
                generarReporte(zonas);
                printf("Datos guardados. Saliendo del sistema...\n");
                break;
            default:
                printf("Opcion no valida.\n");
                break;
        }
    } while (opcion != 0);

    return 0;
}
