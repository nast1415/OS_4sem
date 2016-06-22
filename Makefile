all: format mkdir import_file export_file rmdir rm ls

format: format.o help.o
	g++ format.o help.o -o format

mkdir: mkdir.o help.o
	g++ mkdir.o help.o -o mkdir

import_file: import_file.o help.o mkfile.o
	g++ import_file.o help.o mkfile.o -o import

export_file: export_file.o help.o
	g++ export_file.o help.o -o export

rmdir: rmdir.o help.o
	g++ rmdir.o help.o -o rmdir

rm: rm.o help.o
	g++ rm.o help.o -o rm

ls: ls.o help.o
	g++ ls.o help.o -o ls

help.o: help.cpp
	g++ -c help.cpp

format.o: format.cpp
	g++ -c format.cpp

ls.o: ls.cpp
	g++ -c ls.cpp

mkdir.o: mkdir.cpp
	g++ -c mkdir.cpp

mkfile.o: mkfile.cpp
	g++ -c mkfile.cpp

rmdir.o: rmdir.cpp
	g++ -c rmdir.cpp

rm.o: rm.cpp
	g++ -c rm.cpp

export_file.o: export_file.cpp
	g++ -c export_file.cpp

import_file.o: import_file.cpp
	g++ -c import_file.cpp

clean:
	rm -rf *.o format ls mkdir import export rmdir rm