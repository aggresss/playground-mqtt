# CA 相关
# CA key
openssl genrsa -out ca.key 2048
# CA csr
openssl req -new -key ca.key -out ca.csr
# CA crt
openssl x509 -req -in ca.csr -out ca.crt -signkey ca.key -days 3650

# 用户证书相关

# server key
openssl genrsa -out server.key 2048
# server.csr
openssl req -new -key server.key -out server.csr
# server.crt
openssl x509 -req -in server.csr -out server.crt -CA ca.crt -CAkey ca.key -CAcreateserial -days 3650

# client key
openssl genrsa -out client.key 2048
# client.csr
openssl req -new -key client.key -out client.csr
# client.crt
openssl x509 -req -in client.csr -out client.crt -CA ca.crt -CAkey ca.key -CAcreateserial -days 3650

