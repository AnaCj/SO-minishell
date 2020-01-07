# minishell

### Programa principal (función main): 
Primero declaramos las variables:
- Tres de tipo bool, que van a determinar si hay redirección de entrada, salida o error, respectivamente.
- Tres strings en los que van a ir los nombres de los ficheros de redirección.
- Un puntero a tline, que almacena los datos introducidos por pantalla.
- Un array de caracteres buf, que servirá para leer en la mini Shell.
- Un puntero a un fichero.
- Un array de int para los pid de los procesos en background.
- Una array que almacena los mandatos y argumentos en background.
- Tres contadores de tipo int.

Imprimimos msh> que es el prompt.

Evitamos que la mini Shell se cierre con ctrl + C o con ctrl + \ al aparecer el prompt por primera vez.

Entramos en un bucle while que va a leer por pantalla hasta encontrar un fin de fichero.

Dentro del while:

Evitamos que se cierre la Shell al pulsar ctrl + C y ctrl + \ durante el bucle.

Utilizamos la función tokenize para separar los mandatos.

Si hay redirección de entrada, salida o error se pone a true las variables al principio mencionadas y se almacena el valor del fichero.

Si hay un único mandato y este no existe pueden ocurrir varias cosas:
- Que el mandato sea jobs, entra en un bucle for, e imprime todos los mandatos que hay en background.
- Que el mandato sea fg, que trae el proceso que se le indique a foreground y lo muestra por pantalla.
- Que el mandato no exista, en este caso imprime el error y redirecciona la salida del error si es necesario.

Si el proceso se tiene que ejecutar en background se llama a la función auxBg: se guarda en el array de pid de procesos en background el pid devuelto por la función auxBg y en el array de comandos en background el comando que hemos ejecutado. Se aumenta el contador de procesos en background y se imprime por pantalla el numero de proceso y su pid.

Si el proceso se tiene que ejecutar en foreground se llama a la función auxFg.
  

### Función auxiliar para ejecución en foreground (auxFg): 
Declaramos las variables:
- Un array de pipes.
- Tres punteros a fichero.
- Un tipo pid.
- Dos contadores de int.
- Un entero llamado anterior que servirá para almacenar la posición anterior a la actual.

Si hay redirección de error se realiza.

Se crean tantos pipes como el número de mandatos menos 1. Se accede a un bucle for para crear todos los hijos necesarios. 

Dentro del for:

Evitamos que la Shell se cierre al pulsar ctrl + C o ctrl + \. 

Hacemos un fork() y capturamos el error si no se ha hecho bien. 

Si es el hijo:
- Si es el primer mandato: redireccionamos la entrada si es necesario. Si además no es el último mandato redireccionamos su salida al pipe y cerramos los que no utilizamos. Si por el contrario es también el último mandato, redireccionamos la salida si es necesario.
- Si no es ni el primero ni el último mandato: redireccionamos su entrada y su salida a los pipes y cerramos los que no utilicemos.
- Si es el último mandato: redireccionamos su entrada al pipe, cerramos los pipes que no utilizamos y si hay redirección de salida la hace.

Ejecutamos el mandato.

Si es el padre:
- Comprueba que el comando a ejecutar existe.
- Si hay redirección de entrada comprueba que el fichero existe.
- Cuando acaba de crear hijos cierra todos sus pipes.

Al acabar el for, entra en otro bucle que espera a todos los hijos que ha creado.
 

### Función auxiliar para ejecución en background (auxBg):
Declaramos las variables:
- Un array de pipes.
- Tres punteros a fichero.
- Un tipo pid.
- Dos contadores de int.
- Un entero llamado anterior que servirá para almacenar la posición anterior a la
actual.

Si hay redirección de error se realiza.

Se crean tantos pipes como el número de mandatos menos 1. 

Se accede a un bucle for para crear todos los hijos necesarios. Dentro del for:

Evitamos que la Shell se cierre al pulsar ctrl + C o ctrl + \. 

Hacemos un fork() y capturamos el error si no se ha hecho bien. 

Si es el hijo:
- Si es el primer mandato: redireccionamos la entrada si es necesario. Si además no es el último mandato redireccionamos su salida al pipe y cerramos los que no utilizamos. Si por el contrario es también el último mandato, redireccionamos la salida si es necesario.
- Si no es ni el primero ni el último mandato: redireccionamos su entrada y su salida a los pipes y cerraos los que no utilicemos.
- Si es el último mandato: redireccionamos su entrada al pipe, cerramos los pipes que no utilizamos y si hay redirección de salida la hace.

Ejecutamos el mandato.

Si es el padre:
- Comprueba que el comando a ejecutar existe.
- Si hay redirección de entrada comprueba que el fichero existe. 
- Cuando acaba de crear hijos cierra todos sus pipes.

Al acabar el for, hace un wait no bloqueante por el último hijo, devuelve su pid y sale de la función.
