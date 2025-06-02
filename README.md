## Prerequisites 

- make


## Install GNU ARM Toolchain on Ubuntu/linux

Explanations taken from [lindevs website](https://lindevs.com/install-arm-gnu-toolchain-on-ubuntu)

For this project we use GNU ARM Toolchain verion 11.3 rel1
```
$ curl -Lo gcc-arm-none-eabi.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz"
$ sudo mkdir /opt/gcc-arm-none-eabi/11.3.rel1
$ sudo tar xf gcc-arm-none-eabi.tar.xz --strip-components=1 -C /opt/gcc-arm-none-eabi/11.3.rel1
$ echo 'export PATH=$PATH:/opt/gcc-arm-none-eabi/11.3.rel1/bin' | sudo tee -a /etc/profile.d/gcc-arm-none-eabi.sh
$ source /etc/profile
```

check version

``` 
$ arm-none-eabi-gcc --version
```

clean folder

``` 
$ rm -rf gcc-arm-none-eabi.tar.xz
```
## How to install stm32ai on Ubuntu/linux

X-CUBE-AI is supposed to be installed from stm32CubeMX but from a server or WSL it may not be possible. here is another solution :

First step is to download X-CUBE-AI for linux from st.com.

Then :
```shell
mkdir /opt/stm32ai
unzip en.x-cube-ai-linux_v8.1.0.zip
sudo unzip stm32ai-linux-8.1.0.zip "linux/* -d /opt/stm32ai/
rm en.x-cube-ai-linux_v8.1.0.zip stm32ai-linux-8.1.0.zip
```

If you want you can add stm32ai to your PATH. One solution is to :

```shell
echo 'export PATH=$PATH:/opt/stm32ai' | sudo tee -a /etc/profile.d/stm32ai.sh
source /etc/profile
```

Now check that stm32ai is available with `stm32ai --version`


