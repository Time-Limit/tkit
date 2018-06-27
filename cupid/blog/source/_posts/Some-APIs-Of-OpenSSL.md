---
title: 一些 OpenSSL 的基础 API
date: 2018-06-24 14:53:28
tags: OpenSSL
---

## SSL_new

### 名称

SSL_dup, SSL_new, SSL_up_ref - 为链接创建一个 SSL 结构。

### 简介

```C++
 #include <openssl/ssl.h>

 SSL *SSL_dup(SSL *s);
 SSL *SSL_new(SSL_CTX *ctx);
 int SSL_up_ref(SSL *s);
```

<!-- more -->

### 描述

`SSL_new()` 创建一个用于为 TLS/SSL 链接保存数据的 SSL 对象。这个对象继承`ctx`的设置：链接方法，选项，验证设置，超时设置。SSL 对象是引用计数的。创建SSL对象时递增引用计数，使用 `SSL_Free()`释放它时递减引用计数。当引用计数减为 0 时，所有的内存和资源都会被释放掉。

`SSL_up_ref()` 对一个存在的 SSL 对象的引用计数进行递增操作。

`SSL_dup()` 复制一个存在的 SSL 对象，并创建一个新的 SSL 对象。所有的设置都继承自原始的 SSL 对象。动态的数据(比如已经建立的链接的详细信息)不会被复制，这个新的 SSL 对象被初始为接收(服务器端)或链接(客户端)状态。

### 返回值

可能会出现下述返回值：

NULL

> 创建 SSL 对象失败。检查错误堆栈以确定原因。

指向 SSL 队列的指针

> 返回一个指向 SSL 对象的指针。`SSL_up_ref()`返回 1 表示成功，0 表示失败。



## SSL_CTX_new

### 名称

TLSv1_2_method, TLSv1_2_server_method, TLSv1_2_client_method, SSL_CTX_new, SSL_CTX_up_ref, SSLv3_method, SSLv3_server_method, SSLv3_client_method, TLSv1_method, TLSv1_server_method, TLSv1_client_method, TLSv1_1_method, TLSv1_1_server_method, TLSv1_1_client_method, TLS_method, TLS_server_method, TLS_client_method, SSLv23_method, SSLv23_server_method, SSLv23_client_method, DTLS_method, DTLS_server_method, DTLS_client_method, DTLSv1_method, DTLSv1_server_method, DTLSv1_client_method, DTLSv1_2_method, DTLSv1_2_server_method, DTLSv1_2_client_method - 为 TLS/SSL 或者 DTLS 函数创建一个新的 `SSL_CTX`对象。

### 简介

```c++
 #include <openssl/ssl.h>

 SSL_CTX *SSL_CTX_new(const SSL_METHOD *method);
 int SSL_CTX_up_ref(SSL_CTX *ctx);

 const SSL_METHOD *TLS_method(void);
 const SSL_METHOD *TLS_server_method(void);
 const SSL_METHOD *TLS_client_method(void);

 const SSL_METHOD *SSLv23_method(void);
 const SSL_METHOD *SSLv23_server_method(void);
 const SSL_METHOD *SSLv23_client_method(void);

 #ifndef OPENSSL_NO_SSL3_METHOD
 const SSL_METHOD *SSLv3_method(void);
 const SSL_METHOD *SSLv3_server_method(void);
 const SSL_METHOD *SSLv3_client_method(void);
 #endif

 #ifndef OPENSSL_NO_TLS1_METHOD
 const SSL_METHOD *TLSv1_method(void);
 const SSL_METHOD *TLSv1_server_method(void);
 const SSL_METHOD *TLSv1_client_method(void);
 #endif

 #ifndef OPENSSL_NO_TLS1_1_METHOD
 const SSL_METHOD *TLSv1_1_method(void);
 const SSL_METHOD *TLSv1_1_server_method(void);
 const SSL_METHOD *TLSv1_1_client_method(void);
 #endif

 #ifndef OPENSSL_NO_TLS1_2_METHOD
 const SSL_METHOD *TLSv1_2_method(void);
 const SSL_METHOD *TLSv1_2_server_method(void);
 const SSL_METHOD *TLSv1_2_client_method(void);
 #endif

 const SSL_METHOD *DTLS_method(void);
 const SSL_METHOD *DTLS_server_method(void);
 const SSL_METHOD *DTLS_client_method(void);

 #ifndef OPENSSL_NO_DTLS1_METHOD
 const SSL_METHOD *DTLSv1_method(void);
 const SSL_METHOD *DTLSv1_server_method(void);
 const SSL_METHOD *DTLSv1_client_method(void);
 #endif

 #ifndef OPENSSL_NO_DTLS1_2_METHOD
 const SSL_METHOD *DTLSv1_2_method(void);
 const SSL_METHOD *DTLSv1_2_server_method(void);
 const SSL_METHOD *DTLSv1_2_client_method(void);
 #endif
```

