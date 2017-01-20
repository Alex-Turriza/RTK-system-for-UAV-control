/** PROGRAMA RTKSOCKET
* Programa que crea un nuevo proceso hijo que ejecuta RTKLIB
* mientras el proceso padre se conecta a él mediante socket cliente.
*
* Por Alex A. Turriza Suárez - 2017
**/

#include <sys/socket.h> //Usado por socket()
#include <sys/types.h> //Linux Programmer's Manual recomienda incluirlo por portabilidad $man connect
#include <unistd.h> //Usado por fork()
#include <errno.h> //Usado por errno
#include <netinet/in.h> //Declaración de IPPROTO_TCP y de inet_aton()
#include <arpa/inet.h> //Usado por inet_aton()
#include <cstdio> //Usado por perror
#include <iostream>

int openSocket()
/*Función que crea un socket y devuelve el descriptor*/
{
	int filedes;
	if((filedes = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		perror("socket() ERROR");
		return -1;
	}
	return filedes;
}

int conecta(char * Direccion, int puerto, int sockfd)
/*Función que conecta un socket a una dirección dada y un determinado puerto*/
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(puerto);
	if(!(inet_aton(Direccion, (struct in_addr *) &addr.sin_addr.s_addr)))
	{
		std::cout << "inet_aton() ERROR: Dirección no válida" << std::endl;
		return (-1);
	}
	if(connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		perror("connect() ERROR");
		return (-1);
	}
	return 0;
}

int leeSckt(int fdSocket, void * buf, size_t bytes)
{
	int nbytes;
	if((nbytes = read(fdSocket, buf, bytes)) < 0)
	{
		perror("read() ERROR:");
		return (-1);
	}
	return nbytes;
}

int main()
{
	int cPID, fd;
	char bufferLectura;
	if(!(cPID = fork()))
	{
		//Inicia rutina del programa hijo
		if(chdir("/home/alexrt07/RTKLIB-rtklib_2.4.3/app/rtkrcv/gcc"))
		{
			perror("chdir() ERROR:");
			return (-1);
		}
		else
		{
			std::cout << "Aquí debería correr RTKRCV" << std::endl;
			/**if(execl("rtkrcv", "rtkrcv", "-s", "-o", "uuuu.conf", NULL))
			{
				perror("Execl() ERROR:");
				return (-1);
			}**/
		}
	}
	else
	{
		//Inicia rutina del programa padre
		/*ALGORITMO: Crear socket
		/*Conectar 127.0.0.1:8989
		/*Obtener mensaje en buffer
		/*Desplegar mensajes
		/*FIN de algoritmo*/

		sleep(3); //Demos tiempo a que RTKLIB inicie correctamente.
		if((fd = openSocket()) >= 0)
		{
			if(!(conecta("127.0.0.1", 8988, fd)))
			{	
				//Conexión exitosa.
				std::cout << "Conexión exitosa" << std::endl;
				do
				{
					if(leeSckt(fd, &bufferLectura, sizeof(bufferLectura)) == sizeof(bufferLectura))
					{
						std::cout << bufferLectura;
					}
				}
				while(true);				
			}
		}	
		if(close(fd))
		{
			perror("close() socket ERROR");
			return -1;
		}
	}
	return 0;
}
