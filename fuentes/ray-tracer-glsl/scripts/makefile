.SUFFIXES:
.PHONY: all

## -----------------------------------------------------------------------------------
## Ray-tracer en un fragment shader con WebGL 
## Makefile para recompilar todos los fuentes 
## (si es necesario, si ya están compilados no hace nada)
##
## Última modificación: 23 de Marzo de 2026 (se añade este bloque de comentarios).
## -----------------------------------------------------------------------------------

dir_src_ts  := ../src-ts
dir_js      := ../public_html/js
opciones    := -strict -noEmitOnError -target ES2022 --outDir $(dir_js) 
archs_ts    := $(wildcard $(dir_src_ts)/*.ts)
archs_js    := $(addprefix $(dir_js)/, $(addsuffix .js, $(basename $(notdir $(archs_ts)))))

all: $(archs_js) 
	

$(archs_js): $(archs_ts) makefile
	tsc $(opciones) $(archs_ts) 

clean:
	cd $(dir_js) ; rm -rf *.js