include Makefile.param

CONF := "./conf.h"
OEM := "./oem.h"
ROOTFSDIR := "./rootfs"
KERNELDIR := "./app/bsp/kernel"

UBOOT := $(ROOTFSDIR)"/jn_uboot"
UBOOT_ENV := $(ROOTFSDIR)"/jn_uenv"
LOGO := $(ROOTFSDIR)"/jn_logo"
KERNEL := $(ROOTFSDIR)"/jn_kernel"
ROOTFS := $(ROOTFSDIR)"/rootfs"
LOAD35XX := $(ROOTFS)"/root/module/load"

.PHONY: all all3 premake postmake
all:
	@echo "cat the Makefile to see detail"

premake:
	chmod a+x ./premake.sh
	bash ./premake.sh

postmake:
	chmod a+x ./postmake.sh
	bash ./postmake.sh

.PHONY: sdk_3515 sdk_3520 sdk_3520A sdk_3521 sdk_3531
sdk_3515:
	$(RM)   sdk/sdk $(KERNELDIR) plat.mk $(ROOTFS)
	$(LINK) hi3515 sdk/sdk
	$(LINK) $(SDK_PATH)/kbuild-FULL_REL $(KERNELDIR)
	$(CP)   conf/plat_hi3515.mk plat.mk
	$(LINK) rootfs_15 $(ROOTFS)

sdk_3520:
	$(RM)   sdk/sdk $(KERNELDIR) plat.mk $(ROOTFS)
	$(LINK) hi3520 sdk/sdk
	$(LINK) $(SDK_PATH)/kbuild-FULL_REL $(KERNELDIR)
	$(CP)   conf/plat_hi3520.mk plat.mk
	$(LINK) rootfs_20 $(ROOTFS)

sdk_3520A:
	$(RM)   sdk/sdk $(KERNELDIR) plat.mk $(ROOTFS)
	$(LINK) hi3520A sdk/sdk
	$(LINK) $(SDK_PATH)/kbuild-FULL_REL $(KERNELDIR)
	$(CP)   conf/plat_hi3520A.mk plat.mk
	$(LINK) rootfs_A9 $(ROOTFS)

sdk_3520D:
	$(RM)   sdk/sdk $(KERNELDIR) plat.mk $(ROOTFS)
	$(LINK) hi3520D sdk/sdk
	$(LINK) $(SDK_PATH)/kbuild-FULL_REL $(KERNELDIR)
	$(CP)   conf/plat_hi3520D.mk plat.mk
	$(LINK) rootfs_A9 $(ROOTFS)

sdk_3520D-lite:
	$(RM)   sdk/sdk $(KERNELDIR) plat.mk $(ROOTFS)
	$(LINK) hi3520D sdk/sdk
	$(LINK) $(SDK_PATH)/kbuild-FULL_REL $(KERNELDIR)
	$(CP)   conf/plat_hi3520D-lite.mk plat.mk
	$(LINK) rootfs_A9 $(ROOTFS)

sdk_3521:
	$(RM)   sdk/sdk $(KERNELDIR) plat.mk $(ROOTFS)
	$(LINK) hi3521 sdk/sdk
	$(LINK) $(SDK_PATH)/kbuild-FULL_REL $(KERNELDIR)
	$(CP)   conf/plat_hi3521.mk plat.mk
	$(LINK) rootfs_A9 $(ROOTFS)

sdk_3531:
	$(RM)   sdk/sdk $(KERNELDIR) plat.mk $(ROOTFS)
	$(LINK) hi3531 sdk/sdk
	$(LINK) $(SDK_PATH)/kbuild-FULL_REL $(KERNELDIR)
	$(CP)   conf/plat_hi3531.mk plat.mk
	$(LINK) rootfs_A9 $(ROOTFS)

.PHONY: prepare_mem mem128_08io mem128_16io mem256_08io mem256_16io mem128_hi3520 mem128_hi3520A mem256_hi3520A mem256_hi3521 mem512_hi3531
prepare_mem:
	make mk_uboot_env -C app/
	$(CP) app/mk_uboot_env $(ROOTFSDIR)