### 描述

`SSL_CTX_new()`创建一个新的`SSL_CTX`对象，作为框架以建立启用 TLS/SSL 或者 DTLS 的链接。一个`SSL_CTX`对象是引用计数的。创建`SSL_CTX`对象时递增引用计数，释放它时(使用 `SSL_CTX_free`)递减引用计数。当引用计数减为零时，为`SSL_CTX`对象分配的内存和其他资源全部被释放。`SSL_CTX_up_ref() `可以增加一个已经存在的`SSL_CTX`对象的引用计数。

### 提示

SSL_CTX 对象使用`method` 参数指定的方法作为链接方法。这些方法有通用类型（客户端和服务器使用）、服务器类型和客户端类型。方法可以下述类型之一：

TLS_method(), TLS_server_method(), TLS_client_method() 

> 这些是通用的版本灵活的 SSL/TLS 方法。所使用的实际协议版本是客户端和服务端均支持的最高版本。支持的协议有 SSLv3，TLSv1，TLSv1.1，TLSv1.2。应用程序应该使用这些方法，并且应该避免使用下述指定版本的方法。

SSLv23_method(), SSLv23_server_method(), SSLv23_client_method() 

> 不赞成使用这些方法。应该被分别替换成 TLS_method()，TLS_server_method()，TLS_client_method()。

TLSv1_2_method(), TLSv1_2_server_method(), TLSv1_2_client_method() 

> 只能使用 TLSv1.2 建立 TLS/SSL 链接。 

TLSv1_1_method(), TLSv1_1_server_method(), TLSv1_1_client_method() 

> 只能使用 TLSv1.1 建立 TLS/SSL 链接。

TLSv1_method(), TLSv1_server_method(), TLSv1_client_method() 

>  只能使用 TLSv1 建立 TLS/SSL 链接。

SSLv3_method(), SSLv3_server_method(), SSLv3_client_method() 

> 只能使用 SSLv3 建立 TLS/SSL 链接。但是不赞成使用该版本。

DTLS_method(), DTLS_server_method(), DTLS_client_method() 

> 这是一个版本灵魂的 DTLS 方法。目前仅支持 DTLS1.0 和 DTLS1.2。

DTLSv1_2_method(), DTLSv1_2_server_method(), DTLSv1_2_client_method() 

> 仅使用 DTLSv1.2

DTLSv1_method(), DTLSv1_server_method(), DTLSv1_client_method() 

> 仅使用 DTLSv1.0

`SSL_CTX_new()` 使用默认值初始化密码，会话缓存设置，回调函数，密钥和证书。

`TLS_method()`，`TLS_server_method()`，`TLS_client_method()`，`DTLS_method()`，`DTLS_server_method()`，`DTLS_client_method()`都是版本灵活的方法。其他所有的方法仅支持一个方法。应该使用版本灵活的方法代替其他指定版本的方法。

如果你想限制版本灵活方法支持的协议版本，你可以使用`SSL_CTX_set_min_proto_version()`和`SSL_set_max_proto_version()`。使用上述方法可以控制和所有客户端建链使用的协议版本，但是只支持`TLS1.0`，`TLS1.1`和`TLS1.2`。

### 返回值

可能会出现下列返回值:

NULL

> 创建 SSL_CTX 失败。检查错误堆栈确认原因。

指向 SSL_CTX 对象的指针

> 返回一个指向 SSL_CTX 对象的指针。 `SSL_CTX_up_ref()`返回 1 表示成功，0 表示失败。



