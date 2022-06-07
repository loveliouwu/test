Chacha20是一种流加密算法，它创建在基于add-rotate-xor（ARX）操作的伪随机函数之上---32位的模加、异或和循环移位操作。 

Chacha替换了基本的Salsa20循环函数R（a,b,c,k）

谷歌选择了带poly1305消息认证码的chacha20（即ChaCha20-Poly1305）作为Openssl中RC4的替代品 

ChaCha20-Poly1305的优势： 
避开了现有发现的所有安全漏洞和攻击； 
针对移动端设备大量使用的ARM芯片做了优化，能够充分利用ARM向量指令，在移动设备上加解密速度更快、更省电； 
更加节省带宽，Poly16的输出是16字节，而HMAC-SHA1是20字节，可以节省16%的overhead消耗。