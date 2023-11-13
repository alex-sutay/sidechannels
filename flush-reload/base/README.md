From https://github.com/skyler-ferrante/flush-reload
# Flush+Reload

Build
```
./build.sh
```

Run
```
(./bin/clock_receiver > clock &); (./bin/data_receiver > data&); ./bin/transmitter sample
```

Parse
```
./parser/parser2.py clock data raw
```

Graph (debug)
```
./show_graph.py < clock
```
