- 根据国密标准，一种设备类型可以有多个设备，每个设备内可以有多个应用，每个应用里可以有多个容器，每个容器里可以有一对证书，签名证书和加密证书 

## SKF_EnumDev(BOOL bPresent, LSPTR szNameList, ULONG *pulSize)  
调用这个方法用来遍历电脑上的设备，这个方法的第一个参数一般传`TRUE`，表示遍历的是插上的设备。第二个参数就是返回的设备名称列表。第三个参数是设备名称列表缓冲区长度。   
按照惯例，这个方法应该被调用两次：  第一次，szNameList传NULL，pulSize返回长度；第二次，给szNameList分配pulSize长度，返回设备列表，每个设备的名称以单个`'\0'`结束，以`双'\0'`表示列表的结束。    

## SKF_ConnectDev(LSPTR szName, DEVHANDLE *phDev)     
通过循环调用这方法用来链接每一个具体的设备，返回phDev为设备句柄。   

## SKF_EnumApplication(DEVHANDLE hDev, LSPTR szAppNameList, ULONG *pulSize);
得到设备句柄后，再通过此方法枚举得到设备里的应用列表，hDev为链接设备时返回的设备句柄，saAppNameList返回应用名称列表，pulSize是列表缓冲区长度，这个方法也是`照例要调用两次`。同样的'\0'结束规则  

## SKF_OpenApplication(DEVHANDLE hDev, LSPTR szAppName, HAPPLICATION *phApplication)  
可循环打开设备里的应用，返回应用句柄。   

## SKF_EnumContainer(IN HAPPLICATION hApplication, OUT LPSTR szContainerNameList, OUT ULONG *pulSize)  
得到应用句柄后，我们就可以用此方法遍历应用中的所有容器，用法与上面两个枚举类似。  

## SKF_OpenContainer(HAPPLICATION hApplication,LPSTR szContainerName,HCONTAINER *phContainer)  

## SKF_ExportCertificate(HCONTAINER hContainer, BOOL bSignFlag, BYTE* pbCert, ULONG *pulCertLen)  
通过这个方法获取每个容器里的证书，bSignFlag为导出的证书类型，**TRUE表示导出的是签名证书，FALSE表示导出的是加密证书**,`pbCert`为返回的证书数据，`pulCerLen`是整数的长度，同样需要调用两次。


# 数字签名  
在数字签名时，要指定签名所使用的证书。通过遍历本机上的证书，与签名用的证书进行对比，定位到签名证书在USBKEY中的位置，得到设备、应用和容器的句柄，然后使用证书的私钥进行签名。另外,由于数字签名会用到私钥,因此这里需要验证口令  

## SKF_VerifyPIN(HAPPLICATION hApplication, ULONG ulPINType, LPSTR szPIN, ULONG *pulRetryCount) 
此方法用来验证证书所在应用的PIN码，及上面说的口令，为后面的签名取得权限。hApplication是应用句柄；ulPINType是PIN类型，可以为0是管理员账户，1为普通用户，这个参数一般选择1。szPIN值是PIN码，pulRetryCount为出错后返回的重试次数。  

## SKF_ExportPublicKey(HCONTAINER hContainer, BOOL bSignFlag, BYTE* pbBlob, ULONG* pulBlobLen)
这个方法用来导出容器中的签名公钥，hContainer为证书所在容器句柄；bSignFlag 为导出密钥类型，TRUE表示导出签名公钥，FALSE表示导出加密公钥，这里选择TRUE；pbBlob为返回公钥的数据；pulBlobLen为数据的长度。这里这个方法可以不用调用两次，因为公钥结构是已知的，其长度也是固定的，因此可以直接为pbBlob分配固定长度的数据，以返回公钥  

