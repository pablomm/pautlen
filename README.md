# Proyecto de Autómatas y Lenguajes 2017 (pautlen)

Compilador desarrollado para la asignatura Proyecto de AUTómatas y LENguages. Compila código alfa en ensamblador nasm, puedes encontrar una copia de la sintaxis de alfa en formato ANTLR [aquí](https://gist.github.com/ManuelBlanc/da631744d6407ca5a63811d89c326f99). Puedes encontrar ejemplos de programas escritos en alfa en la carpeta `misc`.

## Makefile y compilación

Para la compilación del proyecto basta on ejecutar `make all`, se generará el ejecutable `alfa` en la raíz del proyecto.
Si se compila con `make debug`, el código nasm que producirá el compilador alfa contendrá comentarios de todas las instrucciones, muy útil para depurar.

Se puede obtener más información de los comandos del makefile ejecutando `make help`

    Posibles comandos:
        all      - construye el/los ejecutable ./alfa
        test     - genera los ejecutables para las pruebas situadas en test/
        debug    - compila todo usando simbolos de depuracion
        clean    - borra todos los ficheros generados
        pruebas  - ejecuta el script de pruebas pruebas.bash
        zip      - comprime la rama activa del repositorio
        astyle   - estiliza el codigo acorde al fichero fichero .astylerc
        graph    - genera un diagrama utilizando dot a partir de la salida de bison
        nasm     - compila los nasm junto con alfalib situados en la carpeta ./
        help     - muestra esta ayuda
        
Algunos comandos del makefile dependen de programas externos para funcionar
- astyle - Programa [astyle](http://astyle.sourceforge.net/)
- graph  - Programa `dot` del paquete gráfico [Graphviz](https://www.graphviz.org/)
- nasm   - Paquete de 32 bits de `gcc`

## Ejecución

    ./alfa [input] [output] [errors]

El ejecutable `alfa` recibe hasta 3 argumentos:
1. El fichero de entrada de código ALFA (por defecto: entrada estándar)
2. El fichero de salida del código NASM (por defecto: salida estándar)
3. El fichero de salida de los errores de compilación (por defecto: salida estándar)

Este formato conforma con el del enunciado, pero tiene la ventaja de que nos permitió probar facilmente el compilador durante el desarrollo y ser compatible con todas las prácticas anteriores.

## Pruebas
Hemos escrito una serie de pruebas para probar todos los componentes de nuestro compilador.
Para ejecutarlas:

    ./pruebas.bash

Se llamarán a todas las pruebas incluidas en la carpeta misc. Hemos adaptado el compilador para que siga siendo compatible con todas las entregas anteriores, y realizamos pruebas con todos los
ficheros de correción proporcionados en moodle de las prácticas anteriores. Además para probar el correcto funcionamiento del compilador final hemos creado pruebas extensivas de todas las operaciones
que soporta y hemos añadido los ficheros de pruebas de correción utilizados en otros años.

## Versiones
La version entregada para la práctica final de la asignatura puede descargarse [aquí (v1.1)](https://github.com/pablomm/pautlen/archive/v1.1.zip).

La versión actual del compilador soporta funcionalidades adicionales

- Soporte de llamadas anidadas.
Se permiten llamadas anidadas del estilo `f(3, g(2,g(2,x)))` usando propagación de atributos para la lista de argumentos en vez de una variable global como se especificaba en la práctica.

- Sentencias `compare with`.

- Inicialización de vectores con sentencias `init`.

- Incrementos de variables con `+=`.

## Trie en vez de tabla hash
En la rama [`feat-trie`](https://github.com/pablomm/pautlen/tree/feat-trie) el compilador utiliza un árbol de lexemas en lugar de la tabla hash, con el cual se pueden desarrollar funcionalidades interesantes como en caso de
de utilizar una variable sin declarar, en la que es posible devolver un mensaje de error con una sugerencia de que otra variable declarada tiene un nombre muy similar en base a la distancia levenshtein. Pero el desarrollo de esta funcionalidad no ha sido completado.

## Licencia
El proyecto se encuentra bajo la licencia MIT, puede encontrar una copia [aquí](https://github.com/pablomm/pautlen/blob/master/LICENSE).

## Autores
- [Manuel Blanc](https://github.com/ManuelBlanc)
- [Pablo Marcos](https://github.com/pablomm)



