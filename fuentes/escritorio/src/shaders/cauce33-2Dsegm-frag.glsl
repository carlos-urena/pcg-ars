#version 330 core

// fragment shader super sencillo para el contorno

in vec4 v2_color ;        // color del vértice 
layout(location = 0) out vec4 out_color_fragmento ; // color que se calcula como resultado final de este shader en 'main'

void main()
{
  out_color_fragmento = v2_color ; // el color del pixel es el resultado de evaluar iluminación
}
