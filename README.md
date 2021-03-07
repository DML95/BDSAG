# BDSAG
## Base de Datos de Sesion Acelerada por GPU

Base de datos REST en RAM acelerada por GPU especializada en la creacion y gestion de sesiones

###### config.lua

Script LUA de configuaracion de la base de datos

- Funciones obligatorias

  - main()

    Se ejecuta antes que cualquier otra funcion

  - getConfig()

    Devuelve la configuracion de la base de datos

    - Retorno

      | Posicion | Obligatorio | Tipo | Valores | Description |
      | --- | --- | --- | --- | --- |
      | 1 | :white_check_mark: | Long | Mayores que 0 | Usuarios reservados en la base de datos |
      | 2 | :white_check_mark: | Long | Mayores que 0 | Numero de caracteres de la sesion |
      | 3 | :white_check_mark: | Long | Entre 0 y 65535 | Puerto del servidor REST |
      | 4 | :white_check_mark: | Enum | NONE, FATAL, ERROR, WARN, INFO, DEBUG, TRACE, ALL | Visibilidad de los logs |

  - checkDeviceUse(platformName,deviceType,deviceName)

    Selecciona si el dispositivo puede que ser usado por la Base de datos

    - Parametros

      | Parametro | Tipo | Valores | Description |
	  | --- | --- | --- | --- |
	  | platformName | String |  | Nombre de la plataforma |
	  | deviceType | Enum | DEFAULT, CPU, GPU, ACCELERATOR, CUSTOM | Tipo de dipositivo |
	  | deviceName | String |  | Nombre del dispostivo |

    - Retorno

      | Posicion | Obligatorio | Tipo | Valores | Description |
      | --- | --- | --- | --- | --- |
      | 1 | :white_check_mark: | Boolean |  | Devualve si quiere que el dispositivo sea usado |

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

###### GET [HOST]/database/sessions/count

Cuanta el numero de sesiones creadas

- Entrada

- Salida
  - 200 OK
    - Body

      | Clave | Tipo | Valores | Description |
      | --- | --- | --- | --- |
      | count | Long |  | Numero de sesiones creadas |
      | maxcount | Long |  | Numero maximo de sesiones (sizeDB de config.json) |

###### Requistos

GPUs con OpenCL 1.2 o superior compatible con
   * cl_khr_global_int32_base_atomic
   * cl_khr_global_int32_extended_atomic
   * cl_khr_int64_base_atomics
   * cl_khr_int64_extended_atomics

###### Entorno de desarrollo

* Copilador ->  mingw-w64, GCC
* IDE -> Eclipse CDT
* Sistema Opertivo -> Windows, Linux
* Dependencias -> RapidJSON, microhttpd, OpenCL

###### Instalacion y ejecuación

1. Abrir la consola de comandos
2. Situarse en la carpeta del proyecto
3. Ejecutar el comando **make**
4. Ejecutar el archivo **bin/BDSAG**