## SSL_CTX_use_certificate

### 名称

SSL_CTX_use_certificate, SSL_CTX_use_certificate_ASN1, SSL_CTX_use_certificate_file, SSL_use_certificate, SSL_use_certificate_ASN1, SSL_use_certificate_file, SSL_CTX_use_certificate_chain_file, SSL_use_certificate_chain_file, SSL_CTX_use_PrivateKey, SSL_CTX_use_PrivateKey_ASN1, SSL_CTX_use_PrivateKey_file, SSL_CTX_use_RSAPrivateKey, SSL_CTX_use_RSAPrivateKey_ASN1, SSL_CTX_use_RSAPrivateKey_file, SSL_use_PrivateKey_file, SSL_use_PrivateKey_ASN1, SSL_use_PrivateKey, SSL_use_RSAPrivateKey, SSL_use_RSAPrivateKey_ASN1, SSL_use_RSAPrivateKey_file, SSL_CTX_check_private_key, SSL_check_private_key - 加载证书和密钥数据。

### 简介

```C++
 #include <openssl/ssl.h>

 int SSL_CTX_use_certificate(SSL_CTX *ctx, X509 *x);
 int SSL_CTX_use_certificate_ASN1(SSL_CTX *ctx, int len, unsigned char *d);
 int SSL_CTX_use_certificate_file(SSL_CTX *ctx, const char *file, int type);
 int SSL_use_certificate(SSL *ssl, X509 *x);
 int SSL_use_certificate_ASN1(SSL *ssl, unsigned char *d, int len);
 int SSL_use_certificate_file(SSL *ssl, const char *file, int type);

 int SSL_CTX_use_certificate_chain_file(SSL_CTX *ctx, const char *file);
 int SSL_use_certificate_chain_file(SSL *ssl, const char *file);

 int SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type);
 int SSL_use_PrivateKey_file(SSL *ssl, const char *file, int type);

 int SSL_CTX_check_private_key(const SSL_CTX *ctx);
 int SSL_check_private_key(const SSL *ssl);
```



### 描述

这些函数将证书和密钥数据加载到 SSL_CTX 或者 SSL 对象。

SSL_CTX_* 函数集把证书和密钥数据加载到 SSL_CTX 对象 `ctx`中。`ctx`通过 `SSL_new()` 创建 SSL 对象时是通过复制传递数据的，所以在`ctx`上的修改不会传递给已存在的 SSL 对象。

SSL_* 函数集只将证书和密钥数据加载到指定的 SSL 对象。当调用`SSL_clear`时，指定的信息会被保留。

`SSL_CTX_use_certificate()`将证书`x`加载到`ctx`中。`SSL_use_certificate()`将证书`x`加载到`ssl`中。形成证书链所需的剩余证书可以通过`SSL_CTX_add_extra_chain_cert()`指定。

`SSL_CTX_use_certificate_ASN1() `从内存地址`d` 处加载 ASN1 格式的证书到 ctx 中，`SSL_use_certificate_ASN1()`加载 ASN1 格式的证书到`ssl`中。

`SSL_CTX_use_cerificate_file()`加载`file`中的第一个证书到 `ssl`中。证书的格式类型必须是`SSL_FILETYPE_PEM`或者`SSL_FILETYPE_ASN1`。`SSL_use_cerificate_file()`从`file`中加载证书到`ssl`中。在_注意_中查看`SSL_CTX_use_cerificate_file()`更优的原因。

`SSL_CTX_use_certificate_chain_file()`从`file`中加载证书链到`ctx`中。这些证书必须是 PEM 格式的，且必须按子证书(通常是服务端和客户端证书)，中级 CA 证书，根证书的顺序排序。`SSL_use_certificate_chain_file()`的功能相似，知识它把证书链加载到`ssl`中。

`SSL_CTX_use_PrivateKey_file()`添加`file`中的第一个密钥到`ctx`。密钥的类型必须是 SSL_FILETYPE_PEM 或者 SSL_FILETYPE_ASN1。`SSL_use_PrivateKey_file()`添加`file`中的第一个密钥到`ssl`中。

