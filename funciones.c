#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funciones.h"

void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int leerEntero(const char *mensaje, int min, int max) {
    int valor;
    int valido;

    do {
        printf("%s", mensaje);
        valido = scanf("%d", &valor);

        if (valido != 1 || valor < min || valor > max) {
            printf("Error: ingrese un numero entero entre %d y %d.\n", min, max);
            limpiarBuffer();
        }
    } while (valido != 1 || valor < min || valor > max);

    limpiarBuffer();
    return valor;
}

float leerFloat(const char *mensaje, float min, float max) {
    float valor;
    int valido;

    do {
        printf("%s", mensaje);
        valido = scanf("%f", &valor);

        if (valido != 1 || valor < min || valor > max) {
            printf("Error: ingrese un valor entre %.2f y %.2f.\n", min, max);
            limpiarBuffer();
        }
    } while (valido != 1 || valor < min || valor > max);

    limpiarBuffer();
    return valor;
}

void leerTexto(const char *mensaje, char texto[], int tam) {
    int valido;
    int i;

    do {
        valido = 1;
        printf("%s", mensaje);

        if (fgets(texto, tam, stdin) == NULL) {
            valido = 0;
        }

        if (texto[0] == '\n' || texto[0] == '\0') {
            printf("Error: el texto no puede estar vacio.\n");
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

void inicializarZonas(Zona *zonas, int numZonas) {
    int i, j;

    for (i = 0; i < numZonas; i++) {
        snprintf((zonas + i)->nombre, MAX_NOMBRE, "Zona %d", i + 1);
        (zonas + i)->numDias = MAX_DIAS;
        (zonas + i)->climaActual.temperatura = 0.0f;
        (zonas + i)->climaActual.viento = 0.0f;
        (zonas + i)->climaActual.humedad = 0.0f;
        (zonas + i)->actual.co2 = 0.0f;
        (zonas + i)->actual.so2 = 0.0f;
        (zonas + i)->actual.no2 = 0.0f;
        (zonas + i)->actual.pm25 = 0.0f;
        (zonas + i)->prediccion.co2 = 0.0f;
        (zonas + i)->prediccion.so2 = 0.0f;
        (zonas + i)->prediccion.no2 = 0.0f;
        (zonas + i)->prediccion.pm25 = 0.0f;

        for (j = 0; j < MAX_DIAS; j++) {
            (zonas + i)->historico[j].co2 = 0.0f;
            (zonas + i)->historico[j].so2 = 0.0f;
            (zonas + i)->historico[j].no2 = 0.0f;
            (zonas + i)->historico[j].pm25 = 0.0f;
        }
    }
}

void cargarDatosPrueba(Zona *zonas, int numZonas) {
    const char *nombres[MAX_ZONAS] = {
        "Centro", "Norte_Industrial", "Sur_Residencial", "Valle_Chillos", "Aeropuerto"
    };

    int i, d;

    for (i = 0; i < numZonas; i++) {
        strncpy((zonas + i)->nombre, nombres[i % MAX_ZONAS], MAX_NOMBRE - 1);
        (zonas + i)->nombre[MAX_NOMBRE - 1] = '\0';
        (zonas + i)->numDias = MAX_DIAS;

        for (d = 0; d < MAX_DIAS; d++) {
            float base = (float)(i * 3);
            (zonas + i)->historico[d].co2 = 420.0f + base * 20.0f + (float)(d % 9);
            (zonas + i)->historico[d].so2 = 8.0f + base + (float)(d % 6);
            (zonas + i)->historico[d].no2 = 10.0f + base + (float)(d % 7);
            (zonas + i)->historico[d].pm25 = 7.0f + base + (float)(d % 8);
        }

        (zonas + i)->climaActual.temperatura = 20.0f + (float)(i * 2);
        (zonas + i)->climaActual.viento = 8.0f + (float)(i * 3);
        (zonas + i)->climaActual.humedad = 45.0f + (float)(i * 6);

        calcularNivelActual(zonas + i);
        calcularPrediccion(zonas + i);
    }
}

int cargarDatosArchivo(Zona *zonas, int numZonas, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "rb");
    int leidos;

    if (archivo == NULL) {
        return 0;
    }

    leidos = fread(zonas, sizeof(Zona), numZonas, archivo);
    fclose(archivo);

    return leidos == numZonas;
}

int guardarDatosArchivo(Zona *zonas, int numZonas, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "wb");
    int escritos;

    if (archivo == NULL) {
        printf("Error: no se pudo abrir el archivo de datos.\n");
        return 0;
    }

    escritos = fwrite(zonas, sizeof(Zona), numZonas, archivo);
    fclose(archivo);

    return escritos == numZonas;
}

void ingresarDatosZona(Zona *zona) {
    int d;

    leerTexto("Nombre de la zona: ", zona->nombre, MAX_NOMBRE);

    zona->climaActual.temperatura = leerFloat("Temperatura actual en C: ", 0.0f, 60.0f);
    zona->climaActual.viento = leerFloat("Velocidad del viento en km/h: ", 0.0f, 200.0f);
    zona->climaActual.humedad = leerFloat("Humedad relativa en porcentaje: ", 0.0f, 100.0f);

    zona->numDias = leerEntero("Cuantos dias de historico desea ingresar (1-30): ", 1, MAX_DIAS);

    for (d = 0; d < zona->numDias; d++) {
        printf("\nDia %d\n", d + 1);
        zona->historico[d].co2 = leerFloat("CO2 ppm: ", 0.0f, 5000.0f);
        zona->historico[d].so2 = leerFloat("SO2 ug/m3: ", 0.0f, 1000.0f);
        zona->historico[d].no2 = leerFloat("NO2 ug/m3: ", 0.0f, 1000.0f);
        zona->historico[d].pm25 = leerFloat("PM2.5 ug/m3: ", 0.0f, 1000.0f);
    }

    calcularNivelActual(zona);
    calcularPrediccion(zona);
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

    calcularNivelActual(zona);
    calcularPrediccion(zona);
}

void calcularNivelActual(Zona *zona) {
    int ultimo = zona->numDias - 1;

    if (ultimo < 0) {
        return;
    }

    zona->actual = zona->historico[ultimo];
}

float calcularPromedioPonderado(float *valores, int n) {
    float sumaPesos = 0.0f;
    float sumaValores = 0.0f;
    int i;

    for (i = 0; i < n; i++) {
        float peso = (float)(i + 1);
        sumaValores += valores[i] * peso;
        sumaPesos += peso;
    }

    if (sumaPesos == 0.0f) {
        return 0.0f;
    }

    return sumaValores / sumaPesos;
}

float calcularFactorClimatico(Clima *clima) {
    float factor = 1.0f;

    factor += clima->humedad / 200.0f;
    factor += clima->temperatura / 300.0f;
    factor -= clima->viento / 80.0f;

    if (factor < 0.5f) {
        factor = 0.5f;
    }

    if (factor > 1.6f) {
        factor = 1.6f;
    }

    return factor;
}

void calcularPrediccion(Zona *zona) {
    float co2vals[MAX_DIAS];
    float so2vals[MAX_DIAS];
    float no2vals[MAX_DIAS];
    float pm25vals[MAX_DIAS];
    float factorClima;
    int i;
    int n = zona->numDias;

    if (n <= 0) {
        return;
    }

    for (i = 0; i < n; i++) {
        co2vals[i] = zona->historico[i].co2;
        so2vals[i] = zona->historico[i].so2;
        no2vals[i] = zona->historico[i].no2;
        pm25vals[i] = zona->historico[i].pm25;
    }

    factorClima = calcularFactorClimatico(&zona->climaActual);

    zona->prediccion.co2 = calcularPromedioPonderado(co2vals, n) * factorClima;
    zona->prediccion.so2 = calcularPromedioPonderado(so2vals, n) * factorClima;
    zona->prediccion.no2 = calcularPromedioPonderado(no2vals, n) * factorClima;
    zona->prediccion.pm25 = calcularPromedioPonderado(pm25vals, n) * factorClima;
}

void calcularPromedioHistorico(Zona *zona, Contaminantes *promedio) {
    float sCo2 = 0.0f;
    float sSo2 = 0.0f;
    float sNo2 = 0.0f;
    float sPm25 = 0.0f;
    int i;
    int n = zona->numDias;

    if (n <= 0) {
        n = 1;
    }

    for (i = 0; i < zona->numDias; i++) {
        sCo2 += zona->historico[i].co2;
        sSo2 += zona->historico[i].so2;
        sNo2 += zona->historico[i].no2;
        sPm25 += zona->historico[i].pm25;
    }

    promedio->co2 = sCo2 / n;
    promedio->so2 = sSo2 / n;
    promedio->no2 = sNo2 / n;
    promedio->pm25 = sPm25 / n;
}

int contaminanteExcedeLimite(const char *tipo, float valor) {
    if (strcmp(tipo, "CO2") == 0) {
        return valor > LIMITE_CO2;
    }

    if (strcmp(tipo, "SO2") == 0) {
        return valor > LIMITE_SO2;
    }

    if (strcmp(tipo, "NO2") == 0) {
        return valor > LIMITE_NO2;
    }

    if (strcmp(tipo, "PM25") == 0) {
        return valor > LIMITE_PM25;
    }

    return 0;
}

int evaluarLimites(Contaminantes *c) {
    int mascara = 0;

    if (contaminanteExcedeLimite("CO2", c->co2)) {
        mascara |= 1;
    }

    if (contaminanteExcedeLimite("SO2", c->so2)) {
        mascara |= 2;
    }

    if (contaminanteExcedeLimite("NO2", c->no2)) {
        mascara |= 4;
    }

    if (contaminanteExcedeLimite("PM25", c->pm25)) {
        mascara |= 8;
    }

    return mascara;
}

void generarAlerta(Zona *zona, char *buffer, int tamBuffer) {
    int mActual = evaluarLimites(&zona->actual);
    int mPred = evaluarLimites(&zona->prediccion);

    buffer[0] = '\0';

    if (mActual == 0 && mPred == 0) {
        snprintf(buffer, tamBuffer, "Sin alerta");
        return;
    }

    if (mActual != 0) {
        strncat(buffer, "Actual:", tamBuffer - strlen(buffer) - 1);

        if (mActual & 1) {
            strncat(buffer, " CO2", tamBuffer - strlen(buffer) - 1);
        }

        if (mActual & 2) {
            strncat(buffer, " SO2", tamBuffer - strlen(buffer) - 1);
        }

        if (mActual & 4) {
            strncat(buffer, " NO2", tamBuffer - strlen(buffer) - 1);
        }

        if (mActual & 8) {
            strncat(buffer, " PM2.5", tamBuffer - strlen(buffer) - 1);
        }
    }

    if (mPred != 0) {
        if (strlen(buffer) > 0) {
            strncat(buffer, " | ", tamBuffer - strlen(buffer) - 1);
        }

        strncat(buffer, "Prevista:", tamBuffer - strlen(buffer) - 1);

        if (mPred & 1) {
            strncat(buffer, " CO2", tamBuffer - strlen(buffer) - 1);
        }

        if (mPred & 2) {
            strncat(buffer, " SO2", tamBuffer - strlen(buffer) - 1);
        }

        if (mPred & 4) {
            strncat(buffer, " NO2", tamBuffer - strlen(buffer) - 1);
        }

        if (mPred & 8) {
            strncat(buffer, " PM2.5", tamBuffer - strlen(buffer) - 1);
        }
    }
}

void generarRecomendaciones(Zona *zona, char *buffer, int tamBuffer) {
    int mascara = evaluarLimites(&zona->actual) | evaluarLimites(&zona->prediccion);

    buffer[0] = '\0';

    if (mascara == 0) {
        snprintf(buffer, tamBuffer, "Mantener monitoreo regular.");
        return;
    }

    if (mascara & 1) {
        strncat(buffer, "Reducir trafico. ", tamBuffer - strlen(buffer) - 1);
    }

    if (mascara & 2) {
        strncat(buffer, "Controlar industrias. ", tamBuffer - strlen(buffer) - 1);
    }

    if (mascara & 4) {
        strncat(buffer, "Restringir horas pico. ", tamBuffer - strlen(buffer) - 1);
    }

    if (mascara & 8) {
        strncat(buffer, "Evitar aire libre. ", tamBuffer - strlen(buffer) - 1);
    }
}

void mostrarMenu(void) {
    printf("\n");
    printf("+------------------------------------------------------+\n");
    printf("|   SIGPCA - SISTEMA DE CONTAMINACION DEL AIRE         |\n");
    printf("+------------------------------------------------------+\n");
    printf("| 1. Mostrar monitoreo actual en tabla                 |\n");
    printf("| 2. Mostrar prediccion de contaminacion 24h           |\n");
    printf("| 3. Mostrar alertas preventivas                       |\n");
    printf("| 4. Mostrar promedio historico 30 dias vs limites     |\n");
    printf("| 5. Mostrar recomendaciones de mitigacion             |\n");
    printf("| 6. Ingresar manualmente datos de una zona            |\n");
    printf("| 7. Guardar datos del sistema                         |\n");
    printf("| 8. Exportar reporte completo TXT                     |\n");
    printf("| 9. Exportar historico CSV                            |\n");
    printf("| 0. Salir                                             |\n");
    printf("+------------------------------------------------------+\n");
}

void mostrarLineaTabla(void) {
    printf("+----+--------------------+----------+----------+----------+----------+----------+----------+----------+\n");
}

void mostrarTodasZonasTabla(Zona *zonas, int numZonas) {
    int i;

    mostrarLineaTabla();
    printf("| ID | Zona               | CO2      | SO2      | NO2      | PM2.5    | Temp C   | Viento   | Humedad  |\n");
    mostrarLineaTabla();

    for (i = 0; i < numZonas; i++) {
        calcularNivelActual(zonas + i);
        printf("| %-2d | %-18s | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f |\n",
               i + 1,
               (zonas + i)->nombre,
               (zonas + i)->actual.co2,
               (zonas + i)->actual.so2,
               (zonas + i)->actual.no2,
               (zonas + i)->actual.pm25,
               (zonas + i)->climaActual.temperatura,
               (zonas + i)->climaActual.viento,
               (zonas + i)->climaActual.humedad);
    }

    mostrarLineaTabla();
}

void mostrarPrediccionesTabla(Zona *zonas, int numZonas) {
    int i;

    mostrarLineaTabla();
    printf("| ID | Zona               | CO2      | SO2      | NO2      | PM2.5    | Temp C   | Viento   | Humedad  |\n");
    mostrarLineaTabla();

    for (i = 0; i < numZonas; i++) {
        calcularPrediccion(zonas + i);
        printf("| %-2d | %-18s | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f | %-8.2f |\n",
               i + 1,
               (zonas + i)->nombre,
               (zonas + i)->prediccion.co2,
               (zonas + i)->prediccion.so2,
               (zonas + i)->prediccion.no2,
               (zonas + i)->prediccion.pm25,
               (zonas + i)->climaActual.temperatura,
               (zonas + i)->climaActual.viento,
               (zonas + i)->climaActual.humedad);
    }

    mostrarLineaTabla();
}

void mostrarPromediosTabla(Zona *zonas, int numZonas) {
    int i;
    Contaminantes prom;

    printf("+----+--------------------+----------+----------+----------+----------+\n");
    printf("| ID | Zona               | CO2      | SO2      | NO2      | PM2.5    |\n");
    printf("+----+--------------------+----------+----------+----------+----------+\n");

    for (i = 0; i < numZonas; i++) {
        calcularPromedioHistorico(zonas + i, &prom);

        printf("| %-2d | %-18s | %-8.2f | %-8.2f | %-8.2f | %-8.2f |\n",
               i + 1,
               (zonas + i)->nombre,
               prom.co2,
               prom.so2,
               prom.no2,
               prom.pm25);
    }

    printf("+----+--------------------+----------+----------+----------+----------+\n");
    printf("| Limites usados: CO2 %.0f | SO2 %.0f | NO2 %.0f | PM2.5 %.0f |\n",
           LIMITE_CO2, LIMITE_SO2, LIMITE_NO2, LIMITE_PM25);
}

void mostrarAlertasTabla(Zona *zonas, int numZonas) {
    int i;
    char alerta[MAX_LINEA];

    printf("+----+--------------------+------------------------------------------------+\n");
    printf("| ID | Zona               | Alerta                                         |\n");
    printf("+----+--------------------+------------------------------------------------+\n");

    for (i = 0; i < numZonas; i++) {
        calcularNivelActual(zonas + i);
        calcularPrediccion(zonas + i);
        generarAlerta(zonas + i, alerta, MAX_LINEA);

        printf("| %-2d | %-18s | %-46s |\n", i + 1, (zonas + i)->nombre, alerta);
    }

    printf("+----+--------------------+------------------------------------------------+\n");
}

void mostrarRecomendacionesTabla(Zona *zonas, int numZonas) {
    int i;
    char recomendacion[MAX_LINEA];

    printf("+----+--------------------+------------------------------------------------+\n");
    printf("| ID | Zona               | Recomendacion                                  |\n");
    printf("+----+--------------------+------------------------------------------------+\n");

    for (i = 0; i < numZonas; i++) {
        calcularNivelActual(zonas + i);
        calcularPrediccion(zonas + i);
        generarRecomendaciones(zonas + i, recomendacion, MAX_LINEA);

        printf("| %-2d | %-18s | %-46s |\n", i + 1, (zonas + i)->nombre, recomendacion);
    }

    printf("+----+--------------------+------------------------------------------------+\n");
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
    fprintf(archivo, "============================================================\n\n");

    fprintf(archivo, "%-3s %-18s %-9s %-9s %-9s %-9s %-9s\n",
            "ID", "Zona", "CO2", "SO2", "NO2", "PM2.5", "PredPM25");

    for (i = 0; i < numZonas; i++) {
        Zona *z = zonas + i;

        calcularNivelActual(z);
        calcularPrediccion(z);
        calcularPromedioHistorico(z, &promedio);
        generarAlerta(z, alerta, MAX_LINEA);
        generarRecomendaciones(z, recomendacion, MAX_LINEA);

        fprintf(archivo, "%-3d %-18s %-9.2f %-9.2f %-9.2f %-9.2f %-9.2f\n",
                i + 1,
                z->nombre,
                z->actual.co2,
                z->actual.so2,
                z->actual.no2,
                z->actual.pm25,
                z->prediccion.pm25);

        fprintf(archivo, "Promedio: CO2 %.2f | SO2 %.2f | NO2 %.2f | PM2.5 %.2f\n",
                promedio.co2, promedio.so2, promedio.no2, promedio.pm25);

        fprintf(archivo, "Alerta: %s\n", alerta);
        fprintf(archivo, "Recomendacion: %s\n", recomendacion);
        fprintf(archivo, "------------------------------------------------------------\n");
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
                    z->nombre,
                    d + 1,
                    z->historico[d].co2,
                    z->historico[d].so2,
                    z->historico[d].no2,
                    z->historico[d].pm25);
        }
    }

    fclose(archivo);
    printf("Historico exportado correctamente en '%s'\n", nombreArchivo);
}
