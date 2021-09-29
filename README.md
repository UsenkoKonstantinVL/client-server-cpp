# client-server-cpp

# Сборка
```
mkdir build
cd build
cmake ..
cmake --build .
```

# Запуск сервера
По tcp:
```bash
cd build
./main_server tcp
```
По udp:
```bash
cd build
./main_server udp
```

# Запуск клиента
По tcp:
```bash
cd build
./main_client tcp
```
По udp:
```bash
cd build
./main_client udp
```