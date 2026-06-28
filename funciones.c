#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funciones.h"

void inicializarZonas(Zona *zonas, int numZonas) {
    int i, j;
    for (i = 0; i < numZonas; i++) {
        (zonas + i)->numDias = 0;
        (zonas + i)->climaActual.temperatura = 0.0f;
        (zonas + i)->climaActual.viento = 0.0f;
        (zonas + i)->climaActual.humedad = 0.0f;

        for (j = 0; j < MAX_DIAS; j++) {
            (zonas + i)->historico[j].co2  = 0.0f;
            (zonas + i)->historico[j].so2  = 0.0f;
            (zonas + i)->historico[j].no2  = 0.0f;
            (zonas + i)->historico[j].pm25 = 0.0f;
        }
    }
}

void cargarDatosPrueba(Zona *zonas, int numZonas) {
    const char *nombres[MAX_ZONAS] = {
        "Centro Historico", "Norte Industrial", "Sur Residencial",
        "Valle de los Chillos", "Aeropuerto"
    };
    int i, d;

    srand((unsigned int) time(NULL));

    for (i = 0; i < numZonas; i++) {
        strncpy((zonas + i)->nombre, nombres[i % MAX_ZONAS], MAX_NOMBRE - 1);
        (zonas + i)->nombre[MAX_NOMBRE - 1] = '\0';

        (zonas + i)->numDias = MAX_DIAS;

        for (d = 0; d < MAX_DIAS; d++) {
            float base = 5.0f * (float) i;
            (zonas + i)->historico[d].co2  = 380.0f + base * 10 + (rand() % 150);
            (zonas + i)->historico[d].so2  = 10.0f  + base       + (rand() % 35);
            (zonas + i)->historico[d].no2  = 8.0f   + base       + (rand() % 22);
            (zonas + i)->historico[d].pm25 = 5.0f   + base       + (rand() % 18);
        }

        (zonas + i)->climaActual.temperatura = 15.0f + (rand() % 12);
        (zonas + i)->climaActual.viento      = 2.0f  + (rand() % 20);
        (zonas + i)->climaActual.humedad     = 40.0f + (rand() % 45);
    }
}

void ingresarDatosZona(Zona *zona) {
    int d;

    printf("Nombre de la zona: ");
    scanf(" %49[^\n]", zona->nombre);

    printf("Temperatura actual (C): ");
    scanf("%f", &zona->climaActual.temperatura);
    printf("Velocidad del viento (km/h): ");
    scanf("%f", &zona->climaActual.viento);
    printf("Humedad relativa (%%): ");
    scanf("%f", &zona->climaActual.humedad);

    printf("Cuantos dias de historico desea ingresar (max %d)? ", MAX_DIAS);
    scanf("%d", &zona->numDias);
    if (zona->numDias > MAX_DIAS) zona->numDias = MAX_DIAS;
    if (zona->numDias < 1) zona->numDias = 1;

    for (d = 0; d < zona->numDias; d++) {
        printf("  Dia %d - CO2 (ppm): ", d + 1);
        scanf("%f", &zona->historico[d].co2);
        printf("  Dia %d - SO2 (ug/m3): ", d + 1);
        scanf("%f", &zona->historico[d].so2);
        printf("  Dia %d - NO2 (ug/m3): ", d + 1);
        scanf("%f", &zona->historico[d].no2);
        printf("  Dia %d - PM2.5 (ug/m3): ", d + 1);
        scanf("%f", &zona->historico[d].pm25);
    }
}

void registrarNuevoDia(Zona *zona, Contaminantes nuevoDato) {
    int j;
    if (zona->numDias < MAX_DIAS) {
        zona->historico[zona->numDias] = nuevoDato;
        zona->numDias++;
    } else {
        for (j = 0; j < MAX_DIAS - 1; j++) {
            zona->historico[j] = zona->historico[j + 1];
        }
        zona->historico[MAX_DIAS - 1] = nuevoDato;
    }
}

void calcularNivelActual(Zona *zona) {
    int ultimo = zona->numDias - 1;
    if (ultimo < 0) return;
    zona->actual = zona->historico[ultimo];
}

