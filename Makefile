all: 
	gcc -o generate_xml generate_xml.c `xml2-config --cflags --libs` -g
