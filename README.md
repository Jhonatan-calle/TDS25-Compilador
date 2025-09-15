Proyecto para la materia Compiladores, del segundo cuatrimestre del 2025.

Integrantes: **Jhonatan Calle Galeano** y **Joaquín Pablo Tissera**

# Compilación
Correr el script **./compile** ubicado en la raíz del repositorio.
Si es necesario permisos de administrador, hacer **chmod +x compile.sh**

# Ejecución de casos de prueba
Correr el script **./executeTests.sh** ubicado en la raíz del repositorio.
Si es necesario permisos de administrador, hacer **chmod +x executeTests.sh**

Si se quiere añadir más casos, se debe agregar el .ctds de prueba correspondiente a la carpeta _test/_.

# Entregas

Descripciones de cada una de las 6 etapas del compilador en el orden en se realizarán para implementar
el compilador.

Cada etapa requiere entregar el código fuente completo (todos los archivos necesarios para compilar el
proyecto), un ejecutable (para correr los casos de test), los casos de test utilizados y la documentación
correspondiente a la etapa.


## Asignación de Branches para las entregas
La rama [Main](https://github.com/Jhonatan-calle/TDS25-Compilador/) contiene la última entrega del proyecto, que se acompaña activamente con el [versionado estable y Releases](https://github.com/Jhonatan-calle/TDS25-Compilador/releases)

<center> <b> Branches </b> </center>

* Primera entrega: [Analizador Sintáctico y Léxico](https://github.com/Jhonatan-calle/TDS25-Compilador/tree/analizador-sintactico-y-lexico)

* Segunda entrega: [Analizador Semántico](https://github.com/Jhonatan-calle/TDS25-Compilador/tree/analizador-semantico)

* Tercera entrega: [Generador Código Intermedio](https://github.com/Jhonatan-calle/TDS25-Compilador/tree/generador-codigo-intermedio)

* Cuarta entrega: [Generador Código Objeto](https://github.com/Jhonatan-calle/TDS25-Compilador/tree/generador-codigo-objeto)

* Quinta entrega: [Optimizador/Extensiones](https://github.com/Jhonatan-calle/TDS25-Compilador/tree/optimizador-extensiones)

### 1: Analizador Sintáctico y Léxico (Scanner y Parser)
El Analizador Léxico toma como entrada un archivo con código fuente TDS25 y retorna tokens.
Un token representa a una clase de símbolos del lenguaje.
Por ejemplo, operadores (*,+,<), delimitadores ((,{), palabras reservadas (while, else), literales (342, 3.5) o identificadores (var1, cant).
Símbolos que no son tokens son descartados, por ejemplo, espacios, tabulaciones, nuevas líneas y comentarios.
Aquellos símbolos que no son permitidos en el lenguaje deben ser reportados, por ejemplo, $, #.


El Analizador Sintáctico, toma como entrada la secuencia de tokens y verifica que esta secuencia sea
una secuencia válida, es decir, que cumpla con la especificación sintáctica del lenguaje.
La verificación controla que, por ejemplo, los paréntesis y llaves estén balanceados, la presencia de operadores, etc.
Verificaciones de tipado, nombres de variables y funciones no son realizadas en esta etapa.
La salida de esta etapa puede ser el árbol sintáctico (o de parsing) o simplemente si la entrada es correcta o no (sintácticamente).
La gramática (especificación sintáctica) del lenguaje TDS25 se presenta en otro documento.
Es necesario separar la especificación del Analizador Léxico de la especificación del Analizador Sintáctico.

**Las herramientas usadas para realizar estas actividades son: lex/flex y yacc/bison.**


### 2: Analizador Semántico
Esta etapa verifica las reglas semánticas del lenguaje, por ejemplo, compatibilidad de tipos, visibilidad
y alcance de los identificadores, etc.
En esta etapa es necesario implementar una tabla de símbolos para mantener la información de los símbolos (identificadores) de un programa.
El análisis semántico se realizará sobre el árbol de parsing utilizando la información almacenada en la tabla de símbolos.
Recomendamos no subestimar la complejidad ni el trabajo requerido para esta etapa para obtener el analizador semántico completo.


### 3: Generador Código Intermedio
Esta etapa del compilador retorna una representación intermedia (IR) del código.
A partir de esta representación intermedia se generará el código objeto. También, cabe aclarar, que algunas optimizaciones y análisis de código pueden ser realizadas sobre el IR.
En esta etapa se utilizará como código intermedio: Código de Tres Direcciones para las operaciones con tipos enteros y lógicos y para las operaciones de control de flujo.
En esta etapa se concluye con la construcción del front-end del compilador.


### 4: Generador Código Objeto
En esta etapa se genera código assembly x86-64 (sin optimizaciones) a partir del código intermedio.
Dado el tiempo asignado a esta etapa, se recomienda concentrarse en la corrección del código generado y no en la eficiencia o elegancia del código assembly generado.
En esta etapa es muy importante el proceso de testing que se realice para detectar posibles errores y/o funcionalidades no implementadas.


### 5: Optimizador/Extensiones
Las extensiones y optimizaciones y/o análisis a realizar en esta etapa son determinadas de común
acuerdo con cada grupo.


## Interface de la línea de comandos
El compilador deberá tener la siguiente interface para la línea de comando.

``> c-tds [opcion] nombreArchivo.ctds``

Los argumentos de la línea de comandos permitidos son los indicados en en la siguiente tabla.
| Opción             | Acción                                                                 |
|--------------------|------------------------------------------------------------------------|
| `-o <salida>`      | Renombra el archivo ejecutable a `<salida>`.                           |
| `-target <etapa>`  | `<etapa>` es una de `scan`, `parse`, `codinter` o `assembly`. La compilación procede hasta la etapa dada. |
| `-opt [optimización]` | Realiza la lista de optimizaciones. `all` realiza todas las optimizaciones soportadas. |
| `-debug`           | Imprime información de debugging. Si la opción no es dada, cuando la compilación es exitosa no debería imprimirse ninguna salida. |

**Tabla 1:** Argumentos de la línea de comandos del Compilador

El compilador toma como entrada un archivo de texto (el código fuente del programa a compilar), el nombre del
archivo no puede empezar con ’-’ y la extensión del archivo debe ser .ctds.
El comportamiento por defecto del compilador es realizar la compilación, del archivo pasado por parámetro, hasta la etapa corriente (la etapa que se este implementando) y generar un archivo de salida con extensión basada en la etapa (.lex, .sint, .sem, .ci, .ass, .opt) o el ejecutable con extensión .out si no se indica el target.
El nombre del ejecutable por defecto debe ser nombreArchivo.out.
Por defecto, ninguna optimización es realizada.
La lista de optimizaciones posibles para implementar serán establecidas con cada grupo.
