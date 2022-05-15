# Documentación

# Instalación de docker

Si no tenemos docker instalado, procedemos a instalarlo de la siguiente forma:

### <font size="5">**En Windows**</font>

1. Instalar Docker y Docker Compose, para esto se puede apoyar en la siguiente página: <https://docs.docker.com/desktop/windows/install/>
    - Puede descargar el ejecutable que incluse Docker y Docker Compose en: <https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe>


### <font size="5">**En linux**</font>

#### <font size="4">**Configurar repositorio:**</font>

1. Actualizar la lista de paquetes con:

```bash
sudo apt-get update
```

1. Permitir apt usar un repositorio por medio de HTTPS

```bash
sudo apt-get install \
ca-certificates \
curl \
gnupg \
lsb-release
```

3. Añadir la llave oficial GPG de Docker
```curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg```
4. Configurar la rama stable del Repositorio:

```bash
echo \
"deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
$(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
```

#### <font size="4">**Instalar Docker Engine:**</font>

1. Instalamos la última versión de Docker Engine, containerd y Docker Compose:

```bash
sudo apt-get update && \
sudo apt-get install docker-ce docker-ce-cli containerd.io docker-compose-plugin
```

#### <font size="4">**Instalar Docker Compose:**</font>

1. Usamos este comando para descargar la última versión estable de Docker Compose:

```bash
DOCKER_CONFIG=${DOCKER_CONFIG:-$HOME/.docker} && \
mkdir -p $DOCKER_CONFIG/cli-plugins && \
curl -SL https://github.com/docker/compose/releases/download/v2.4.1/docker-compose-linux-x86_64 -o && \
$DOCKER_CONFIG/cli-plugins/docker-compose
```

1. Damos permisos para ejecutar el instalador:

```bash
chmod +x $DOCKER_CONFIG/cli-plugins docker-compose
```

1. Probamos la instalación con:

```bash
docker compose version
```


# Ejecutar la aplicación

Para ejecutar el servidor o calculadora de red, se habre la carpeta del proyecto y dentro de la carpeta se habre una consola en donde se ejecuta:

### <font size="5">**En Windows**</font>
```bash
docker-compose up -d --build
```

- Algunas veces en Windows es necesario añadir manualmente la carpeta de \Docker\resources\bin a la
variable de entorno PATH para que reconozca el comando ***docker-compose***.

### <font size="5">**En linux**</font>

``` bash
docker-compose up -d --build
```

- Recomendación: Utilizar Visual Studio Code para abrir la carpeta y una consola.

Para detener el proyecto o servidor, solo se utiliza el comando:
```bash
docker-compose down 
```

# Instalar Telnet

### <font size="5">**En Windows**</font>
```bash
Install-WindowsFeature Telnet-Client
```

### <font size="5">**En Linux**</font>
```bash
apt-get install telnet
```

# Pruebas

Una ves encendido el servidor e instalado telnet, podemos realizar las siguientes pruebas:

### <font size="5">**Conectarse al servidor como cliente con Telnet**</font>

En una consola introducimos el comando:
```bash
telnet 0.0.0.0 9666
```

Con el anterior comando ingresamos como nuevo cliente al servidor y ya podemos realizar las peticiones.

### <font size="5">**Peticiones de ejemplo**</font>

Obtener el broadcast de la red

```bash
GET BROADCAST IP 10.8.2.5 MASK /29
o
GET BROADCAST IP 172.16.0.56 MASK 255.255.255.128
```

Obtener la dirección de la red

```bash
GET NETWORK NUMBER IP 10.8.2.5 MASK /29
o
GET NETWORK NUMBER IP 172.16.0.56 MASK 255.255.255.128
```

Obtener el rango de direcciones ip

```bash
GET HOSTS RANGE IP 10.8.2.5 MASK /29
o
GET HOSTS RANGE IP 172.16.0.56 MASK 255.255.255.128
```

