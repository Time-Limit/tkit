#ifndef _ENUMS_H_
#define _ENUMS_H_

#include "exptype.h"
#include <map>

enum HTTP_STATUS_CODE
{
	HTTP_SC_CONTINUE			=	100,
	HTTP_SC_SWITCHING_PROTOCOLS		=	101,
	HTTP_SC_OK				=	200,
	HTTP_SC_CREATED				=	201,
	HTTP_SC_ACCEPTED			=	202,
	HTTP_SC_NON_AUTHORITATIVE_INFORMATION	=	203,
	HTTP_SC_NO_CONTENT			=	204,
	HTTP_SC_RESET_CONTENT			=	205,
	HTTP_SC_PARTIAL_CONTENT			=	206,
	HTTP_SC_MULTIPLE_CHOICES		=	300,
	HTTP_SC_MOVED_PERMANENTLY		=	301,
	HTTP_SC_FOUND				=	302,
	HTTP_SC_SEE_OTHER			=	303,
	HTTP_SC_NOT_MODIFIED			=	304,
	HTTP_SC_USE_PROXY			=	305,
	HTTP_SC_UNUSED				=	306,
	HTTP_SC_TEMPORARY_REDIRECT		=	307,
	HTTP_SC_BAD_REQUEST			=	400,
	HTTP_SC_UNAUTHORIZED			=	401,
	HTTP_SC_PAYMENT_REQUIRED		=	402,
	HTTP_SC_FORBIDDEN			=	403,
	HTTP_SC_NOT_FOUND			=	404,
	HTTP_SC_METHOD_NOT_ALLOWED		=	405,
	HTTP_SC_NOT_ACCEPTABLE			=	406,
	HTTP_SC_PROXY_AUTHENTICATION_REQUIRED	=	407,
	HTTP_SC_REQUEST_TIME_OUT		=	408,
	HTTP_SC_CONFLICT			=	409,
	HTTP_SC_GONE				=	410,
	HTTP_SC_LENGTH_REQUIRED			=	411,
	HTTP_SC_PRECONDITION_FAILED		=	412,
	HTTP_SC_REQUEST_ENTITY_TOO_LARGE	=	413,
	HTTP_SC_REQUEST_URI_TOO_LARGE		=	414,
	HTTP_SC_UNSUPPORTED_MEDIA_TYPE		=	415,
	HTTP_SC_REQUESTED_RANGE_NOT_SATISFIABLE	=	416,
	HTTP_SC_EXPECTATION_FAILED		=	417,	
	HTTP_SC_INTERNAL_SERVER_ERROR		=	500,	
	HTTP_SC_NOT_IMPLEMENTED			=	501,	
	HTTP_SC_BAD_GATEWAY			=	502,	
	HTTP_SC_SERVICE_UNAVAILABLE		=	503,	
	HTTP_SC_GATEWAY_TIME_OUT		=	504,	
	HTTP_SC_HTTP_VERSION_NOT_SUPPORTED	=	505,
};

std::string GetStatusCodeInfo(HTTP_STATUS_CODE reason);

extern std::map<suffix_name_t, mime_type_t> HTTP_MIME_MAP;

const mime_type_t& GetMimeType(const suffix_name_t&);

// HTTP-HEADER
extern std::string HTTP_ACCEPT ;
extern std::string HTTP_ACCEPT_CHARSET ;
extern std::string HTTP_ACCEPT_DATETIME ;
extern std::string HTTP_ACCEPT_ENCODING ;
extern std::string HTTP_ACCEPT_LANGUAGE ;
extern std::string HTTP_ACCEPT_PATCH ;
extern std::string HTTP_ACCEPT_RANGES ;
extern std::string HTTP_ACCESS_CONTROL_ALLOW_ORIGIN ;
extern std::string HTTP_AGE ;
extern std::string HTTP_ALLOW ;
extern std::string HTTP_AUTHORIZATION ;
extern std::string HTTP_CACHE_CONTROL ;
extern std::string HTTP_CONNECTION ;
extern std::string HTTP_CONTENT_DISPOSITION ;
extern std::string HTTP_CONTENT_ENCODING ;
extern std::string HTTP_CONTENT_LANGUAGE ;
extern std::string HTTP_CONTENT_LENGTH ;
extern std::string HTTP_CONTENT_LOCATION ;
extern std::string HTTP_CONTENT_MD5 ;
extern std::string HTTP_CONTENT_RANGE ;
extern std::string HTTP_CONTENT_TYPE ;
extern std::string HTTP_COOKIE ;
extern std::string HTTP_DATE ;
extern std::string HTTP_ETAG ;
extern std::string HTTP_EXPECT ;
extern std::string HTTP_EXPIRES ;
extern std::string HTTP_FROM ;
extern std::string HTTP_HOST ;
extern std::string HTTP_IF_MATCH ;
extern std::string HTTP_IF_MODIFIED_SINCE ;
extern std::string HTTP_IF_NONE_MATCH ;
extern std::string HTTP_IF_RANGE ;
extern std::string HTTP_IF_UNMODIFIED_SINCE ;
extern std::string HTTP_LAST_MODIFIED ;
extern std::string HTTP_LINK ;
extern std::string HTTP_LOCATION ;
extern std::string HTTP_MAX_FORWARDS ;
extern std::string HTTP_ORIGIN ;
extern std::string HTTP_P3P ;
extern std::string HTTP_PRAGMA ;
extern std::string HTTP_PROXY_AUTHENTICATE ;
extern std::string HTTP_PROXY_AUTHORIZATION ;
extern std::string HTTP_PUBLIC_KEY_PINS ;
extern std::string HTTP_RANGE ;
extern std::string HTTP_REFERER ;
extern std::string HTTP_REFRESH ;
extern std::string HTTP_RETRY_AFTER ;
extern std::string HTTP_SERVER ;
extern std::string HTTP_SET_COOKIE ;
extern std::string HTTP_STATUS ;
extern std::string HTTP_STRICT_TRANSPORT_SECURITY ;
extern std::string HTTP_TE ;
extern std::string HTTP_TRAILER ;
extern std::string HTTP_TRANSFER_ENCODING ;
extern std::string HTTP_UPGRADE ;
extern std::string HTTP_USER_AGENT ;
extern std::string HTTP_VARY ;
extern std::string HTTP_VIA ;
extern std::string HTTP_WARNING ;
extern std::string HTTP_WWW_AUTHENTICATE ;

#endif
