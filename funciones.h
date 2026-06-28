#ifndef FUNCIONES_H
#define FUNCIONES_H

#define MAX_ZONAS 5
#define MAX_DIAS 30
#define MAX_NOMBRE 50
#define MAX_LINEA 500

#define ARCHIVO_DATOS "datos_zonas.dat"
#define ARCHIVO_REPORTE "reporte_contaminacion.txt"
#define ARCHIVO_CSV "historico_contaminacion.csv"

#define LIMITE_CO2 1000.0f
#define LIMITE_SO2 40.0f
#define LIMITE_NO2 25.0f
#define LIMITE_PM25 15.0f

typedef struct {
    float co2;
    float so2;
    float no2;
    float pm25;
} Contaminantes;

typedef struct {
    float temperatura;
    float viento;
    float humedad;
} Clima;

typedef struct {
    char nombre[MAX_NOMBRE];
    Contaminantes historico[MAX_DIAS];
    int numDias;
    Clima climaActual;
    Contaminantes actual;
    Contaminantes prediccion;
} Zona;

void limpiarBuffer(void);
int leerEntero(const char *mensaje, int min, int max);
float leerFloat(const char *mensaje, float min, float max);
void leerTexto(const char *mensaje, char texto[], int tam);

void inicializarZonas(Zona *zonas, int numZonas);
void cargarDatosPrueba(Zona *zonas, int numZonas);
int cargarDatosArchivo(Zona *zonas, int numZonas, const char *nombreArchivo);
int guardarDatosArchivo(Zona *zonas, int numZonas, const char *nombreArchivo);

void ingresarDatosZona(Zona *zona);
void registrarNuevoDia(Zona *zona, Contaminantes nuevoDato);

void calcularNivelActual(Zona *zona);
float calcularPromedioPonderado(float *valores, int n);
float calcularFactorClimatico(Clima *clima);
void calcularPrediccion(Zona *zona);
void calcularPromedioHistorico(Zona *zona, Contaminantes *promedio);

int contaminanteExcedeLimite(const char *tipo, float valor);
int evaluarLimites(Contaminantes *c);
void generarAlerta(Zona *zona, char *buffer, int tamBuffer);
void generarRecomendaciones(Zona *zona, char *buffer, int tamBuffer);

void mostrarMenu(void);
void mostrarLineaTabla(void);
void mostrarTodasZonasTabla(Zona *zonas, int numZonas);
void mostrarPrediccionesTabla(Zona *zonas, int numZonas);
void mostrarPromediosTabla(Zona *zonas, int numZonas);
void mostrarAlertasTabla(Zona *zonas, int numZonas);
void mostrarRecomendacionesTabla(Zona *zonas, int numZonas);

void guardarReporte(Zona *zonas, int numZonas, const char *nombreArchivo);
void guardarHistoricoCSV(Zona *zonas, int numZonas, const char *nombreArchivo);

#endif