mem128_08io:prepare_mem
	$(LINK) u-boot-khflash-LXM20130130-128M.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_128m.txt rootfs/jn_uenv_128m
	$(LINK) jn_uenv_128m $(UBOOT_ENV)
	$(CP) conf/load3515_128m_08io $(LOAD35XX)
mem128_08io_960h:prepare_mem
	$(LINK) u-boot-khflash-LXM20130130-128M.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_128m_960h.txt rootfs/jn_uenv_128m
	$(LINK) jn_uenv_128m $(UBOOT_ENV)
	$(CP) conf/load3515_128m_08io_960h $(LOAD35XX)
mem128_16io:prepare_mem
	$(LINK) u-boot-khflash-LXM20130130-128M.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_128m.txt rootfs/jn_uenv_128m
	$(LINK) jn_uenv_128m $(UBOOT_ENV)
	$(CP) conf/load3515_128m_16io $(LOAD35XX)
mem256_08io:prepare_mem
	$(LINK) u-boot-wdflash-LXM20111208-256M.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_256m.txt rootfs/jn_uenv_256m
	$(LINK) jn_uenv_256m $(UBOOT_ENV)
	$(CP) conf/load3515_256m_08io $(LOAD35XX)
mem256_08io_n9104:prepare_mem
	$(LINK) u-boot-wdflash-LXM20111208-256M.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_256m_n9104.txt rootfs/jn_uenv_256m
	$(LINK) jn_uenv_256m $(UBOOT_ENV)
	$(CP) conf/load3515_256m_08io_n9104 $(LOAD35XX)
mem256_16io:prepare_mem
	$(LINK) u-boot-wdflash-LXM20111208-256M.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_256m.txt rootfs/jn_uenv_256m
	$(LINK) jn_uenv_256m $(UBOOT_ENV)
	$(CP) conf/load3515_256m_16io $(LOAD35XX)
mem128_hi3520:prepare_mem
	$(LINK) u-boot-3520-master-test.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520_128m.txt rootfs/jn_uenv_3520_128m
	$(LINK) jn_uenv_3520_128m $(UBOOT_ENV)
	$(CP) conf/load3520_128m_24io $(LOAD35XX)
mem128_hi3520A:prepare_mem
	$(LINK) u-boot-3520a-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520A_128m.txt rootfs/jn_uenv_3520A_128m
	$(LINK) jn_uenv_3520A_128m $(UBOOT_ENV)
	$(CP) conf/load3520A_128m_04io $(LOAD35XX)
mem256_hi3520A:prepare_mem
	$(LINK) u-boot-3520a-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520A_256m.txt rootfs/jn_uenv_3520A_256m
	$(LINK) jn_uenv_3520A_256m $(UBOOT_ENV)
	$(CP) conf/load3520A_256m_04io $(LOAD35XX)
mem256_hi3520A_nvr:prepare_mem
	$(LINK) u-boot-3520a-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520A_256m.txt rootfs/jn_uenv_3520A_256m
	$(LINK) jn_uenv_3520A_256m $(UBOOT_ENV)
	$(CP) conf/load3520A_256m_04io $(LOAD35XX)
mem256_hi3520D:prepare_mem
	$(LINK) u-boot-3520d-rel-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520D_256m.txt rootfs/jn_uenv_3520D_256m
	$(LINK) jn_uenv_3520D_256m $(UBOOT_ENV)
	$(CP) conf/load3520D_256m_04io $(LOAD35XX)
mem128_hi3520D-04io:prepare_mem
	$(LINK) u-boot-3520d-lite-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520D_128m_04io.txt rootfs/jn_uenv_3520D_128m_04io
	$(LINK) jn_uenv_3520D_128m_04io $(UBOOT_ENV)
	$(CP) conf/load3520D_128m_04io $(LOAD35XX)