`SSL_CTX_check_private_key()`负责检查加载到`ctx`的密钥和其对应的证书的一致性。如果有多对密钥和证书被加载，那么只会检查最后一对。`SSL_check_private_key()`以同样的规则对`ssl`进行检查。如果没有向`ssl`添加过证书和密钥，那么将检查最后向`ctx`添加的证书和密钥。

### 返回值

返回 1 表示成功。返回其他值表示失败，检查错误堆栈以确认原因。

## SSL_library_init

### 名称

SSL_library_init, OpenSSL_add_ssl_algorithm - 注册算法并初始化 SSL 库。

### 简介

```c++
 #include <openssl/ssl.h>

 int SSL_library_init(void);

 int OpenSSL_add_ssl_algorithms(void);
```

### 注意

`SSL_library_init() `的调用必须先于其他所有操作。`SSL_library_init()`不可重入。

### 警告

`SSL_library_init()` 添加由 SSL/TLS 直接或简介调用的密码和摘要。

### 返回值

`SSL_library_init()` 总是返回 1，因此可以放心的将返回值丢弃。

## SSL_set_fd

### 名称

SSL_set_fd - 链接 SSL 对象和文件描述符。

### 简介

```C++
 #include <openssl/ssl.h>

 int SSL_set_fd(SSL *ssl, int fd);
```

### 描述

`SSL_set_fd()`将 `ssl` 和 `fd`链接起来。`ssl`将在`fd`上进行读写。`fd`应该是 socket 文件描述符或者一个网络链接。在执行操作时，会自动创建 socket BIO 作为 `ssl`和`fd`之间的接口。如果`fd`是非阻塞的，那么 `ssl`也是非阻塞的。

### 返回值

* 0 - 操作失败。检查错误堆栈以确认具体原因。
* 1 - 操作成功。

## SSL_read

### 名称

SSL_read_ex，SSL_read - 从 TLS/SSL 链接中读取数据。

### 简介

```C++
 #include <openssl/ssl.h>

 int SSL_read_ex(SSL *ssl, void *buf, size_t num, size_t *readbytes);
 int SSL_read(SSL *ssl, void *buf, int num);
```

### 描述

`SSL_read_ex()`和`SSL_read()`尝试从 `ssl`中读取`num`个字节到 `buf`中。如果成功，`SSL_read_ex()`将真正读取到的字节数存储到`readbytes`中。

### 注意

在本段落中，用_读函数_ 代替 `SSL_read_ex()`和`SSL_read()`。如果没有执行过`SSL_connect`或者`SSL_accept`，读函数会达成一个 TLS/SSL 会话。如果对端要求重协商，在读函数操作时会透明的执行该动作。读函数的具体行为依赖于底层的 BIO。

为了使透明协商成功，`ssl`必须已经初始化为客户端或服务器模式。在第一次调用读函数之前必须调用`SSL_set_connect_state()`或者`SSL_set_accept_state()`。

读函数的工作基于 SSL/TLS 记录。数据以记录的形式接收(记录的最大尺寸为16KB)。只有当记录被完全接收时，才能对齐进行处理(解密和完整性检查)。因此，上次读调用未读到的数据仍有可能在SSL的缓冲区中，并将在下一次读调用中再次被检索。如果`num`大于缓冲区内的字节数，那么读函数返回缓冲区内的字节数。如果缓冲区内字节数小于`num`，那么将会触发下一条记录的处理。只有当记录被完全接收和处理时，读函数才会返回成功。记录中的内容会被尽可能多的返回。由于SSL/TLS记录的大小可能超过底层传输（例如TCP）的最大分组大小，因此可能需要在记录完成之前读取来自传输层的多个分组，之后读取调用才可以成功。

如果底层 BIO 是堵塞的，那么读函数只在读操作完成或者错误发生时返回。有一个例外是当重协商发生是，会发生`SSL_ERRO_WANT_READ`错误。这个行为可以通过调用`SSL_CTX_set_mode()`设置`SSL_MODE_AUPT_REPLY`来控制。

