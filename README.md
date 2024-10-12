# Repositorio para las asignaturas PCG y ARS del MDS

## Cosas pendientes 

En la aplicación Web, cuando se activa el cauce de visualización, está activado el uso de sombras arrojadas, pero:

  - No se puede activar o desactivar, hay un booleano que lo controla, pero no se puede cambiar en el interfaz ni el shader lo lee. Hay que incializalo a un valor.
  - Antes de visualizar el frame con la escena con sombras, creo que no se está haciendo el binding de la textura de profundidad, por lo que a veces la textura de color está binded en el slot de la de profundidad. Probarlo con GrafoTest2, la que tiene las cuadricas con texturas. En otros objetos funciona de casualidad.
  - Estudiar los mip maps de las texturas en ese GrafoTest2, las texturas salen pixeladas. Creo que la generación de mipmaps no está activada en texturas que no sean potencia de 2 (o algo así).