mem128_hi3520D-08io:prepare_mem
	$(LINK) u-boot-3520d-lite-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520D_128m_08io.txt rootfs/jn_uenv_3520D_128m_08io
	$(LINK) jn_uenv_3520D_128m_08io $(UBOOT_ENV)
	$(CP) conf/load3520D_128m_08io $(LOAD35XX)
mem256_hi3520D-cf:prepare_mem
	$(LINK) u-boot-3520d-lite-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520D_256m-cf.txt rootfs/jn_uenv_3520D_256m-cf
	$(LINK) jn_uenv_3520D_256m-cf $(UBOOT_ENV)
	$(CP) conf/load3520D_256m_04io $(LOAD35XX)
mem256_hi3520D-hl:prepare_mem
	$(LINK) u-boot-3520d-lite-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520D_256m-hl.txt rootfs/jn_uenv_3520D_256m-hl
	$(LINK) jn_uenv_3520D_256m-hl $(UBOOT_ENV)
	$(CP) conf/load3520D_256m_hl $(LOAD35XX)
mem256_hi3520D-hf:prepare_mem
	$(LINK) u-boot-3520d-rel-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3520D_256m-hf.txt rootfs/jn_uenv_3520D_256m-hf
	$(LINK) jn_uenv_3520D_256m-hf $(UBOOT_ENV)
	$(CP) conf/load3520D_256m_hl $(LOAD35XX)
mem256_hi3521:prepare_mem
	$(LINK) u-boot-3521-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3521_256m.txt rootfs/jn_uenv_3521_256m
	$(LINK) jn_uenv_3521_256m $(UBOOT_ENV)
	$(CP) conf/load3521_256m_08io $(LOAD35XX)
mem512_hi3521_32io:prepare_mem
	$(LINK) u-boot-3521-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3521_512m_32io.txt rootfs/jn_uenv_3521_512m_32io
	$(LINK) jn_uenv_3521_512m_32io $(UBOOT_ENV)
	$(CP) conf/load3521_512m_32io $(LOAD35XX)
mem512_hi3521_16d1:prepare_mem
	$(LINK) u-boot-3521-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3521_512m_16d1.txt rootfs/jn_uenv_3521_512m_16d1
	$(LINK) jn_uenv_3521_512m_16d1 $(UBOOT_ENV)
	$(CP) conf/load3521_512m_16d1 $(LOAD35XX)
mem512_hi3521_nvr:prepare_mem
	$(LINK) u-boot-3521-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3521_512m_nvr.txt rootfs/jn_uenv_3521_512m_nvr
	$(LINK) jn_uenv_3521_512m_nvr $(UBOOT_ENV)
	$(CP) conf/load3521_512m_nvr $(LOAD35XX)
mem512_hi3521_nvr_test:prepare_mem
	$(LINK) u-boot-3521-test-20130328-hzj.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3521_512m_nvr.txt rootfs/jn_uenv_3521_512m_nvr
	$(LINK) jn_uenv_3521_512m_nvr $(UBOOT_ENV)
	$(CP) conf/load3521_512m_nvr $(LOAD35XX)
mem512_hi3531:prepare_mem
	$(LINK) u-boot-3531-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3531_512m.txt rootfs/jn_uenv_3531_512m
	$(LINK) jn_uenv_3531_512m $(UBOOT_ENV)
	$(CP) conf/load3531_512m_16io $(LOAD35XX)
mem512_hi3531_8io:prepare_mem
	$(LINK) u-boot-3531-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3531_512m_8io.txt rootfs/jn_uenv_3531_512m_8io
	$(LINK) jn_uenv_3531_512m_8io $(UBOOT_ENV)
	$(CP) conf/load3531_512m_8io $(LOAD35XX)
mem1024_hi3531_16io:prepare_mem
	$(LINK) u-boot-3531-lxm-20130815.bin $(UBOOT)
	rootfs/mk_uboot_env conf/uboot_env_3531_1024m_16io.txt rootfs/jn_uenv_3531_1024m_16io
	$(LINK) jn_uenv_3531_1024m_16io $(UBOOT_ENV)
	$(CP) conf/load3531_1024m_16io $(LOAD35XX)

