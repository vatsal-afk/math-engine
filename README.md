run:

```
make
./window
```

Note : Error handling functions defined explicitly will not work on windows!!


Set up emscripten:

after completing the installation process of emsdk and adding to env 

emcc sierpinski.cpp -o index.js -s WASM=1 -s USE_GLFW=3 -s FULL_ES2=1

create an index.html as in web folder

python3 -m http.server
(in the web folder)
