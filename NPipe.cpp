/**PROGRAMA NPIPE.CPP
*Programa que crea un archivo FIFO para recepción de datos 
*de otro programa que será invocado mediante execl()
*
* Por Alex A. Turriza Suárez - Enero 2017
**/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>

/* Función que abre un FIFO dado un nombre. De no existir, le crea.
La función recibe dos parámetros:
 - Apuntador nombre, al primer caracter de una cadena.
 - Caracter modo, que indicará el modo de apertura del FIFO. 
   Se admiten:
	- 'r' para modo lectura.
	- 'w' para modo escritura.
*/
int openFifo(char *nombre, char modo)
{
	int fd;
	//std::cout << "Intentando abrir FIFO..." << std::endl;
	if(mkfifo(nombre, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH))
	{
		switch(errno)
		{
		case EEXIST:
			//perror(NULL); //Impresión para términos de depuración
			break;
		default:
			perror("mkfifo() ERROR:");
			return(-1);
		}
	}
	else
	{
		std::cout << "FIFO No existe, FIFO creado con éxito" << std::endl;
	}

	switch(modo)
	{
		case 'r':
		case 'R':
			fd = open(nombre, O_RDONLY | O_NONBLOCK);
			if(fd < 0)
			{
				perror("open() npipe_r ERROR:");
				return (-1);	
			}			
			break;
		case 'w':
		case 'W':
			do
			{
				fd = open(nombre, O_WRONLY | O_NONBLOCK);
				if(fd < 0 && errno != ENXIO) //Monitorear ENXIO error si se coloca máscara de O_NONBLOCK en open().
				{					
					perror("open() npipe_w ERROR:");
					return (-1);
				}
			}
			while(errno == ENXIO); //Revisar documentación de open para más info de error ENXIO: $man 3 open
			break;
	}
	return (fd);
}

int escribeNPIPE(int fdNPIPE, void * buf, size_t bytes)
{
	int nbytes;
	nbytes = write(fdNPIPE, buf, bytes);
	if(nbytes < 0 && errno != EAGAIN)
	{
		perror("ERROR escritura NPIPE:");
		return (-1);
	}
	return nbytes;
}

int leeNPIPE(int fdNPIPE, void * buf, size_t bytes)
{
	int nbytes;
	nbytes = read(fdNPIPE, buf, bytes);
	if(nbytes < 0 && errno != EAGAIN)
	{		
		perror("ERROR lectura NPIPE:");
		return (-1);
	}
	return nbytes;
}

int main()
{
	int cPID, fd;
	char dato[25] = "You'll never walk alone!";
	char bufferLectura;
	char* userLogin;
	if(!(cPID = fork()))
	{
		//Inicia rutina del programa hijo
		if(chdir("/home/alexrt07/Escritorio")) //Colocar FIFO en escritorio, ya que dropbox puede causar conflicto
			perror("chdir() ERROR:");
                else
			fd = openFifo("rtkFIFO", 'w');
		if (fd < 0)
			return -1;
		else
			for(int i = 0; i<24; i++)
				escribeNPIPE(fd, &dato[i], sizeof(dato[i]));			
			close(fd);
		//execl(); //PENDIENTE DE IMPLEMENTACIÓN
	}
	else
	{
		//Inicia rutina del programa padre
		if(chdir("/home/alexrt07/Escritorio")) //Colocar FIFO en escritorio, ya que dropbox puede causar conflicto
                        perror("chdir() ERROR:");
                else
			fd = openFifo("rtkFIFO", 'r');
		if (fd < 0)
			return -1;
		else
			std::cout << std::endl;
			do
			{
				if(leeNPIPE(fd, &bufferLectura, sizeof(bufferLectura)) == sizeof(bufferLectura))
					std::cout << bufferLectura; 
			}
			while(bufferLectura != '!');
			close(fd);
			std::cout << std::endl;
	}
return 0;
}
