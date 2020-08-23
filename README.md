# BDSAG
## Base de Datos de Sesion Acelerada por GPU

Base de datos REST en RAM acelerada por GPU especializada en la creacion y gestion de sesiones

###### config.json

Archivo de configuaracion de la base de datos

| Clave | Obligatorio | Tipo | Valores | Description |
| --- | --- | --- | --- | --- |
| sizeDB | - [x] | Long | Mayores que 0 | Usuarios reservados en la base de datos |
| sizeSesion | - [x] | Long | Mayores que 0 | Numero de caracteres de la sesion |
| port | - [x] | Long | Entre 0 y 65535 | Puerto del servidor REST |
| logs | - [x] | Enum | NONE, FATAL, ERROR, WARN, INFO, DEBUG, TRACE, ALL | Visibilidad de los logs |

###### POST [HOST]/database/session

Crea una sesion

- Entrada
  - Header

    | Clave | Obligatorio | Tipo | Valores | Description |
    | --- | --- | --- | --- | --- |
    | User-Agent | - [x] | String |  | User-Agent del navegador del usuario usado para aumentar la seguridad |

  - Body

    | Clave | Obligatorio | Tipo | Valores | Description |
    | --- | --- | --- | --- | --- |
    | expiretime | - [x] | Long | Mayores que 0 | Tiempo en segundos para que caduque la sesion |
    | value | - [x] | String |  | Valor que se le asigna a la sesion |

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
    | User-Agent | - [x] | String |  | User-Agent del navegador del usuario usado para aumentar la seguridad |

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
* IDE -> Code::Blocks
* Sistema Opertivo -> Windows
* Dependencias -> RapidJSON, microhttpd, OpenCL