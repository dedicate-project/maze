all:
	g++ -std=c++17 src/tiles.cpp src/player.cpp src/maze.cpp src/main.cpp -o maze

clean:
	rm -f maze
