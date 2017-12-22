# Proyecto de Autómatas y Lenguajes 2017 (paulen-2017)

## Compilación y ejecucción

Basta con ejecutar:

    make all

Para que se genere el ejecutable `alfa` en la raíz del proyecto.
Si se compila con `make debug`, el código nasm que generará el compilador alfa contendrá comentarios de todas las instrucciones, muy útil para depurar.

El ejecutable `alfa` recibe hasta 3 argumentos:
1. El fichero de entrada de código ALFA (por defecto: entrada estándar)
2. El fichero de salida del código NASM (por defecto: salida estándar)
3. Información de la reducción de reglas de flex y bison (por defecto deshabilitado)

Este formato conforma con del enunciado, pero tiene la ventaja de que nos permitió probar facilmente el compilador durante el desarrollo y ser compatible con todas las prácticas anteriores.

## Pruebas
Hemos escrito una serie de pruebas para probar todos los componentes de nuestro compilador.
Para ejecutarlas:

    ./pruebas.sh

Se llamarán a todas las pruebas incluidas en la carpeta misc. Hemos adaptado el compilador para que siga siendo compatible con todas las entregas anteriores, y realizamos pruebas con todos los
ficheros de correción proporcionados en moodle de las prácticas anteriores. Además para probar el correcto funcionamiento del compilador final hemos creado pruebas extensivas de todas las operaciones
que soporta y hemos añadido los ficheros de pruebas de correción utilizados en otros años.

## Contenidos no entregados
Teníamos otra versión del compilador con funcionalidad adicional. Sin embargo, finalmente decidimos no entregarlo porque no hemos tenido tiempo de probarlos a fondo y hemos preferido centrar nuestro esfuerzo en los objetivos del guión.

### Soporte de llamadas anidadas
Hemos conseguido soportar llamadas anidadas del estilo `f(3, g(2,g(2,x)))` usando propagación de atributos para la lista de argumentos en vez de una variable global.

### Trie en vez de tabla hash
Está versión del compilador utiliza un arbol de lexemas en lugar de la tabla hash, con el cual se pueden desarrollar funcionalidades interesantes como en caso de
de utilizar una variable sin declarar, en la que es posible devolver un mensaje de error con una sugerencia de que otra variable declarada tiene un nombre muy similar en base a la distancia levenshtein.

## Autores
- Manuel Blanc
- Pablo Marcos