float calcularPromedioPonderado(float *valores, int n) {
    float sumaPesos = 0.0f;
    float sumaValores = 0.0f;
    int i;

    for (i = 0; i < n; i++) {
        float peso = (float)(i + 1); 
        sumaValores += valores[i] * peso;
        sumaPesos   += peso;
    }
    if (sumaPesos == 0.0f) return 0.0f;
    return sumaValores / sumaPesos;
}

float calcularFactorClimatico(Clima *clima) {
    float factor = 1.0f;
    factor += (clima->humedad / 200.0f);
    factor += (clima->temperatura / 300.0f);
    factor -= (clima->viento / 80.0f);

    if (factor < 0.5f) factor = 0.5f;   
    if (factor > 1.6f) factor = 1.6f;
    return factor;
}

void calcularPrediccion(Zona *zona) {
    float co2vals[MAX_DIAS], so2vals[MAX_DIAS], no2vals[MAX_DIAS], pm25vals[MAX_DIAS];
    int i, n = zona->numDias;
    float factorClima;

    for (i = 0; i < n; i++) {
        co2vals[i]  = zona->historico[i].co2;
        so2vals[i]  = zona->historico[i].so2;
        no2vals[i]  = zona->historico[i].no2;
        pm25vals[i] = zona->historico[i].pm25;
    }

    factorClima = calcularFactorClimatico(&zona->climaActual);

    zona->prediccion.co2  = calcularPromedioPonderado(co2vals, n)  * factorClima;
    zona->prediccion.so2  = calcularPromedioPonderado(so2vals, n)  * factorClima;
    zona->prediccion.no2  = calcularPromedioPonderado(no2vals, n)  * factorClima;
    zona->prediccion.pm25 = calcularPromedioPonderado(pm25vals, n) * factorClima;
}

void calcularPromedioHistorico(Zona *zona, Contaminantes *promedio) {
    int i, n = zona->numDias;
    float sCo2 = 0, sSo2 = 0, sNo2 = 0, sPm25 = 0;

    for (i = 0; i < n; i++) {
        sCo2  += zona->historico[i].co2;
        sSo2  += zona->historico[i].so2;
        sNo2  += zona->historico[i].no2;
        sPm25 += zona->historico[i].pm25;
    }
    if (n == 0) n = 1;

    promedio->co2  = sCo2  / n;
    promedio->so2  = sSo2  / n;
    promedio->no2  = sNo2  / n;
    promedio->pm25 = sPm25 / n;
}

int contaminanteExcedeLimite(const char *tipo, float valor) {
    if (strcmp(tipo, "CO2") == 0)  return valor > LIMITE_CO2;
    if (strcmp(tipo, "SO2") == 0)  return valor > LIMITE_SO2;
    if (strcmp(tipo, "NO2") == 0)  return valor > LIMITE_NO2;
    if (strcmp(tipo, "PM25") == 0) return valor > LIMITE_PM25;
    return 0;
}

int evaluarLimites(Contaminantes *c) {
    int mascara = 0;
    if (contaminanteExcedeLimite("CO2", c->co2))   mascara |= 1;
    if (contaminanteExcedeLimite("SO2", c->so2))   mascara |= 2;
    if (contaminanteExcedeLimite("NO2", c->no2))   mascara |= 4;
    if (contaminanteExcedeLimite("PM25", c->pm25)) mascara |= 8;
    return mascara;
}

