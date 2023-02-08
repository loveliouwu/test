# OpenSSL  
## 简介  
开源SSL安全工具，包括SSL协议实现，大量软算法（对称/非对称/摘要），非对称算法密钥生成，ASN.1编解码库、证书请求编解码，数字证书，CRL编解码，OCSP协议，PKCS7标准实现，PSCK12个人数字证书格式实现等功能。

## 源码框架   
- **asn.1** DER编码解码，包含数字证书请求，CRL吊销列表及PKCS8编解码函数;
- **BIO** 抽象IO，包含各种输入输出抽象，文件，内存，stdio，socket，SSL;
- **BN** 大数运算，用于非对称算法密钥生成和各种加解密;
- **字符缓存** crypto/buffer 目录;
- **配置文件读取** crypto/conf 目录，主要配置文件为openssl.cnf;
- **DSO动态共享对象** 抽象各平台动态库加载函数，提供统一接口;
- **硬件引擎** 提供了规定接口;
- **错误处理** 提供处理接口;以堆栈显示错误
- **EVP** 对称算法、非对称算法及摘要算法封装 encrypt/evp  
- **HMAC** 实现基于对称算法的MAC
- **HASH** 实现散列表数据结构  
- **OCSP数字证书在线认证** 实现ocsp协议的编解码等;
- **PEM文件格式处理** 生成和读取PEM文件;
- **队列** 实现队列数据结构，用于DTLS;
- **随机数** 实现伪随机数的生成，支持用户自定义;
- **堆栈** 实现堆栈数据结构;
- **线程支持** openssl支持多线程，但是用户必须实现相关接口;
- **文本数据库** txt_db目录;
- **对称算法** aes bf cast omp des等;
- **摘要算法** md2/4/5/sha 以及密钥交换/认证算法

SSL库源代码在ssl目录下，包含sslv2 sslv3 tlsv1和DTLS的源码，包含客户端源码，服务器源码，通用，底层包，方法以及协议相关密钥计算源码;
工具源码在crypto/apps目录，demo中有硬件引擎engines源码  


 

## 重点函数说明和用法   
```c
#include <openssl/crypto.h>
typedef struct ossl_lib_ctx_st OSSL_LIB_CTX;

OSSL_LIB_CTX *OSSL_LIB_CTX_new(void);
OSSL_LIB_CTX *OSSL_LIB_CTX_new_from_dispatch(const OSSL_CORE_HANDLE *handle,
                                            const OSSL_DISPATCH *in);
OSSL_LIB_CTX *OSSL_LIB_CTX_new_child(const OSSL_CORE_HANDLE *handle,
                                    const OSSL_DISPATCH *in);
int OSSL_LIB_CTX_load_config(OSSL_LIB_CTX *ctx, const char *config_file);
void OSSL_LIB_CTX_free(OSSL_LIB_CTX *ctx);
OSSL_LIB_CTX *OSSL_LIB_CTX_get0_global_default(void);
OSSL_LIB_CTX *OSSL_LIB_CTX_set0_default(OSSL_LIB_CTX *ctx);
```
- OSSL_LIB_CTX是内部使用的库的上下文类型  
- **OSSL_LIB_CTX_new_from_dispatch**创建了一个新的OpenSSL库上下文，该上下文初始化为使用来自OSSL_DISPATCH结构的回调。  
- **OSSL_LIB_CTX_new_child**仅对提供程序作者有用，并且执行与**OSSL_LIB_CTX_new_from_dispatch**相同的操作，只是它另外将新库上下文链接到应用程序库上下文，新的库上下文本山就是一个完整的库上下文，但它将具有在应用程序库上下文中可用的所有相同的提供程序（无需重新加载他们），如果应用程序从应用程序库上下文中加载或卸载提供程序，这将自动镜像在子库上下文中。     
此外，未在父库上下文中加载的提供程序可以独立于父库上下文显式加载到子库上下文中，以这种方式独立加载的提供程序将不会在父库上下文中镜像。   
除了提供程序外，子库上下文还将镜像父库上下文中的默认属性。
- **OSSL_LIB_CTX_get0_global_default**返回对全局默认库上下文的具体（非NULL）引用。  

## provider  
在OPENSSL术语中，提供程序是一个代码单元，它为可能想要执行的各种算法的各种操作提供一个或多个实现。  
一个操作可以是加密或解密，密钥派生，MAC计算，签名验签等。  
provider提供一个初始化函数，作为**OSSL_DISPATH**中的一个基本函数。



### `OSSL_OP_`开头的宏，用数字来表示操作   
当前可用的操作包括 ： 
- 摘要  
	方法对象：EVP_MD；
	编号：OSSL_OP_DIGEST
- 对称密码  
	方法对象：EVP_CIPHER
	编号：OSSL_OP_CIPHER  
- 消息验证码 MAC 
	EVP_MAC
	OSSL_OP_MAC 
- 密钥派生函数 KDF  
	EVP_KDF 
	OSSL_OP_KDF  
- 密钥交换   
	EVP_KEYEXCH  
	OSSL_OP_KEYEXCH  
- 非对称密码  
	EVP_ASYM_CIPHER  
	OSSL_OP_ASYM_CIPHER   
- 非对称密钥封装  
	EVP_KEM  
	OSSL_OP_KEM   
- 编码  
	OSSL_ENCODER  
	OSSL_OP_ENCODER  
	
	
	
