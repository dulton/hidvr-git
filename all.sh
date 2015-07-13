#! /bin/sh

make conf_ja7204
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja7204s
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja7204i
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja7208
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja7208s
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja7208i
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja7216nc
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja7216cx
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja2008nc
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja2024nc
make clean_all
make compile_all
cd rootfs
make rom
cd ..

make conf_ja2032nc
make clean_all
make compile_all
cd rootfs
make rom
cd ..



