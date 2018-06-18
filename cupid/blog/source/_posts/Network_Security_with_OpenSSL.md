---
title: 网络安全与OpenSSL
date: 2018-05-07 15:17:23
tags: 网络安全与OpenSSL

---

### 作者: Pravir Chandra, Matt Messier, John Viega

### 出版社: O Reilly

### 出版时间: 2002.06

### ISBN: 0-596-00270-X

### 页数: 384

OpenSSL 是 SSL/TLS 的一个受欢迎的，高效的，开源的版本，被广泛的应用于安全网络通
信。关于这个主题的唯一指引，《Network Security with OpenSSL》阐述了安全网络通信
中的挑战，并且告诉了你如何使用 OpenSSL 的工具来以最好的姿态迎接这些挑战。本书非
常注重实用，所以仅提供了关于高效安全的使用OpenSSL的所必须的信息。

<!-- more -->

## 目录
* 第一章 介绍
	* 密码学初探
	* SSL 概览
	* SSL 的一些问题
	* SSL 的短板
	* OpenSSL 基础
	* 加密第三方软件
* 第二章 命令行接口
	* 基础
	* 信息摘要算法
	* 对称加密
	* 公钥加密
	* S/MIME (安全多用途网际邮件扩展协议)
	* 密码和口令
	* 伪随机数生成器的种子
* 第三章 公钥架构
	* 证书
	* 如何获得证书
	* 建立认证机构
* 第四章 支持设施
	* 多线程支持
	* 内部错误处理
	* 抽象的输入输出
	* 随机数生成
	* 任意精度的数学
	* 使用引擎
* 第五章 SSL/TLS 编程
	* 使用 SSL 编程
	* SSL 编程进阶
* 第六章 对称加密
	* 对称加密的概念
	* 使用 EVP API 加密
	* 一些建议
* 第七章 哈希和 MAC
	* 哈希和 MAC 概览
	* 使用 EVP API 进行哈希
	* 使用 MAC
	* 加密 HTTP Cookies
* 第八章 公钥算法
	* 何时使用公约加密
	* Diffie-Hellman
	* 数字签名算法
	* RSA
	* EVP 公钥接口
	* 对象的编解码
* 第九章 在其他语言中使用 OpenSSL(略过)
	* Perl 与 Net::SSLeay
	* Python 与 MCrypto
	* PHP 与 OpenSSL
* 第十章 编程主题进阶
	* 对象的堆栈
	* 配置文件
	* X.509
	* PKCS#7 and S/MIME
	* PKCS#12
* 附录 A
* 后记

# 第一章 介绍

在当今的网络环境中，大多数的应用必须是安全的，加密技术是提供安全的主要工具之一
。加密技术的主要目标包括加密数据，保证数据完整，权限检验，问责机制，可以阻止绝
大多数类型的基于网络的攻击，包括窃听，IP地址欺诈，连接劫持，篡改数据。OpenSSL 
是一个加密库。它实现了工业界最好的算法的实现，包括诸如 3DES，AES 和 RSA 这样的
加密算法，也包括了报文摘要算法和消息认证码。

以一种安全可靠的方式使用加密算法比人们想象的要困难的多。加密协议是一个很难解决
的问题，算法只是加密协议的基石。密码学家们很难设计出一套可以阻止所有已知攻击的
协议。而且开发者往往会给出很糟糕的实现。举个例子，开发者经常依赖于发送前加密，
接收后解密这种简单方法来保护网络通信。这个策略在保证数据完整方面相当失败。很多
情况下，攻击者可以窃听甚至篡改数据。即使协议设计的很好，实现的错误也很常见。大
多数的加密协议的实用性很有限，如保护在线投票。然而，在不安全的媒介上进行安全通
信的协议具有普遍适用性。这是 SSL 协议，以及它的派生协议 TLS 的基本目标：用最少
的加密技术向任意的（基于 TCP 协议的）网络连接提供加密服务。

最终，如果开发者和管理员不需要知道任何关于加密甚至保护他们应用的事情，那就真是
太好了。如果在开发一个程序时，加密就像链接不同的套接字库一样简单，那就真是太好
了。 OpenSSL 尽可能朝着这个理想前进，而且在现实中，即使是 SSL 协议也需要很好的
理解安全原则，以便安全的使用。的确，大量使用了 SSL 的应用更易受到攻击。

尽管如此， SSL 确实让保护网络通信变简单了。使用 SSL 根本不需要知道加密算法是如
何运作的。相反，你只需要明白那些重要算法的基础特性。相似的，开发者也不需要担心
加密协议；使用 SSL 也不需要对其内部的工作方式有任何了解。你只需要了解如何正确地
应用该算法。

