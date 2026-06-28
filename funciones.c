#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int leerEntero(char mensaje[], int min, int max) {
    int valor, correcto;
    do {
        printf("%s", mensaje);
        correcto = scanf("%d", &valor);
        if (correcto != 1 || valor < min || valor > max) {
            printf("Error. Ingrese un numero entre %d y %d.\n", min, max);
            limpiarBuffer();
        }
    } while (correcto != 1 || valor < min || valor > max);
    limpiarBuffer();
    return valor;
}

float leerFloat(char mensaje[], float min, float max) {
    float valor;
    int correcto;
    do {
        printf("%s", mensaje);
        correcto = scanf("%f", &valor);
        if (correcto != 1 || valor < min || valor > max) {
            printf("Error. Ingrese un valor entre %.2f y %.2f.\n", min, max);
            limpiarBuffer();
        }
    } while (correcto != 1 || valor < min || valor > max);
    limpiarBuffer();
    return valor;
}

void leerTexto(char mensaje[], char texto[], int tam) {
    int valido, i;
    do {
        valido = 1;
        printf("%s", mensaje);
        fgets(texto, tam, stdin);
        if (texto[0] == '\n' || texto[0] == '\0') {
            printf("Error. El texto no puede estar vacio.\n");
            valido = 0;
        }
    } while (valido == 0);

    for (i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == '\n') {
            texto[i] = '\0';
            break;
        }
    }
}

void inicializarZonas(Zona zonas[]) {
    int i, j;
    for (i = 0; i < MAX_ZONAS; i++) {
        sprintf(zonas[i].nombre, "Zona_%d", i + 1);
        zonas[i].clima.temperatura = 0;
        zonas[i].clima.viento = 0;
        zonas[i].clima.humedad = 0;
        zonas[i].actual.co2 = zonas[i].actual.so2 = zonas[i].actual.no2 = zonas[i].actual.pm25 = 0;
        zonas[i].prediccion.co2 = zonas[i].prediccion.so2 = zonas[i].prediccion.no2 = zonas[i].prediccion.pm25 = 0;
        for (j = 0; j < MAX_DIAS; j++) {
            zonas[i].historico[j].co2 = 0;
            zonas[i].historico[j].so2 = 0;
            zonas[i].historico[j].no2 = 0;
            zonas[i].historico[j].pm25 = 0;
        }
    }
}

int cargarDatos(Zona zonas[]) {
    FILE *archivo = fopen(ARCHIVO_DATOS, "rb");
    if (archivo == NULL) return 0;
    fread(zonas, sizeof(Zona), MAX_ZONAS, archivo);
    fclose(archivo);
    return 1;
}

void guardarDatos(Zona zonas[]) {
    FILE *archivo = fopen(ARCHIVO_DATOS, "wb");
    if (archivo == NULL) {
        printf("No se pudo guardar el archivo de datos.\n");
        return;
    }
    fwrite(zonas, sizeof(Zona), MAX_ZONAS, archivo);
    fclose(archivo);
}

void mostrarMenu() {
    printf("\n           SISTEMA DE CONTAMINACION DEL AIRE           \n");
    printf("1. Monitoreo actual\n");
    printf("2. Prediccion de contaminacion 24 horas\n");
    printf("3. Alertas preventivas\n");
    printf("4. Promedio historico de 30 dias\n");
    printf("5. Recomendaciones de mitigacion\n");
    printf("6. Ingresar o actualizar datos\n");
    printf("0. Salir\n");
    printf("====================================================================\n");
}

void ingresarDatosZona(Zona *zona) {
    int i;
    leerTexto("Nombre de la zona: ", zona->nombre, MAX_NOMBRE);

    printf("\nIngrese factores climaticos actuales:\n");
    zona->clima.temperatura = leerFloat("Temperatura: ", 0, 60);
    zona->clima.viento = leerFloat("Velocidad del viento: ", 0, 200);
    zona->clima.humedad = leerFloat("Humedad: ", 0, 100);

    printf("\nIngrese datos historicos de contaminacion de los ultimos 30 dias:\n");
    for (i = 0; i < MAX_DIAS; i++) {
        printf("\nDia %d\n", i + 1);
        zona->historico[i].co2 = leerFloat("CO2: ", 0, 5000);
        zona->historico[i].so2 = leerFloat("SO2: ", 0, 1000);
        zona->historico[i].no2 = leerFloat("NO2: ", 0, 1000);
        zona->historico[i].pm25 = leerFloat("PM2.5: ", 0, 1000);
    }

    calcularActual(zona);
    predecirZona(zona);
}

