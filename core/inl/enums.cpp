#include "enums.h"

std::string GetStatusCodeInfo(HTTP_STATUS_CODE reason)
{
	switch(reason)
	{
	case HTTP_SC_CONTINUE : return "CONTINUE"; break;
	case HTTP_SC_SWITCHING_PROTOCOLS : return "SWITCHING_PROTOCOLS"; break;
	case HTTP_SC_OK : return "OK"; break;
	case HTTP_SC_CREATED : return "CREATED"; break;
	case HTTP_SC_ACCEPTED : return "ACCEPTED"; break;
	case HTTP_SC_NON_AUTHORITATIVE_INFORMATION : return "NON_AUTHORITATIVE_INFORMATION"; break;
	case HTTP_SC_NO_CONTENT : return "NO_CONTENT"; break;
	case HTTP_SC_RESET_CONTENT : return "RESET_CONTENT"; break;
	case HTTP_SC_PARTIAL_CONTENT : return "PARTIAL_CONTENT"; break;
	case HTTP_SC_MULTIPLE_CHOICES : return "MULTIPLE_CHOICES"; break;
	case HTTP_SC_MOVED_PERMANENTLY : return "MOVED_PERMANENTLY"; break;
	case HTTP_SC_FOUND : return "FOUND"; break;
	case HTTP_SC_SEE_OTHER : return "SEE_OTHER"; break;
	case HTTP_SC_NOT_MODIFIED : return "NOT_MODIFIED"; break;
	case HTTP_SC_USE_PROXY : return "USE_PROXY"; break;
	case HTTP_SC_UNUSED : return "UNUSED"; break;
	case HTTP_SC_TEMPORARY_REDIRECT : return "TEMPORARY_REDIRECT"; break;
	case HTTP_SC_BAD_REQUEST : return "BAD_REQUEST"; break;
	case HTTP_SC_UNAUTHORIZED : return "UNAUTHORIZED"; break;
	case HTTP_SC_PAYMENT_REQUIRED : return "PAYMENT_REQUIRED"; break;
	case HTTP_SC_FORBIDDEN : return "FORBIDDEN"; break;
	case HTTP_SC_NOT_FOUND : return "NOT_FOUND"; break;
	case HTTP_SC_METHOD_NOT_ALLOWED : return "METHOD_NOT_ALLOWED"; break;
	case HTTP_SC_NOT_ACCEPTABLE : return "NOT_ACCEPTABLE"; break;
	case HTTP_SC_PROXY_AUTHENTICATION_REQUIRED : return "PROXY_AUTHENTICATION_REQUIRED"; break;
	case HTTP_SC_REQUEST_TIME_OUT : return "REQUEST_TIME_OUT"; break;
	case HTTP_SC_CONFLICT : return "CONFLICT"; break;
	case HTTP_SC_GONE : return "GONE"; break;
	case HTTP_SC_LENGTH_REQUIRED : return "LENGTH_REQUIRED"; break;
	case HTTP_SC_PRECONDITION_FAILED : return "PRECONDITION_FAILED"; break;
	case HTTP_SC_REQUEST_ENTITY_TOO_LARGE : return "REQUEST_ENTITY_TOO_LARGE"; break;
	case HTTP_SC_REQUEST_URI_TOO_LARGE : return "REQUEST_URI_TOO_LARGE"; break;
	case HTTP_SC_UNSUPPORTED_MEDIA_TYPE : return "UNSUPPORTED_MEDIA_TYPE"; break;
	case HTTP_SC_REQUESTED_RANGE_NOT_SATISFIABLE : return "REQUESTED_RANGE_NOT_SATISFIABLE"; break;
	case HTTP_SC_EXPECTATION_FAILED : return "EXPECTATION_FAILED"; break;
	case HTTP_SC_INTERNAL_SERVER_ERROR : return "INTERNAL_SERVER_ERROR"; break;
	case HTTP_SC_NOT_IMPLEMENTED : return "NOT_IMPLEMENTED"; break;
	case HTTP_SC_BAD_GATEWAY : return "BAD_GATEWAY"; break;
	case HTTP_SC_SERVICE_UNAVAILABLE : return "SERVICE_UNAVAILABLE"; break;
	case HTTP_SC_GATEWAY_TIME_OUT : return "GATEWAY_TIME_OUT"; break;
	case HTTP_SC_HTTP_VERSION_NOT_SUPPORTED : return "HTTP_VERSION_NOT_SUPPORTED"; break;
	}
	return "UNKOWN REASON";
}