本书的目标是记录 OpenSSL 库以及如何正确的使用它。这本书是为从业人员写的，而不是
安全专家。我们将为您解释该如何高效的使用加密，但是我们不想为那些对密码学如何工
作感兴趣的人写一篇全面介绍。为此，我们向您推荐一本由 Bruce Schneier(John Wiley 
& Sons)编写的《 Applied Cryptography 》，我们还推荐 Menezes, van Oorschot 和 
Vanstone 编写的《Applied Cryptography》(CRC Press)。相同的，我们也不会记录 SSL 
协议本身，我们仅记录它的应用。如果你对协议的详细信息感兴趣，我们推荐
Eric Rescorla 的 《SSL and TLS》(Addison-Wesley)。

### 1.1 密码学初探

对于那些以前从未在工作中使用过密码学的人，本章节将为您介绍理解本书其余部分所需
要的基本原理。首先，我们先了解一下密码学旨在解决的问题，然后我们了解一下现代密
码学提供的原语。如果你有了解过密码学基础，你可以跳过该章节。

#### 1.1.1 密码学的目标

密码学最主要的目标就是保护那些在不安全媒介上传递的数据。通常，这些媒介是计算机
网络。

加密算法有很多种，他们都能向应用提供一种或多种下述服务:

##### 保密

即使数据在不安全的媒介上传递，它们也只能由正确的证书来解密。在实践中，这意味者
潜在的攻击者只能看见“被上锁”的混乱数据，而且如果他们没有正确的信息将无法解开这
些数据。在传统的加密算法中，编码过程是私密的。在现代的加密算法中，这是不可行的
。算法是公开的，而且加密秘钥被用在编解码的过程中。唯一需要保密的就是这个秘钥。
除此之外，在一些我们马上就要论证的常见的例子中，并不是所有的秘钥都需要保密。

##### 完整性（防篡改)

数据完整性的基本思想是，一段数据的接受者有办法确定这些数据在一段时间内是否被修
改过。举个例子，完整性检查可用于确保在线路上传输的数据在传输过程中没有被修改过
。许多知名的校验和可以发现甚至修正简单的错误。然而，这些校验和很难发现那些针对
数据的精心修改。一些加密校验和在使用得当的情况下没有这些缺点。但是要注意，这些
编码不能保证数据完整性。所有加密算法都会受到“位翻转”攻击。这是一种通过改变加密
位来改变真实数据的对应位的方法。

##### 身份验证

出于身份认证的目的，加密算法可以帮助验证身份。

##### 不可否认

加密算法可以使 Bob 证明他收到的来自 Alice 的消息是真的来自 Alice。当 Alice 向 
Bob发送一条消息时，她必须承担相应的责任，而且她也无法否认她曾经发过。在现实世界
中，你可以假设一个攻击者不会破坏独有的加密秘钥。SSL 协议不支持“不可否认”特性，
但是可以通过数字签名来很方便的实现。

这些服务可以用来对抗绝大多数的网络攻击，包括：

##### 偷听

攻击者可以监测网络流量，并记录感兴趣的数据，比如信用卡信息。

##### 篡改

攻击者监视网络流量，并且可以恶意修改传输中的数据（举个例子，攻击者可以修改邮
件的内容）。

##### 欺骗

攻击者可以伪造网络数据，让这些数据看起来像是来自不同的网络地址，而不是他真实的
地址。这种攻击方法可以用来阻挠基础主机信息进行身份验证的系统。

##### 劫持

一旦一个合法用户认证，欺诈攻击可以劫持这个链接。

##### 捕获重放

在一些环境中，攻击者可以记录和重放网络事务，从而造成不良影响。举个例子，你在高
价位出售了一股股票。如果网络协议是不安全的，攻击者可以记录这个交易，并且可以在
低价时重现它，往复循环直到你丢失了所有股票。

许多人认为这些攻击在现实生活中是行不通的。然而，这远非现实。特别是由于有dsniff
这样的工具集存在，只要能进入到两个端点之间的任意一个节点，就可以轻而易举的发动
上述的所有攻击。如果你在同一个本地网络中作为端点之一，攻击同样轻而易举。那些可
以通过别人的软件来入侵机器并操纵它们的天才高中生可以很轻松使用那些工具攻击真实
的系统。

传统上讲，诸如 HTTP，SMTP，FTP，NNTP 和 Telnet 这些网络协议都没有提供足够的防御
机制来应对这些攻击。在电子商务兴起于九十年代中期之前，安全并不是最大的问题，尤
其是考虑到互联网本身就是一个分享学术研究和资源的平台。虽然很多协议提供了基于密
码登陆的身份验证，但是他们中的大多数根本没有处理保密性和完整性。从而导致了所有
的攻击都是可能的。此外，身份验证信息通常可以被窥探到。