## SKF_DigestInit(DEVHANDLE hDev, ULONG ulAlgID, ECCPUBLICKEYBLOB *pPubKey, unsigned char *pucID, ULONG ulIDLen, HANDLE *phHash)   
此方法进行杂凑（国密标准里把摘要称之为杂凑）运算初始化，并指定计算消息杂凑的算法。hDev为设备句柄；ulAlgID是杂凑算法标识，这里选择SGD_SM3（0x00000001），表明使用SM3算法；pPubKey为签名用证书公钥数据；pucID为签名者的ID值；ulIDLen是签名者的ID值的长度；phHash为返回的杂凑对象句柄。加入签名者ID值是SM2数字签名的一个重要特征，默认使用"1234567812345678"这个字符串值  

## SKF_Digest(HANDLE hHash, BYTE *pbData, ULONG ulDataLen, BYTE *pbHashData, ULONG *pulHashLen)  
初始化后，调用此方法进行数据杂凑运算。hHash是SKF_DigestInit方法返回的杂凑对象句柄； pbData为产生签名的原文，ulDataLen是原文数据的长度，pbHashData返回杂凑数据； pulHashLen返回杂凑结果的长度。同样，因为杂凑数据的长度都是固定的，这里同样可以为pbHashData事先分配固定长度，而不用再调用两遍

## SKF_DigestUpdate(HANDLE hHash, BYTE *pbData, ULONG ulDataLen)  

## SKF_DigestFinal(HANDLE hHash, BYTE *pHashData, ULONG *pulHashLen)  
对每一组数据都使用SKF_DigestUpdate，最后调用SKF_DigestFinal返回杂凑值。当然，在数字签名运算中不存在分块计算签名的情况，所以这里也不会把数据分块杂凑  

## SKF_ECCSignData(HCONTAINER hContainer, BYTE *pbData, ULONG ulDataLen, PECCSIGNATUREBLOB pSignature)  
最后调用此方法进行数字签名。hContainer用来签名的私钥所在容器句柄，也就是遍历对比证书得到的容器句柄；pbData是被签名的数据；ulDataLen是被签名数据长度，必须小于密钥模长； pbSignature为返回的签名值  


## SKF_CreateContainer(HAPPLICATION hApplication, LPSTR szContainerName, HCONTAINER *phContainer)  
调用此方法创建一个临时容器。hApplication为容器所在的应用句柄；szContainerName是ASCII字符串，表示所建立容器的名称，最大长度不能超过64字节；phContainer是返回所建立容器的容器句柄。   

## SKF_ImportCertificate(HCONTAINER hContainer, BOOL bSignFlag, BYTE* pbCert, ULONG ulCertLen)  
将签名用的证书导入到容器中。hContainer为容器句柄，即用上一方法创建的临时容器；bSignFlag为证书类型，TRUE表示签名证书，FALSE表示加密证书，这里选TRUE；pbCert,是证书数据；ulCertLen为证书数据长度
 
## SKF_ECCVerify(DEVHANDLE hDev , ECCPUBLICKEYBLOB* pECCPubKeyBlob, BYTE *pbData, ULONG ulDataLen, PECCSIGNATUREBLOB pSignature);  
进行签名验证。hDev是设备句柄；pECCPubKeyBlob是公钥数据结构，即第3步得到的公钥； pbData为待验证签名的数据；ulDataLen是待验证签名数据长度；pbSignature待验证的签名值

## SKF_DeleteContainer(HAPPLICATION hApplication, LPSTR szContainerName);

删除临时容器。hApplication为容器所在的应用句柄；szContainerName为容器名称。


## SKF_GenerateAgreementDataWithECC(HCONTAINER hContainer, ULONG ulAlgId,ECCPUBLICKEYBLOB* pTempECCPubKeyBlob,BYTE* pbID, ULONG ulIDLen,HANDLE *phAgreementHandle);