std::map<suffix_name_t, mime_type_t> HTTP_MIME_MAP = 
{
	{"",	"application/octet-stream"},
	{"323",	"text/h323"},
	{"acx",	"application/internet-property-stream"},
	{"ai",	"application/postscript"},
	{"aif",	"audio/x-aiff"},
	{"aifc",	"audio/x-aiff"},
	{"aiff",	"audio/x-aiff"},
	{"asf",	"video/x-ms-asf"},
	{"asr",	"video/x-ms-asf"},
	{"asx",	"video/x-ms-asf"},
	{"au",	"audio/basic"},
	{"avi",	"video/x-msvideo"},
	{"axs",	"application/olescript"},
	{"bas",	"text/plain"},
	{"bcpio",	"application/x-bcpio"},
	{"bin",	"application/octet-stream"},
	{"bmp",	"image/bmp"},
	{"c",	"text/plain"},
	{"cat",	"application/vnd.ms-pkiseccat"},
	{"cdf",	"application/x-cdf"},
	{"cer",	"application/x-x509-ca-cert"},
	{"class",	"application/octet-stream"},
	{"clp",	"application/x-msclip"},
	{"cmx",	"image/x-cmx"},
	{"cod",	"image/cis-cod"},
	{"cpio",	"application/x-cpio"},
	{"crd",	"application/x-mscardfile"},
	{"crl",	"application/pkix-crl"},
	{"crt",	"application/x-x509-ca-cert"},
	{"csh",	"application/x-csh"},
	{"css",	"text/css"},
	{"dcr",	"application/x-director"},
	{"der",	"application/x-x509-ca-cert"},
	{"dir",	"application/x-director"},
	{"dll",	"application/x-msdownload"},
	{"dms",	"application/octet-stream"},
	{"doc",	"application/msword"},
	{"dot",	"application/msword"},
	{"dvi",	"application/x-dvi"},
	{"dxr",	"application/x-director"},
	{"eps",	"application/postscript"},
	{"etx",	"text/x-setext"},
	{"evy",	"application/envoy"},
	{"exe",	"application/octet-stream"},
	{"fif",	"application/fractals"},
	{"flr",	"x-world/x-vrml"},
	{"gif",	"image/gif"},
	{"gtar",	"application/x-gtar"},
	{"gz",	"application/x-gzip"},
	{"h",	"text/plain"},
	{"hdf",	"application/x-hdf"},
	{"hlp",	"application/winhlp"},
	{"hqx",	"application/mac-binhex40"},
	{"hta",	"application/hta"},
	{"htc",	"text/x-component"},
	{"htm",	"text/html"},
	{"html",	"text/html"},
	{"htt",	"text/webviewhtml"},
	{"ico",	"image/x-icon"},
	{"ief",	"image/ief"},
	{"iii",	"application/x-iphone"},
	{"ins",	"application/x-internet-signup"},
	{"isp",	"application/x-internet-signup"},
	{"jfif",	"image/pipeg"},
	{"jpe",	"image/jpeg"},
	{"jpeg",	"image/jpeg"},
	{"jpg",	"image/jpeg"},
	{"js",	"application/x-javascript"},
	{"latex",	"application/x-latex"},
	{"lha",	"application/octet-stream"},
	{"lsf",	"video/x-la-asf"},
	{"lsx",	"video/x-la-asf"},
	{"lzh",	"application/octet-stream"},
	{"m13",	"application/x-msmediaview"},
	{"m14",	"application/x-msmediaview"},
	{"m3u",	"audio/x-mpegurl"},
	{"man",	"application/x-troff-man"},
	{"mdb",	"application/x-msaccess"},
	{"me",	"application/x-troff-me"},
	{"mht",	"message/rfc822"},
	{"mhtml",	"message/rfc822"},
	{"mid",	"audio/mid"},
	{"mny",	"application/x-msmoney"},
	{"mov",	"video/quicktime"},
	{"movie",	"video/x-sgi-movie"},
	{"mp2",	"video/mpeg"},
	{"mp3",	"audio/mpeg"},
	{"mpa",	"video/mpeg"},
	{"mpe",	"video/mpeg"},
	{"mpeg",	"video/mpeg"},
	{"mpg",	"video/mpeg"},
	{"mpp",	"application/vnd.ms-project"},
	{"mpv2",	"video/mpeg"},
	{"ms",	"application/x-troff-ms"},
	{"mvb",	"application/x-msmediaview"},
	{"nws",	"message/rfc822"},
	{"oda",	"application/oda"},
	{"p10",	"application/pkcs10"},
	{"p12",	"application/x-pkcs12"},
	{"p7b",	"application/x-pkcs7-certificates"},
	{"p7c",	"application/x-pkcs7-mime"},
	{"p7m",	"application/x-pkcs7-mime"},
	{"p7r",	"application/x-pkcs7-certreqresp"},
	{"p7s",	"application/x-pkcs7-signature"},
	{"pbm",	"image/x-portable-bitmap"},
	{"pdf",	"application/pdf"},
	{"pfx",	"application/x-pkcs12"},
	{"pgm",	"image/x-portable-graymap"},
	{"pko",	"application/ynd.ms-pkipko"},
	{"pma",	"application/x-perfmon"},
	{"pmc",	"application/x-perfmon"},
	{"pml",	"application/x-perfmon"},
	{"pmr",	"application/x-perfmon"},
	{"pmw",	"application/x-perfmon"},
	{"pnm",	"image/x-portable-anymap"},
	{"pot,",	"application/vnd.ms-powerpoint"},
	{"ppm",	"image/x-portable-pixmap"},
	{"pps",	"application/vnd.ms-powerpoint"},
	{"ppt",	"application/vnd.ms-powerpoint"},
	{"prf",	"application/pics-rules"},
	{"ps",	"application/postscript"},
	{"pub",	"application/x-mspublisher"},
	{"qt",	"video/quicktime"},
	{"ra",	"audio/x-pn-realaudio"},
	{"ram",	"audio/x-pn-realaudio"},
	{"ras",	"image/x-cmu-raster"},
	{"rgb",	"image/x-rgb"},
	{"rmi",	"audio/mid"},
	{"roff",	"application/x-troff"},
	{"rtf",	"application/rtf"},
	{"rtx",	"text/richtext"},
	{"scd",	"application/x-msschedule"},
	{"sct",	"text/scriptlet"},
	{"setpay",	"application/set-payment-initiation"},
	{"setreg",	"application/set-registration-initiation"},
	{"sh",	"application/x-sh"},
	{"shar",	"application/x-shar"},
	{"sit",	"application/x-stuffit"},
	{"snd",	"audio/basic"},
	{"spc",	"application/x-pkcs7-certificates"},
	{"spl",	"application/futuresplash"},
	{"src",	"application/x-wais-source"},
	{"sst",	"application/vnd.ms-pkicertstore"},
	{"stl",	"application/vnd.ms-pkistl"},
	{"stm",	"text/html"},
	{"svg",	"image/svg+xml"},
	{"sv4cpio",	"application/x-sv4cpio"},
	{"sv4crc",	"application/x-sv4crc"},
	{"swf",	"application/x-shockwave-flash"},
	{"t",	"application/x-troff"},
	{"tar",	"application/x-tar"},
	{"tcl",	"application/x-tcl"},
	{"tex",	"application/x-tex"},
	{"texi",	"application/x-texinfo"},
	{"texinfo",	"application/x-texinfo"},
	{"tgz",	"application/x-compressed"},
	{"tif",	"image/tiff"},
	{"tiff",	"image/tiff"},
	{"tr",	"application/x-troff"},
	{"trm",	"application/x-msterminal"},
	{"tsv",	"text/tab-separated-values"},
	{"txt",	"text/plain"},
	{"uls",	"text/iuls"},
	{"ustar",	"application/x-ustar"},
	{"vcf",	"text/x-vcard"},
	{"vrml",	"x-world/x-vrml"},
	{"wav",	"audio/x-wav"},
	{"wcm",	"application/vnd.ms-works"},
	{"wdb",	"application/vnd.ms-works"},
	{"wks",	"application/vnd.ms-works"},
	{"wmf",	"application/x-msmetafile"},
	{"wps",	"application/vnd.ms-works"},
	{"wri",	"application/x-mswrite"},
	{"wrl",	"x-world/x-vrml"},
	{"wrz",	"x-world/x-vrml"},
	{"xaf",	"x-world/x-vrml"},
	{"xbm",	"image/x-xbitmap"},
	{"xla",	"application/vnd.ms-excel"},
	{"xlc",	"application/vnd.ms-excel"},
	{"xlm",	"application/vnd.ms-excel"},
	{"xls",	"application/vnd.ms-excel"},
	{"xlt",	"application/vnd.ms-excel"},
	{"xlw",	"application/vnd.ms-excel"},
	{"xof",	"x-world/x-vrml"},
	{"xpm",	"image/x-xpixmap"},
	{"xwd",	"image/x-xwindowdump"},
	{"z",	"application/x-compress"},
	{"zip",	"application/zip"},

	/***********************************************/

	{"unkown suffix name", "unkown suffix name"},
};

