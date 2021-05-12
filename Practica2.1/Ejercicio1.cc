#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[])
{
    addrinfo* results;

    // guardamos los flags necesarios

    // si no tenemos la entrada esperada, no podemos hacer nada
    if(argc < 2)
    {
        std::cout << "Faltan argumentos.\n";
        return -1;
    }

    // si no, conseguimos la informacion del address que nos han pasado
    int rc = getaddrinfo(argv[1], argv[2], NULL, &results);

    // rc guarda el codigo de error (si hay alguno)
    if(rc != 0)
    {
        std::cout << "Error: " << gai_strerror(rc) << "\n";
        return -1;
    }

    // si no ha habido ningun error, imprimimos la informacion
    addrinfo* info = results;
    while(info)
    {
        char host[NI_MAXHOST];
        //char serv[NI_MAXSERV];

        getnameinfo(info->ai_addr, info->ai_addrlen, 
                    host, NI_MAXHOST, 
                    NULL, NULL,
                    NI_NUMERICHOST | NI_NUMERICSERV);
        
        std::cout << "Host: " << host << " Family: "<< info->ai_family << " Type: " << info->ai_socktype << "\n";


        info = info->ai_next;
    }

    freeaddrinfo(results);

    return 0;
}