对于传统的网络协议来讲，SSL 简直就是一份巨大的福利，因为它可以为基于 TCP 的不安
全的协议添加透明的保密性和完整性服务。它也能提供身份验证服务，最重要的是客户端
可以确认它们正在和目标服务器同行，而不是一些在窥探服务器的攻击者。


#### 1.1.2 加密算法

SSL 协议可以满足大多数的加密需求。虽然有时候并不尽善尽美。举个例子，你可能希望
加密位于终端用户浏览器上的 HTTP cookies。 SSL 无法为存贮在磁盘上的文件提供保护
。对于这样的情况，OpenSSL 给出了用于实现 SSL 协议的底层加密算法。

一般来讲，你应该尽可能的避免直接使用加密算法。你不太可能只需选择一个算法就能获
得完全安全的系统并且还能使用它。通常，加密算法被封装到了加密协议中。一个基于加
密的协议会由大量的不明显的错误。这是为什么你应该去选择一个知名的加密协议来完成
你想做的，而不是你自己发明一些东西。事实上，即使是密码学家发明的协议也经常存在
漏洞。

如果没有公开审查，许多使用中的协议也是不安全的。想一下IEEE 802.11无线网络的WEP
协议。WEP 是一个应该向数据提供和物理线路相同级别的保护的协议。这是一个挑战，因
为数据在空气中传播，而不是通过线路。WEP 有一个经验丰富的程序员设计，但是没有征
求任何专业的密码学家或者安全协议开发人员的意见。虽然在一个有着中等安全知识的经
验丰富的开发人员眼里，这个协议看起来很好，但实际上，它完全缺乏安全性。

尽管如此，有时您可能会找到一个可以满足需求的协议，但是却找不到适合您需要的实现
。或者，您可能会发现您的确需要提出自己的协议。对于这些情况，我们编写了 SSL 的加
密 API。

本书会讨论物种加密算法：对称密钥加密，公钥加密，加密哈希算法，消息验证码，数字
签名。


##### 1.1.2.1

对称加密算法用相同的密钥加解密数据。加密数据可以在不安全的媒介中传递，只有持有
相同密钥的人才能解开这些数据，这是通过把密钥和密钥都传入解密算法实现的。显而易
见，只有当密钥仍然是私有的，这个方案才有效。

对称加密算法最主要的缺陷是密钥必须一直保持私有。特别的，替换密码是非常困难的，
因为您通常试图在使用加密保护的统意媒介上交互密钥。在使用该密钥之前，将该密钥以
明文发送出去，这可能会使攻击者在你发送数据之前就保存了这份密钥。

密钥分发问题的一个解决办法是使用密钥交换协议。OpenSSL 提供了 Diffie-Hellman 协
议已完成这个目的，该协议不用在网络上传递密钥就能完成密钥协商。然而，该协议不能
验证和你交换密钥的对端的身份。必须使用魔种身份验证机制来确保您不会和攻击者交换
密钥。

目前，DES3是最保守的对称加密算法。它被广泛使用，但是 AES，这份新的gap及加密标准
，将代替它成为使用最广泛的加密算法。AES 的确比 DES3 快，但是在很长的一段时间内
，对于那些偏执狂 DES3 仍然是最保守的选择。

值得一提的是现在的客户端和服务器大都支持 RC4。它比 DES3 要快，但却很难被正确安
装。对于那些不支持 AES 和 3DES 的软件的兼容问题，RC4非常感兴趣。如果没有一个好
的理由，我们不建议支持其他算法。出于兴趣，我们将在第六章讨论加密算法的选择。

#### 1.1.2.2 公钥加密
公钥加密算法解决了对称加密中的密钥分发问题。最流行的公钥加密方式是，每一方都有
两把钥匙，一个必须保密，另一个可以随意分发。两把钥匙有着很特殊的算术关系。如果
Alice 要对 Bob 发送消息，那她首先要有 Bob 的公钥。她要用 Bob 的公钥加密数据然后
发送。一经加密，则只有支持 Bob 密钥的人才能解开这份加密数据。

公钥加密算法解决了密钥分发问题，有一些办法可以找到 Bob 的公钥并确认这份公钥的确
属于 Bob。在实际生活中，公钥在传递使附带了一些被称为证书的支持信息，这些证书可
以在可信的第三方处验证。一般来说，可信的第三方一般是一个机构，它会核验那些希望
持有他们证书的人是否合法。SSL 通过可信的第三方来解决公钥分发问题。公钥加密算法
有一个显著的缺点，思考一下：如果数据过大，它将会慢的难以忍受。对称密钥加密通常
可以很快完成，以便加密和解密机器可以管理的所有网络通信量。加密算法一般会受限于
加密的速度，而不是进入计算机的带宽，特别是服务器，需要同时处理许多链接。

