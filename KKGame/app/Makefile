


LIBRARY = -L/usr/lib -L/usr/local/lib -levent -lchipmunk -lz -lstdc++ -lm -lrt -lcrypto -lssl -lpthread -ldl
INCLUDE = -I/usr/local/include -I../../app -I../../js -I../../app/GA -I../../lib -I../../lib/duktape  -I../../lib/Chipmunk2D/include -I/usr/local/opt/openssl/include -I../../../kk-object/include -I../../../kk-element/include
CFLAGS = -g
CCFLAGS = -std=gnu++11 -g

DUKTAPE = ../../../kk-duktape/lib/duktape/src
SRC = ../../app
GA = ../../app/GA
KK_OBJECT = ../../../kk-object/src
KK_ELEMENT = ../../../kk-element/src

OUTPUT = ../../bin
TARGET = $(OUTPUT)/kk-app
OBJOUTPUT = ./objs
 
OBJECTS := $(OBJOUTPUT)/kk-object.o $(OBJOUTPUT)/kk-event.o $(OBJOUTPUT)/kk-string.o \
	$(OBJOUTPUT)/kk-element.o $(OBJOUTPUT)/kk-element-style.o $(OBJOUTPUT)/kk-document.o $(OBJOUTPUT)/kk-document-binary.o \
	$(OBJOUTPUT)/kk-script.o $(OBJOUTPUT)/kk-crypto.o $(OBJOUTPUT)/kk-app.o \
	$(OBJOUTPUT)/kk-ev.o $(OBJOUTPUT)/kk-y.o $(OBJOUTPUT)/kk-ws.o $(OBJOUTPUT)/kk-http.o  \
	$(OBJOUTPUT)/kk-block.o $(OBJOUTPUT)/kk-dispatch.o $(OBJOUTPUT)/kk-wk.o $(OBJOUTPUT)/kk-binary.o $(OBJOUTPUT)/kk-chan.o $(OBJOUTPUT)/kk-bio.o  \
	$(OBJOUTPUT)/GAContext.o $(OBJOUTPUT)/GAScene.o $(OBJOUTPUT)/GABody.o $(OBJOUTPUT)/GAShape.o \
	$(OBJOUTPUT)/GAAction.o $(OBJOUTPUT)/GAActionWalk.o $(OBJOUTPUT)/GATileMap.o $(OBJOUTPUT)/GADocument.o \
	$(OBJOUTPUT)/GAInCollision.o $(OBJOUTPUT)/GACollision.o \
	$(OBJOUTPUT)/duktape.o  \
	$(OBJOUTPUT)/main.o 

$(TARGET):$(OBJECTS)
	g++ $(OBJECTS) -o $(TARGET) $(CCFLAGS) $(LIBRARY)
 
$(OBJOUTPUT)/main.o: ./main.cpp
	gcc $(CCFLAGS) $(INCLUDE) -c ./main.cpp -o $(OBJOUTPUT)/main.o

$(OBJOUTPUT)/kk-object.o:$(KK_OBJECT)/kk-object.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-object.cc -o $(OBJOUTPUT)/kk-object.o

$(OBJOUTPUT)/kk-event.o:$(KK_OBJECT)/kk-event.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-event.cc -o $(OBJOUTPUT)/kk-event.o

$(OBJOUTPUT)/kk-string.o:$(KK_OBJECT)/kk-string.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-string.cc -o $(OBJOUTPUT)/kk-string.o

$(OBJOUTPUT)/kk-script.o:$(KK_OBJECT)/kk-script.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-script.cc -o $(OBJOUTPUT)/kk-script.o

$(OBJOUTPUT)/kk-crypto.o:$(KK_OBJECT)/kk-crypto.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-crypto.cc -o $(OBJOUTPUT)/kk-crypto.o

$(OBJOUTPUT)/kk-app.o:$(SRC)/kk-app.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-app.cc -o $(OBJOUTPUT)/kk-app.o

$(OBJOUTPUT)/kk-ev.o:$(KK_OBJECT)/kk-ev.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-ev.cc -o $(OBJOUTPUT)/kk-ev.o

$(OBJOUTPUT)/kk-y.o:$(SRC)/kk-y.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(SRC)/kk-y.cc -o $(OBJOUTPUT)/kk-y.o

$(OBJOUTPUT)/kk-ws.o:$(KK_OBJECT)/kk-ws.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-ws.cc -o $(OBJOUTPUT)/kk-ws.o

$(OBJOUTPUT)/kk-http.o:$(KK_OBJECT)/kk-http.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-http.cc -o $(OBJOUTPUT)/kk-http.o

$(OBJOUTPUT)/kk-binary.o:$(KK_OBJECT)/kk-binary.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-binary.cc -o $(OBJOUTPUT)/kk-binary.o

$(OBJOUTPUT)/kk-block.o:$(KK_OBJECT)/kk-block.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-block.cc -o $(OBJOUTPUT)/kk-block.o

$(OBJOUTPUT)/kk-dispatch.o:$(KK_OBJECT)/kk-dispatch.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-dispatch.cc -o $(OBJOUTPUT)/kk-dispatch.o

$(OBJOUTPUT)/kk-wk.o:$(KK_OBJECT)/kk-wk.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-wk.cc -o $(OBJOUTPUT)/kk-wk.o

$(OBJOUTPUT)/kk-chan.o:$(KK_OBJECT)/kk-chan.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-chan.cc -o $(OBJOUTPUT)/kk-chan.o

$(OBJOUTPUT)/kk-bio.o:$(KK_OBJECT)/kk-bio.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_OBJECT)/kk-bio.cc -o $(OBJOUTPUT)/kk-bio.o

$(OBJOUTPUT)/kk-element.o:$(KK_ELEMENT)/kk-element.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_ELEMENT)/kk-element.cc -o $(OBJOUTPUT)/kk-element.o

$(OBJOUTPUT)/kk-element-style.o:$(KK_ELEMENT)/kk-element-style.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_ELEMENT)/kk-element-style.cc -o $(OBJOUTPUT)/kk-element-style.o

$(OBJOUTPUT)/kk-document.o:$(KK_ELEMENT)/kk-document.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_ELEMENT)/kk-document.cc -o $(OBJOUTPUT)/kk-document.o

$(OBJOUTPUT)/kk-document-binary.o:$(KK_ELEMENT)/kk-document-binary.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(KK_ELEMENT)/kk-document-binary.cc -o $(OBJOUTPUT)/kk-document-binary.o

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

$(OBJOUTPUT)/GAInCollision.o:$(GA)/GAInCollision.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GAInCollision.cc -o $(OBJOUTPUT)/GAInCollision.o

$(OBJOUTPUT)/GACollision.o:$(GA)/GACollision.cc
	gcc $(CCFLAGS) $(INCLUDE) -c $(GA)/GACollision.cc -o $(OBJOUTPUT)/GACollision.o

$(OBJOUTPUT)/duktape.o:$(DUKTAPE)/duktape.c
	gcc $(CFLAGS) $(INCLUDE) -c $(DUKTAPE)/duktape.c -o $(OBJOUTPUT)/duktape.o

clean:
	rm -rf $(OBJOUTPUT)/*.o $(TARGET)
