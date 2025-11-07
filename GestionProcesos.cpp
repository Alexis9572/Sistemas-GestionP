#include <iostream>     // Permite usar cout y cin
#include <string>       // Para manejar cadenas de texto
#include <cstdlib>      // Para usar system("cls") y system("pause")
#include <cctype>       // Para validar caracteres (isdigit, isspace)
using namespace std;    // Evita escribir std:: antes de cada función

//---------------- FUNCIONES AUXILIARES ----------------//

// Elimina los espacios en blanco al inicio y final de una cadena
string trim(const string &s) {
    size_t start = 0;  // Índice del primer carácter no vacío
    while (start < s.size() && isspace((unsigned char)s[start])) start++;
    if (start == s.size()) return "";  // Si toda la cadena está vacía
    size_t end = s.size() - 1;         // Índice del último carácter no vacío
    while (end > start && isspace((unsigned char)s[end])) end--;
    return s.substr(start, end - start + 1);  // Devuelve la parte sin espacios
}

// Convierte una cadena numérica en un número entero
int stringADecimal(const string &s) {
    int valor = 0;  // Variable acumuladora del número convertido
    for (size_t i = 0; i < s.size(); ++i)
        valor = valor * 10 + (s[i] - '0');  // Convierte carácter por carácter
    return valor;  // Retorna el valor convertido
}

// Lee un número entero positivo desde el teclado
int leerEnteroPositivo(const string &mensaje) {
    string entrada;  // Variable para almacenar la entrada del usuario
    while (true) {
        cout << mensaje;          // Muestra el mensaje
        getline(cin, entrada);    // Lee la entrada completa
        entrada = trim(entrada);  // Elimina espacios

        if (entrada.empty()) {    // Si está vacío
            cout << "Error: Ingrese un número entero positivo.\n";
            continue;
        }

        bool valido = true;       // Bandera para validar caracteres
        for (size_t i = 0; i < entrada.size(); ++i) {
            if (!isdigit((unsigned char)entrada[i])) {  // Si hay letras o símbolos
                valido = false;
                break;
            }
        }

        if (!valido) {
            cout << "Error: Solo se permiten números.\n";
            continue;
        }

        int valor = stringADecimal(entrada);  // Convierte la cadena a número
        if (valor <= 0) {
            cout << "Error: El número debe ser mayor que 0.\n";
            continue;
        }

        return valor;  // Devuelve el número válido
    }
}

//---------------- ESTRUCTURAS PRINCIPALES ----------------//

// Cada proceso contiene su identificador (PID), nombre, prioridad y un puntero al siguiente
struct Proceso {
    int pid;                // Identificador único del proceso
    string nombre;          // Nombre descriptivo del proceso
    int prioridad;          // Nivel de prioridad (menor número = mayor prioridad)
    Proceso* siguiente;     // Enlace al siguiente proceso en la lista
};
Proceso* cabezaProcesos = NULL;   // Puntero al inicio de la lista de procesos

// Nodo para representar la cola de planificación de CPU
struct NodoCola {
    Proceso* proceso;       // Apunta al proceso asociado
    NodoCola* siguiente;    // Enlace al siguiente nodo en la cola
};
NodoCola* cabezaCola = NULL;      // Inicio de la cola (vacía al comienzo)

// Nodo para representar los bloques de memoria como una pila
struct BloqueMemoria {
    Proceso* proceso;        // Proceso que ocupa el bloque de memoria
    int tamanio;             // Tamaño asignado al bloque
    BloqueMemoria* siguiente;// Enlace al siguiente bloque
};
BloqueMemoria* topeMemoria = NULL; // Puntero al bloque superior de la pila

//---------------- FUNCIONES DE BÚSQUEDA ----------------//

// Busca un proceso por su PID en la lista enlazada
Proceso* buscarProcesoPorPID(int pid) {
    Proceso* actual = cabezaProcesos;  // Apunta al primer proceso
    while (actual != NULL) {
        if (actual->pid == pid) return actual;  // Si encuentra coincidencia
        actual = actual->siguiente;             // Avanza al siguiente
    }
    return NULL;  // Retorna NULL si no lo encuentra
}

// Verifica si un proceso ya está en la cola del planificador
bool estaEnCola(int pid) {
    NodoCola* actual = cabezaCola;  // Apunta al primer nodo
    while (actual != NULL) {
        if (actual->proceso->pid == pid) return true;  // Encontrado
        actual = actual->siguiente;                    // Avanza
    }
    return false;  // No encontrado
}

//---------------- GESTOR DE PROCESOS ----------------//

