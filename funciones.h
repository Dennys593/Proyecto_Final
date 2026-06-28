#ifndef FUNCIONES_H
#define FUNCIONES_H

#define MAX_ZONAS 5
#define MAX_DIAS 30
#define MAX_NOMBRE 50
#define MAX_TEXTO 300

#define ARCHIVO_DATOS "datos_zonas.dat"
#define ARCHIVO_REPORTE "reporte_contaminacion.txt"

#define LIMITE_CO2 1000.0f
#define LIMITE_SO2 40.0f
#define LIMITE_NO2 25.0f
#define LIMITE_PM25 15.0f

typedef struct {
    float co2;
    float so2;
    float no2;
    float pm25;
} Contaminacion;

typedef struct {
    float temperatura;
    float viento;
    float humedad;
} Clima;

typedef struct {
    char nombre[MAX_NOMBRE];
    Contaminacion historico[MAX_DIAS];
    Contaminacion actual;
    Contaminacion prediccion;
    Clima clima;
} Zona;

void limpiarBuffer();
int leerEntero(char mensaje[], int min, int max);
float leerFloat(char mensaje[], float min, float max);
void leerTexto(char mensaje[], char texto[], int tam);

void inicializarZonas(Zona zonas[]);
int cargarDatos(Zona zonas[]);
void guardarDatos(Zona zonas[]);

void mostrarMenu();
void ingresarDatosZona(Zona *zona);
void ingresarDatosSistema(Zona zonas[]);

void calcularActual(Zona *zona);
float promedioPonderado(float valores[]);
void predecirZona(Zona *zona);
void calcularPromedioHistorico(Zona *zona, Contaminacion *promedio);

int superaLimites(Contaminacion datos);
void generarAlerta(Zona zona, char alerta[]);
void generarRecomendacion(Zona zona, char recomendacion[]);

void mostrarMonitoreoActual(Zona zonas[]);
void mostrarPredicciones(Zona zonas[]);
void mostrarAlertas(Zona zonas[]);
void mostrarPromedios(Zona zonas[]);
void mostrarRecomendaciones(Zona zonas[]);

void generarReporte(Zona zonas[]);

#endif
