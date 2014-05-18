all:
	make -C pc_host all
	make -C ns_client all

clean:
	make -C pc_host clean
	make -C ns_client clean
	
install:
	make -C ns_client install
	
uninstall:
	make -C ns_client uninstall