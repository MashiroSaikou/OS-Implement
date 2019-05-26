all:
	#make_initrd test.txt test.txt test2.txt test2.txt
	cd src;\
	make clean;\
	make;\

	./update_image.sh
	./run_bochs.sh