Obtener direcciones de subredes aleatorias

```bash
GET RANDOM SUBNETS NETWORK NUMBER 10.0.0.0 MASK /8 NUMBER 3 SIZE /24
```

Para desconectar el cliente del servidor utilizamos el comando:
``` bash
EXIT
o
exit
```

# Recomendaciones
- Utilizar visual studio code para abrir la carpeta de la aplicación y ejecutarla.
- Utilizar calculadoras de red en linea para corroborar respuesta.


## Referencias
- How to implement TCP sockets in C. Educative: Interactive Courses for Software Developers. (2022). Retrieved 15 May 2022, from https://www.educative.io/edpresso/how-to-implement-tcp-sockets-in-c.

- Regular Expressions Cookbook. O’Reilly Online Learning. (2022). Retrieved 15 May 2022, from https://www.oreilly.com/library/view/regular-expressions-cookbook/9780596802837/ch07s16.html.
- Flags for POSIX Regexps (The GNU C Library). Gnu.org. (2022). Retrieved 15 May 2022, from https://www.gnu.org/software/libc/manual/html_node/Flags-for-POSIX-Regexps.html.
- How to write regular expressions in C. Educative: Interactive Courses for Software Developers. (2022). Retrieved 15 May 2022, from https://www.educative.io/edpresso/how-to-write-regular-expressions-in-c.
- RegExr: Learn, Build, & Test RegEx. RegExr. (2022). Retrieved 15 May 2022, from https://regexr.com/.
- C, R. (2022). Regular Expression in C | Functions of Regular Expressions in C. EDUCBA. Retrieved 15 May 2022, from https://www.educba.com/regular-expression-in-c/.
- Khintibidze, L. (2022). Usar typedef enum en C. Delft Stack. Retrieved 15 May 2022, from https://www.delftstack.com/es/howto/c/c-typedef-enum/#:~:text=enum%20en%20C.-,Usar%20enum%20para%20definir%20constantes%20enteras%20con%20nombre%20en%20C,modificados%20en%20tiempo%20de%20ejecuci%C3%B3n.
- bitwise not operator in c Code Example. Codegrepper.com. (2022). Retrieved 15 May 2022, from https://www.codegrepper.com/code-examples/c/bitwise+not+operator+in+c.
- C, I., Richards, K., Perrone, R., & Dandoulakis, N. (2022). Integer to IP Address - C. Stack Overflow. Retrieved 15 May 2022, from https://stackoverflow.com/questions/1680365/integer-to-ip-address-c.
- mask, C., & Jakob, J. (2022). Calculate broadcast address from ip and subnet mask. Stack Overflow. Retrieved 15 May 2022, from https://stackoverflow.com/questions/777617/calculate-broadcast-address-from-ip-and-subnet-mask.
- Cadenas de texto (strings). Platea.pntic.mec.es. (2022). Retrieved 15 May 2022, from http://platea.pntic.mec.es/vgonzale/cyr_0204/cyr_01/control/lengua_C/cadenas.htm#:~:text=Para%20recorrer%20la%20cadena%20necesitamos,por%20tanto%2C%20'E'.
- What is the broadcast address in terms of Ethernet network. | FAQs | Schneider Electric Spain. Proface.com. (2022). Retrieved 15 May 2022, from https://www.proface.com/support/index?page=content&country=ES&lang=en&locale=en_US&id=FA22433&prd=.
- simultaneously, T., Shulman, A., Shulman, A., & Brown, D. (2022). Tokenizing multiple strings simultaneously. Stack Overflow. Retrieved 15 May 2022, from https://stackoverflow.com/questions/9472865/tokenizing-multiple-strings-simultaneously.
- How to implement TCP sockets in C. Educative: Interactive Courses for Software Developers. (2022). Retrieved 15 May 2022, from https://www.educative.io/edpresso/how-to-implement-tcp-sockets-in-c.