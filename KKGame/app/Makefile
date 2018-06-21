
LIBRARY = -L/usr/lib -L/usr/lib -lchipmunk -levent -lz -lstdc++ -lm -lrt
INCLUDE = -I/usr/include -I../../app -I../../js -I../../app/GA -I../../lib -I../../lib/duktape/src  -I../../lib/Chipmunk2D/include
CFLAGS = -g
CCFLAGS = -std=gnu++11 -g

DUKTAPE = ../../lib/duktape/src
SRC = ../../app
GA = ../../app/GA

OUTPUT = ../../bin
TARGET = $(OUTPUT)/kk-app
OBJOUTPUT = ./objs
 

OBJECTS := $(OBJOUTPUT)/kk-object.o $(OBJOUTPUT)/kk-event.o $(OBJOUTPUT)/kk-element.o $(OBJOUTPUT)/kk-string.o \
	$(OBJOUTPUT)/kk-script.o $(OBJOUTPUT)/kk-crypto.o $(OBJOUTPUT)/kk-app.o \
	$(OBJOUTPUT)/kk-ev.o $(OBJOUTPUT)/kk-y.o $(OBJOUTPUT)/kk-ws.o $(OBJOUTPUT)/kk-http.o  \
	$(OBJOUTPUT)/GAContext.o $(OBJOUTPUT)/GAScene.o $(OBJOUTPUT)/GABody.o $(OBJOUTPUT)/GAShape.o \
	$(OBJOUTPUT)/GAAction.o $(OBJOUTPUT)/GAActionWalk.o $(OBJOUTPUT)/GATileMap.o $(OBJOUTPUT)/GADocument.o \
	$(OBJOUTPUT)/duktape.o  \
	$(OBJOUTPUT)/main.o 

$(TARGET):$(OBJECTS)
	g++ $(OBJECTS) -o $(TARGET) $(CCFLAGS) $(LIBRARY)
 
$(OBJOUTPUT)/main.o: ./main.cpp
	gcc $(CCFLAGS) $(INCLUDE) -c ./main.cpp -o $(OBJOUTPUT)/main.o

$(OBJOUTPUT)/kk-object.o:$(SRC)/kk-object.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-object.cc -o $(OBJOUTPUT)/kk-object.o

$(OBJOUTPUT)/kk-event.o:$(SRC)/kk-event.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-event.cc -o $(OBJOUTPUT)/kk-event.o

$(OBJOUTPUT)/kk-element.o:$(SRC)/kk-element.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-element.cc -o $(OBJOUTPUT)/kk-element.o

$(OBJOUTPUT)/kk-string.o:$(SRC)/kk-string.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-string.cc -o $(OBJOUTPUT)/kk-string.o

$(OBJOUTPUT)/kk-script.o:$(SRC)/kk-script.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-script.cc -o $(OBJOUTPUT)/kk-script.o

$(OBJOUTPUT)/kk-crypto.o:$(SRC)/kk-crypto.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-crypto.cc -o $(OBJOUTPUT)/kk-crypto.o

$(OBJOUTPUT)/kk-app.o:$(SRC)/kk-app.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-app.cc -o $(OBJOUTPUT)/kk-app.o

$(OBJOUTPUT)/kk-ev.o:$(SRC)/kk-ev.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-ev.cc -o $(OBJOUTPUT)/kk-ev.o

$(OBJOUTPUT)/kk-y.o:$(SRC)/kk-y.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-y.cc -o $(OBJOUTPUT)/kk-y.o

$(OBJOUTPUT)/kk-ws.o:$(SRC)/kk-ws.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-ws.cc -o $(OBJOUTPUT)/kk-ws.o

$(OBJOUTPUT)/kk-http.o:$(SRC)/kk-http.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-http.cc -o $(OBJOUTPUT)/kk-http.o

$(OBJOUTPUT)/GAContext.o:$(GA)/GAContext.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GAContext.cc -o $(OBJOUTPUT)/GAContext.o

$(OBJOUTPUT)/GAScene.o:$(GA)/GAScene.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GAScene.cc -o $(OBJOUTPUT)/GAScene.o

$(OBJOUTPUT)/GABody.o:$(GA)/GABody.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GABody.cc -o $(OBJOUTPUT)/GABody.o

$(OBJOUTPUT)/GAShape.o:$(GA)/GAShape.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GAShape.cc -o $(OBJOUTPUT)/GAShape.o

$(OBJOUTPUT)/GAAction.o:$(GA)/GAAction.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GAAction.cc -o $(OBJOUTPUT)/GAAction.o

$(OBJOUTPUT)/GAActionWalk.o:$(GA)/GAActionWalk.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GAActionWalk.cc -o $(OBJOUTPUT)/GAActionWalk.o

$(OBJOUTPUT)/GATileMap.o:$(GA)/GATileMap.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GATileMap.cc -o $(OBJOUTPUT)/GATileMap.o

$(OBJOUTPUT)/GADocument.o:$(GA)/GADocument.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GADocument.cc -o $(OBJOUTPUT)/GADocument.o

$(OBJOUTPUT)/duktape.o:$(DUKTAPE)/duktape.c
	gcc $(CFLAGS) $(INCLUDE) -c $(DUKTAPE)/duktape.c -o $(OBJOUTPUT)/duktape.o

clean:
	rm -rf $(OBJOUTPUT)/*.o $(TARGET)
