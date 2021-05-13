#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <time.h>


int main(int argc, char* argv[])
{
    addrinfo hints;
    addrinfo* results;

    memset(&hints, 0, sizeof(struct addrinfo));

    // guardamos los flags necesarios
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;

    // si no tenemos la entrada esperada, no podemos hacer nada
    if(argc != 4)
    {
        std::cout << "Los argumentos tienen que seguir el formato: programa host puerto comando\n";
        return -1;
    }

    // si no, conseguimos la informacion del address que nos han pasado
    int rc = getaddrinfo(argv[1], argv[2], &hints, &results);

    // rc guarda el codigo de error (si hay alguno)
    if(rc)
    {
        std::cout << "Error: ", gai_strerror(rc);
        return -1;
    }

    // intentamos crear el socket
    int socketd = socket(results->ai_family, results->ai_socktype, 0);
    if(socketd == -1)
    {
        std::cout << "Error con la creacion de sockets\n";
        return -1;
    }
    char* command = argv[3];
    int bytes = sendto(socketd, command, strlen(command) + 1, 0, results->ai_addr, results->ai_addrlen);

    if(bytes != 2)
    {
        std::cout << "Error al enviar el paquete\n";
        return -1;
    }

    char buffer[255];
    memset(&buffer, '\0', 255);

    //recibimos el mensaje y comprobamos que no haya errores
    bytes = recvfrom(socketd, buffer, sizeof(char) * 254, 0,
                results->ai_addr, &results->ai_addrlen);

    if(bytes == -1)
    {
        std::cout << "Error al recibir datos\n";
        return -1;
    }
    // cerramos el mensaje con \0 para evitar errores
    buffer[bytes] = '\0';
    std::cout << buffer << "\n";

    close(socketd);
    freeaddrinfo(results);

    return 0;
}