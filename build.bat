g++ ./src/main.cpp ./src/gl_core_3_3.cpp -static-libgcc -static-libstdc++ -L./lib -I./include -lglfw3 -lopengl32  -lgdi32 -o ./ForeverCube.exe -std=c++11