这导致了现在许多使用公钥加密算法的系统，包括 SSL，都尽可能的少使用它。一般来说
，公钥加密用于在对称算法上商定加密密钥，然后使用对称算法完成所有进一步加密。因
此公钥加密通常用于密钥交换协议和需要不可否认性时。

RSA 是最流行的公钥加密算法。Diffie-Hellman 密钥交换协议基于公钥技术，可以用来交
换对称密钥。为了使公钥方案更加有效，通常需要一个身份验证机制，该机制设计与加密
本身分离的可信第三方。在大多数情况下，数字签名会提供身份验证，我们后面会讨论。

公钥加密算法中的密钥实际上是一个有着许多特性的大数。因此，公钥密码中密钥的比特
长度不能直接与对称算法相媲美。在公钥加密算法中，你应该使用1024比特甚至更长的密
钥以确保安全。512比特的密钥有点靠不住。比2048比特大的可能会很慢而且也不会产生更
实用的安全性。最近，有人担心1024位键太弱了，但是在撰写本文时，还没有确凿的证据
。但对于短期攻击来说，1024比特是保证安全的最低限度了。如果你的密钥可能会使用数
年之久，你应该考虑使用2048比特的密钥。

当你选择公钥加密算法的密钥长度时，你也需要选择对称加密算法的密钥长度。建议经常
变化，但是我们建议，当你的对称密钥的长度小于100比特时，你可以使用1024比特的密钥
。如果你使用 3DES 或者 128比特的密钥，我们建议2048比特的公钥。如果你固执的使用
192比特或者更多，我们建议你使用4096的公钥。

如果你使用 ECC，密钥长度的要求会有一些变化。这是对公钥加密的修改，可以使用更快
的操作和较小的密钥提供相同数量的安全性。OpenSSL 还不支持 ECC，对于像使用它的人
或许会有一些挥之不去的专利问题。对于那些对该话题感兴趣的开发者，我们推荐 
Michael Rosing 写的 《Implementing Elliptic Curve Cryptography》。

##### 1.1.2.3 加密哈希算法和消息验证码
加密哈希算法本质是一个校验和算法，它有一个特殊的属性。你把数据传进哈希算法，它
会输出一段固定长度的校验和，这段校验和通常被称为消息摘要。传递给哈希算法的数据
相同则得到的结果相同。然而，这个结果并不会给出任何和输入相关的信息。此外，几乎
不可能找到产生相同消息摘要的两个输入。一般来说，当我们讨论这样的函数时，我们讨
论的是单向函数。单向函数是指，在任何环境下用结果和算法都无法构造出输入数据。当
然有可逆的散列函数，但我们不在本书的范围内考虑这些事情。

对于一般使用用途，最低安全的加密哈希算法应该有一个比最低安全对称加密算法两倍大
的摘要。MD5 和 SHA1 是最流行的单项加密算法。MD5 的摘要长度只有128比特，SHA1 有
160比特。对于某些用途，MD5 的摘要的长度是合适的，但是对于其他应用，这是有风险的
。为了安全，我们建议只是用摘要长度大于等于160比特的加密哈希算法，除非你需要支持
传统算法。另外，由于加密算法的一些弱点，MD5被广泛的认识是几乎不可用的。因为，我
们建议你在新应用中尽量避免使用 MD5。

加密哈希算法被应用到多种用途。他们经常被用作密码存储方案的一部分。在这种环境下
，登陆时会通过计算密码和一些额外数据的哈希摘要，然后将摘要和存储的值比对。那样
，服务器就不需要存储真正的密码，即使攻击者掌握了密码数据库，一个精心设计的密码
也是安全的。

人们也喜欢用加密哈希算法来发布他们的软件。举个例子，OpenSSL 可以计算一个资源的
MD5 值。当你下载这份资源时，你也能下载到这份校验和。然后你就可以计算下载资源的
校验，然后查看两份校验和是否匹配。你可以认为如果两份校验和匹配，那你得到的就是
真正的资源，并不会包含一个特洛伊木马。不幸的是，事实并非如此，因为没有放入任何
私密数据。攻击者可以替换资源，也可以替换对应的校验和，因为消息摘要算法是公开的
，没有放入任何私密数据。

如果你和软件发布者共享一份密钥，然后发布者将资源和密钥组合起来计算一份摘要，那
么在没有的情况下，攻击者就无法伪造资源了。使用加密哈希的方案，如在哈希中掺入密
钥，这种做法被称为消息验证码。消息验证码通常用来保证数据在传输过程中的完整性，
无论数据是否进行了加密。实际上，SSL 也用消息验证码实现这个目的。