如果底层 BIO 是非阻塞的，当底层 BIO 不能满足操作继续时，读函数也会返回。这种情况下，将读函数返回值作为参数调用`SSL_get_error()`将会得到`SSL_ERROR_WANT_READ`或者`SSL_ERROR_WANT_WRITE`。重协商可以发生在任何时间点，所以读函数也会导致写操作。在调用适当的动作之后，应该再次调用之前的函数，以满足读函数的需要。具体细节依赖于底层的 BIO。当使用非阻塞的 socket 时，不会做任何事情，但是可以使用`select()`检查是否满足所要求的条件。

### 警告

当`SSL_get_error()`返回`SSL_ERROR_WANT_READ`或者`SSL_ERROR_WANT_WRITE`，应该以相同的参数再次调用读函数。

### 返回值

`SSL_read_ex()`返回 1 代表成功，0 代表失败。成功意味着从 SSL 链接中读取了至少一个字节的数据。失败意味着没有读取到任何数据。

`SSL_read()`的返回值如下所述：

* \> 0 - 读操作成功。返回值为从 TLS/SSL 中实际读取的字节数。
* <= 0 - 读操作失败。调用`SSL_get_error()`获取具体原因。

## SSL_write

### 名称

SSL_wirte_ex， SSL_write - 向 TLS/SSL 链接写入数据。

### 简介

```c++
 #include <openssl/ssl.h>

 int SSL_write_ex(SSL *s, const void *buf, size_t num, size_t *written);
 int SSL_write(SSL *ssl, const void *buf, int num);
```

### 描述

`SSL_write_ex()` 和`SSL_write()`向指定的`ssl`写入数据。成功时，`SSL_write_ex()`将把写入字节数储存在`written`中。

### 注意

本段落中，使用_写函数_代替`SSL_write_ex()`和`SSL_write()`。

如果没有执行`SSL_connect()`或者`SSL_accept()`，写函数将达成一个会话。如果对端要求重协商，在写函数操作时会透明的执行该动作。写函数的具体行为依赖于底层的 BIO。

为了使透明协商成功，`ssl`必须已经初始化为客户端或服务器模式。在第一次调用写函数之前必须调用`SSL_set_connect_state()`或者`SSL_set_accept_state()`。

如果底层 BIO 是阻塞的，只有当执行完成或者发生错误时返回。有一个例外是重协商发生时，将会发生`SSL_ERROR_WANT_READ`。可以通过调用`SSL_CTX_set_mode()`设置`SSL_MODE_AUTO_RETRY`来控制该行为。

如果底层 BIO 是非阻塞的，当底层 BIO 不能满足操作继续时，写函数也会返回。这种情况下，将写函数的返回值作为参数调用`SSL_get_error()`会获得`SSL_ERROR_WANT_READ`或者`SSL_ERROR_WANT_WRITE`。重协商可能发生在任何时候，所以调用写函数会导致读操作。在调用适当的动作之后，应该再次调用之前的函数，以满足写函数的需要。具体细节依赖于底层的 BIO。当使用非阻塞的 socket 时，不会做任何事情，但是可以使用`select()`检查是否满足所要求的条件。

写函数只有当`buf`指向的所有数据都被写完时才返回成功。可以通过调用`SSL_CTX_set_mode()`设置`SSL_MODE_ENABLE_PARTIAL_WRITE`来控制该行为。设置了该标志时，写函数写入部分数据时也返回成功。这时认为写函数已完成。发送数据后，应该再次以新缓冲区(移除了已发送数据)调用写函数。部分写最优的消息块的尺寸是16KB。

### 警告

当`SSL_get_error()`返回`SSL_ERROR_WANT_READ`或者`SSL_ERROR_WANT_WRITE`时，应该以同样的参数再次调用写函数。

### 返回值

`SSL_write_ex()`返回 1 表示成功，0 表示失败。返回成功时意味着所有的数据都已被写入`ssl`，如果设置了`SSL_MODE_ENABLE_PARTIAL_WRITE`，则意味了至少向`ssl`写入了一个字节。当未设置`SSL_MODE_ENABLE_PARTIAL_WRITE`时，失败意味着没能写入所有的数据。当设置了`SSL_MODE_ENABLE_PARTIAL_WRITE`时，失败意味着没有写入任何数据。失败时调用`SSL_get_error()`以确认具体的原因。

`SSL_write()`的返回值为下述值之一：

