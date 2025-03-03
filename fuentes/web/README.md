# PCG: Instalación, compilación y ejecución de la aplicación Web

Prerequisitos e instrucciones de compilación y ejecución para la aplicación Web

## Prerequisitos: 

+ Intérprete de *python3* para lanzar el servidor
+ Compilador de *typescript* para compilar fuentes


## Instalación del compilador de _Typescript_

### Linux

El compilador de _typescript_ se puede instalar con el gestor de paquetes de _Node Package Manager_ (NPM: [npmjs.com](https://www.npmjs.com/)). Para instalar NPM, se puede instalar antes la utilidad _Node Version Manager_ (NVM).

Se puede instalar NVM con `wget` según se dice en el repo: https://github.com/nvm-sh/nvm#installing-and-updating. Para comprobar la instalación, podemos ejecutar la orden 

```
nvm -v
```

y así verificar que tenemos instalado `nvm` y saber su versión. Una vez esta herramienta está disponible, la usamos para instalar NPM (_node package manager_), con la orden:

```
nvm install 18
```

Podemos comprobar que la instalación ha ido bien ejecutando 

```
npm -v
```

Una vez que sabemos que está disponible la orden `npm` , la usamos para instalar el paquete que tiene el compilador de _typescript_ ([www.npmjs.com/package/typescript](https://www.npmjs.com/package/typescript)), así: 

```
npm install -g typescript
```



### macOS

Es necesario instalar _Node_, se puede hacer con el gestor de paquetes _Brew_ ([brew.sh](https://brew.sh)), con la orden: 

```
brew install node
```

o bien descargarlo e instalarlo: [nodejs.org/en/download](https://nodejs.org/en/download). Una vez instalado, se dispone de la orden `npm`, la usamos para instalar el compilador de _typescript_, así:

```
npm install -g typescript
```

### Windows

En Windows usaremos siempre el terminal de tipo _Developer Powershell_. Hay que dar estos pasos:

Descargar e instalar _Node_ de aquí: 

[nodejs.org/en/download](https://nodejs.org/en/download)

Instalar el compilador 

```
npm install -g typescript 
``` 

Con esto, si no ha habido errores, estará disponible la orden `tsc`, se puede verificar poniendo:

`tsc --version`

## Ejecución de la aplicación en un navegador:

### Linux o macOS

Ejecutar el script `comp-lanzar.sh` que hay en la carpeta `scripts` (estando en ella), es decir:

``` 
cd scripts
./comp-lanzar.sh
``` 

Esto compila los fuentes Typescript (que necesiten recompilarse), y genera los fuentes Javascript. Si no ha habido errores al compilar, lanza un servidor Web local que hace accesible la aplicación Web desde cualquier navegador que se esté ejecutando en el mismo equipo. Se debe usar una de estas dos URLs:

   - `http://localhost:8000`
   - `http://0.0.0.0:8000` 

### Windows

Ir a la carpeta `scripts` y ejecutar el script en el archivo `comp-lanzar.ps1` :

```
cd scripts 
./comp-lanzar.ps1 
```

La primera vez habrá que dar permisos al interprete de Python para que use la red (aunque sea localmente) para recibir peticiones y devolver la respuesta. 

Para probar la aplicación, se debe ejecutar cualquier navegador moderno en el mismo equipo en el que se ha lanzado el servidor, se debe usar la URL 

  - `http://localhost:8000` 