使用最广的， SSL 和 OpenSSL 目前支持的消息验证码是 HMAC。HMAC 可以用于所有的消
息摘要算法。

##### 1.1.2.4 数字签名

对于许多应用，消息验证码并不是特别有用，因为它要求协商一个密钥。如果消息验证码
不需要共享一个密钥那就太好了。公钥加密算法使此成为可能。如果 Alice 用它密钥签发
一条消息，那么所有人都可以用它的公钥来验证她签发的这条消息。RSA 提供了数字签名
。实际上，公钥和私钥是可以互换的。如果 Alice 使用密钥加密了数据，那么人们可以用
公钥解开数据。如果没有使用密钥加密过，那么通过公钥解密将会得到一堆垃圾。

有一个非常流行的方案叫做 DSA，SSL 和 OpenSSL 都支持它。

像公钥加密一样，数字签名也非常慢。为了加速这个过程，这个算法并不会处理全部的数
据。这些数据会先经过加密哈希处理，然后签名这个哈希值。虽然如此，签名方案仍然是
非常昂贵的。处于这个原因，如果已经交换完了密钥，那么 MAC 还是略胜一筹。

数字签名的一个广泛应用是证书管理。如果 Alice 想使 Bob 的证书合法化，她可以用自
己的私钥签名这个证书。一旦签名完成，Bob 把这份签名附着在证书上。现在，让我们假
设他会把这份证书给 Charlie，但是 Charlie 无法验证来自 Bob 的证书，但是他相信
Alice。在这种情况下，Charlie 验证 Alice 的签名，从而证明该证书确实属于 Bob。

由于数字签名是公开密钥加密的一种形式，所以必须确保密钥长度为1024位或更高，以确
保安全性。

### 1.2 SSL 概览

SSL 是目前使用最广泛的安全协议。它是 HTTPS 协议的基础，是在 WEB 浏览器角落中产
生一把小锁的原因。SSL 能够保护任何基于 TCP 的协议。

一个 SSL 事务开始于客户端向服务器发送一个握手协议。在服务器的响应中，包含了服务
器的证书。向之前提到的，证书包含了分配给服务器的公钥和其他一些有趣的东西，比如
证书的持有者，过期时间，与服务器相关联的完全限定域名。

在连接期间，服务器通过使用私钥来成功解密客户端用服务器公钥加密的挑战协议，从而
证明其身份。客户端需要接收正确的未加密的数据以继续后续流程。因此，证书可以保持
公开状态 ———— 一个攻击者需要一份证书的副本，还需要相关密钥才能伪装成已知的服务
器。

然而，攻击者可以一直拦截服务器消息，并且出示攻击者自己的证书。伪造证书的数据字
段看上去是合法的（比如分配给服务器的域名，和与证书相关联的实体的名称）。在这种
情况下，攻击者可能会建立与预期服务器的代理连接，然后窃听所有数据。这种攻击被称
为中间人攻击。为了完全击败中间人攻击，客户端不能只核验服务器的证书，而且还要有
某种方式来确定证书本身是否可信。确认可信度的一个方法是把有效证书的列表硬编码到
客户端。这个解决方案的问题是它不可伸缩。想象一下，在您开始浏览之前，需要安装所
有的你需要访问的安全的 HTTP 网站的证书。这个问题的实际解决方案是让可信的第三方
负责保存有效证书的数据库。这个第三方被称为证书颁发机构，用它自己的私钥签发合法
的服务器证书。这个签名表面证书颁发机构已经对所颁发证书的实体进行了背景检查，从
而在一定程度上确保证书呈现的数据是准确的。证书中包含了这个签名，并在连接时使用
。

客户端如果有证书颁发机构的公钥，就可以验证证书颁发机构的签名。如果检查成功，客
户端就可以确信这份证书的持有者是一个第三方已知的实体，也可以检查储存在证书的数
据，比如证书是否过期。虽然罕见，服务器也可以向客户端索取一份证书。在证书验证完
成之前，客户端和服务器还要协商使用何种加密算法。在证书验证之后，客户端和服务器
使用安全密钥协商协议协商对称密钥（使用对称密钥加密算法传输数据）。一旦所有的协
商都完成，客户端和服务器就可以任意交换数据了。

SSL 协议的细节有点麻烦。消息验证码被广泛的用于确保数据完整性。另外，在证书验证
期间，一方可以去证书颁发机构获取证书撤销列表，以确保看上去有效的证书的确没有被
偷取。

