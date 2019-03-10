all:
	cd src;\
	make clean;\
	make;\

	./update_image.sh
	./run_bochs.sh