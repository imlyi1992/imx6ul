cc = $$CC
obj_name = main    
obj = main.c common.c usb.c eim.c file.c i2c.c
all:
	$(cc) $(obj) -o $(obj_name) -lpthread  -D_GNU_SOURCE
	
clean:
	-rm $(obj_name)