我们不会深究 SSL 协议的细节（或者它的继承者，TLS）。对于我们的目的，我们可以把
这一切都当作黑盒对待。如果您对细节感兴趣，您可以阅读 Eric Rescorla 的
《SSL 和 TLS》。

### 1.3 SSL 的问题

SSL 是一份卓越的协议。像许多工具一样，在一个了解如何更好使用它的人手里，它是极
有效，但也很容易误用。在部署 SSL 的过程中，有许多容易掉进去的陷阱，但只要做极少
的工作就可以避免其中的大部分。

#### 1.3.1 性能

SSL 比传统的未加密的 TCP/IP 链接要慢很多。这个问题是提供足够安全的直接结果。当
一个新的 SSL 会话建立时，服务器和客户端交换会交换大量的数据，这些数据被用来互相
验证，并为该会话协商一个密钥。这种初始握手使用了大量公钥加密技术，正如我们前面
提到的那样，速度非常慢。这也是使用 SSL 时最大的减速。在当前的高端 PC 硬件中，
OpenSSL 在实际工作负载下最多每秒 100 个链接。

一旦初始握手完成并建立会话，开销就会显著减少，不过和不安全的 TCP/IP 链接进行比
较，还有一些额外开销仍然存在。具体来说，比正常传输更多的数据。数据是在包中传输
的，包中包含 SSL 协议所需要的信息，以及所使用的对称密码所要求的数据填充。当然，
也有加密和解密数据的开销，但好消息是使用了对称密码，所以它通常不是一个瓶颈。对
称加密的效率取决于使用何种算法以及密钥的长度。然而，即使是最慢的算法也足够有效
了，所以这根本不是瓶颈。因为公钥加密的效率不足，当人们意识到 SSL 并不能处理足够
的负载，人们决定不使用它。一些人完全不适用加密，这显然不是好主意。作为补偿，一
些人选择设计自己的协议。这也是坏主意，因为会有很多并非显而易见的陷阱围攻你。不
是由一个熟练的密码学家设计的协议不可避免的会有一些问题。SSL 的设计是相当高效的
；它根本不愿意牺牲安全来提高速度。您应该对使用更高效的协议持怀疑态度。

不用放弃这个协议也会很多办法解决问题。SSL 支持会话复用机制，使得客户端在断链之
后可以更快的重连，而不需要承受建立链接的全部开销。对于 HTTP 来说这很有用，但是
对于其他协议不会太有效。

##### 1.3.1.1 加密加速硬件

比较普遍的加速 SSL 的方法是使用加速硬件。许多供应商提供 PCI，可以为你的处理器减
轻加密操作的负担，OpenSSL 支持大多数加速硬件。我们将会在第四章讨论加速硬件。

##### 1.3.1.2 负载均衡

另一个解决 SSL 的效率问题的流行方式是负载均衡，透明的分配链接至多台计算机，使得
这一组机器像一台机器一样对外提供服务。和加速芯片相比，这是一个花费巨大的解决方
案，特别是你已经有了一地的硬件。通常，负载均衡要求更多的工作来确保持久化数据可
以被所有的后端服务器获取到。另一个负载均衡的问题是，许多解决方案把新链接路由到
任意的机器上，这回极大的削弱链接服用的益处，在重连是只会由一小部分客户端链接到
原始的机器上。

一个简单的负载均衡方案是 round-robin DNS，将多个 IP 地址分配给一个 DNS 名称。在
对DNS查找的响应中，DNS服务器在发出相同的地址两次之前，遍历了DNS名称的所有地址。
这是一个流行的解决方案，应为它开销低，并且不需要额外的硬件。由于机器倾向于保持
对DNS结果的短期记忆，所以链接复用可以很好的工作。这个解决方案的一个问题是DNS服
务器处理负载管理，并且不考虑单个服务器上的实际负载。此外，大型 ISP 可以执行 DNS
缓存，从而导致负载分配不均。要解决这个问题，必须经常将条目设置为过期，这会增加
DNS服务器的负载。

硬件负载均衡器的价格和功能各不相同。那些能够记住外部机器，并将来自同一台机器它
多台链接都映射到同一台内部机器上的硬件通常都会很贵，不过对 SSL 很有效。

0.9.7版本的 OpenSSL 添加了新特性，允许应该通过操纵会话 ID 来处理负载均衡。会话
是 SSL 连接操作参数的子集，我们将在第5章中详细讨论。

### 1.3.2 清除密钥