* \> 0 - 写操作成功，返回值为实际写入的字节数。
* <= 0 - 写操作失败，操作失败，调用`SSL_get_error()`以确认具体原因。

## SSL_set_connect_state

### 名称

SSL_set_conenct_state，SSL_set_accept_state，SSL_is_server - 用以修改和检测 SSL 对象的服务器或客户端模式的函数。

### 简介

```C++
 #include <openssl/ssl.h>
 void SSL_set_connect_state(SSL *ssl)；
 void SSL_set_accept_state(SSL *ssl);
 int SSL_is_server(const SSL *ssl);
```

### 描述

`SSL_set_connect_state()` 使`ssl`以客户端模式工作。

`SSL_set_accept_state()`使`ssl`以服务器模式工作。

`SSL_is_server()`检查`ssl`是否以服务器模式工作。

当开启一次新的握手时，SSL 引擎必须要知道需调用 connect 或者 accept 例程。即使明确设置了服务器或者客户端方法，也必须明确指定该使用哪个握手例程。

当使用`SSL_conenct`或者`SSL_accept`时，会自动调用对应的握手例程。但是当使用`SSL_write_ex()`，`SSL_write()`，`SSL_read_ex()`，`SSL_read()`进行隐式的握手时，则必须通过`SSL_set_connect_state()`或者`SSL_set_accept_state()`显式的指定该使用哪个握手例程。

如果在调用`SSL_set_connect_state()`或者`SSL_set_accept_state()`之前，调用`SSL_is_server()`，其结果依赖于调用`SSL_CTX_new()`时使用了何种方法。如果使用了通用方法或者确定的服务器方法，将返回 1，否则返回 0 。

### 返回值

`SSL_set_connect_set()`和`SSL_set_accept_set()`不返回任何值。

`SSL_is_server()`返回 1 表示`ssl`以服务器方式工作，0 表示以客户端方式工作。

SSL_is_server() returns 1 if **ssl** is working in server mode or 0 for client mode. 

## SSL_get_error

### 名称

SSL_get_error - 获取 TLS/SSL I/O 操作的结果码。

### 简介

```c++
#include <openssl/ssl.h>

int SSL_get_error(const SSL *ssl, int ret);

```

### 描述

`SSL_get_error() ` 返回一个结果码(可以用于 C 的 `switch` 语句)，这个结果码用于表示最近一次在 `ssl`参数上调用`SSL_connect()`，`SSL_accept()`，`SSL_do_handshake()`，`SSL_read_ex()`，`SSL_read()`，`SSL_peek_ex()`，`SSL_peek()`，`SSL_write_ex()`，`SSL_write() ` 等函数的结果。TLS/SSL I/O 函数的返回值必须通过参数 `ret`传递给`SSL_get_error()`。

除了 `ssl` 和 `ret` 参数， `SSL_get_error()` 还会检查当前线程的 OpenSSL 的错误队列。因此，`SSL_get_error()` 必须与 TLS/SSL I/O 操作在同一个线程调用，并且两者之间不能有其他 OpenSSL 函数被调用。当前线程的错误队列在 TLS/SSL I/O 操作被调用之间必须空的，否则 `SSL_get_error()` 将可能无法正常工作。

### 返回值

接下来的这些返回值都有可能出现：

#### SSL_ERROR_NONE

> TLS/SSL I/O 操作完成了。当且仅当 `ret > 0` 时才会返回该值。

#### SSL_ERROR_ZERO_RETURN

> 该 TLS/SSL 链接被关闭了。如果协议版本号是 SSL 3.0 或更高， 只有在协议中出现关闭警告时才会返回该值，例如，当链接被完全关闭时。注意，返回该值时不代表底层链接已经被关闭了。

#### SSL_ERROR_WANT_READ, SSL_ERROR_WANT_WRITE 