// Inserta un nuevo proceso al final de la lista enlazada
void insertarProceso() {
    cout << "\n--- Insertar nuevo proceso ---\n";

    int pid = leerEnteroPositivo("Ingrese el PID: ");  // PID único

    if (buscarProcesoPorPID(pid) != NULL) {            // Verifica duplicados
        cout << "Error: Ya existe un proceso con ese PID.\n";
        return;
    }

    string nombre;
    cout << "Ingrese el nombre del proceso: ";
    getline(cin, nombre);
    nombre = trim(nombre);

    while (nombre.empty()) {  // Evita nombres vacíos
        cout << "Error: El nombre no puede estar vacío.\n";
        cout << "Ingrese el nombre del proceso: ";
        getline(cin, nombre);
        nombre = trim(nombre);
    }

    int prioridad = leerEnteroPositivo("Ingrese la prioridad: ");  // Prioridad

    // Crea un nuevo nodo de proceso
    Proceso* nuevo = new Proceso;
    nuevo->pid = pid;
    nuevo->nombre = nombre;
    nuevo->prioridad = prioridad;
    nuevo->siguiente = NULL;

    // Inserta el proceso al final de la lista
    if (cabezaProcesos == NULL)
        cabezaProcesos = nuevo;
    else {
        Proceso* actual = cabezaProcesos;
        while (actual->siguiente != NULL) actual = actual->siguiente;
        actual->siguiente = nuevo;
    }

    cout << "Proceso agregado correctamente.\n";
}

// Elimina un proceso de la lista según su PID
void eliminarProceso() {
    cout << "\n--- Eliminar proceso ---\n";
    int pid = leerEnteroPositivo("Ingrese el PID a eliminar: ");

    if (cabezaProcesos == NULL) {  // Si no hay procesos
        cout << "No hay procesos registrados.\n";
        return;
    }

    if (cabezaProcesos->pid == pid) {  // Si el proceso está al inicio
        Proceso* temp = cabezaProcesos;
        cabezaProcesos = cabezaProcesos->siguiente;
        delete temp;  // Libera memoria
        cout << "Proceso eliminado correctamente.\n";
        return;
    }

    Proceso* actual = cabezaProcesos;
    while (actual->siguiente != NULL && actual->siguiente->pid != pid)
        actual = actual->siguiente;  // Recorre la lista

    if (actual->siguiente != NULL) {
        Proceso* temp = actual->siguiente;
        actual->siguiente = temp->siguiente;
        delete temp;
        cout << "Proceso eliminado correctamente.\n";
    } else {
        cout << "Error: No se encontró el proceso.\n";
    }
}

// Muestra todos los procesos de la lista
void mostrarProcesos() {
    cout << "\n--- Lista de procesos ---\n";
    if (cabezaProcesos == NULL) {
        cout << "No hay procesos registrados.\n";
        return;
    }
    Proceso* actual = cabezaProcesos;
    while (actual != NULL) {
        cout << "PID: " << actual->pid
             << " | Nombre: " << actual->nombre
             << " | Prioridad: " << actual->prioridad << endl;
        actual = actual->siguiente;
    }
}

//---------------- PLANIFICADOR DE CPU ----------------//

// Encola un proceso según su prioridad
void encolarProcesoEnPlanificador() {
    cout << "\n--- Encolar proceso ---\n";
    int pid = leerEnteroPositivo("Ingrese el PID: ");
    Proceso* p = buscarProcesoPorPID(pid);

    if (p == NULL) {
        cout << "Error: No existe un proceso con ese PID.\n";
        return;
    }
    if (estaEnCola(pid)) {
        cout << "Error: El proceso ya está en la cola.\n";
        return;
    }

    NodoCola* nuevo = new NodoCola;
    nuevo->proceso = p;
    nuevo->siguiente = NULL;

    // Inserta ordenado por prioridad
    if (cabezaCola == NULL || cabezaCola->proceso->prioridad > p->prioridad) {
        nuevo->siguiente = cabezaCola;
        cabezaCola = nuevo;
    } else {
        NodoCola* actual = cabezaCola;
        while (actual->siguiente != NULL &&
               actual->siguiente->proceso->prioridad <= p->prioridad)
            actual = actual->siguiente;
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
    }
    cout << "Proceso encolado correctamente.\n";
}

// Desencola el proceso de mayor prioridad (al frente)
void desencolarYEjecutarProceso() {
    cout << "\n--- Desencolar y ejecutar ---\n";
    if (cabezaCola == NULL) {
        cout << "La cola está vacía.\n";
        return;
    }

    NodoCola* temp = cabezaCola;
    cabezaCola = cabezaCola->siguiente;
    cout << "Ejecutando proceso PID: " << temp->proceso->pid
         << " | Nombre: " << temp->proceso->nombre
         << " | Prioridad: " << temp->proceso->prioridad << endl;
    delete temp;
}

