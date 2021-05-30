#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME 80

class Jugador: public Serializable
{
private:
    //  Coordenada x del jugador
    int16_t x;
    //  Coordenada y del jugador
    int16_t y;
public:
    //  Nombre del jugador
    char name[MAX_NAME];

    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};
    
    //  Devuelve la coordena x del jugador
    int16_t getX() const{
        return x;
    }

    //  Devuelve la coordenada y del jugador
    int16_t getY() const{
        return y;
    }

    void to_bin() override
    {
        //  Guardamos el tamaño necesario en _size
		_size = sizeof(int16_t) * 2 + sizeof(char) * MAX_NAME;
        //  Aloc al tamaño
		alloc_data(_size);
		//  Guardamos el nombre
		char *actData = _data;
		memcpy(actData, name, MAX_NAME * sizeof(char));
		actData += MAX_NAME * sizeof(char);
		//  Guardamos la x
		memcpy(actData, &x, sizeof(int16_t));
		actData += sizeof(int16_t);
		//  Guardamos la y
		memcpy(actData, &y, sizeof(int16_t));
		actData += sizeof(int16_t);
    }

    int from_bin(char * data) override
    {
        //Puntero que apunte a la direccion de memoria
        //para copiar
        char *actData = data;
        //Copiar nombre
        memcpy(name, actData, MAX_NAME * sizeof(char));
        //Avanzar puntero
        actData += MAX_NAME * sizeof(char);
        //Copiar entero x
        memcpy(&x, actData, sizeof(int16_t));
        //Avanzar puntero
        actData += sizeof(int16_t);
        //Copiar entero y
        memcpy(&y, actData, sizeof(int16_t));
        return 0;
    }


};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int lect = open("./data_jugador.txt", O_CREAT | O_TRUNC |O_RDWR, 0666);
    ssize_t tam = write(lect,one_w.data(), one_w.size());
    if(tam != one_w.size()){
        std::cout << "Error al escribir el fichero de player 1\n";
        return -1;
    }
    close(lect);

    // 3. Leer el fichero
    lect = open("./data_jugador.txt", O_RDONLY, 0666);
    char buffer[tam];
    if(read(lect, &buffer, tam) == -1){
        std::cerr << "Error al leer el fichero del player 1\n";
        return -1;
    }
    close(lect);
    
    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);

    // 5. Mostrar el contenido de one_r
    std::cout << "Nombre: " << one_r.name 
    << "\n Coordenadas : (x , y) " << one_r.getX() 
    << " , "  << one_r.getY() << std::endl;
    return 0;
}