> 操作并未完成。稍后应调用同样的 TLS/SSL I/O 函数。如果稍后底层的 BIO 变为可读(如果返回值为 `SSL_ERROR_WANT_READ`) 或可写(如果返回值为 `SSL_ERROR_WABT_WRITE`)，那么协议应该做些事情，亦即，或读或写部分 TLS/SSL 记录。注意重新调用也会导致`SSL_ERROR_WANT_READ` 或 `SSL_ERROR_WANT_WRITE`。在获得具体的应用层协议之前，并没有次数限制。
>
> 对于 socket BIO(比如当 SSL_set_fd() 被调用过)，可以使用`select()`或者`poll()` 来确定应何时重试 TLS/SSL I/O 函数。
>
> 附加说明：TLS/SSL I/O 函数都能导致 `SSL_ERROR_WANT_READ` 或者 `SSL_ERROR_WANT_WRITE`。特别的，`SSL_read_ex()`，`SSL_read()` ，`SSL_peek_ex()`或者`SSL_peek()`会导致`SSL_ERROR_WANT_WRITE`，`SSL_write()`，`SSL_write_ex()` 会导致`SSL_ERROR_WANT_READ`。这是非常重要的，因为 TLS/SSL 握手随时都可能发生(由客户端或服务器发起)；`SSL_read_ex`，`SSL_read()`，`SSL_peek_ex()`，`SSL_peek()`，`SSL_write_ex()`， `SSL_write()`都能处理所有未决的握手。

#### SSL_ERROR_WANT_CONNECT, SSL_ERROR_WANT_ACCEPT 

> 操作并未完成。稍后应调用同样的 TLS/SSL I/O 函数。底层的 BIO 尚未完成链接到对端并且调用可能会阻塞在 `connect()/accept()`上。在链接建立完成后，应该再次调用 SSL 函数。`SSL_ERROR_WANT_CONNECT`和`SSL_ERROR_WANT_ACCEPT` 分别伴随`BIO_s_connect()` 和`BIO_s_accept()` 出现。在一些平台上可以使用 `select()` 和`poll()`来确定链接何时完成。

#### SSL_ERROR_WANT_X509_LOOKUP 

> 由于通过`SSL_CTX_set_client_cert_cb()`设置的应用回调函数需被再次调用，导致操作未完成。TLS/SSL I/O 函数应再次被调用。具体细节依赖于应用程序。

#### SSL_ERROR_WANT_ASYNC 

> 由于异步引擎仍在操作数据，导致操作未完成。只有当通过`SSL_CTX_set_mode()`和`SSL_set_mode`将模式设置为`SSL_MODE_ASYNC`并且异步引擎被使用时才会出现。应用可以通过使用`select()`和`poll()`监听异步文件描述符来确定异步引擎是否完成工作。该文件描述符可以通过`SSL_get_all_async_fds`和`SSL_get_changed_async_fds`来获得。稍后应当再次调用 TLS/SSL I/O函数。两次调用应该在一个线程。

#### SSL_ERROR_WANT_ASYNC_JOB 

> 异步作业无法开始，因为池子里没有异步作业(参考 `ASYNC_init_thread(3)`)。只有当通过`SSL_CTX_set_mode()`和`SSL_set_mode`将模式设置为`SSL_MODE_ASYNC`并且通过`ASYNC_init_thread()`给异步作业池设置上了最大上限。应用应当在当前线程完成当前异步作业后再次重试操作。

#### SSL_ERROR_WANT_CLIENT_HELLO_CB 

> 由于通过`SSL_CTX_set_client_cert_cb()`设置的应用回调函数需被再次调用，导致操作未完成。TLS/SSL I/O 函数应再次被调用。具体细节依赖于应用程序。

#### SSL_ERROR_SYSCALL

> 发生了不可恢复的I/O错误。OpenSSL 错误队列或许会有关于此错误的更多内容。对于 Unix 操作系统的 socket I/O，查看`errno`以获取详细信息。

#### SSL_ERROR_SSL 

> SSL 库发生了错误，通常是协议错误。penSSL 错误队列会有关于此错误的更多内容。

### 历史

> `SSL_ERROR_WANT_ASYNC`首次出现于 OpenSSL 1.1.0。
>
> `SSL_ERROR_WANT_CLIENT_HELLO_CB`首次出现于 OpenSSL 1.1.1。

### COPYRIGHT

Copyright 2000-2017 The OpenSSL Project Authors. All Rights Reserved.

Licensed under the OpenSSL license (the "License"). You may not use this file except in compliance with the License. You can obtain a copy in the file LICENSE in the source distribution or at <https://www.openssl.org/source/license.html>.
