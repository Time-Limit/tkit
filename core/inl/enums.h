#ifndef _ENUMS_H_
#define _ENUMS_H_

#include "exptype.h"
#include <map>

enum HTTP_STATUS_CODE
{
	HTTP_SC_CONTINUE			=	100,	//	¼ÌÐø¡£¿Í»§¶ËÓ¦¼ÌÐøÆäÇëÇó
	HTTP_SC_SWITCHING_PROTOCOLS		=	101,	//	ÇÐ»»Ð­Òé¡£·þÎñÆ÷¸ù¾Ý¿Í»§¶ËµÄÇëÇóÇÐ»»Ð­Òé¡£Ö»ÄÜÇÐ»»µ½¸ü¸ß¼¶µÄÐ­Òé£¬ÀýÈç£¬ÇÐ»»µ½HTTPµÄÐÂ°æ±¾Ð­Òé
	HTTP_SC_OK				=	200,	//	ÇëÇó³É¹¦¡£Ò»°ãÓÃÓÚGETÓëPOSTÇëÇó
	HTTP_SC_CREATED				=	201,	//	ÒÑ´´½¨¡£³É¹¦ÇëÇó²¢´´½¨ÁËÐÂµÄ×ÊÔ´
	HTTP_SC_ACCEPTED			=	202,	//	ÒÑ½ÓÊÜ¡£ÒÑ¾­½ÓÊÜÇëÇó£¬µ«Î´´¦ÀíÍê³É
	HTTP_SC_NON_AUTHORITATIVE_INFORMATION	=	203,	//	·ÇÊÚÈ¨ÐÅÏ¢¡£ÇëÇó³É¹¦¡£µ«·µ»ØµÄmetaÐÅÏ¢²»ÔÚÔ­Ê¼µÄ·þÎñÆ÷£¬¶øÊÇÒ»¸ö¸±±¾
	HTTP_SC_NO_CONTENT			=	204,	//	ÎÞÄÚÈÝ¡£·þÎñÆ÷³É¹¦´¦Àí£¬µ«Î´·µ»ØÄÚÈÝ¡£ÔÚÎ´¸üÐÂÍøÒ³µÄÇé¿öÏÂ£¬¿ÉÈ·±£ä¯ÀÀÆ÷¼ÌÐøÏÔÊ¾µ±Ç°ÎÄµµ
	HTTP_SC_RESET_CONTENT			=	205,	//	ÖØÖÃÄÚÈÝ¡£·þÎñÆ÷´¦Àí³É¹¦£¬ÓÃ»§ÖÕ¶Ë£¨ÀýÈç£ºä¯ÀÀÆ÷£©Ó¦ÖØÖÃÎÄµµÊÓÍ¼¡£¿ÉÍ¨¹ý´Ë·µ»ØÂëÇå³ýä¯ÀÀÆ÷µÄ±íµ¥Óò
	HTTP_SC_PARTIAL_CONTENT			=	206,	//	²¿·ÖÄÚÈÝ¡£·þÎñÆ÷³É¹¦´¦ÀíÁË²¿·ÖGETÇëÇó
	HTTP_SC_MULTIPLE_CHOICES		=	300,	//	¶àÖÖÑ¡Ôñ¡£ÇëÇóµÄ×ÊÔ´¿É°üÀ¨¶à¸öÎ»ÖÃ£¬ÏàÓ¦¿É·µ»ØÒ»¸ö×ÊÔ´ÌØÕ÷ÓëµØÖ·µÄÁÐ±íÓÃÓÚÓÃ»§ÖÕ¶Ë£¨ÀýÈç£ºä¯ÀÀÆ÷£©Ñ¡Ôñ
	HTTP_SC_MOVED_PERMANENTLY		=	301,	//	ÓÀ¾ÃÒÆ¶¯¡£ÇëÇóµÄ×ÊÔ´ÒÑ±»ÓÀ¾ÃµÄÒÆ¶¯µ½ÐÂURI£¬·µ»ØÐÅÏ¢»á°üÀ¨ÐÂµÄURI£¬ä¯ÀÀÆ÷»á×Ô¶¯¶¨Ïòµ½ÐÂURI¡£½ñºóÈÎºÎÐÂµÄÇëÇó¶¼Ó¦Ê¹ÓÃÐÂµÄURI´úÌæ
	HTTP_SC_FOUND				=	302,	//	ÁÙÊ±ÒÆ¶¯¡£Óë301ÀàËÆ¡£µ«×ÊÔ´Ö»ÊÇÁÙÊ±±»ÒÆ¶¯¡£¿Í»§¶ËÓ¦¼ÌÐøÊ¹ÓÃÔ­ÓÐURI
	HTTP_SC_SEE_OTHER			=	303,	//	²é¿´ÆäËüµØÖ·¡£Óë301ÀàËÆ¡£Ê¹ÓÃGETºÍPOSTÇëÇó²é¿´
	HTTP_SC_NOT_MODIFIED			=	304,	//	Î´ÐÞ¸Ä¡£ËùÇëÇóµÄ×ÊÔ´Î´ÐÞ¸Ä£¬·þÎñÆ÷·µ»Ø´Ë×´Ì¬ÂëÊ±£¬²»»á·µ»ØÈÎºÎ×ÊÔ´¡£¿Í»§¶ËÍ¨³£»á»º´æ·ÃÎÊ¹ýµÄ×ÊÔ´£¬Í¨¹ýÌá¹©Ò»¸öÍ·ÐÅÏ¢Ö¸³ö¿Í»§¶ËÏ£ÍûÖ»·µ»ØÔÚÖ¸¶¨ÈÕÆÚÖ®ºóÐÞ¸ÄµÄ×ÊÔ´
	HTTP_SC_USE_PROXY			=	305,	//	Ê¹ÓÃ´úÀí¡£ËùÇëÇóµÄ×ÊÔ´±ØÐëÍ¨¹ý´úÀí·ÃÎÊ
	HTTP_SC_UNUSED				=	306,	//	ÒÑ¾­±»·ÏÆúµÄHTTP×´Ì¬Âë
	HTTP_SC_TEMPORARY_REDIRECT		=	307,	//	ÁÙÊ±ÖØ¶¨Ïò¡£Óë302ÀàËÆ¡£Ê¹ÓÃGETÇëÇóÖØ¶¨Ïò
	HTTP_SC_BAD_REQUEST			=	400,	//	¿Í»§¶ËÇëÇóµÄÓï·¨´íÎó£¬·þÎñÆ÷ÎÞ·¨Àí½â
	HTTP_SC_UNAUTHORIZED			=	401,	//	ÇëÇóÒªÇóÓÃ»§µÄÉí·ÝÈÏÖ¤
	HTTP_SC_PAYMENT_REQUIRED		=	402,	//	±£Áô£¬½«À´Ê¹ÓÃ
	HTTP_SC_FORBIDDEN			=	403,	//	·þÎñÆ÷Àí½âÇëÇó¿Í»§¶ËµÄÇëÇó£¬µ«ÊÇ¾Ü¾øÖ´ÐÐ´ËÇëÇó
	HTTP_SC_NOT_FOUND			=	404,	//	·þÎñÆ÷ÎÞ·¨¸ù¾Ý¿Í»§¶ËµÄÇëÇóÕÒµ½×ÊÔ´£¨ÍøÒ³£©¡£Í¨¹ý´Ë´úÂë£¬ÍøÕ¾Éè¼ÆÈËÔ±¿ÉÉèÖÃ"ÄúËùÇëÇóµÄ×ÊÔ´ÎÞ·¨ÕÒµ½"µÄ¸öÐÔÒ³Ãæ
	HTTP_SC_METHOD_NOT_ALLOWED		=	405,	//	¿Í»§¶ËÇëÇóÖÐµÄ·½·¨±»½ûÖ¹
	HTTP_SC_NOT_ACCEPTABLE			=	406,	//	·þÎñÆ÷ÎÞ·¨¸ù¾Ý¿Í»§¶ËÇëÇóµÄÄÚÈÝÌØÐÔÍê³ÉÇëÇó
	HTTP_SC_PROXY_AUTHENTICATION_REQUIRED	=	407,	//	ÇëÇóÒªÇó´úÀíµÄÉí·ÝÈÏÖ¤£¬Óë401ÀàËÆ£¬µ«ÇëÇóÕßÓ¦µ±Ê¹ÓÃ´úÀí½øÐÐÊÚÈ¨
	HTTP_SC_REQUEST_TIME_OUT		=	408,	//	·þÎñÆ÷µÈ´ý¿Í»§¶Ë·¢ËÍµÄÇëÇóÊ±¼ä¹ý³¤£¬³¬Ê±
	HTTP_SC_CONFLICT			=	409,	//	·þÎñÆ÷Íê³É¿Í»§¶ËµÄPUTÇëÇóÊÇ¿ÉÄÜ·µ»Ø´Ë´úÂë£¬·þÎñÆ÷´¦ÀíÇëÇóÊ±·¢ÉúÁË³åÍ»
	HTTP_SC_GONE				=	410,	//	¿Í»§¶ËÇëÇóµÄ×ÊÔ´ÒÑ¾­²»´æÔÚ¡£410²»Í¬ÓÚ404£¬Èç¹û×ÊÔ´ÒÔÇ°ÓÐÏÖÔÚ±»ÓÀ¾ÃÉ¾³ýÁË¿ÉÊ¹ÓÃ410´úÂë£¬ÍøÕ¾Éè¼ÆÈËÔ±¿ÉÍ¨¹ý301´úÂëÖ¸¶¨×ÊÔ´µÄÐÂÎ»ÖÃ
	HTTP_SC_LENGTH_REQUIRED			=	411,	//	·þÎñÆ÷ÎÞ·¨´¦Àí¿Í»§¶Ë·¢ËÍµÄ²»´øContent-LengthµÄÇëÇóÐÅÏ¢
	HTTP_SC_PRECONDITION_FAILED		=	412,	//	¿Í»§¶ËÇëÇóÐÅÏ¢µÄÏÈ¾öÌõ¼þ´íÎó
	HTTP_SC_REQUEST_ENTITY_TOO_LARGE	=	413,	//	ÓÉÓÚÇëÇóµÄÊµÌå¹ý´ó£¬·þÎñÆ÷ÎÞ·¨´¦Àí£¬Òò´Ë¾Ü¾øÇëÇó¡£Îª·ÀÖ¹¿Í»§¶ËµÄÁ¬ÐøÇëÇó£¬·þÎñÆ÷¿ÉÄÜ»á¹Ø±ÕÁ¬½Ó¡£Èç¹ûÖ»ÊÇ·þÎñÆ÷ÔÝÊ±ÎÞ·¨´¦Àí£¬Ôò»á°üº¬Ò»¸öRetry-AfterµÄÏìÓ¦ÐÅÏ¢
	HTTP_SC_REQUEST_URI_TOO_LARGE		=	414,	//	ÇëÇóµÄURI¹ý³¤£¨URIÍ¨³£ÎªÍøÖ·£©£¬·þÎñÆ÷ÎÞ·¨´¦Àí
	HTTP_SC_UNSUPPORTED_MEDIA_TYPE		=	415,	//	·þÎñÆ÷ÎÞ·¨´¦ÀíÇëÇó¸½´øµÄÃ½Ìå¸ñÊ½
	HTTP_SC_REQUESTED_RANGE_NOT_SATISFIABLE	=	416,	//	¿Í»§¶ËÇëÇóµÄ·¶Î§ÎÞÐ§
	HTTP_SC_EXPECTATION_FAILED		=	417,	//	·þÎñÆ÷ÎÞ·¨Âú×ãExpectµÄÇëÇóÍ·ÐÅÏ¢
	HTTP_SC_INTERNAL_SERVER_ERROR		=	500,	//	·þÎñÆ÷ÄÚ²¿´íÎó£¬ÎÞ·¨Íê³ÉÇëÇó
	HTTP_SC_NOT_IMPLEMENTED			=	501,	//	·þÎñÆ÷²»Ö§³ÖÇëÇóµÄ¹¦ÄÜ£¬ÎÞ·¨Íê³ÉÇëÇó
	HTTP_SC_BAD_GATEWAY			=	502,	//	³äµ±Íø¹Ø»ò´úÀíµÄ·þÎñÆ÷£¬´ÓÔ¶¶Ë·þÎñÆ÷½ÓÊÕµ½ÁËÒ»¸öÎÞÐ§µÄÇëÇó
	HTTP_SC_SERVICE_UNAVAILABLE		=	503,	//	ÓÉÓÚ³¬ÔØ»òÏµÍ³Î¬»¤£¬·þÎñÆ÷ÔÝÊ±µÄÎÞ·¨´¦Àí¿Í»§¶ËµÄÇëÇó¡£ÑÓÊ±µÄ³¤¶È¿É°üº¬ÔÚ·þÎñÆ÷µÄRetry-AfterÍ·ÐÅÏ¢ÖÐ
	HTTP_SC_GATEWAY_TIME_OUT		=	504,	//	³äµ±Íø¹Ø»ò´úÀíµÄ·þÎñÆ÷£¬Î´¼°Ê±´ÓÔ¶¶Ë·þÎñÆ÷»ñÈ¡ÇëÇó
	HTTP_SC_HTTP_VERSION_NOT_SUPPORTED	=	505,	//	·þÎñÆ÷²»Ö§³ÖÇëÇóµÄHTTPÐ­ÒéµÄ°æ±¾£¬ÎÞ·¨Íê³É´¦Àí
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
