# Пример работы клеточного автомата на SDL 2

## Сборка (Native)

```terminal
cmake -S . -B ./build/
cd build && make
```


## Сборка (Web)

```terminal
em++ main.cpp engine/Engine.cpp engine/cell/Cell.cpp -s USE_SDL=2 -o main.html
```

## Запуск проекта

Для Python 3: 

```terminal
python3 -m http.server
```

Для Python 2:

```terminal
python -m SimpleHTTPServer
```
