Repositorio creado para las prácticas de la asignatura de Redes y Videojuegos en Red, en la Universidad Complutense de Madrid.

El repositorio tiene la siguiente estructura:

<details markdown='1'><summary>Archivos adicionales</summary>

* [Prediction](./Archivos_Adicionales/prediction/), con código base proporcionado por el profesor.

* [xlibw](./Archivos_Adicionales/xlibw), con código y ejemplos proporcionados por el profesor.
</details>

<details markdown='1'><summary>Práctica 2.1</summary>

* [x] [Ejercicio1.cc](./Practica2.1/Ejercicio1.cc), que obtiene todas las posibles direcciones con las que se podría crear un socket asociado a un host dado.

* [x] [Ejercicio2.cc](./Practica2.1/Ejercicio2.cc), que recibe e interpreta los comandos de un cliente conectado a él, utilizando el protocolo UDP. Comandos registrados:
    - t: devuelve la hora.
    - d: devuelve la fecha.
    - q: termina el proceso servidor.
* [x] [Ejercicio3.cc](./Practica2.1/Ejercicio3.cc), que sirve de cliente para el ejercicio anterior. Envía el comando y recibe la respuesta.
* [x] [Ejercicio4.cc](./Practica2.1/Ejercicio4.cc), que recibe los mensajes de un cliente conectado a él, y los devuelve, utilizando el protocolo TCP. Solo admite un cliente en cada momento.
* [x] [Ejercicio5.cc](./Practica2.1/Ejercicio5.cc), que sirve de cliente para el ejercicio anterior. Envía el mensaje y lo recibe de vuelta (truncado si es muy largo). Si el usuario envía "Q", se termina la conexión; este mensaje no se devuelve.
* [x] [Ejercicio6.cc](./Practica2.1/Ejercicio6.cc), que es una implementación multi-threaded del ejercicio 2. Si se escribe en el servidor el caracter 'q', o 'Q', se terminan todos los hilos.
* [x] [Ejercicio7.cc](./Practica2.1/Ejercicio7.cc), que es una implementación multi-threaded del ejercicio 4. Cuando se recibe una conexión, se crea un thread que la trata.
</details>
<details markdown='1'><summary>Práctica 2.2</summary>

* [x] [Serializacion](./Practica2.2/serializacion/), con el [ejercicio 1](./Practica2.2/serializacion/ejercicio1.cc), en el que leemos y escribimos en un archivo un objeto de tipo Jugador, que se recupera más tarde y se utiliza.
* [x] [Replicacion-chat](./Practica2.2/replicacion-chat/), con el código base para la práctica. El desarrollo de los ejercicios se ha llevado a cabo en los archivos [Socket.cc](./Practica2.2/replicacion-chat/Socket.cc) y [Chat.cc](./Practica2.2/replicacion-chat/Chat.cc):
    - Ejercicios 4 y 5, que implementa la clase Socket utilizada para enviar y recibir objetos serializables.
    - Ejercicio 6, que implementa un servidor con un socket y una lista de clientes conectados.
    - Ejercicio 7, que implementa un cliente que utiliza dos threads para enviar y recibir mensajes.

</details>