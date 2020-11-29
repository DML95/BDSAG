# BDSAG
## Base de Datos de Sesion Acelerada por GPU

Base de datos REST en RAM acelerada por GPU especializada en la creacion y gestion de sesiones

###### config.json

Archivo de configuaracion de la base de datos

| Clave | Obligatorio | Tipo | Valores | Description |
| --- | --- | --- | --- | --- |
| sizeDB | :white_check_mark: | Long | Mayores que 0 | Usuarios reservados en la base de datos |
| sizeSesion | :white_check_mark: | Long | Mayores que 0 | Numero de caracteres de la sesion |
| port | :white_check_mark: | Long | Entre 0 y 65535 | Puerto del servidor REST |
| logs | :white_check_mark: | Enum | NONE, FATAL, ERROR, WARN, INFO, DEBUG, TRACE, ALL | Visibilidad de los logs |

###### GET [HOST]/database/config

Devuelve el contenido del archivo de configuracion con el que se inicio la base de datos

- Salida
  - 200 OK
    - Body
	
	  Contenido del archivo de config.json

###### POST [HOST]/database/session

Crea una sesion

- Entrada
  - Header

    | Clave | Obligatorio | Tipo | Valores | Description |
    | --- | --- | --- | --- | --- |
    | User-Agent |  :white_check_mark: | String |  | User-Agent del navegador del usuario usado para aumentar la seguridad |

  - Body

    | Clave | Obligatorio | Tipo | Valores | Description |
    | --- | --- | --- | --- | --- |
    | expiretime | :white_check_mark: | Long | Mayores que 0 | Tiempo en segundos para que caduque la sesion |
    | value | :white_check_mark: | String |  | Valor que se le asigna a la sesion |

- Salida
  - 201 Created
    - Body
      
	  | Clave | Tipo | Valores | Description |
	  | --- | --- | --- | --- |
      | id | String |  | Identificador de la sesion |
      | expireepoch | String |  | Tiempo Epoch en segundos en el que expira la sesion |
	  
  - 507 Insufficient Storage

###### GET [HOST]/database/session/{id}

Busca una sesion por un identificador

- Entrada
  - Path
  
    | Clave | Tipo | Valores | Description |
    | --- | --- | --- | --- |
    | id | String |  | Identificador de la sesion |
  
  - Header

    | Clave | Obligatorio | Tipo | Valores | Description |
    | --- | --- | --- | --- | --- |
    | User-Agent | :white_check_mark: | String |  | User-Agent del navegador del usuario usado para aumentar la seguridad |

- Salida
  - 200 OK
    - Body
	
      | Clave | Tipo | Valores | Description |
	  | --- | --- | --- | --- |
      | value | String |  | Valor de la sesion |
      | expireepoch | String |  | Tiempo Epoch en segundos en el que expira la sesion |
  
  - 404 Not Found
  
###### PATCH [HOST]/database/session/{id}

Modifica una sesion

- Entrada
  - Header

    | Clave | Obligatorio | Tipo | Valores | Description |
    | --- | --- | --- | --- | --- |
    | User-Agent |  :white_check_mark: | String |  | User-Agent del navegador del usuario usado para aumentar la seguridad |

  - Body

    | Clave | Obligatorio | Tipo | Valores | Description |
    | --- | --- | --- | --- | --- |
    | expiretime |  | Long | Mayores que 0 | Tiempo en segundos para que caduque la sesion |
    | value |  | String |  | Valor que se le asigna a la sesion |

- Salida
  - 200 OK
    - Body
      
	  | Clave | Tipo | Valores | Description |
	  | --- | --- | --- | --- |
      | value | String |  | Valor de la sesion |
      | expireepoch | String |  | Tiempo Epoch en segundos en el que expira la sesion |
	  
  - 404 Not Found

###### DELETE [HOST]/database/session/{id}

Elimina una sesion por un identificador

- Entrada
  - Path
  
    | Clave | Tipo | Valores | Description |
    | --- | --- | --- | --- |
    | id | String |  | Identificador de la sesion |
  
  - Header

    | Clave | Obligatorio | Tipo | Valores | Description |
    | --- | --- | --- | --- | --- |
    | User-Agent | :white_check_mark: | String |  | User-Agent del navegador del usuario usado para aumentar la seguridad |

- Salida
  - 200 OK
    - Body
	
      | Clave | Tipo | Valores | Description |
	  | --- | --- | --- | --- |
      | value | String |  | Valor de la sesion |
      | expireepoch | String |  | Tiempo Epoch en segundos en el que expira la sesion |
  
  - 404 Not Found

###### Entorno de desarrollo

* Copilador -> GCC (mingw-w64)
* IDE -> Eclipse CDT
* Sistema Opertivo -> Windows
* Dependencias -> RapidJSON, microhttpd, OpenCL

###### Instalacion y ejecuación

1. Abrir la consola de comandos
2. Situarse en la carpeta del proyecto
3. Ejecutar el comando **make**
4. Ejecutar el archivo **bin/BDSAG**
