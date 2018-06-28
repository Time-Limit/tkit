## NOTES
* 编译时, OpenSSL 库需使用 `libssl.so.1.1`, `libcrypto.so.1.1`，或更高版本。
* class Thread 的析构函数会调用 `pthread_cancel`，故异步执行的逻辑应有取消点(如，`pthread_testcancel`等)。
