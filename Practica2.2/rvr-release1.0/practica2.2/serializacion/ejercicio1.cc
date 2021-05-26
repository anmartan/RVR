#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


class Jugador: public Serializable
{
private:
    static const size_t MAX_NAME = 20;

public:
    Jugador(const char * _n, int16_t _x, int16_t _y):pos_x_(_x),pos_y_(_y)
    {
        strncpy(name_, _n, MAX_NAME);
        _size = 2 * sizeof(int16_t) + MAX_NAME * sizeof(char);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        // reservamos la memoria necesaria
        alloc_data(_size);

        size_t offset = 0;
        // guardamos las variables
        memcpy(_data + offset, name_, MAX_NAME * sizeof(char));
        offset += MAX_NAME;

        memcpy(_data + offset, &pos_x_, sizeof(int16_t));
        offset +=  sizeof(int16_t);

        memcpy(_data + offset, &pos_y_,  sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        // si el tamano del data no es el correcto, devolvemos -1
        if(sizeof(data) > sizeof(char) * MAX_NAME + 2 * sizeof(int16_t))
            return -1;
            
        size_t offset = 0;
        // guardamos las variables
        memcpy(name_, data + offset, sizeof(char) * MAX_NAME);
        offset += sizeof(char) * MAX_NAME;

        memcpy(&pos_x_, data + offset, sizeof(int16_t));
        offset +=  sizeof(int16_t);

        memcpy(&pos_y_, data + offset, sizeof(int16_t));
        return 0;
    }


public:
    char name_[MAX_NAME];

    int16_t pos_x_;
    int16_t pos_y_;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int file = open("./player.player", O_RDWR | O_CREAT, 0666);
    if(file == -1)
    {
        printf("Error al crear el archivo de escritura\n");
        return -1;
    }
    write(file, one_w.data(), one_w.size());
    close(file);

    // 3. Leer el fichero
    file = open("./player.player", O_RDONLY, 0666);
    if(file == -1)
    {
        printf("Error al abrir el archivo de lectura\n");
        return -1;
    }

    // 4. "Deserializar" en one_r
    char* buffer = (char*)malloc(one_r.size());
    int bytes = read(file, buffer, one_r.size());
    close(file);
    if(bytes == -1)
    {
        printf("Error al leer el archivo\n");
    }
    one_r.from_bin(buffer);

    // 5. Mostrar el contenido de one_r
    printf("Nombre del jugador: %s \nPosicion en x: %d \nPosicion en y: %d\n", one_r.name_, one_r.pos_x_, one_r.pos_y_);
    return 0;
}

