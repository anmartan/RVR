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
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    // si no tenemos la entrada esperada, no podemos hacer nada
    if(argc != 3)
    {
        std::cout << "Los argumentos tienen que seguir el formato: programa host puerto\n";
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

    // intentamos conectarnos
    int connection = connect(socketd, results->ai_addr, results->ai_addrlen);
    if(connection)
    {
        std::cout << "Error al conectarse con el servidor\n";
        return -1;
    }

    char buffer[255];
    memset(&buffer, '\0', sizeof(char) * 255);    
    
    bool exit = false;
    while(!exit)
    {
        std::cin >> buffer;
        buffer[254] = '\0'; // cortamos el mensaje si fuera necesario para evitar errores
        
        if((buffer[0] == 'Q' || buffer [0] == 'q') && buffer[1] =='\0')
        {
            exit = true;
        }
        else
        {
            // intentamos enviar el mensaje
            int message = send(socketd, buffer, strlen(buffer), 0);
            if(message == -1)
            {
                std::cout << "Error al enviar el mensaje\n";
                exit = true;
            }
            // si hay errores, salimos del programa, no recibimos la respuesta
            else
            {
                // recibimos la respuesta
                int bytes = recv(socketd, &buffer, 254, 0);
                
                // cortamos el mensaje si es necesario para evitar errores
                buffer[bytes] = '\0';       
                std::cout << buffer << '\n';
            }
        }
    }
    freeaddrinfo(results);
    close(socketd);

    return 0;
}