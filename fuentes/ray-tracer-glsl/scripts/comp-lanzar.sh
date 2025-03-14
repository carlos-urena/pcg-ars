#/bin/bash -f   

dir_src_ts="../src-ts"
dir_js="../public_html/js"
opciones="-v -strict -noEmitOnError -target ES2022 --outDir ${dir_js}"
archs_ts=$(ls ${dir_src_ts}/*.ts | xargs ) ## xargs hace que aparezcan todos en una línea
 

## compilar y obtener resultado en 'res'
echo "Va TSC ....opciones == [${opciones}], fuentes:"
ls -la $archs_ts
tsc -v $opciones $archs_ts
res=$?
echo "TSC terminado, resultado == " $res ", generados:" 
ls -la ../public_html/js
echo "----"

## si no ha habido errores, lanzar servidor
if [ "$res" == "0" ] ; then 
   echo "Lanzando servidor, página raiz en 'localhost:8000' (o 'http://localhost:8000')" 
   pushd ../public_html >/dev/null
   python3 -m http.server
   popd >/dev/null
else
   echo "Errores: no se lanza servidor."
fi
