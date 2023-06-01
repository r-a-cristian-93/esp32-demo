Demo projects for esp32 in qemu with esp-idf framework.

# Requirements:
- a linux machine
- esp-idf
- qemu for esp32

# Install dependencies  
```bash
sudo apt update
sudo apt install git wget flex bison gperf python3 python3-pip python3-setuptools python3-venv \
	cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 \
	libglib2.0-dev libpixman-1-dev libgcrypt20-dev -y
ln -s /usr/bin/puthon3 /usr/bin/pyhon
```

# Install ESP-IDF
```bash
cd /opt 
mkdir /esp
chown $USER esp
cd esp
git clone https://github.com/espressif/esp-idf.git
cd esp-idf
export IDF_TOOLS_PATH="/opt/esp"
./install.sh
. ./export.sh
```

# Settup alias for ease of acces
```bash
# add this to your shell profile
alias get_idf='. /opt/esp/esp-idf/export.sh'
```

# Compile and install qemu for esp32
```bash
cd ~
git clone https://github.com/espressif/qemu.git
cd qemu
mkdir -p build
cd build
../configure --target-list=xtensa-softmmu --enable-debug --enable-sanitizers --disable-strip --disable-capstone --disable-vnc
make vga=no
make install
```

# Build and run a demo project:

1. Navigate to the directory you downloaded the repository
```bash
cd /path/to/download/directory/esp32-demo
```

2. Settup esp-idf by:
You have to do this every time you open a new teminal
```bash
#  execute get_idf if you have previously settup the alias
get_idf
```

```bash
# execute export.sh directly
. ./opt/esp/esp-idf/export.sh
```

3. Build a project 
```bash
./build.sh/[project_name]
```

4. Run a compiled demo project:
```bash
./run.sh/[project_name]
```