在典型的 SSL 安装中，服务器维护凭据，以便客户端可以对服务器进行身份验证。 除了
连接时提供的证书之外，服务器还维护一个私钥，这对于确定提供证书的服务器实际上呈
现自己的证书是必需的。私钥保存在服务器某个地方。最安全的解决方案是使用加密加速
硬件。大多数设备可以产生并储存私钥，并且即使攻击者已经进入了机器，也不能访问到
密钥。为了实现这个目的，密钥只能在芯片上使用，除非特殊的环境，否则密钥不能离开
芯片。有种情况下，硬件解决方案同样不可行，如果一个攻击者获取的 root 权限，没有
一种绝对安全的办法来保护私钥，因为，在处理新链接的时候，密钥在内存中必须是未加
密的。如果攻击者有 root 权限，他可以将调试器附着到服务器进程，并提取未加密的密
钥。

这种情况下有两个选择。第一个，你可以把未加密的密钥放到磁盘上。这是最简单的解决
方案，但也让可以物理接触机器的攻击者的工作变得轻松。他可以切断机器电源并拿走硬
盘，或者重引导到但用户模式。另一个可选的方式是，你可以通过口令加密在磁盘上的密
钥，在启动 SSL 服务时，管理员必须输入口令。在这种情况下，密钥只在服务器进程的地
址空间中未加密，因此对于关闭计算机并直接访问磁盘的用户无法得到密钥。

# 第五章 SSL/TLS 编程

OpenSSL 库最重要的特性就是它实现了 SSL 和 TLS。最开始的时候是 Netscape 为了 WEB
事务开发的，但这个协议逐渐成长为加密流通信的通用解决方案。Netscape 的发布的第一
版 SSL 是 SSLv2。从那时起，安全专家们开始致力于修正 SSLv2 的一些瑕疵，这使得
SSLv2 得以问世。同时，基于 SSL 的传输层安全协议也开发完成了，也就是 TSLv1。因为
SSLv2有着这样或那样的问题，所以现在的应用不应该支持它。在这一章中，我们仅仅讨论
如何使用 OpenSSL 支持的 SSLv3 或者 TSLv1 进行编程。除非特别之处，否则当我们提及
SSL时，是指的 SSLv3 和 TLSv1。

从设计的角度出发，除了在我们应用中用到的 SSL 相关的知识，我们还应该了解一些其他
的。由于协议设置的复杂性，大量的API，和开发者对库函数缺乏经验，所以一份使用 SSL
的程序的正确实现是非常困难的。OpenSSL 的 SSL 支持部分模仿了 Unix 的套接字接口。
然而，当我们开始研究 API 的精华之处时，这些类似就显得微不足道了。为了使熟悉大量
库函数的过程变得简单，我们举了一个简单的例子，例子演示了客户端和服务端如何一
步步的建立基于 SSL 的安全链接。为了帮助理解，我们做了一些现实世界中的程序不会出
现的简单假设。

从这一点出发，我们将逐步缩小与 OpenSSL 的更新进的特性的差距。本章的目标是把库函
数的特点划分成更小的单元以便于建立起对程序的理解。我们希望当开发人员在他们自己
的应用中使用 SSL 时，这份程序会成为他们实现的模板。在向应用程序添加 SSL 时，必
须考虑应用程序的独特需求，必须对安全性和功能性做出最佳决策。

## 5.1 使用 SSL 编程

OpenSSL 中关于 SSL 的 API 对于一个没有经验的开发者来说是非常巨大且令人沮丧的。
此外，正如在第一章中提到的，如果实现不正确，SSL 的安全目标根本无法实现。这些因
素使得开发人员难以完成任务。为了理清实现安全程序的神秘面纱，我们将这个问题分解
三步。在每一步中，开发者都需要向 SSL 提供必须要的信息，以使 SSL 完成它的工作。
举个例子，一个高度兼容的开发者和一个高安全服务应用的开发者所做的完全不同。

下面的步骤为开发人员在实现SSL客户端或服务器时遵循的模板提供了一个模板。我们从一
个小例子开始，并以它为基础。在所有的步骤都经过深思熟虑之前，这个例子的安全性并
不能满足我们。在每一步中，我们将引入一小部分 API。在所有的步骤之后，开发者应该
能更清楚的全盘思考如何去设计一个内置 SSL 的应用。在完成所有的步骤之后，路还有很
长。为了满足许多应用程序的要求，我们应该走的更远，应该去看看 API 的高级特性。

### 5.1.1 加密应用程序

我们先使用两个很简单的程序：一个客户端和服务器，服务器只简单的将来自客户端的数
据传给控制台。我们的目标是不断改善这两个程序，以使得他们能在敌对的环境中完成任
务。换句话说，我们将使两个程序都对对端进行严格的身份链接。在我们逐步实现内置
SSL 的程序的过程中，我们将讨论开发者在每个阶段所必须做的抉择。

在开始之前，让我们先看一下我们的程序。总共有四个文件：common.h， common.c，
client.c 和 server.c 。所有的代码都在 例5-1 到 例5-4 中。我们也会用 例4-2 中的
代码，以使得我们可以使用多线程。在 Unix systems，我们会继续使用 POSIX 线程。

