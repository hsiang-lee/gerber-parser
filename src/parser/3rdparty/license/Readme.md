# License


## Build

```bash
mkdir build
cd build
cmake ..
make -j8
```

*注意：在linux上编译时需要`GLIBC`版本>=2.25*

### GLIBC 升级

查看系统(CentOS)与GLIBC版本

```bash
cat /etc/redhat-release
strings /lib64/libc.so.6 | grep GLIBC_
```

升级

```bash
wget http://ftp.gnu.org/gnu/glibc/glibc-2.25.tar.gz 
tar -xvf  glibc-2.25.tar.gz 
mkdir glibc-2.25/build
cd glibc-2.25/build 
../configure  --prefix=/usr --disable-profile --enable-add-ons --with-headers=/usr/include --with-binutils=/usr/bin
make
make install
```

## App

在`Windows`下运行工具时，需要把`libsodium.dll`放在`Release`文件夹下。

## MachineID

获取机器码。

```powershell
PS D:\Release> .\machine_id.exe
BFEBFBFF000A06552CF05D40ED86
```

## Key Generator

生成加密时需要的两个密钥对。

**样例**

```txt
PS D:\Release> .\key_gen.exe
Public key:
13 196 43 242 52 6 87 80 0 88 180 251 124 44 240 7 153 17 27 72 251 84 105 11 155 87 157 192 173 93 97 64
Secret key:
238 111 167 200 2 92 90 49 153 33 45 187 220 216 191 23 24 254 101 101 176 172 157 171 179 132 110 64 110 234 185 175 13 196 43 242 52 6 87 80 0 88 180 251 124 44 240 7 153 17 27 72 251 84 105 11 155 87 157 192 173 93 97 64
```

## License Generator

生成证书。支持绑定多台机器码。需要根据提示输入Keydistributer中的对应密钥。

**样例**

```txt
PS D:\Release>  .\license_gen.exe
Enter application name:
testapp
Enter secret key (64 numbers from 0-255):
86 101 47 159 145 111 161 68 77 159 199 1 187 39 104 178 108 214 212 71 127 80 119 89 55 142 151 158 131 253 61 18 27 228 205 99 17 115 93 54 246 191 12 48 150 96 31 235 0 26 40 111 90 54 2 193 222 10 237 66 154 60 84 60
Enter number of machine ID's :
1
Enter machine ID:
BFEBFBFF000A06552CF05D40ED86
Enter expiration time:
123456
Enter issuer
testuser
Enter license path
test.lic
```

## License Checker

在本机上检查证书合法性。需要根据提示输入Keydistributer中的对应密钥。

```txt
PS D:\Release> .\license_check.exe
Enter application name:
testapp
Enter public key (32 numbers from 0-255):
27 228 205 99 17 115 93 54 246 191 12 48 150 96 31 235 0 26 40 111 90 54 2 193 222 10 237 66 154 60 84 60
Enter license path
test.lic
license passed
```

## Service 

### 构建服务镜像

```bash
# 构建服务镜像
docker build -f ./service/dockerfile/dockerfile --target licenseserver -t licenseserver .
```

### 启动

```bash
# docker
# 如果需要配置服务端口，在service/dockerfile/k8s/license.yaml中修改gateway和grpc对应端口号，并且将yaml文件挂载进去
docker run -d --net=host licenseserver (-v service/dockerfile/k8s/license.yaml:/yaml/license.yaml)

# k8s
# 如果需要配置服务端口，需要添加挂载配置
kubectl apply -f service/dockerfile/k8s/license.yaml
```

### 用法

- gRPC: 见service/proto中接口定义
- http:
	- 8080:/v1/license/keygeneration<br/>
        request

        ```json
        {
            "RequestID": "RequestID",
        }
        ```
        response
        ```json
        {
            "RequestID":"RequestID",
            "PubKey":[
            	13, 196 ,43 ,242 ,52,... 64
            ],
            "SecKey":[
            	238, 111, 167,200, 2, ... 64
            ]
        }
        ```
    - 8080:/v1/license/licensegeneration<br/>
         request
        ```json
        {
            "RequestID": "RequestID",
            "LicenseInfo":{
                "AppName": "AppName",
                "Issuer":"Issuer",
                "MachineIDs":[
                    "MachineID1", "MachineID2"
                ],
                "ExpiredTimestamp":"ExpiredTimestamp",
                "Payloads": {
                     "保留字段"："",
                },
                "SecKey":[
                    238, 111, 167,200, 2, ... 64
                ]
             }
        }
        ```
        response
        ```json
        {
            "RequestID":"RequestID",
            "License":"License",
            "Status":{
                "Code":"Code",
                "Message":"Message"    //0："成功"、1："无效的参数"、3："内部错误"
            }
        }
        ```