#ifndef FUNCIONES_H
#define FUNCIONES_H

#define MAX_ZONAS   5
#define MAX_DIAS    30
#define MAX_NOMBRE  50
#define MAX_LINEA   300

#define LIMITE_CO2   1000.0   
#define LIMITE_SO2   40.0     
#define LIMITE_NO2   25.0     
#define LIMITE_PM25  15.0     

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

void inicializarZonas(Zona *zonas, int numZonas);
void cargarDatosPrueba(Zona *zonas, int numZonas);
void ingresarDatosZona(Zona *zona);
void registrarNuevoDia(Zona *zona, Contaminantes nuevoDato);

void calcularNivelActual(Zona *zona);
float calcularPromedioPonderado(float *valores, int n);
void calcularPrediccion(Zona *zona);
void calcularPromedioHistorico(Zona *zona, Contaminantes *promedio);
float calcularFactorClimatico(Clima *clima);

int contaminanteExcedeLimite(const char *tipo, float valor);
int evaluarLimites(Contaminantes *c);
void generarAlerta(Zona *zona, char *buffer, int tamBuffer);
void generarRecomendaciones(Zona *zona, char *buffer, int tamBuffer);

void mostrarMenu(void);
void mostrarDatosZona(Zona *zona);
void mostrarTodasZonas(Zona *zonas, int numZonas);
void mostrarPrediccion(Zona *zona);
void mostrarPromedioHistorico(Zona *zona);

void guardarReporte(Zona *zonas, int numZonas, const char *nombreArchivo);
void guardarHistoricoCSV(Zona *zonas, int numZonas, const char *nombreArchivo);

#endif