void generarAlerta(Zona *zona, char *buffer, int tamBuffer) {
    int mActual = evaluarLimites(&zona->actual);
    int mPred   = evaluarLimites(&zona->prediccion);
    char temp[MAX_LINEA];

    buffer[0] = '\0';

    if (mActual == 0 && mPred == 0) {
        snprintf(buffer, tamBuffer,
            "Sin alertas: los niveles actuales y previstos estan dentro de los limites de la OMS.");
        return;
    }

    if (mActual != 0) {
        snprintf(temp, MAX_LINEA, "ALERTA ACTUAL en %s -> excede limite en:", zona->nombre);
        strncat(buffer, temp, tamBuffer - strlen(buffer) - 1);
        if (mActual & 1) strncat(buffer, " CO2",  tamBuffer - strlen(buffer) - 1);
        if (mActual & 2) strncat(buffer, " SO2",  tamBuffer - strlen(buffer) - 1);
        if (mActual & 4) strncat(buffer, " NO2",  tamBuffer - strlen(buffer) - 1);
        if (mActual & 8) strncat(buffer, " PM2.5", tamBuffer - strlen(buffer) - 1);
        strncat(buffer, ". ", tamBuffer - strlen(buffer) - 1);
    }

    if (mPred != 0) {
        snprintf(temp, MAX_LINEA, "ALERTA PREVENTIVA en %s -> se prevee exceso en:", zona->nombre);
        strncat(buffer, temp, tamBuffer - strlen(buffer) - 1);
        if (mPred & 1) strncat(buffer, " CO2",  tamBuffer - strlen(buffer) - 1);
        if (mPred & 2) strncat(buffer, " SO2",  tamBuffer - strlen(buffer) - 1);
        if (mPred & 4) strncat(buffer, " NO2",  tamBuffer - strlen(buffer) - 1);
        if (mPred & 8) strncat(buffer, " PM2.5", tamBuffer - strlen(buffer) - 1);
        strncat(buffer, " en las proximas 24h.", tamBuffer - strlen(buffer) - 1);
    }
}

void generarRecomendaciones(Zona *zona, char *buffer, int tamBuffer) {
    int mascara = evaluarLimites(&zona->actual) | evaluarLimites(&zona->prediccion);
    buffer[0] = '\0';

    if (mascara == 0) {
        snprintf(buffer, tamBuffer,
            "No se requieren medidas de mitigacion; mantener monitoreo regular.");
        return;
    }

    if (mascara & 1) /* CO2 */
        strncat(buffer, "Reducir el trafico vehicular y fomentar transporte publico/electrico. ",
                 tamBuffer - strlen(buffer) - 1);
    if (mascara & 2) /* SO2 */
        strncat(buffer, "Suspender o reducir temporalmente actividad industrial cercana. ",
                 tamBuffer - strlen(buffer) - 1);
    if (mascara & 4) /* NO2 */
        strncat(buffer, "Restringir circulacion vehicular en horas pico (pico y placa reforzado). ",
                 tamBuffer - strlen(buffer) - 1);
    if (mascara & 8) /* PM2.5 */
        strncat(buffer, "Suspender actividades al aire libre y recomendar uso de mascarillas. ",
                 tamBuffer - strlen(buffer) - 1);
}

void mostrarMenu(void) {
    printf(" SISTEMA DE GESTION Y PREDICCION DE CONTAMINACION\n");
    printf("\n");
    printf("1. Mostrar monitoreo actual de todas las zonas\n");
    printf("2. Mostrar prediccion de contaminacion (24h)\n");
    printf("3. Mostrar alertas preventivas\n");
    printf("4. Mostrar promedio historico (30 dias) vs OMS\n");
    printf("5. Mostrar recomendaciones de mitigacion\n");
    printf("6. Ingresar manualmente datos de una zona\n");
    printf("7. Exportar reporte completo a archivo\n");
    printf("8. Exportar historico en formato CSV\n");
    printf("0. Salir\n");
    printf("Seleccione una opcion: ");
}

void mostrarDatosZona(Zona *zona) {
    printf("\nZona: %s\n", zona->nombre);
    printf("  Clima actual -> Temp: %.1f C | Viento: %.1f km/h | Humedad: %.1f%%\n",
           zona->climaActual.temperatura, zona->climaActual.viento, zona->climaActual.humedad);
    printf("  Nivel actual -> CO2: %.2f ppm | SO2: %.2f | NO2: %.2f | PM2.5: %.2f ug/m3\n",
           zona->actual.co2, zona->actual.so2, zona->actual.no2, zona->actual.pm25);
}

void mostrarTodasZonas(Zona *zonas, int numZonas) {
    int i;
    for (i = 0; i < numZonas; i++) {
        calcularNivelActual(zonas + i);
        mostrarDatosZona(zonas + i);
    }
}