void ingresarDatosSistema(Zona zonas[]) {
    int opcion, indice, i;
    printf("\n1. Ingresar datos de una sola zona\n");
    printf("2. Ingresar datos de las 5 zonas\n");
    opcion = leerEntero("Seleccione una opcion: ", 1, 2);

    if (opcion == 1) {
        indice = leerEntero("Ingrese el numero de zona (1-5): ", 1, MAX_ZONAS);
        ingresarDatosZona(&zonas[indice - 1]);
    } else {
        for (i = 0; i < MAX_ZONAS; i++) {
            printf("\n========== ZONA %d ==========\n", i + 1);
            ingresarDatosZona(&zonas[i]);
        }
    }

    guardarDatos(zonas);
    generarReporte(zonas);
    printf("\nDatos guardados correctamente.\n");
    printf("Reporte generado automaticamente en %s\n", ARCHIVO_REPORTE);
}

void calcularActual(Zona *zona) {
    zona->actual = zona->historico[MAX_DIAS - 1];
}

float promedioPonderado(float valores[]) {
    float suma = 0, sumaPesos = 0;
    int i;
    for (i = 0; i < MAX_DIAS; i++) {
        int peso = i + 1;
        suma += valores[i] * peso;
        sumaPesos += peso;
    }
    return suma / sumaPesos;
}

void predecirZona(Zona *zona) {
    float co2[MAX_DIAS], so2[MAX_DIAS], no2[MAX_DIAS], pm25[MAX_DIAS];
    float factor = 1.0f;
    int i;

    for (i = 0; i < MAX_DIAS; i++) {
        co2[i] = zona->historico[i].co2;
        so2[i] = zona->historico[i].so2;
        no2[i] = zona->historico[i].no2;
        pm25[i] = zona->historico[i].pm25;
    }

    if (zona->clima.temperatura > 30) factor += 0.10f;
    if (zona->clima.humedad > 70) factor += 0.05f;
    if (zona->clima.viento > 20) factor -= 0.10f;
    if (factor < 0.50f) factor = 0.50f;

    zona->prediccion.co2 = promedioPonderado(co2) * factor;
    zona->prediccion.so2 = promedioPonderado(so2) * factor;
    zona->prediccion.no2 = promedioPonderado(no2) * factor;
    zona->prediccion.pm25 = promedioPonderado(pm25) * factor;
}

void calcularPromedioHistorico(Zona *zona, Contaminacion *promedio) {
    int i;
    float co2 = 0, so2 = 0, no2 = 0, pm25 = 0;
    for (i = 0; i < MAX_DIAS; i++) {
        co2 += zona->historico[i].co2;
        so2 += zona->historico[i].so2;
        no2 += zona->historico[i].no2;
        pm25 += zona->historico[i].pm25;
    }
    promedio->co2 = co2 / MAX_DIAS;
    promedio->so2 = so2 / MAX_DIAS;
    promedio->no2 = no2 / MAX_DIAS;
    promedio->pm25 = pm25 / MAX_DIAS;
}

int superaLimites(Contaminacion datos) {
    if (datos.co2 > LIMITE_CO2) return 1;
    if (datos.so2 > LIMITE_SO2) return 1;
    if (datos.no2 > LIMITE_NO2) return 1;
    if (datos.pm25 > LIMITE_PM25) return 1;
    return 0;
}

void generarAlerta(Zona zona, char alerta[]) {
    if (superaLimites(zona.actual) || superaLimites(zona.prediccion))
        strcpy(alerta, "ALERTA: niveles de contaminacion superan los limites.");
    else
        strcpy(alerta, "Sin alerta: niveles aceptables.");
}

void generarRecomendacion(Zona zona, char recomendacion[]) {
    if (superaLimites(zona.actual) || superaLimites(zona.prediccion))
        strcpy(recomendacion, "Reducir trafico, controlar industrias y evitar actividades al aire libre.");
    else
        strcpy(recomendacion, "Mantener monitoreo y medidas actuales.");
}

void mostrarMonitoreoActual(Zona zonas[]) {
    int i;
    printf("\n+----+--------------------+----------+----------+----------+----------+\n");
    printf("| ID | Zona               | CO2      | SO2      | NO2      | PM2.5    |\n");
    printf("+----+--------------------+----------+----------+----------+----------+\n");
    for (i = 0; i < MAX_ZONAS; i++) {
        calcularActual(&zonas[i]);
        printf("| %-2d | %-18s | %-8.2f | %-8.2f | %-8.2f | %-8.2f |\n",
               i + 1, zonas[i].nombre, zonas[i].actual.co2, zonas[i].actual.so2,
               zonas[i].actual.no2, zonas[i].actual.pm25);
    }
    printf("+----+--------------------+----------+----------+----------+----------+\n");
}

