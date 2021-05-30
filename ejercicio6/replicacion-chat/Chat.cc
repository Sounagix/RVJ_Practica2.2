#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

	 //Para los datos de nick, type y message -> _data
	char *currData = _data;

	memcpy(currData, &type, sizeof(uint8_t));
	currData += sizeof(uint8_t);

	memcpy(currData, nick.c_str(), 8 * sizeof(char));
	currData += 8 * sizeof(char);

	memcpy(currData, message.c_str(), 80 * sizeof(char));
	currData += 80 * sizeof(char);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char *currData = _data;

    memcpy(&type, currData, sizeof(uint8_t));
    currData += sizeof(uint8_t);
    nick = currData;
    message = currData;
    currData += sizeof(char) * 8;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        ChatMessage currMsg;
		Socket *client = &socket; 
		socket.recv(currMsg, client);

		std::unique_ptr<Socket> clientPtrSocket(client);

		switch (currMsg.type)
		{
		case ChatMessage::LOGIN :
            std::cout << "User: " << currMsg.nick <<  " conected\n";
            //  Se añade a los clientes
            clients.push_back(std::move(std::make_unique<Socket>(*client)));
			break;
		case ChatMessage::MESSAGE:
			for (auto it = clients.begin(); it != clients.end(); it++)
			{
				if (!(**it == *clientPtrSocket))
				{
					socket.send(currMsg, **it);
				}
			}
			break;
		case ChatMessage::LOGOUT:
		{
			bool clientFound = false;
			auto it = clients.begin();
			while (!clientFound && it != clients.end())
			{
				if (**it == *clientPtrSocket)
				{
					it = clients.erase(it);
                    std::cout << "User: " << currMsg.nick <<  " disconected\n";
					clientFound = true;
				}
				else  it++;
			}
			break;
		}
		default:
            std::cerr << "Undefined message \n";
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
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

