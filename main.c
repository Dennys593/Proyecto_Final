#include <stdio.h>
#include "funciones.h"

int main(void) {
    Zona zonas[MAX_ZONAS];
    int opcion;
    int indice;

    inicializarZonas(zonas, MAX_ZONAS);

    if (cargarDatosArchivo(zonas, MAX_ZONAS, ARCHIVO_DATOS)) {
        printf("Datos cargados desde archivo correctamente.\n");
    } else {
        printf("No existe archivo de datos. Se cargaran datos de prueba.\n");
        cargarDatosPrueba(zonas, MAX_ZONAS);
    }

    do {
        mostrarMenu();
        opcion = leerEntero("Seleccione una opcion: ", 0, 9);

        switch (opcion) {
            case 1:
                mostrarTodasZonasTabla(zonas, MAX_ZONAS);
                break;

            case 2:
                mostrarPrediccionesTabla(zonas, MAX_ZONAS);
                break;

            case 3:
                mostrarAlertasTabla(zonas, MAX_ZONAS);
                break;

            case 4:
                mostrarPromediosTabla(zonas, MAX_ZONAS);
                break;

            case 5:
                mostrarRecomendacionesTabla(zonas, MAX_ZONAS);
                break;

            case 6:
                indice = leerEntero("Ingrese el numero de zona a editar (1-5): ", 1, MAX_ZONAS);
                ingresarDatosZona(&zonas[indice - 1]);
                guardarDatosArchivo(zonas, MAX_ZONAS, ARCHIVO_DATOS);
                printf("Datos de la zona actualizados y guardados.\n");
                break;

            case 7:
                if (guardarDatosArchivo(zonas, MAX_ZONAS, ARCHIVO_DATOS)) {
                    printf("Datos guardados correctamente en '%s'.\n", ARCHIVO_DATOS);
                }
                break;

            case 8:
                guardarReporte(zonas, MAX_ZONAS, ARCHIVO_REPORTE);
                break;

            case 9:
                guardarHistoricoCSV(zonas, MAX_ZONAS, ARCHIVO_CSV);
                break;

            case 0:
                guardarDatosArchivo(zonas, MAX_ZONAS, ARCHIVO_DATOS);
                printf("Saliendo del sistema...\n");
                break;

            default:
                printf("Opcion invalida.\n");
                break;
        }

    } while (opcion != 0);

    return 0;
}
