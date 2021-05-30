#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    size_t offset = 0, size = sizeof(type);
    memcpy(_data, &type, size);
    offset += size;

    size = sizeof(char) * 8;
    memcpy(_data + offset, &nick, size);
    offset += size;

    size = sizeof(char) * 80;
    memcpy(_data + offset, &message, size);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    size_t offset = 0, size = sizeof(type);
    memcpy(&type, bobj + offset, size);
    offset += size;

    size = sizeof(char) * 8;
    memcpy(&nick, _data + offset, size);
    offset += size;

    size = sizeof(char) * 80;
    memcpy(&message, _data + offset, size);
    
    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        ChatMessage message;
        Socket* client;

        socket.recv(message, client);
        std::unique_ptr<Socket> cl(client);

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        switch (message.type)
        {
            int i;
            bool found;
        case ChatMessage::MessageType::LOGIN:

            // anadimos el cliente
            clients.push_back(std::move(cl));
            break;
        case ChatMessage::MessageType::LOGOUT:
            i = 0;
            found = false;

            // buscamos el cliente
            while(i < clients.size() && !found)
            {
                found = clients.at(i) == cl;
                i++;
            }

            // eliminamos el cliente
            if(found)
            {
                clients.erase(clients.begin() + i -1);
            }
            break;
        case ChatMessage::MessageType::MESSAGE:


            for(int i = 0; i < clients.size(); i++)
            {
                if(clients.at(i) != cl)
                {
                    socket.send(message, *clients.at(i));
                }
            }
            break;
        default:
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
    std::string message;
    ChatMessage out(nick, message);
    out.type = ChatMessage::LOGOUT;

    socket.send(out, socket);
}

void ChatClient::input_thread()
{
    bool exit = false;
    while (!exit)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        ChatMessage message;
        std::string msg;
        std::getline(std::cin, msg);

        message.type = ChatMessage::MESSAGE;
        message.nick = nick;
        message.message = msg;

        // si el usuario quiere salir, mandamos el mensaje de logout
        if(msg == "!exit")
        {
            logout();
            exit = true;
        }
        // si no, enviamos un mensaje con los datos correctos
        else
        {
            socket.send(message, socket);
        }
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage message;
        socket.recv(message);

        printf("%s: %s", message.nick, message.message);
    }
}

