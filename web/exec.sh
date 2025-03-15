echo "compiling and building for wasm..."
# emcc sierpinski.cpp -o index.js -s WASM=1 -s USE_GLFW=3 -s FULL_ES2=1
emcc sierpinski.cpp -o index.js -s USE_GLFW=3 -s FULL_ES3=1 -s WASM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -I/home/virtual/Downloads/imgui/ -I/home/virtual/Downloads/imgui/backends/ \
    /home/virtual/Downloads/imgui/imgui.cpp /home/virtual/Downloads/imgui/backends/imgui_impl_glfw.cpp /home/virtual/Downloads/imgui/backends/imgui_impl_opengl3.cpp

# echo "activating the virtual environments"
# source ../sierpinski/web/venv/bin/activate

# echo "starting the server"
# python3 -m http.server

echo "starting the server"
emrun --no_browser --port 8080 index.html

