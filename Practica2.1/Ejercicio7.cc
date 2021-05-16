#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <thread>
#include <pthread.h>

int threadIndex = 0;
class MyThread
{
public:
    MyThread(int sock): sockd(sock), th(&MyThread::receiveMessage, this){}
    ~MyThread()
    {
        th.join();
    }

    void receiveMessage()
    {
        bool exit = false;
        char buffer[255];
        memset(&buffer, '\0', sizeof(char) * 255);
        while(!exit)
        {
            //recibimos el mensaje y comprobamos que no haya errores
            int bytes = recv(sockd, &buffer, 255, 0);

            if(bytes < 1)
            {
                std::cout << "Conexion terminada\n";
                exit = true;
            }
            else
            {
                buffer[bytes] = '\0';
                send(sockd, &buffer, bytes, 0);
            }
        }
        close(sockd);
    }
private:
    int sockd;
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    char buffer[255];
    std::thread th;
};

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

    if(bind(socketd, results->ai_addr, results->ai_addrlen))
    {
        std::cout << "Error al hacer el bind\n";
        return -1;
    }
    freeaddrinfo(results);

    //ponemos el socket a "escuchar" hasta que alguien se conecte
    if(listen(socketd, 1))
    {
        std::cout << "Error al intentar poner el socket a \"escuchar\" posibles conexiones \n";
        return -1;
    }

    socklen_t cliente_len = sizeof(struct sockaddr);
    struct sockaddr cliente;

    // creamos los hilos
    int max_threads = 25;
    MyThread* mythreads[max_threads];
    while(true)
    {
        // se conecta un cliente
        int idCliente = accept(socketd, &cliente, &cliente_len);
        if(idCliente < 0)
        {
            std::cout << "Error al conectar el cliente con el servidor \n";
            return -1;        
        }

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        // para evitar errores
        memset(&host, '\0', sizeof(char) * NI_MAXHOST);
        memset(&serv, '\0', sizeof(char) * NI_MAXSERV);

        if(getnameinfo(&cliente, cliente_len, 
                    host, NI_MAXHOST, 
                    serv, NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICSERV))
        {
            std::cout << "Error al intentar obtener informacion del cliente\n";
            return -1;    
        }

        // para que no haya infinitos clientes
        if(threadIndex < max_threads)
        {
            std::cout << "Conexion desde " << host << "  " << serv << "\n";
            mythreads[threadIndex] = new MyThread(idCliente);
        }
        
    }

    // eliminamos la memoria creada
    for(int i = 0; i < threadIndex; i++)
    {
        delete mythreads[i]; mythreads[i] = nullptr;
    }

    return 0;
}