void mostrarPrediccion(Zona *zona) {
    printf("\nPrediccion 24h - %s\n", zona->nombre);
    printf("  CO2: %.2f ppm | SO2: %.2f | NO2: %.2f | PM2.5: %.2f ug/m3\n",
           zona->prediccion.co2, zona->prediccion.so2,
           zona->prediccion.no2, zona->prediccion.pm25);
}

void mostrarPromedioHistorico(Zona *zona) {
    Contaminantes prom;
    calcularPromedioHistorico(zona, &prom);
    printf("\nPromedio historico (%d dias) - %s\n", zona->numDias, zona->nombre);
    printf("  CO2: %.2f (limite OMS: %.1f)\n", prom.co2, LIMITE_CO2);
    printf("  SO2: %.2f (limite OMS: %.1f)\n", prom.so2, LIMITE_SO2);
    printf("  NO2: %.2f (limite OMS: %.1f)\n", prom.no2, LIMITE_NO2);
    printf("  PM2.5: %.2f (limite OMS: %.1f)\n", prom.pm25, LIMITE_PM25);
}


void guardarReporte(Zona *zonas, int numZonas, const char *nombreArchivo) {
    FILE *archivo;
    int i;
    char alerta[MAX_LINEA];
    char recomendacion[MAX_LINEA];
    Contaminantes promedio;
    time_t ahora = time(NULL);

    archivo = fopen(nombreArchivo, "w");
    if (archivo == NULL) {
        printf("Error: no se pudo crear el archivo de reporte.\n");
        return;
    }

    fprintf(archivo, "REPORTE DE CALIDAD DEL AIRE\n");
    fprintf(archivo, "Fecha de generacion: %s", ctime(&ahora));
    fprintf(archivo, "============================================\n\n");

    for (i = 0; i < numZonas; i++) {
        Zona *z = zonas + i;
        calcularNivelActual(z);
        calcularPrediccion(z);
        calcularPromedioHistorico(z, &promedio);
        generarAlerta(z, alerta, MAX_LINEA);
        generarRecomendaciones(z, recomendacion, MAX_LINEA);

        fprintf(archivo, "ZONA: %s\n", z->nombre);
        fprintf(archivo, "Clima actual: Temp %.1fC, Viento %.1f km/h, Humedad %.1f%%\n",
                z->climaActual.temperatura, z->climaActual.viento, z->climaActual.humedad);
        fprintf(archivo, "Nivel actual    -> CO2:%.2f SO2:%.2f NO2:%.2f PM2.5:%.2f\n",
                z->actual.co2, z->actual.so2, z->actual.no2, z->actual.pm25);
        fprintf(archivo, "Prediccion 24h  -> CO2:%.2f SO2:%.2f NO2:%.2f PM2.5:%.2f\n",
                z->prediccion.co2, z->prediccion.so2, z->prediccion.no2, z->prediccion.pm25);
        fprintf(archivo, "Promedio %d dias -> CO2:%.2f SO2:%.2f NO2:%.2f PM2.5:%.2f\n",
                z->numDias, promedio.co2, promedio.so2, promedio.no2, promedio.pm25);
        fprintf(archivo, "Alerta: %s\n", alerta);
        fprintf(archivo, "Recomendaciones: %s\n", recomendacion);
        fprintf(archivo, "--------------------------------------------\n\n");
    }

    fclose(archivo);
    printf("Reporte generado correctamente en '%s'\n", nombreArchivo);
}

void guardarHistoricoCSV(Zona *zonas, int numZonas, const char *nombreArchivo) {
    FILE *archivo;
    int i, d;

    archivo = fopen(nombreArchivo, "w");
    if (archivo == NULL) {
        printf("Error: no se pudo crear el archivo CSV.\n");
        return;
    }

    fprintf(archivo, "Zona,Dia,CO2,SO2,NO2,PM2.5\n");
    for (i = 0; i < numZonas; i++) {
        Zona *z = zonas + i;
        for (d = 0; d < z->numDias; d++) {
            fprintf(archivo, "%s,%d,%.2f,%.2f,%.2f,%.2f\n",
                    z->nombre, d + 1,
                    z->historico[d].co2, z->historico[d].so2,
                    z->historico[d].no2, z->historico[d].pm25);
        }
    }

    fclose(archivo);
    printf("Historico exportado correctamente en '%s'\n", nombreArchivo);
}