.PHONY: logo72xx logo72xx_256k logolonse_256k logoxmedia logotab logo_juan_128k logo_juan_256k
logo72xx:
	$(LINK) logo_jn72xx $(LOGO)
logo72xx_256k:
	$(LINK) logo_jn72xx_256k $(LOGO)
logolonse:
	$(LINK) logo_lonse $(LOGO)
logolonse_256k:
	$(LINK) logo_lonse_256k $(LOGO)
logoxmedia:
	$(LINK) logo_xmedia $(LOGO)
logotab:
	$(LINK) logo_tab $(LOGO)
logoenter:
	$(LINK) logo_enter $(LOGO)
logo_juan_128k:
	$(LINK) logo_juan_128k $(LOGO)
logo_juan_256k:
	$(LINK) logo_juan_256k $(LOGO)

.PHONY: jn_def_dvr_chn_logo jn_def_hvr_chn_logo jn_def_nvr_chn_logo
	
jn_def_dvr_chn_logo:
	chmod a+x ./bmp2yuv.sh;bash ./bmp2yuv.sh -d1

jn_def_hvr_chn_logo:
	chmod a+x ./bmp2yuv.sh;bash ./bmp2yuv.sh -h1

jn_def_nvr_chn_logo:
	chmod a+x ./bmp2yuv.sh;bash ./bmp2yuv.sh -n1


.PHONY: jn_kernel jn_kernel_3520 jn_kernel_3520A jn_kernel_3521 jn_kernel_3531
jn_kernel:
	$(LINK) uImage-hi3515-ppp-huaiwei3g-loop-cramfs-sqfs4-lxm-20130402 $(KERNEL)

jn_kernel_3520:
	$(LINK) uImage-hi3520-ppp-huaiwei3g-loop-cramfs-lxm-20130203 $(KERNEL)

jn_kernel_3520A:
	$(LINK) uImage-hi3520a-lxm-sfc-20140612 $(KERNEL)

jn_kernel_3520D:
	$(LINK) uImage-hi3520d-lxm-hdd-3g-20140121 $(KERNEL)

jn_kernel_3521:
	$(LINK) uImage-hi3521_-lxm-sfc-20140612 $(KERNEL)

jn_kernel_3531:
	$(LINK) uImage-hi3531_-lxm-hdd-3g-20140121 $(KERNEL)
	
jn_kernel_DNVR_3521_vlan:
	$(LINK) uImage-hi3521-dnvr-vlan $(KERNEL)
	
jn_kernel_DNVR_3521_test:
	$(LINK) uImage-hi3521-test-hzj-20130415 $(KERNEL)

.PHONY: conf_r1004n conf_ja7204 conf_ja7204i conf_ja7204e 
.PHONY: conf_ja7208 conf_ja7208i 
.PHONY: conf_ja7216cx conf_ja7216nc
.PHONY: conf_tn9104 conf_n9104 conf_n9108 conf_n9116
.PHONY: conf_d9104 conf_d9108 conf_d9116
.PHONY: conf_n10804
conf_ja7204:
	make premake
	$(CP) conf/conf_ja7204.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	make rmlink
	make mem128_08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_ja7204s:
	make premake
	$(CP) conf/conf_ja7204s.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	make rmlink
	make mem128_08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_ja7204i:
	make premake
	$(CP) conf/conf_ja7204i.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	make rmlink
	make mem128_08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_r1004n:
	make premake
	$(CP) conf/conf_ja7204.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	./replace_str oem \"JA7204\" \"R1004N\"
	make rmlink
	make mem128_08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_ja7208:
	make premake
	$(CP) conf/conf_ja7208.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	make rmlink
	make mem128_08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_ja7208s:
	make premake
	$(CP) conf/conf_ja7208s.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	make rmlink
	make mem128_08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_ja7208i:
	make premake
	$(CP) conf/conf_ja7208i.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	make rmlink
	make mem128_08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_r1008n:
	make premake
	$(CP) conf/conf_ja7208.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	./replace_str oem \"JA7208\" \"R1008N\"
	make rmlink
	make mem128_08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_ja7216cx:
	make premake
	$(CP) conf/conf_ja7216cx.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	make rmlink
	make mem256_16io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_ja7216nc:
	make premake
	$(CP) conf/conf_ja7216nc.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	make rmlink
	make mem256_08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_ja2008nc:
	make premake
	$(CP) conf/conf_ja2008nc.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520
	sh svn2conf
	make rmlink
	make mem128_hi3520
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520
	make postmake
