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

    if(bind(socketd, results->ai_addr, results->ai_addrlen))
    {
        std::cout << "Error al hacer el bind\n";
        return -1;
    }
    freeaddrinfo(results);

    bool exit = false;
    while(!exit)
    {
        socklen_t cliente_len = sizeof(struct sockaddr);
        struct sockaddr cliente;

        int sendBytes = 0;
        time_t rawtime;
        tm* t;

        char sendBuffer[255];
        char buffer[255];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        // para evitar errores
        memset(&host, '\0', sizeof(char) * NI_MAXHOST);
        memset(&serv, '\0', sizeof(char) * NI_MAXSERV);
        memset(&buffer, '\0', sizeof(char) * 255);
        memset(&sendBuffer, '\0', sizeof(char) * 255);


        //recibimos el mensaje y comprobamos que no haya errores
        int bytes = recvfrom(socketd, buffer, sizeof(char) * 254, 0, 
                            &cliente, &cliente_len);
        if(bytes == -1)
        {
            std::cout << "Error al recibir datos\n";
            return -1;
        }

        // cerramos el mensaje con \0 para evitar errores
        buffer[bytes] = '\0';

        getnameinfo(&cliente, cliente_len, 
                    host, NI_MAXHOST, 
                    serv, NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICSERV);
        
        
        std::cout << bytes << "bytes de " << host << ":" << serv << "\n";
        
        // enviamos la respuesta
        rawtime = time(0);
        t = localtime(&rawtime);
        if(bytes == 2)
        {
            if(buffer[0] == 'q') exit = true;
            else if(buffer[0] == 't') sendBytes = strftime(sendBuffer, 80, "%H:%M", t);
            else if(buffer[0] == 'd') sendBytes = strftime(sendBuffer, 80, "%Y-%m-%d", t);            
        }
        else
        {
            std::cout << "Comando no reconocido: " << buffer << "\n";
        }

        // enviamos la respuesta adecuada
        if(sendBytes != 0)
            sendto(socketd, sendBuffer, sendBytes, 0, &cliente, cliente_len);
    }
    std::cout << "Saliendo...\n";
    close(socketd);

    return 0;
}