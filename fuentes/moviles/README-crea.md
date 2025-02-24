# Creación del proyecto desde cero.


Lanzo Android Studio y en la pantalla inicial selecciono _New Project_

Hay que elegir el tipo de proyecto, el lenguaje, la carpeta, etc... todo lo que se describe aquí son pruebas:


**Tipo de proyecto**: Empty Activity

**Nombre**: "App Android PCG - v1"

**Package name**:

Para el _package name_, tener en cuenta esto sobre dicho nombre: 

_The package name of an Android app uniquely identifies your app on the device, in Google Play Store, and in supported third-party Android stores_ (de: [Google Help](https://support.google.com/admob/answer/9972781))

así que elijo **mds.pcg1**


**Save location**:  elijo una carpeta dentro del repositorio de la asignatura, en concreto eligo dentro de `App_Android_PCG_v1.proyecto_AS` (dentro de `proyecto-as` )

**Minimun SDK**: API 24 ("Nougat"; Android 7)

Espero a que se descargue todo, me ha creado una aplicación con un archivo principal en Kotlin (`MainActivity.kt`), con una declaracion de la clase `MainActivity`


``` 

``` 

Lo ha creado todo dentro de la `AppPCGMoviles` (nombre del proy, sin espacios), tiene 724k en este momento. Me salgo del editor. Veo que AS ha añadido cosas al repo del github, no todas, yo añado todo lo que faltaba.

Para volver a ejecutar AS hay que volver a lanzar el script `studio.sh`. Se puede usar un alias de linux para hacer esto más fácilmente.
Una vez que se lanza AS, abrirá el último proyecto antes de cerrar. Se puede cerrar un proyecto y luego abrir otro cualquiera. Para abrir un proyecto, usar _Open_ y seleccionar la carpeta que lo contiene, en este caso la carpeta `App_Android_PCG_v1.proyecto_AS`.


Seguir lo que se dice aquí:

[Vistas OpenGL](https://developer.android.com/develop/ui/views/graphics/opengl)

Sobre comprobar la versión, mirar:

[Vistas OpenGL: comprobación de versiones](https://developer.android.com/develop/ui/views/graphics/opengl/about-opengl#version-check)



Ejecuto _Run_ en AS. Va bien.


## Conectar un dispositivo.

Para conectar un dispositivo con USB y ejecutar la App en dicho dispositivo, seguir estas instrucciones:

[Como ejecutar apps en un dispositivo hardware](https://developer.android.com/studio/run/device?hl=es-419)

## Añadir los shaders a los assets 

Los fuentes de los shaders se guardan en el _Assets folder_, lo he creado en la carpeta `app`,

    - En AS, pincho con el botón derecho en esa carpeta y selecciono _New -> Folder -> Assets Folder_
    - Dentro de ese folder, he creado el folder `shaders`, y dentro de ese creo los archivos con extensión `.glsl` (con _New --> File_)
 

## Plugin de shaders GLSL

Hay un plugin de AS para fuentes de shaders, se llama GLSL, lo instalo. Va bien.















