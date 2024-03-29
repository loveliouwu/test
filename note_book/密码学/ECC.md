# ECC概念总结

## 名词术语

### **椭圆曲线密码学的两种公钥算法**
>
用于加密的ECDH和用于数字签名的ECDSA
>

### **Diffie-Hellman密钥交换协议/算法**

>
该算法限于密钥交换的用途，常用做密钥交换技术。这种密钥交换技术的目的在于使得两个用户安全地交换一个秘密密钥以便以后的报文加密，Diffie-Hellman密钥交换算法的有效性依赖于计算离散对数的难度。
>
>
定义离散对数：首先定义一个素数p的原根，为其各次幂产生从1 到p-1的所有整数根，也就是说，如果a是素数p的一个原根，那么数值 a mod p,a^2 mod p,...,a^(p-1) mod p 是各不相同的整数，并且以某种排列方式组成了从1到p-1的所有整数. 对于一个整数b和素数p的一个原根a，可以找到惟一的指数i，使得 b = a^i mod p 其中0 ≤ i ≤ （p-1） 指数i称为b的以a为基数的模p的离散对数或者指数.该值被记为inda,p(b).
>
#### **算法描述**
>
1. 有两个全局公开的参数，一个素数q和一个整数a，a是q的一个原根
2. 假设用户A和B希望交换一个密钥，用户A选择一个作为私有密钥的随机数`XA（XA < q）`，并计算公开密钥`YA = a^XA mod q`，A对XA的值保密存放而使YA能被B公开获得。类似的，用户B选择一个私有的随机数`XB<q`，并计算公开密钥`YB = a^XB mod q`，B对XB的值保密存放而使YB能被A公开获得。
3. 用户A产生的共享秘密密钥的计算方式是`K = (YB)^XA mod q`，同样，用户B产生共享秘密密钥的计算是`K = (YA)XB mod q`，这两个计算产生相同的结果：`K = (YB)^XA mod q = (a^XB mod q)^XA mod q = (a^XB mod q)^XA mod q = (根据取模运算规则得到) a^(XAXB) mod q = (a^XA)^XB mod q = (a^XA mod q)^XB mod q = (YA)^YB mod q`,因此相当于双方已经交换了一个相同的秘密密钥。
4. 因为XA和XB是保密的，一个敌对方可以利用的参数只有q、a、YA和YB，因而敌对方被迫取离散对数来确定密钥。当AB双方都获取公开密钥之后，各自通过计算得到双方共享的秘密密钥`K`。
>

### **ECDH**

>
ECDH是椭圆曲线的笛福赫尔曼算法的变种，它其实不单单是一种加密算法，而是一种密钥协商协议，也就是说ECDH定义了密钥怎么样在通信双方之间生成和交换。
>

### **ECDHE**
>
ECDHE中的E代表着[短暂的]，是指交换的密钥是暂时的动态的，而不是固定的静态的。在TLS中就使用了ECDHE，连接建立时，服务器和客户端都动态生成公私钥。
>

### **DSA 数字签名算法**
>
>

### **ECDSA**
>
ECDSA是DSA作用于椭圆曲线的一个变种算法，ECDSA需要使用明文的哈希结果，而不是明文本身，哈希函数的选择取决于使用者，为了是哈希结果的比特长度和n（子群的阶）的比特长度一致，消息的哈希结果需要被截断，被截断后的哈希值会是一个整数，我们用`Z`表示
>

#### **ECDSA签名步骤**
>
1. 在`{1，...，n-1}`范围内选取一个随机数k（n是子群的阶）
2. 计算点`P = kG`（G是子群的基点）
3. 计算数字`r = xp mod n`(xp是P的x轴坐标)
4. 如果`r = 0`,另选一个k并重新计算
5. 计算`s = k^(-1) * (z + rda) mod n`(da是A的私钥，k^(-1)是k mod n的乘法逆元)
6. 如果`s = 0`，另选一个k并重新计算
7. (r,s)就是签名，（一般情况，最后的结果r和s是用asn1格式封装的，至少的TLS签名和数字证书签名中是这样的，不是简单的r+s这样字节直接拼接）
>
>
通俗的说，这个算法一开始生成了k，得益于点乘k被隐藏在了r中，然后通过等式`s = k^(-1) * (z + rda) mod n`将r绑定到了消息散列值上
为了计算s，我们必须计算k的逆 mod n,只有在n是素数的情况下才能保证这一过程，如果子群的阶不是一个素数，ECDSA将不起作用，几乎所有标准的曲线都是素数阶的。
>

#### **验证签名**

>
为了验证签名，我们需要 A 的公钥 `HA` ，被截断的哈希值 `z`，还有签名 `(r,s)`
>
1. 计算整数`u1 = s^(-1) * z mod n`
2. 计算整数`u2 = s^(-1) * r mod n`
3. 计算点`P = u1*G + u2 * HA`
4. 只有当`r = xp mod n`的时候签名才被成功验证
>
>
验签公式推导
>
>
```txt
P = u1G + u2 * HA
  = u1G + u2 * dA*G
  = (u1 + (u2 * dA)) * G

u1 = s^(-1) * z mod n
u2 = s^(-1) * r mod n

P = (u1 + (u2 * dA)) * G
  = ((s^(-1) * z) + (s^(-1) *r * dA)) * G
  = (s^(-1) * (z + r * dA)) *G

s = k^(-1) * (z + r * dA) mod n
等式两边同乘以`k`再同除`s`得到
k = s^(-1) * (z + r *dA) mod n
代入
P = s^(-1)(z + r *dA) * G
  = kG
```
>