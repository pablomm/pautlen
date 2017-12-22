# Proyecto de Autómatas y Lenguajes 2017 (paulen-2017)

## Compilación y ejecucción

Basta con ejecutar

    make all

para que se genera el ejecutable `alfa` en la raiz del proyecto.
Si se compila con `make debug`, el código nasm que generará el compilador alfa contendrá comentarios de todas las instrucciones, muy útil para depurar.

El ejecutable `alfa` recibe hasta 3 argumentos:
1. El fichero de entrada de código ALFA (por defecto: entrada estandar)
2. El fichero de salida del código NASM (por defecto: salida estandar)
3. El fichero de errores (morfológicos, sintacticos y semanticos) (por defecto: salida estandar)

Este formato conforma con del enunciado, pero tiene la ventaja de que nos permitio probar facilmente el compilador durante el desarrollo.

## Pruebas

Hemos escrito una serie de pruebas para probar todos los componentes de nuestro compilador.

Si se ejecuta el script `pruebas.bash` incluido en la entrega se llamarán a todas las pruebas incluidas en la carpeta misc
Hemos adaptado el compilador para que siga siendo compatible con todas las entregas anteriores, realiza pruebas de todos los
ficheros de correción proporcionados en moodle las prácticas anteriores.
Además para probar el correcto funcionamiento del compilador final hemos creado pruebas extensivas de todas las operaciones
que soporta y hemos añadido los ficheros de pruebas de correción utilizados otros años.

## Contenidos no entregados

Teníamos otra versión del compilador la cual soporta llamadas anidadas del estilo `f(3, g(2,g(2,x)))` y que utiliza un
árbol de lexemas en lugar de la tabla hash, con el cual se pueden desarrollar funcionalidades interesantes como en caso de
de utilizar una variable sin declarar, en la que es posible devolver un mensaje de error con una sugerencia de que otra
variable declarada tiene un nombre muy similar. Pero hemos decidido no entregarlo porque no hemos tenido tiempo de probarlo
adecuadamente y aun quedaban cosa por pulir que podían fallar.


### Autores
- Manuel Blanc
- Pablo Marcos
