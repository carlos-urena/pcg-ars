# Cosas pendientes de los fuentes de PCG y ARS 

## Aplicación Web

- Estudiar los mip maps de las texturas en ese GrafoTest2, las texturas salen pixeladas. Creo que la generación de mipmaps no está activada en texturas que no sean potencia de 2 (o algo así).

- En el objeto compuesto, al añadir un objeto, se le cambia a ese objeto su matriz de transformación, y esto no es lógico, ya que el objeto se modifica según donde se ponga en el grafo de escena. Habría que añadir una matriz de instanciación por cada hijo, pero en el objeto compuesto, no en el hijo (usando un array de matrices).
