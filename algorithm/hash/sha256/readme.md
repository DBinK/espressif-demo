# sha256

目前没有单独封装为Arduino 库则直接使用ESP-IDF中mbedtls 。同时 **mbedtls** 有类似其他的sha算法 sha1 sha512 。
类似之前的MD5例程可以分段mbedtls_sha256_update 计算大量数据最终的sha256值
