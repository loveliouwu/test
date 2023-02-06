### DER(Distinguished Encoding Rules 可辨别编码规则)
ASN.1本身只定义了表示信息的抽象句法，但是没有限定其编码的方法。各种ASN.1编码规则提供了由ASN.1描述其抽象句法的数据的值的传送语法（具体表达）。标准的ASN.1编码规则有基本编码规则（BER，Basic Encoding Rules）、规范编码规则（CER，Canonical Encoding Rules）、唯一编码规则（DER，Distinguished Encoding Rules）、压缩编码规则（PER，Packed Encoding Rules）和XML编码规则（XER，XML Encoding Rules）。为了使ASN.1能够描述一些原先没有使用ASN.1定义，因此不适用上述任一编码规则的数据传输和表示的应用和协议，另外制订了ECN来扩展ASN.1的编码形式。ECN可以提供非常灵活的表明方法，但还没有得到普遍应用   

ASN.1与特定的ASN.1编码规则一起通过使用独立于计算机架构和编程语言的方法来描述数据结构，为结构化数据的交互提供了手段，特别是在网络环境的应用程序

- 一个标准的ASN.1编码对象有四个域：对象标识域、数据长度域、数据域、结束标志（可选，长度不可知情况下需要）

### 对象标识域 
对象标识域有两种形式，低Tag数字（Tag值再0到30）和高Tag数字（Tag大于30）形式。   
低Tag数字形式只有一个字节，包含三部分，从低位1开始编号，8和7位是Tag类型，共有四种，分别是`universal(0 0)`、`application(0 1)`、`context-specific(1 0)`和`private(1 1)`,第6位是0， 表明编码类型是基本类型，第5-1位是Tag值。


### 数据长度域  
数据长度域也有两种形式，短形式和长形式。
短形式的数据长度域只有一个字节，第8位为0，其它低7位给出数据长度。
长形式的数据长度域有2到127个字节。第一个字节的第8位为1，其它低7位给出后面该域使用的字节的数量，从该域第二个字节开始给出数据的长度，基于256，高位优先。

### 数据域  
数据域给出了具体的数据值


### ASN.1基本类型  
- NULL ： 只包含一个值NULL，用于传送一个报告或者作为CHOICE类型中某些值
- INTEGER ： 全部整数，包括正数和负数 
- REAL ： 实数，表示浮点数 
- ENUMERATED ： 标识符的枚举  
- BIT STRING : 比特串  
- OCTET STRING ： 字节串 
- OBJECT IDENTIFIER / RELATIVE-OID : 一个实体的标识符
- EXTERNAL, EMBEDDEDPDV : 表示层上下文交换类型  
- XXXXString : 各种字符串  
    - NumericString:
    - PrintableString:
    - VisibleString:
    - ISO64String:
    - IA5String:
    - TeletexString:
    - T61String:
    - VideotexString:
    - GraphiteString:
    - GeneralString:
    - UniversalString:
    - BMPString:
    - UTF8String:
- CHARACTER STRING ： 允许为字符串协商一个明确的字符表  
- UTCTime、GeneralizedTime : 日期



### 组合类型  
- CHOICE: 选择类型，该字段可能有多重不同的类型来表示  
- SEQUENCE: 由不同类型的值组成一个有序的集合  
- SET：无序集合  
- SEQUENCEOF: 由相同类型的值组成一个有序的集合  
- SETOF： 无序集合，相同类型的值


### 类型定义 
`<新类型的名字> ::= <类型描述>`  

- 注： `新类型的名字`是一个以**大写**字母**开头**的**标识符**，  `类型描述`是基于内建类型或在其他地方定义的类型  
```asn
Married ::= BOOLEAN
Age ::= INTEGER
Picture ::= BIT STRING
Form :: SEQUENCE
{
    name  PrintableString,
    age  Age,
    married Married,
    marriage-certificate Picture OPTIONAL
}

Payment-method ::= CHOICE
{
    check Check-number
    credit-card SEQUENCE
    {
        number Card-number,
        credit-card SEQUENCE
        {
            number Card-number,
            expiry-date Date
        }

    }
}
```
**注意在`SEQUENCE`和`SET`等定义中，最后一个成员结尾没有逗号','**  

- 为了准确描述一个类型，我们需要对值的集合进行一定的限制，这用到了`子类型约束`，在类型之后用圆括号进行标识。   
如：  
```asn
Lottery-number ::= INTERGER(1..49)
Lotter-draw ::= SEQUENCE SIZE(6) OF Lottery-number  
Upper-case-word ::= IA5String(FROM("A".."Z"))
Phone-number ::= NumericString(FROM("0".."9"))(SIZE(10))
```

### 值定义  
`<新的值名字><该值的类型> ::= <值描述>`  
- 新的值的名字是以小写字母开头的标识符  
- 值的类型可以是一个类型的名字， 也可以是类型描述  
- 值描述 是基于整数、字符串、标识符的组合。



### 模块定义  
一般协议 有一个或者多个模块组成，模块用来收集数据结构定义，模块必须以大写字母开头，能以一种全局指针的方式来引用，成为对象标识符，用花括号标识在名字之后。  
```ASN1
<模块名字> DEFINITIONS <缺省Tag> ::= 
BEGIN 
EXPORTS <导出描述>
IMPORTS <导入描述>
<模块体描述>
END
```  


### BER基本规则  
BER是ASN.1最早定义的编码规则，BER传输语法的格式一直是TLV三元组<Typt, Length, Value>,也叫做<Tag, Length, Value>。TLV每个域都是一系列八位数，对于组合结构，其中V还可以是TLV三元组 
BER编码规则与机器无关，而且可以支持各种不同的整数长度，能够很好的保存抽象语法结构，BER对布尔值的显示更加的友好，对于BOOLEAN类型显示为'TRUE'或'FALSE'，而不是码字  
缺点是比较冗长。


### 注释 
```ASN.1

**单独一行的注释格式--

IpAdress ::=OCTET STRING(SIZE(6))   --行尾注释格式  

```