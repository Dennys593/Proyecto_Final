#include <stdio.h>
#include "funciones.h"

int main(void) {
    Zona zonas[MAX_ZONAS];
    int numZonas = MAX_ZONAS;
    int opcion;
    int i;
    char alerta[MAX_LINEA];
    char recomendacion[MAX_LINEA];

    inicializarZonas(zonas, numZonas);
    cargarDatosPrueba(zonas, numZonas); 

    for (i = 0; i < numZonas; i++) {
        calcularNivelActual(&zonas[i]);
        calcularPrediccion(&zonas[i]);
    }

    do {
        mostrarMenu();
        scanf("%d", &opcion);

        switch (opcion) {

            case 1:
                mostrarTodasZonas(zonas, numZonas);
                break;

            case 2:
                for (i = 0; i < numZonas; i++) {
                    calcularPrediccion(&zonas[i]);
                    mostrarPrediccion(&zonas[i]);
                }
                break;

            case 3:
                for (i = 0; i < numZonas; i++) {
                    calcularNivelActual(&zonas[i]);
                    calcularPrediccion(&zonas[i]);
                    generarAlerta(&zonas[i], alerta, MAX_LINEA);
                    printf("\n%s\n", alerta);
                }
                break;

            case 4:
                for (i = 0; i < numZonas; i++) {
                    mostrarPromedioHistorico(&zonas[i]);
                }
                break;

            case 5:
                for (i = 0; i < numZonas; i++) {
                    calcularNivelActual(&zonas[i]);
                    calcularPrediccion(&zonas[i]);
                    generarRecomendaciones(&zonas[i], recomendacion, MAX_LINEA);
                    printf("\nZona: %s\nRecomendacion: %s\n", zonas[i].nombre, recomendacion);
                }
                break;

            case 6: {
                int idx;
                printf("Ingrese el numero de zona a editar (1-%d): ", numZonas);
                scanf("%d", &idx);
                if (idx >= 1 && idx <= numZonas) {
                    ingresarDatosZona(&zonas[idx - 1]);
                    calcularNivelActual(&zonas[idx - 1]);
                    calcularPrediccion(&zonas[idx - 1]);
                    printf("Datos actualizados correctamente.\n");
                } else {
                    printf("Numero de zona invalido.\n");
                }
                break;
            }

            case 7:
                guardarReporte(zonas, numZonas, "reporte_contaminacion.txt");
                break;

            case 8:
                guardarHistoricoCSV(zonas, numZonas, "historico_contaminacion.csv");
                break;

            case 0:
                printf("Saliendo del sistema...\n");
                break;

            default:
                printf("Opcion invalida, intente nuevamente.\n");
        }

    } while (opcion != 0);

    return 0;
}