conf_ja2016cx:
	make premake
	$(CP) conf/conf_ja2016cx.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520
	sh svn2conf
	make rmlink
	make mem128_hi3520
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520
	make postmake
conf_ja2016nc:
	make premake
	$(CP) conf/conf_ja2016nc.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520
	sh svn2conf
	make rmlink
	make mem128_hi3520
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520
	make postmake
conf_ja2016icx:
	make premake
	$(CP) conf/conf_ja2016icx.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520
	sh svn2conf
	make rmlink
	make mem128_hi3520
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520
	make postmake
conf_ja2024nc:
	make premake
	$(CP) conf/conf_ja2024nc.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520
	sh svn2conf
	make rmlink
	make mem128_hi3520
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520
	make postmake
conf_ja2032nc:
	make premake
	$(CP) conf/conf_ja2032nc.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520
	sh svn2conf
	make rmlink
	make mem128_hi3520
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520
	make postmake
conf_ja9204rn:
	make premake
	$(CP) conf/conf_ja9204rn.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3515
	sh svn2conf
	make rmlink
	make mem128_08io_960h
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel
	make postmake
conf_ja2104:
	make premake
	$(CP) conf/conf_ja2104.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520A
	sh svn2conf
	make rmlink
	make mem256_hi3520A
	make logo72xx_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3520A
	make postmake
conf_ja2104c:
	make premake
	$(CP) conf/conf_ja2104c.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520D-lite
	sh svn2conf
	make rmlink
	make mem256_hi3520D-cf
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520D
	make postmake
conf_ja2104cf:
	make premake
	$(CP) conf/conf_ja2104cf.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520D-lite
	sh svn2conf
	make rmlink
	make mem256_hi3520D-cf
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520D
	make postmake
conf_ja2104cl:
	make premake
	$(CP) conf/conf_ja2104cl.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520D-lite
	sh svn2conf
	make rmlink
	make mem128_hi3520D-04io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520D
	make postmake
conf_ja2104hd:
	make premake
	$(CP) conf/conf_ja2104hd.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3521
	sh svn2conf
	make rmlink
	make mem512_hi3521_32io
	make logo_juan_256k
	make jn_def_hvr_chn_logo
	make jn_kernel_3521
	make postmake
conf_ja3108hd:
	make premake
	$(CP) conf/conf_ja3108hd.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3531
	sh svn2conf
	make rmlink
	make mem512_hi3531_8io
	make logo_juan_256k
	make jn_def_hvr_chn_logo
	make jn_kernel_3531
	make postmake
conf_ja2104hl:
	make premake
	$(CP) conf/conf_ja2104hl.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520D-lite
	sh svn2conf
	make rmlink
	make mem256_hi3520D-hl
	make logo_juan_128k
	make jn_def_dvr_chn_logo
	make jn_kernel_3520D
	make postmake
conf_ja2108hl:
	make premake
	$(CP) conf/conf_ja2108hl.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3521
	sh svn2conf
	make rmlink
	make mem512_hi3521_32io
	make logo_juan_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3521
	make postmake
conf_ja2108hv:
	make premake
	$(CP) conf/conf_ja2108hv.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3521
	sh svn2conf
	make rmlink
	make mem512_hi3521_32io
	make logo_juan_256k
	make jn_def_hvr_chn_logo
	make jn_kernel_3521
	make postmake
