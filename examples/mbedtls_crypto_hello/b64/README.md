

URL安全的Base64编码适用于以URL方式传递Base64编码结果的场景。<br>
该编码方式的基本过程是先将内容以Base64格式编码为字符串，然后检查该结果字符串，将字符串中的加号+换成中划线-，并且将斜杠/换成下划线_。<br>
详细编码规范请参考RFC4648标准中的相关描述。<br>


## Reference:

- https://github.com/qiniu/c-sdk/tree/master/b64
- http://synesis.com.au/software/b64.html
- https://github.com/synesissoftware/b64/

