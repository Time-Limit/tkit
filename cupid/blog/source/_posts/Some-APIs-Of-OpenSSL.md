---
title: 一些 OpenSSL 的基础 API
date: 2018-06-24 14:53:28
tags: OpenSSL
---

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

<!-- more -->

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

