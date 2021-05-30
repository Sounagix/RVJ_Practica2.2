#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo.
    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase
    struct addrinfo hints;
    struct addrinfo *result;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(address,port,&hints,&result);

    if (rc != 0)
        std::cerr << "[getaddrinfo] " << gai_strerror(rc) << std::endl;


    sd = socket(result->ai_family, result->ai_socktype, 0);

    if (sd == -1)
        std::cerr << "[socket]" << std::endl;
    

    sa = *result->ai_addr;
    sa_len = result->ai_addrlen;
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    obj.to_bin();
    ssize_t bytes = sendto(sock.sd, obj.data(), obj.size(), 0, &sock.sa, sock.sa_len);
    if(bytes <= 0){
        std::cout << "[sendto] Error al enviar el mensaje\n";
        return -1;
    }
    else return 0;
}

bool operator== (const Socket &s1, const Socket &s2)
{
    //Comparar los campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    //Retornar false si alguno difiere

    struct sockaddr_in *address_in_soc1 = (struct sockaddr_in *) &(s1.sa);
    struct sockaddr_in *address_in_soc2 = (struct sockaddr_in *) &(s2.sa);

    return (address_in_soc1->sin_addr.s_addr == address_in_soc2->sin_addr.s_addr &&
            address_in_soc1->sin_family == address_in_soc2->sin_family && 
            address_in_soc1->sin_port == address_in_soc2->sin_port );
};

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};

