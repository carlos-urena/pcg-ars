# El Cauce Gráfico en dispositivos móviles 

En esta carpeta se encuentra una subcarpeta (`proyecto.android-studio`) con los fuentes _Kotlin_ y demás archivos del proyecto de _Android Studio_ para la asignatura. En este README están las instrucciones para instalación de Android Studio y la compilación y ejecución de la misma, tanto en un dispositivo móvil virtual como en un dispositivo móvil físico.

## Código 

El código fuente de este proyecto está en estas carpetas de este repositorio:

   - Archivos Kotlin (`.kt`): sub-carpeta [pcg1](proyecto.android-studio/app/src/main/java/mds/pcg1)
   - Archivos GLSL (`.glsl`): sub-carpeta [shaders](proyecto.android-studio/app/src/main/assets/shaders)

Este código se edita y compila normalmente usando _Android Studio_

## Instalación de Android Studio 


Para instalar Android Studio se accede aquí: 

 
 [developer.android.com/studio](https://developer.android.com/studio)
 
 Hay versiones para Linux, Windows y macOS.

### Linux

 Los pasos siguientes de instalación se han probado en Linux (Ubuntu 22.04 LTS) con la versión <i>Android Studio Hedgehog</i> - 2023.1.1 Patch 2 para Linux.


Las instrucciones de instalación están aquí:

[developer.android.com/studio/install](https://developer.android.com/studio/install)

Ahí e dice que en Linux de 64 bits hay que instalar unas librerías adicionales. Para comprobar si Ubuntu es de 32 bits o no, se puede acceder a la ventana de configuración de Ubuntu y mirar, en la sección _Acerca de_ (al final), el tipo de SO (64 o 32 bits). En mi caso efectivamente se trata de un Ubuntu de 64 bits, así que es necesario hacer:

``` 
sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386 lib32z1 libbz2-1.0:i386
``` 

El siguiente paso es descargar de la Web el archivo con Android Studio, en Linux es un `.tar.gz`. Lo descargamos a una carpeta nueva vacía. En esa carpeta, extraemos el archivo. Se puede extraer con el administrador de archivos o en la línea de órdenes con:

```
tar zxvf archivo.tar.gz
```

Al abrirlo se crea una subcarpeta, de nombre `android-studio`, a su vez tiene una subcarpeta de nombre `bin`. Hacemos `cd` a la subcarpeta `bin` y lanzamos Android Studio con:

```
./studio.sh
```

Al lanzarlo la primera vez se ejecuta el _Setup Wizard_ para instalar el SDK de Android (conjunto de herramientas y librerías de desarrollo de Google para Android). Durante la instalación se pregunta:

  -  La carpeta de instalación (<i>config or installation directory</i>), lo dejo en blanco para que se instale en la ubicación por defecto, que será 
  en `~/Android/Sdk` 
  - Si se quiere hacer una instalación  _Standard_ o _Custom_, elijo _Standard_ 

En el recuadro para aceptar el acuerdo de licencia hay que pulsar _accept__ **para cada una**  de las componentes que se instalan (es decir, hay que pulsarlo varias veces). La instalación tarda un poco (son 5,5 Gb en Ubuntu).

Una vez que se ha completado la instalación, aparece un recuadro para abrir un proyecto o crear uno nuevo. En ese momento se puede salir de la aplicación y mover la carpeta `android-studio` a su ubicación definitiva, que puede ser, por ejemplo, dentro de la carpeta `~/Android`, junto con el SDK. 

Finalmente para lanzar la aplicación hay que volver a ejecutar el script `studio.sh`. Para poder hacer esto fácilmente desde la línea de órdenes, se puede definir un _alias_ de Linux (de nombre, por ejemplo, `studio`) que lance la aplicación.

### macOS

La instalación en el SO de Apple es similar a la de Linux, excepto que en este caso se descarga un archivo `.dmg`. Después de descargar, la aplicación Android Studio se debe mover a la carpeta `Aplicaciones`, como es habitual en macOs. En la primera ejecución se instalará el SDK, al igual que en Linux. Para lanzar el programa fácilmente, un _alias_, igual que en Linux.

### Windows 

Pendiente.

## Creación de un proyecto para OpenGL 

Aunque en esta asignatura los estudiantes abrirán un proyecto ya creado, en esta subcarpeta se dan detalles acerca de como se ha creado dicho proyecto: 

[Creacion del proyecto](README-crea.md)

## Uso del proyecto _Android Studio_


### Abrir el proyecto en AS 


Lanzar la aplicación _Android Studio_, y abrir el proyecto que está en esta carpeta 

``` 
proyecto.android-studio
``` 

Para abrir ese proyecto se puede seleccionar directamente de la lista de proyectos abiertos en el pasado, o bien (la primera vez) pulsar el botón _open_ y navegar hasta la carpeta que contiene `proyecto.android-studio` y seleccionarla.

Es conveniente instalar la extensión GLSL, para poder editar fácilmente el código en los shaders.

### Edición

Los fuentes del proyecto se pueden editar con el editor de Android Studio, son archivos de extensión `.kt` (fuentes Kotlin). El editor señala todos los posibles errores, si bien a veces pueden ocurrir errores al ejecutar que no han sido señalados por el editor ni el compilador

### Compilación 

El proyecto de puede compilar usando el botón _build_ (es un icono de un martillo). Los posibles errores o warnings aparecerán en el panel correspondiente (el que tiene el símbolo de un martillo). 

### Gestor de dispositivos 

Android Studio puede tener uno o varios dispositivos en ejecución, cada uno de ellos puede ser _virtual_ (se ejecuta en un simulador en el ordenador) o _físico_ (se ejecuta en un dispositivo Android conectado al ordenador por USB o la Wifi). 

Android Studio provee de un dispositivo virtual por defecto, que está activado al inicio. En el gestor de dispositivos podemos ver la pantalla de ese dispositivo.

Se pueden seleccionar otros dispositivos virtuales disponibles en el gestor de dispositivos, así como configurar sus características.

### Ejecución en un dispositivo virtual

El proyecto se puede compilar y ejecutar en el dispositivo virtual actual con el botón _Run_, es un triángulo apuntando a la derecha, que aparece en la parte superior de la ventana, cuando la aplicación se puede ejecutar, aparecerá de color verde. Para terminar la ejecución, hay que pulsar el botón _Stop_, es un botón con un recuadro rojo que aparece a la derecha del botón _Run_. 

Durante la ejecución, los mensajes de _log_ (emitidos desde el código con la función `Log.v`) aparecerán en el panel _logcat_, que puede mostrarse pulsando en el botón con un icono de un gato. 

Durante la ejecución aparecerá a la derecha una imagen de la pantalla del dispositivo móvil, cuyo modelo es el que selecciones de entre los disponibles (por defecto es un Google Pixel 3A).

### Ejecución en un dispositivo físico

Aquí están las instrucciones para ejecutar y depurar la aplicación usando un dispositivo Android conectado al ordenador (probado con un cable USB).
Estas instrucciones se han probado en un Google Pixel en Marzo de 2024. Se pueden consultar estas páginas del sitio Web _Android Developer_: 

  - [Configuración del ordenador](https://developer.android.com/studio/run/device)
  - [Configuración del dispositivo](https://developer.android.com/codelabs/basic-android-kotlin-compose-connect-device)


A continuación los pasos que he dado, siguiendo esas instrucciones:

#### 1. Añadir usuario al grupo _plugdev_ 

```
sudo usermod -aG plugdev $LOGNAME
``` 

Verificar que ha ido bien (el grupo `plugdev` debe estar en la salida de la orden `groups`, que lista los grupos del usuario)

#### 2. Añadir package de Linux con reglas _udev_

```
sudo apt install android-sdk-platform-tools-common
```

(tener en cuenta el _sudo_, no aparece en la página de _Android Developer_)

#### 3. Habilitar ejecución y depuración en el dispositivo. 

En el dispositivo Android es necesario permitir que se ejecuten aplicaciones enviadas desde Android Studio, se hace con:

  1. Ir a _Ajustes_ -- _Información del Teléfono_:  en esa pantalla pulsar 7 veces sobre _Número de compilación_
  2. Ir a  _Ajustes_ -- _Sistema_ -- _Opciones para desarrolladores_ :  activar _Usar opciones para desarrolladores_, activar _Depuración USB_


#### 4. Ejecutar en el dispositivo.

Conectar el dispositivo al ordenador con un cable USB (USB-A o USB-C).

Lanzar Android Studio, si el dispositivo físico no aparece ya seleccionado en la barra superior, seleccionarlo. 
A partir de este momento se puede ejecutar y parar la aplicación, se lanza igual que cuando se usa un dispositivo virtual, solo que ahora la aplicación se ejecuta en el dispositivo. Los mensajes de _log_ aparecen en la ventana _logcat_ de Android Studio en el ordenador.




### Compilación, instalación y ejecución desde la línea de comandos (en Linux)

El proyecto se puede compilar y ejecutar en un dispositivo físico sin usar el IDE de _Android Studio_. Para ello se usa la herramienta de compilación _Gradle_ (https://gradle.org), que se instala al instalar AS. 

Dentro de la carpeta del proyecto AS hay un script shell llamado `gradlew` (por _gradle wrapper_) que se debe ejecutar para compilar y ejecutar la aplicación (el manual de uso de `gradlew` está aquí: https://docs.gradle.org/current/userguide/gradle_wrapper.html).

#### Compilación

Antes de usar el script `gradlew` para compilar debemos de asegurarnos de que el equipo tiene instalado el SDK de Java, versión 17. Puedes comprobar si lo tienes instalado (y en su caso, la versión instalada) ejecutando

```
javac --version
``` 
Si no lo tienes instalado (no existe `javac`) o la versión es inferior a la 17, se puede instalar (en mi caso para un Ubuntu Linux) con:

``` 
sudo apt install openjdk-17-jdk
``` 

(hay que tener en cuenta que esta versión 17 es la mínima que se pide en Marzo de 2024, más adelante podrá cambiar).

Una vez tenemos instalado el SDK de Java, podemos hacer `cd` a la carpeta del proyecto y verificar que  `gradlew` se puede ejecutar. Para ello listamos las posibles tareas de compilación, instalación y ejecución que `gradlew` puede hacer, con la orden:

```
./gradlew tasks
```

Para compilar los fuentes Kotlin se puede ejecutar la tarea `build`

```
./gradlew build 
```

La primera vez tardará, pero acaba en un tiempo razonable incluso en ordenadores no excesivamente potentes. Una vez que se ha hecho este primer build, los siguientes será más rápidos y solo recompilarán los archivos modificados.

Si hubiese algún error durante la compilación de los fuentes, aparecerán en el terminal los correspondientes mensajes de error (con "e:")

Los archivos fuente se pueden editar con cualquier editor o IDE, no necesariamente AS. Cada vez que se haga un _build_, se recompilarán los fuentes afectados por los cambios desde el último _build_.

#### Instalación y ejecución (Linux)

La aplicación se puede instalar y ejecutar en un dispositivo físico. 

Configuramos el dispositivo físico igual que se indica arriba para el caso de usar el IDE. Si teníamos la aplicación instalada en el móvil, la desinstalamos (hacer una pulsación larga sobre el icono la aplicación y seleccionar _Información de la aplicación_ -- _Desinstalar__ ). 

Con _Gradlew_ podemos instalar el archivo APK de la aplicación en el dispositivo físico (el archivo APK es un archivo que contiene dentro el bytecode, los assets, y todos los archivos y datos necesarios para ejecutar la aplicación el móvil). Para hacerlo se usa la tarea _installDebug_, es decir, hacemos:

```
./gradlew installDebug
```

Una vez hecho esto, el icono aparece en la pantalla con todas las aplicaciones del móvil, y se puede lanzar como cualquier otra aplicación. Si la aplicación se lanza así, no veremos los mensajes de _Log_ que dicha aplicación emita. Para eso será necesario usar la orden ADB, tal y como se indica a continuación.

#### Ejecución con ADB

La aplicación _ADB_ (por _Android Debug Bridge_) (https://developer.android.com/tools/adb) permite controlar un dispositivo Android desde un ordenador al que está conectado, incluyendo la ejecución y parada de Apps y la visualización de los mensajes que emiten. Para usarla basta con conectar el ordenador y el dispositivo con un cable o por Wifi, y configurar el dispositivo como se ha indicado arriba. La aplicación permite una gran cantidad de opciones, pero nosotros estamos interesados en ver los mensajes de log del nuestra aplicación. 

En linux se puede instalar ADB con:

```
sudo apt install adb
``` 

Una vez instalada nuestra App en el móvil con _gradlew_ (como se ha dicho arriba) y conectado el dispositivo, se puede usar ADB. En primer lugar verificamos que el dispositivo aparece conectado:

```
adb devices -l
```

A modo de ejemplo, con un dispositivo _Pixel 8_ de Google, obtengo: 

```
* daemon not running; starting now at tcp:5037
* daemon started successfully
List of devices attached
38091FDJG00CU2         device usb:3-4 product:husky model:Pixel_8_Pro device:husky transport_id:1
```

Ahora podemos listar todos los _packages_ instalados en el dispositivo, para eso usamos la sub-orden `pm` (_package manager_) de ADB:

```
adb shell pm list packages 
``` 

La lista es larga (322 paquetes en el Pixel 8 de pruebas, que no tiene instalada ninguna aplicación adicional a las preinstaladas), así que podemos verificar que nuestra aplicación está instalada filtrando la salida para que solo muestre el package `mds.pcg1`

```
adb shell pm list packages | grep 'mds.pcg1' 
``` 

Si no hay problemas debe aparecer una única línea: `package:mds.pcg1`


Para ejecutar una aplicación, usamos el nombre del package y el nombre completo de la actividad principal de la App. En nuestro caso, el paquete es `mds.pcg1` y la actividad es `OpenGLES30Activity`, así que la orden es:

```
adb shell am start mds.pcg1/mds.pcg1.OpenGLES30Activity
```

Esto pone en marcha la aplicación en el dispositivo de forma asíncrona, es decir, la orden `adb` termina pero la aplicación se queda ejecutando en el dispositivo.

ADB puede usarse para ver en el ordenador los mensajes producidos en el dispositivo (de todas las aplicaciones y el propio S.O.), usando la sub-orden `logcat`. Para conectar con el dispositivo y ver todos los mensajes, hacemos:

```
adb logcat
```

El dispositivo está continuamente emitiendo mensajes. Se ejecuta hasta que se pare con Ctrl-C. 

Nosotros estamos interesados en ver únicamente los mensajes que nuestra app genera, mediante las llamadas a la función `Log.v` (u otras variantes de `Log`). Para ello usamos la posibilidad de filtrar la salida de `logcat`. En primer lugar averiguamos el identificador único de la app (el _uid_ de la misma), hacemos:

```
adb shell dumpsys package mds.pcg1
``` 

En la salida debe aparecer una linea con `uid=N`, donde _N_ es un número identificador único que se ha asignado a la aplicación. Si nos cuesta encontrar esa línea, podemos filtrar la salida en el terminal de Linux con `grep`

```
adb shell dumpsys package mds.pcg1 | grep uid
```

A modo de ejemplo, esta última orden produce una salida como esta: 

```
uid=10294 gids=[] type=0 prot=signature
```

Hay que tener en cuenta que cada vez que se recompila la aplicación (y se vuelve a instalar el APK en el dispositivo, con `gradlew installDebug`), este número cambiará.

Una vez que conocemos ese número _N_, podemos filtrar la salida, con:

```
adb logcat --uid=N
```

Si queremos ver únicamente la columna con el texto de cada mensaje emitido (de la columna 7 en adelante), podemos filtrar las columnas en la terminal de linux, con:

```
adb logcat --uid=N | cut -d' ' -f7-
```

### Compilación, instalación y ejecución desde la línea de comandos (Windows y macOS)

En estos sistemas operativos el uso de `gradlew` y `adb` es similar a lo indicado aquí para linux. 

En particular, para ordenador con _macOS_, las instrucciones probablemente sean exactamente las mismas (ya que se usa una terminal ejecutando la misma shell), excepto la instalación del compilador de Java y de ADB.

Para el caso de Windows, habrá que averiguar las instrucciones de instalación de Java y ADB y adaptar las órdenes a las particularidades de PowerShell o el _Command prompt_, pero las tareas de _gradlew_ y la forma de usar ADB deben ser exactamente iguales.






