conf_ja3116hl:
	make premake
	$(CP) conf/conf_ja3116hl.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3531
	sh svn2conf
	make rmlink
	make mem1024_hi3531_16io
	make logo_juan_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3531
	make postmake
conf_ja3116hv:
	make premake
	$(CP) conf/conf_ja3116hv.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3531
	sh svn2conf
	make rmlink
	make mem1024_hi3531_16io
	make logo_juan_256k
	make jn_def_hvr_chn_logo
	make jn_kernel_3531
	make postmake
conf_ja2104hf:
	make premake
	$(CP) conf/conf_ja2104hf.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520D
	sh svn2conf
	make rmlink
	make mem256_hi3520D-hf
	make logo_juan_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3520D
	make postmake
conf_ja2104hv:
	make premake
	$(CP) conf/conf_ja2104hv.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520D
	sh svn2conf
	make rmlink
	make mem256_hi3520D-hf
	make logo_juan_256k
	make jn_def_hvr_chn_logo
	make jn_kernel_3520D
	make postmake
conf_ja2106:
	make premake
	$(CP) conf/conf_ja2106.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520A
	sh svn2conf
	make rmlink
	make mem256_hi3520A
	make logo72xx_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3520A
	make postmake
conf_ja2106c:
	make premake
	$(CP) conf/conf_ja2106c.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520D
	sh svn2conf
	make rmlink
	make mem256_hi3520D
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520D
	make postmake
conf_ja2106cf:
	make premake
	$(CP) conf/conf_ja2106cf.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520D-lite
	sh svn2conf
	make rmlink
	make mem256_hi3520D-cf
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520D
	make postmake
conf_ja2106cl:
	make premake
	$(CP) conf/conf_ja2106cl.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520D-lite
	sh svn2conf
	make rmlink
	make mem128_hi3520D-08io
	make logo72xx
	make jn_def_dvr_chn_logo
	make jn_kernel_3520D
	make postmake
conf_ja2108:
	make premake
	$(CP) conf/conf_ja2108.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3521
	sh svn2conf
	make rmlink
	make mem512_hi3521_32io
	make logo72xx_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3521
	make postmake
conf_ja2116:
	make premake
	$(CP) conf/conf_ja2116.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3520A
	sh svn2conf
	make rmlink
	make mem256_hi3520A
	make logo72xx_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3520A
	make postmake
conf_ja2116d:
	make premake
	$(CP) conf/conf_ja2116d.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3521
	sh svn2conf
	make rmlink
	make mem512_hi3521_16d1
	make logo72xx_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3521
	make postmake
conf_ja2124:
	make premake
	$(CP) conf/conf_ja2124.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3521
	sh svn2conf
	make rmlink
	make mem512_hi3521_32io
	make logo72xx_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3521
	make postmake
conf_ja2132:
	make premake
	$(CP) conf/conf_ja2132.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3521
	sh svn2conf
	make rmlink
	make mem512_hi3521_32io
	make logo72xx_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3521
	make postmake
conf_ja3116tw:
	make premake
	$(CP) conf/conf_ja3116tw.h $(CONF)
	$(CP) oem/oem_default.h $(OEM)
	make sdk_3531
	sh svn2conf
	make rmlink
	make mem512_hi3531
	make logo72xx_256k
	make jn_def_dvr_chn_logo
	make jn_kernel_3531
	make postmake

.PHONY: compile_all
compile_all:
	make -C common
	cd app/bsp;make;cd -;
#	cd app/skin;make;cd -;
	cd app;make;cd -;
	make -C gui
#	make -C avstreamsvr
#	make -C web
	make -C upnp_server

.PHONY: clean_all
clean_all:
	make -C common clean
	make -C app clean
	make -C gui clean
#	make -C avstreamsvr clean
#	make -C web clean
	make -C upnp_server clean

.PHONY: rmlink
rmlink:
	$(RM) $(UBOOT)
	$(RM) $(UBOOT_ENV)
	$(RM) $(LOGO)
	$(RM) $(KERNEL)

