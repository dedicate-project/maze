all:
	g++ -std=c++17 src/main.cpp -o maze

clean:
	rm -f maze
