g++ ./src/main.cpp ./src/glslu.cpp ./src/gl_core_4_4.cpp -static-libgcc -static-libstdc++ -L./lib -I./include -lglfw3 -lopengl32  -lgdi32 -o ./ForeverCube.exe -std=c++11