// Muestra el contenido de la cola
void mostrarColaPlanificador() {
    cout << "\n--- Cola de planificación ---\n";
    if (cabezaCola == NULL) {
        cout << "La cola está vacía.\n";
        return;
    }
    NodoCola* actual = cabezaCola;
    while (actual != NULL) {
        cout << "PID: " << actual->proceso->pid
             << " | Nombre: " << actual->proceso->nombre
             << " | Prioridad: " << actual->proceso->prioridad << endl;
        actual = actual->siguiente;
    }
}

//---------------- GESTOR DE MEMORIA ----------------//

// Asigna memoria a un proceso (operación push)
void asignarMemoria() {
    cout << "\n--- Asignar memoria ---\n";
    int pid = leerEnteroPositivo("Ingrese el PID: ");
    Proceso* p = buscarProcesoPorPID(pid);
    if (p == NULL) {
        cout << "Error: No existe un proceso con ese PID.\n";
        return;
    }

    int tamanio = leerEnteroPositivo("Ingrese el tamaño de memoria: ");
    BloqueMemoria* nuevo = new BloqueMemoria;
    nuevo->proceso = p;
    nuevo->tamanio = tamanio;
    nuevo->siguiente = topeMemoria;
    topeMemoria = nuevo;

    cout << "Memoria asignada correctamente.\n";
}

// Libera el último bloque asignado (operación pop)
void liberarMemoria() {
    cout << "\n--- Liberar memoria ---\n";
    if (topeMemoria == NULL) {
        cout << "No hay memoria para liberar.\n";
        return;
    }

    BloqueMemoria* temp = topeMemoria;
    topeMemoria = topeMemoria->siguiente;
    cout << "Memoria liberada del proceso PID: " << temp->proceso->pid
         << " | Tamaño: " << temp->tamanio << endl;
    delete temp;
}

// Muestra los bloques de memoria actuales
void estadoMemoria() {
    cout << "\n--- Estado de memoria ---\n";
    if (topeMemoria == NULL) {
        cout << "La memoria está libre.\n";
        return;
    }
    BloqueMemoria* actual = topeMemoria;
    while (actual != NULL) {
        cout << "PID: " << actual->proceso->pid
             << " | Nombre: " << actual->proceso->nombre
             << " | Tamaño: " << actual->tamanio << endl;
        actual = actual->siguiente;
    }
}

//---------------- MENÚ PRINCIPAL ----------------//

// Controla la navegación por los distintos módulos
void menu() {
    int opcionPrincipal, opcionSecundaria;
    do {
        cout << "\n========== MENU PRINCIPAL ==========\n";
        cout << "[1] Gestor de Procesos\n";
        cout << "[2] Planificador de CPU\n";
        cout << "[3] Gestor de Memoria\n";
        cout << "[4] Salir\n";

        opcionPrincipal = leerEnteroPositivo("Seleccione una opción: ");

        switch (opcionPrincipal) {
        case 1:
            do {
                cout << "\n--- GESTOR DE PROCESOS ---\n";
                cout << "[1] Insertar proceso\n";
                cout << "[2] Eliminar proceso\n";
                cout << "[3] Mostrar procesos\n";
                cout << "[4] Volver\n";
                opcionSecundaria = leerEnteroPositivo("Opción: ");
                if (opcionSecundaria == 1) insertarProceso();
                else if (opcionSecundaria == 2) eliminarProceso();
                else if (opcionSecundaria == 3) mostrarProcesos();
                system("pause");
                system("cls");
            } while (opcionSecundaria != 4);
            break;

        case 2:
            do {
                cout << "\n--- PLANIFICADOR DE CPU ---\n";
                cout << "[1] Encolar proceso\n";
                cout << "[2] Ejecutar proceso\n";
                cout << "[3] Mostrar cola\n";
                cout << "[4] Volver\n";
                opcionSecundaria = leerEnteroPositivo("Opción: ");
                if (opcionSecundaria == 1) encolarProcesoEnPlanificador();
                else if (opcionSecundaria == 2) desencolarYEjecutarProceso();
                else if (opcionSecundaria == 3) mostrarColaPlanificador();
                system("pause");
                system("cls");
            } while (opcionSecundaria != 4);
            break;

        case 3:
            do {
                cout << "\n--- GESTOR DE MEMORIA ---\n";
                cout << "[1] Asignar memoria\n";
                cout << "[2] Liberar memoria\n";
                cout << "[3] Mostrar estado\n";
                cout << "[4] Volver\n";
                opcionSecundaria = leerEnteroPositivo("Opción: ");
                if (opcionSecundaria == 1) asignarMemoria();
                else if (opcionSecundaria == 2) liberarMemoria();
                else if (opcionSecundaria == 3) estadoMemoria();
                system("pause");
                system("cls");
            } while (opcionSecundaria != 4);
            break;
        }
    } while (opcionPrincipal != 4);
    cout << "Saliendo del programa...\n";
}

// Función principal: punto de inicio del programa
int main() {
    menu();  // Llama al menú principal
    return 0;
}