传输的发起方调用此方法进行密钥协商，为计算会话密钥而产生协商参数，返回临时密钥对的公钥及协商句柄。hContainer是容器句柄，任何有公私钥对的容器均可；ulAlgId是会话密钥算法标识，比如SGD_SMS4_ECB（0x00000401）即SM4算法ECB模式； pTempECCPubKeyBlob返回的发起方临时公钥，此数据要传递给响应方； pbID是发起方的ID，这个ID可根据实际情况设置；ulIDLen是发起方ID的长度，不大于32；phAgreementHandle为返回的密钥协商句柄，备用

## SKF_GenerateAgreementDataAndKeyWithECC(HANDLE hContainer, ULONG ulAlgId,ECCPUBLICKEYBLOB* pSponsorECCPubKeyBlob,ECCPUBLICKEYBLOB* pSponsorTempECCPubKeyBlob,ECCPUBLICKEYBLOB* pTempECCPubKeyBlob,BYTE* pbID, ULONG ulIDLen, BYTE *pbSponsorID, ULONG ulSponsorIDLen,HANDLE *phKeyHandle);

传输的响应方调用此方法，进行密钥协商，产生协商参数并计算会话密钥，输出临时密钥对公钥，并返回产生的密钥句柄。hContainer是容器句柄；ulAlgId 是会话密钥算法标识，这里要和上一个方法里设置的算法标识一致；pSponsorECCPubKeyBlob是发起方的公钥，这个公钥是发起方所使用容器里的公钥，可以采用SKF_ImportCertificate和SKF_ExportPublicKey方法从发起方的证书里获得；pSponsorTempECCPubKeyBlob为发起方的临时公钥；pTempECCPubKeyBlob返回响应方的临时公钥；pbID是响应方的ID；ulIDLen是响应方ID的长度，不大于32；pbSponsorID是发起方的ID；ulSponsorIDLen是发起方ID的长度，不大于32；phKeyHandle返回的对称算法密钥句柄。这样，响应方率先拿到了密钥句柄

## SKF_GenerateKeyWithECC(HANDLE hAgreementHandle, ECCPUBLICKEYBLOB* pECCPubKeyBlob, ECCPUBLICKEYBLOB* pTempECCPubKeyBlob, BYTE* pbID, ULONG ulIDLen, HANDLE *phKeyHandle);

接下来发起方调用此方法，使用自身协商句柄和响应方的协商参数计算会话密钥，同时返回会话密钥句柄。hAgreementHandle是第1步返回的密钥协商句柄；pECCPubKeyBlob是响应方的公钥，即响应方所使用容器里的公钥；pTempECCPubKeyBlob是响应方的临时公钥；pbID 为响应方的ID；ulIDLen是响应方ID的长度，不大于32；phKeyHandle是返回的密钥句柄。

传输双方完成了密钥的协商和交换过程，现在双方都有同样的密钥，可以进行加密传输了

上述密钥交换的过程适用于加密的数据需实时解密的场景，如加密数据在网络传输。如果不需要实时解密，比如加密的数据保存在数据库，以后查询时再解密，那就无法使用密钥交换的方式确定密钥。这时可以使用下面的方法，通过给定的值产生对称密钥   

## SKF_SetSymmKey(DEVHANDLE hDev, BYTE* pbKey, ULONG ulAlgID, HANDLE* phKey)；

hDev是设备句柄；pbKey是输入的密钥值；ulAlgID 是会话密钥的算法标识；phKey 是返回的密钥句柄。这样，加解密双方可以通过输入同样的pbKey产生相同的密钥，进行加解密。当然，这种方法安全性较低，在国密标准里并不推荐使用

## SKF_EncryptInit(HANDLE hKey, BLOCKCIPHERPARAM EncryptParam)

此方法进行数据加密初始化，并设置数据加密的算法相关参数。hKey是加密密钥句柄；EncryptParam是分组密码算法相关参数，包括初始向量、初始向量长度、填充方法、和反馈值的位长度。如果采用CBC模式，就需要设置初始向量等参数