在 例5-1 中展示的 common.h，第一至五行包含了 OpenSSL 有关的头文件。现在我们不会
用到这些头文件中的任何代码，但是我们在后面会用到，所以把他们包含进来了。第22至
24行定义了客户端和服务器会用到的字符串和服务器监听的端口。另外，为了方便错误处
理和像第四章那样为了线程是平台无关的，头文件还包含了一些定义。

```

例5-1

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>

#ifndef WIN32
#include <pthread.h>
#define THREAD_CC
#define THREAD_TYPE pthread_t
#define THREAD_CREATE(tid, entry, arg) pthread_create(&(tid), NULL, \
(entry), (arg))
#else
#include <windows.h>
#define THREAD_CC _ _cdecl
#define THREAD_TYPE DWORD
#define THREAD_CREATE(tid, entry, arg) do { _beginthread((entry), 0,
(arg));\
(tid) =
GetCurrentThreadId(); \
} while (0)
#endif

#define PORT "6001"
#define SERVER "splat.zork.org"
#define CLIENT "shell.zork.org"

#define int_error(msg) handle_error(__FILE__, __LINE_ _, msg)
void handle_error(const char *file, int lineno, const char *msg);

void init_OpenSSL(void);

```

例5-2中的 common.c，定义了我们的错误报告函数 handle_error。我们例子中的错误处理
有一点严厉，在你自己的应用例你或许会用一些对用户有好的错误处理方式。一般来说，
通过使应用程序终端处理所有可能的错误是不合适的。

common.h 也定义了一个函数，这个函数完成诸如 设置 OpenSSL 的多线程，初始化库函数
，加载错误字符串等任务。调用 SSL_load_error_strings 已加载错误码的相关数据，这
样当错误发生的时候，就可以打印错误堆栈了，我们可以得到易于阅读的关于错误的信息
。加载这些诊断用的字符串会占用内存，当我们开发一些诸如内存受限的内置系统或者机
器的时候就不会加载这些字符串。一般来说，加载这些字符串是一个好主意，因为这会使
调试变得简单。

当我们在添加 SSL 相关功能时，common.c 里有客户端和服务器都需要的函数的定义，函
数的声明在 common.h。

```
例5-2

#include "common.h"

void handle_error(const char *file, int lineno, const char *msg)
{
	fprintf(stderr, "** %s:%i %s\n", file, lineno, msg);
	ERR_print_errors_fp(stderr);
	exit(-1);
}

void init_OpenSSL(void)
{
	if (!THREAD_setup() || ! SSL_library_init())
	{
		fprintf(stderr, "** OpenSSL initialization failed!\n");
		exit(-1);
	}
	SSL_load_error_strings();
}
```

客户端的大部分代码——client.c，在例5-3中。它创建了一个连向服务器6001端口的链接，
这个端口定义在 common.h 中。当一个链接建立后，它开始从标准输入读入数据知道遇到
EOF。当数据被读入并被填到缓冲区后，数据将通过链接被发送到服务端。这里应该注意一
下，虽然我们使用了 OpenSSL 进行套接字传输，但我们仍然没有使用 SSL 协议。

第27到29行创建了一个 BIO 对象，包含了一个从 BIO_s_connect 返回的 BIO_METHOD；调
用 BIO_new_connect 可以很轻松的完成这个任务。只要没有错误发生，31到32行将进行
TCP 链接并且检查错误。当一个链接建立完成后，会调用 do_client_loop，这个函数会一
直从标准输入读取数据并写入到套接字。当发生错误或者从控制台读到 EOF，这个函数会
退出并且程序会终止。

```
例5-3

#include "common.h"

void do_client_loop(BIO *conn)
{
	int err, nwritten;
	char buf[80];

	for (;;)
	{
		if (!fgets(buf, sizeof(buf), stdin))
		break;
		for (nwritten = 0; nwritten < sizeof(buf); nwritten +=err)
		{
			err = BIO_write(conn, buf + nwritten, strlen(buf)
				- nwritten);
			if (err <= 0)
			return;
		}
	}
}

int main(int argc, char *argv[])
{
	BIO *conn;

	init_OpenSSL();

	conn = BIO_new_connect(SERVER ":" PORT);
	if (!conn)
	int_error("Error creating connection BIO");

	if (BIO_do_connect(conn) <= 0)
	int_error("Error connecting to remote machine");

	fprintf(stderr, "Connection opened\n");
	do_client_loop(conn);
	fprintf(stderr, "Connection closed\n");

	BIO_free(conn);
	return 0;
}
```