void mostrarPredicciones(Zona zonas[]) {
    int i;
    printf("\n+----+--------------------+----------+----------+----------+----------+\n");
    printf("| ID | Zona               | CO2      | SO2      | NO2      | PM2.5    |\n");
    printf("+----+--------------------+----------+----------+----------+----------+\n");
    for (i = 0; i < MAX_ZONAS; i++) {
        predecirZona(&zonas[i]);
        printf("| %-2d | %-18s | %-8.2f | %-8.2f | %-8.2f | %-8.2f |\n",
               i + 1, zonas[i].nombre, zonas[i].prediccion.co2, zonas[i].prediccion.so2,
               zonas[i].prediccion.no2, zonas[i].prediccion.pm25);
    }
    printf("+----+--------------------+----------+----------+----------+----------+\n");
}

void mostrarAlertas(Zona zonas[]) {
    int i;
    char alerta[MAX_TEXTO];
    printf("\n+----+--------------------+------------------------------------------------+\n");
    printf("| ID | Zona               | Alerta                                         |\n");
    printf("+----+--------------------+------------------------------------------------+\n");
    for (i = 0; i < MAX_ZONAS; i++) {
        generarAlerta(zonas[i], alerta);
        printf("| %-2d | %-18s | %-46s |\n", i + 1, zonas[i].nombre, alerta);
    }
    printf("+----+--------------------+------------------------------------------------+\n");
}

void mostrarPromedios(Zona zonas[]) {
    int i;
    Contaminacion promedio;
    printf("\n+----+--------------------+----------+----------+----------+----------+\n");
    printf("| ID | Zona               | CO2      | SO2      | NO2      | PM2.5    |\n");
    printf("+----+--------------------+----------+----------+----------+----------+\n");
    for (i = 0; i < MAX_ZONAS; i++) {
        calcularPromedioHistorico(&zonas[i], &promedio);
        printf("| %-2d | %-18s | %-8.2f | %-8.2f | %-8.2f | %-8.2f |\n",
               i + 1, zonas[i].nombre, promedio.co2, promedio.so2, promedio.no2, promedio.pm25);
    }
    printf("+----+--------------------+----------+----------+----------+----------+\n");
}

void mostrarRecomendaciones(Zona zonas[]) {
    int i;
    char recomendacion[MAX_TEXTO];
    printf("\n+----+--------------------+------------------------------------------------+\n");
    printf("| ID | Zona               | Recomendacion                                  |\n");
    printf("+----+--------------------+------------------------------------------------+\n");
    for (i = 0; i < MAX_ZONAS; i++) {
        generarRecomendacion(zonas[i], recomendacion);
        printf("| %-2d | %-18s | %-46s |\n", i + 1, zonas[i].nombre, recomendacion);
    }
    printf("+----+--------------------+------------------------------------------------+\n");
}

void generarReporte(Zona zonas[]) {
    FILE *archivo = fopen(ARCHIVO_REPORTE, "w");
    int i;
    char alerta[MAX_TEXTO], recomendacion[MAX_TEXTO];
    Contaminacion promedio;

    if (archivo == NULL) {
        printf("No se pudo crear el reporte.");
        return;
    }

    fprintf(archivo,"====================================================================================================\n");
    fprintf(archivo,"                           REPORTE DE CONTAMINACION DEL AIRE\n");
    fprintf(archivo,"====================================================================================================\n\n");

    for(i=0;i<MAX_ZONAS;i++){
        calcularActual(&zonas[i]);
        predecirZona(&zonas[i]);
        calcularPromedioHistorico(&zonas[i],&promedio);
        generarAlerta(zonas[i],alerta);
        generarRecomendacion(zonas[i],recomendacion);

        fprintf(archivo,"ZONA %d: %s\n",i+1,zonas[i].nombre);
        fprintf(archivo,"+----------------------+------------+------------+------------+------------+\n");
        fprintf(archivo,"| Indicador            |    CO2     |    SO2     |    NO2     |   PM2.5    |\n");
        fprintf(archivo,"+----------------------+------------+------------+------------+------------+\n");
        fprintf(archivo,"| Nivel actual         | %10.2f | %10.2f | %10.2f | %10.2f |\n",zonas[i].actual.co2,zonas[i].actual.so2,zonas[i].actual.no2,zonas[i].actual.pm25);
        fprintf(archivo,"| Prediccion (24 h)    | %10.2f | %10.2f | %10.2f | %10.2f |\n",zonas[i].prediccion.co2,zonas[i].prediccion.so2,zonas[i].prediccion.no2,zonas[i].prediccion.pm25);
        fprintf(archivo,"| Promedio historico   | %10.2f | %10.2f | %10.2f | %10.2f |\n",promedio.co2,promedio.so2,promedio.no2,promedio.pm25);
        fprintf(archivo,"+----------------------+------------+------------+------------+------------+\n");
        fprintf(archivo,"Estado: %s\n",alerta);
        fprintf(archivo,"Recomendacion: %s\n",recomendacion);
        fprintf(archivo,"----------------------------------------------------------------------------------------------------\n\n");
    }
    fclose(archivo);
}