## SKF_Encrypt(HANDLE hKey, BYTE * pbData, ULONG ulDataLen, BYTE *pbEncryptedData, ULONG *pulEncryptedLen);

调用此方法进行数据的加密操作，hKey为加密密钥句柄；pbData是待加密数据；ulDataLen是待加密数据长度；pbEncryptedData返回加密后的数据；pulEncryptedLen输入时表示预设的加密后数据长度；输出时表示返回加密后的数据实际长度。可以将pbEncryptedData设置为NULL，得到加密数据实际长度后，再做第二次调用；也可以预先判断返回的加密数据长度（在待加密数据长度和加密算法已知情况下），给pbEncryptedData分配好长度空间，一次调用。

和杂凑运算类似，上述加密方法只适用于单组数据，如果是分组数据，则需要使用下列方法：

## SKF_EncryptUpdate(HANDLE hKey, BYTE * pbData, ULONG ulDataLen, BYTE *pbEncryptedData, ULONG *pulEncryptedLen);

## SKF_EncryptFinal(HANDLE hKey, BYTE *pbEncryptedData, ULONG *ulEncryptedDataLen);

对于每一组数据使用SKF_EncryptUpdate加密，最后调用SKF_EncryptFinal返回最终的加密数据。可以看出，和杂凑不同，对一个分组数据的加密结果可单独得到


## SKF_ECCExportSessionKey(HCONTAINER hContainer, ULONG ulAlgId, ECCPUBLICKEYBLOB *pPubKey, PECCCIPHERBLOB pData, HANDLE *phSessionKey);

加密方调用此方法生成会话密钥并用外部公钥加密输出。hContainer是容器句柄；ulAlgID 是会话密钥的算法标识；pPubKey是用来加密的外部公钥（这个当然是解密方的加密公钥）；pbData返回会话密钥密文；phSessionKey是返回的会话密钥句柄。然后加密方就可以用会话密钥对数据进行加密了

## SKF_ImportSessionKey(HCONTAINER hContainer, ULONG ulAlgId,BYTE *pbWrapedData,ULONG ulWrapedLen,HANDLE *phKey)
解密方调用此方法导入会话密钥密文，并使用容器内的加密私钥解密得到会话密钥。hContainer是加密私钥所在容器句柄；ulAlgID 是会话密钥的算法标识；pbWrapedData 要导入的密文数据；ulWrapedLen密文数据长度；phKey返回会话密钥句柄。之后便可以用会话密钥句柄对加密的数据进行解密了



### 签名过程 
- 1、 SKF_VerifyPIN
- 2、 SKF_ExportPublicKey
- 3、 SKF_DigestInit
- 4、 SKF_Digest 
- 5、 SKF_ECCSignData  

### 验签过程  
- 1、 SKF_CreateContainer
- 2、 SKF_ImportCertificate
- 3、 SKF_ExportPublicKey
- 4、 SKF_DigestInit 
- 5、 SKF_Digest
- 6、 SKF_ECCVerify
- 7、 SKF_DeleteContainer


### 对称加解密   
一次完整对称加解密分为三个步骤： 密钥交换、加密、 解密  
- 1、 SKF_GenerateAgreementDataWithECC
- 2、 SKF_GenerateAgreementDataAndKeyWithECC
- 3、 SKF_GenerateKeyWithECC
协商得到会话密钥或者设置密钥
- 1、 SKF_SetSymmKey
进而继续进行加密或解密
- 4、 SKF_EncryptInit
- 5、 SKF_Encrypt  

### 非对称加解密   
SKF里的非对称加解密有两种模式：一种是对会话密钥进行加解密，一种是使用外部导入的公私钥进行加解密。很明显，第一种就是我们之前说的数字信封：只对会话密钥进行非对称加解密，用会话密钥对原文进行对称加解密   
- 1、 SKF_ECCExportSessionKey 
- 2、 SKF_ImportSessionKey
- 3、 