const mime_type_t& GetMimeType(const suffix_name_t& sn)
{
	const auto &it = HTTP_MIME_MAP.find(sn);
	return (it != HTTP_MIME_MAP.cend()) ? it->second : HTTP_MIME_MAP["unkown suffix name"];
}

std::string HTTP_ACCEPT = "Accept";
std::string HTTP_ACCEPT_CHARSET = "Accept-Charset";
std::string HTTP_ACCEPT_DATETIME = "Accept-Datetime";
std::string HTTP_ACCEPT_ENCODING = "Accept-Encoding";
std::string HTTP_ACCEPT_LANGUAGE = "Accept-Language";
std::string HTTP_ACCEPT_PATCH = "Accept-Patch";
std::string HTTP_ACCEPT_RANGES = "Accept-Ranges";
std::string HTTP_ACCESS_CONTROL_ALLOW_ORIGIN = "Access-Control-Allow-Origin";
std::string HTTP_AGE = "Age";
std::string HTTP_ALLOW = "Allow";
std::string HTTP_AUTHORIZATION = "Authorization";
std::string HTTP_CACHE_CONTROL = "Cache-Control";
std::string HTTP_CONNECTION = "Connection";
std::string HTTP_CONTENT_DISPOSITION = "Content-Disposition";
std::string HTTP_CONTENT_ENCODING = "Content-Encoding";
std::string HTTP_CONTENT_LANGUAGE = "Content-Language";
std::string HTTP_CONTENT_LENGTH = "Content-Length";
std::string HTTP_CONTENT_LOCATION = "Content-Location";
std::string HTTP_CONTENT_MD5 = "Content-MD5";
std::string HTTP_CONTENT_RANGE = "Content-Range";
std::string HTTP_CONTENT_TYPE = "Content-Type";
std::string HTTP_COOKIE = "Cookie";
std::string HTTP_DATE = "Date";
std::string HTTP_ETAG = "ETag";
std::string HTTP_EXPECT = "Expect";
std::string HTTP_EXPIRES = "Expires";
std::string HTTP_FROM = "From";
std::string HTTP_HOST = "Host";
std::string HTTP_IF_MATCH = "If-Match";
std::string HTTP_IF_MODIFIED_SINCE = "If-Modified-Since";
std::string HTTP_IF_NONE_MATCH = "If-None-Match";
std::string HTTP_IF_RANGE = "If-Range";
std::string HTTP_IF_UNMODIFIED_SINCE = "If-Unmodified-Since";
std::string HTTP_LAST_MODIFIED = "Last-Modified";
std::string HTTP_LINK = "Link";
std::string HTTP_LOCATION = "Location";
std::string HTTP_MAX_FORWARDS = "Max-Forwards";
std::string HTTP_ORIGIN = "Origin";
std::string HTTP_P3P = "P3P";
std::string HTTP_PRAGMA = "Pragma";
std::string HTTP_PROXY_AUTHENTICATE = "Proxy-Authenticate";
std::string HTTP_PROXY_AUTHORIZATION = "Proxy-Authorization";
std::string HTTP_PUBLIC_KEY_PINS = "Public-Key-Pins";
std::string HTTP_RANGE = "Range";
std::string HTTP_REFERER = "Referer";
std::string HTTP_REFRESH = "Refresh";
std::string HTTP_RETRY_AFTER = "Retry-After";
std::string HTTP_SERVER = "Server";
std::string HTTP_SET_COOKIE = "Set-Cookie";
std::string HTTP_STATUS = "Status";
std::string HTTP_STRICT_TRANSPORT_SECURITY = "Strict-Transport-Security";
std::string HTTP_TE = "TE";
std::string HTTP_TRAILER = "Trailer";
std::string HTTP_TRANSFER_ENCODING = "Transfer-Encoding";
std::string HTTP_UPGRADE = "Upgrade";
std::string HTTP_USER_AGENT = "User-Agent";
std::string HTTP_VARY = "Vary";
std::string HTTP_VIA = "Via";
std::string HTTP_WARNING = "Warning";
std::string HTTP_WWW_AUTHENTICATE = "WWW-Authenticate";
