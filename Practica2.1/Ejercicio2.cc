#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


int main(int argc, char* argv[])
{
    addrinfo hints;
    addrinfo* results;

    memset(&hints, 0, sizeof(struct addrinfo));

    // guardamos los flags necesarios
    hints.ai_flags = NI_NUMERICHOST;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_UNSPEC;

    // si no tenemos la entrada esperada, no podemos hacer nada
    if(argc <= 2)
    {
        std::cout << "Faltan argumentos.";
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

    bind(socketd, results->ai_addr, results->ai_addrlen);


    bool exit = false;
    while(!exit)
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        char buffer[80];
        char sendBuffer[80];

        struct sockaddr cliente;
        socklen_t cliente_len = sizeof(struct sockaddr);

        //recibimos el mensaje y comprobamos que no haya errores
        int bytes = recvfrom(socketd, buffer, 80, 0, 
                            &cliente, &cliente_len);

        if(bytes == -1)
        {
            return -1;
        }

        getnameinfo(&cliente, cliente_len, 
                    host, NI_MAXHOST, 
                    serv, NI_MAXSERV,
                    NI_NUMERICHOST);
        
        // escribimos la ip y el puerto del emisor
        char ip[80];
        inet_ntop(cliente.sa_family, &cliente, ip, cliente_len);
        
        tm* t;
        time_t rawtime = time(0);
        t = localtime(&rawtime);
        // struct tm * timeinfo;
        // char time_buffer [80];

        // time (&rawtime);
        // timeinfo = localtime (&rawtime);
        int sendBytes = 0;
        if(bytes == 2)
        {

            if(buffer[0] == 't')
            {
                std::cout << bytes << "bytes de " << ip << ":" << serv << "\n";
                sendBytes = strftime(sendBuffer, 80, "%H:%M", t);
            }
            else if(buffer[0] == 'd')
            {
                std::cout << bytes << "bytes de " << ip << ":" << serv << "\n";
                sendBytes = strftime(sendBuffer, 80, "%Y-%m-%d", t);
            }
            else if(buffer[0] == 'q')
            {
                std::cout << bytes << "bytes de " << ip << ":" << serv << "\n";
                exit = true;
            }
            else
            {
                std::cout << "Comando no reconocido: " << buffer;
            }
            
        }
        else
        {
            std::cout << "Comando no reconocido: " << buffer;
        }
        sendto(socketd, sendBuffer, sendBytes, 0, (sockaddr*) &cliente, cliente_len);

    }
    close(socketd);


    freeaddrinfo(results);
    return 0;
}