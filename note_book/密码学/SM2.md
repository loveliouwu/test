# SM2

## 名词意义

1. SM2
>一种椭圆曲线密码算法，密钥长度为256比特

2. ECB
> 电码本模式

3. SM2私钥
> SM2私钥是一个大于或等于1且小于n-1的整数(n为SM2算法的阶)，简单记为k，长度为256位

4. SM2公钥
> SM2公钥是SM2曲线上的一个点，由横坐标和纵坐标两个分量来表示记为(x,y)，简记为Q,每个分量的长度为256位。

5. 密钥数据格式
> SM2Publickey为BIT string类型，内容为 04||X||Y

6. SM2算法加密后的数据格式为ASN.1定义为：
```C
SM2Cipher::=SEQENCE{
        X;              INTEGER                 x分量
        Y;              INTEGER                 y分量
        HASH            OCTET STRING SIZE[32];  杂凑值
        CipherText      OCTET STRING            密文
}
```
其中HASH用的是SM3算法，固定长度32字节，CipherText的长度是跟明文长度相等

7. SM2算法签名数据格式的ASN.1定义为：
```C
SM2Signature::={
        R       INTERGER
        S       INTERGER
}
```
R,S的长度各为256位

8. 密钥对保护数据格式
> 在SM2密钥对传递时，需要对SM2密钥对进行加密保护，具体的保护方法为：
1.1 产生一个对称密钥
1.2 按对称密码算法标示指定的算法对SM2私钥进行加密，得到私钥的密文，若对称算法为分组算法，则其运算模式为ECB。
1.3 使用外部SM2公钥加密对称密钥得到对称密钥密文
1.4 将私钥密文、对称密钥密文封装到密钥对保护数据中



### 密钥协商
> 密钥协商是在两个用户之间建立一个共享秘密密钥的协商过程，通过这种方式能够确定一个共享秘密密钥的值。
设密钥协商双方为A,B,其密钥对分别为(dA,QA)和(dB,QB),双方需要获得的死要数据的比特长度为klen，密钥协商协议分为两个阶段。
- 第一阶段：产生临时密钥对
> 用户A:  
调用生成密钥算法产生临时密钥对(rA,RA),将RA和用户A的用户身份表示IDA,发送给用户B  

> 用户B: 
调用生成密钥算法产生临时密钥对(rB,RB),将RB和用户B的用户身份表示IDB,发送给用户A 

- 第二阶段：计算共享秘密密钥
> 用户A: 
输入参数： 
```C
        QA      SM2PublicKey    用户A的公钥
        QB      SM2PublicKey    用户B的公钥
        RA      SM2PublicKey    用户A的临时公钥
        IDA     OCTET STRING    用户A的用户身份标识
        RB      SM2PublicKey    用户B的临时公钥
        IDB     OCTET STRING    用户B的用户身份标识
        dA      SM2PrivateKey   用户A的私钥
        rA      SM2PrivateKey   用户A的临时私钥
        klen    INTEGER         需要输出的密钥数据的比特长度  
```
步骤：  
1. 用IDA和QA作为输入参数，调用预处理1得到ZA
2. 用IDB和QB作为输入参数，调用预处理1得到ZB
3. 以klen、ZA、ZB、dA、rA、RA、QB、RB为输入参数，进行运算得到K

> 用户B: 
输入参数： 
```C
        QB      SM2PublicKey    用户B的公钥
        QA      SM2PublicKey    用户A的公钥
        RB      SM2PublicKey    用户B的临时公钥
        IDB     OCTET STRING    用户B的用户身份标识
        RA      SM2PublicKey    用户A的临时公钥
        IDA     OCTET STRING    用户A的用户身份标识
        dB      SM2PrivateKey   用户B的私钥
        rB      SM2PrivateKey   用户B的临时私钥
        klen    INTEGER         需要输出的密钥数据的比特长度  
```
输出为K OCTET STRING 位长为klen的密钥数据





密钥交换协议：
用户A： 
1. 产生随机数rA,属于(1,n-1)
2. 计算RA = rA*G = (x1,y1)
3. 将RA发送给用户B
注：自己要保存rA,因此需要相应结构体存储

密钥交换需要的结构体
```C
typedef struct sm2_exchange_ctx_st{
        EC_POINT pub_key;
        BIGNUM priv_key;

}SM2_EXCHANGE_CTX
```

```C
int interface_1(EC_GROUP *group, SM2_EXCHANGE_CTX *exc_ctx, unsigned char *pubkey);
{
        /* 伪代码流程 */
        BIGNUM k = NULL;
        EC_POINT *pub_key_point;
        EC_POINT *group_g;

        BN_init(&k,NULL,BN_D_MUM);
        group_g = EC_POINT_new(group);
        pub_key_point = EC_POINT_new(group);


        /* 1. 随机数发生器产生随机数rA */
        rand_bytes_generate((unsigned char *)k.d,32);//生成32字节随机数，因为sm2密钥长度为256
        //检测随机数范围[1,n-1]

        /* 2. 计算椭圆曲线点RA */
        EC_POINT_general2montp(group,group_g,&group->G);//基点转蒙域
        if(!EC_)
}
```