# PCG-ARS
# Script Powershell que compila los archivos .ts y lanza un servidor web  

$dir_src_ts    = "../src-ts"
$dir_js        = "../public_html/js"
$opciones      = "-strict", "-noEmitOnError", "-target", "ES2022", "--outDir", $dir_js
$archs_ts      = Get-ChildItem -Path $dir_src_ts -Filter "*.ts"
$archs_ts_path = $archs_ts | ForEach-Object { "$dir_src_ts/$_" }

tsc @opciones @archs_ts_path

## si no ha habido errores, lanzar servidor, si ha habido, no hacer nada

if ( $LASTEXITCODE -ne 0 ) {
   echo "Han habido errores en la compilacion"
}
else 
{
   echo "La compilacion ha ido bien"
   echo "La URL de la pagina es: http://localhost:8000 - o bien: http://127.0.0.1:8000"
   Pushd ../public_html 
   try {
      python3 -m http.server
   }
   finally {
      Popd
   }
}