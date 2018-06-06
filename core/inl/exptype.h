#ifndef _EXPTYPE_H_
#define _EXPTYPE_H_

#define _min(a, b) ((a) < (b) ? (a) : (b))
#define _max(a, b) ((a) > (b) ? (a) : (b))
#define LISTEN_QUEUE_SIZE 10

#include <string>

//BASE
typedef int channel_id_t;
typedef int session_id_t;
typedef unsigned int flag_t;

//HTTP
typedef unsigned short status_code_t;
typedef std::string suffix_name_t;
typedef std::string mime_type_t;

#endif
