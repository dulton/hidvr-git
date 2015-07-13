/*
 *  atp862x.c - Driver for atp8620/atp8624 ahci/ide controller
 *
 *  Maintained by:  G.C.C. <geingching@acard.com>
 *
 *  Copyright 2009 ACARD, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *0.9.4 support kernel 2.6.22/2.6.23
 *0.9.5 support kernel 2.6.30/2.6.31
 */
#define		HIS3520 1

//#define DPRINTK(x...) printk(x)

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#if HIS3520
#include <asm/arch/pci.h>
#else
#include <linux/pci.h>
#endif
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <scsi/scsi_device.h>
#include <linux/device.h>
#include <linux/dmi.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_tcq.h>
#include <scsi/scsi_transport.h>
#include <linux/hdreg.h>
#include <linux/uaccess.h>
#include <linux/libata.h>
#include <linux/cdrom.h>


//===================================================
//NCQ_ENABLE :define to enable NCQ command 1: enable 0: disable
//If disable it stop to send NCQ command in all channel.
//===================================================
#define NCQ_ENABLE 0
//===================================================
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,27)
# define kernel_ver_2628
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28)
# define kernel_ver_2629
#endif

#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,25)
# define kernel_ver_2625
#endif
//#define kernel_ver_2625

/*follow is define to compatable old version*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#  ifndef ata_port_pbar_desc
#   define ata_port_pbar_desc(x...)
#  endif
#endif
#ifndef PM_EVENT_SLEEP
#define PM_EVENT_SLEEP PM_EVENT_SUSPEND
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
# ifndef ATA_EH_RESET
#  define ATA_EH_RESET ATA_EH_RESET_MASK
# endif
#endif
//===================================================
/****His3520 platform don't support atp8620 ide port********/
/*support ide port for atp8620.
If enable it changes queue depth equal one.
The default is disable atp8620 ide port and queue depend on hdd queue depth.
If ide port enable it becomes non-queue.
*/
//#define ATP8620_IDE_PORT
//===================================================
#define DRV_NAME	"atp862x_His"
/*This version for His3520 only*/
#define DRIVE_VERSION	"0.9.7" 

#ifdef kernel_ver_2628
/* Enclosure Management Control */
#define EM_CTRL_MSG_TYPE              0x000f0000

/* Enclosure Management LED Message Type */
#define EM_MSG_LED_HBA_PORT           0x0000000f
#define EM_MSG_LED_PMP_SLOT           0x0000ff00
#define EM_MSG_LED_VALUE              0xffff0000
#define EM_MSG_LED_VALUE_ACTIVITY     0x00070000
#define EM_MSG_LED_VALUE_OFF          0xfff80000
#define EM_MSG_LED_VALUE_ON           0x00010000
/*G.C.C. 2009 0506
static ssize_t atp862x_led_show(struct ata_port *ap, char *buf);
static ssize_t atp862x_led_store(struct ata_port *ap, const char *buf,
			      size_t size);
static ssize_t atp862x_transmit_led_message(struct ata_port *ap, u32 state,
					ssize_t size);*/
#define MAX_SLOTS 8
#endif
static int atp862x_skip_host_reset;
module_param_named(skip_host_reset, atp862x_skip_host_reset, int, 0444);
MODULE_PARM_DESC(skip_host_reset, "skip global host reset (0=don't skip, 1=skip)");
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
/*
static int atp862x_enable_alpm(struct ata_port *ap,
		enum link_pm policy);
static void atp862x_disable_alpm(struct ata_port *ap);
*/
#endif

enum {
	ATP862X_PCI_BAR		= 5,
	ATP862X_PCI_BAR0	= 0,
	ATP862X_PCI_BAR1	= 1,
	ATP862X_USE_CLUSTERING	= 1,
	ATP862X_PCI_BAR2	= 2,
	ATP862X_MAX_PORTS	= 32,
	ATP862X_MAX_SG		= 128, /* hardware max is 64K */	
	ATP862X_DMA_BOUNDARY	= 0xffffffff,
	ATP862X_REG70_MASK	= 0x33333330,
	ATP862X_REG10_MASK	= 0x00000003,
	ATP862X_RX_FIS_D2H  	= 0X11111111,
	ATP862X_RX_FIS_PIO  	= 0X22222222,
	ATP862X_WAIT_AF_RESET	= 150,
	ATP862X_MAX_CMDS	= 32,
	ATP862X_CMD_SZ		= 32,
	ATP862X_CMD_SLOT_SZ	= ATP862X_MAX_CMDS * ATP862X_CMD_SZ,
	ATP862X_RX_FIS_SZ	= 128,
	ATP862X_CMD_TBL_CDB	= 0x40,
	ATP862X_CMD_TBL_HDR_SZ	= 0x80,
	ATP862X_CMD_TBL_SZ	= ATP862X_CMD_TBL_HDR_SZ + (ATP862X_MAX_SG * 16),
	ATP862X_CMD_TBL_AR_SZ	= ATP862X_CMD_TBL_SZ * ATP862X_MAX_CMDS,
	ATP862X_PORT_PRIV_DMA_SZ	= ATP862X_CMD_SLOT_SZ + ATP862X_CMD_TBL_AR_SZ +
				  ATP862X_RX_FIS_SZ,
	ATP8620_IDE_PRD_TBL_SZ	= (256*16),
	ATP862X_IRQ_ON_SG	= (1 << 31),	
	ATP862X_CMD_ATAPI	= (1 << 5),
	ATP862X_CMD_WRITE	= (1 << 6),
	ATP862X_CMD_PREFETCH	= (1 << 7),
	ATP862X_CMD_RESET	= (1 << 8),
	ATP862X_CMD_CLR_BUSY	= (1 << 10),	

	RX_FIS_PIO		= 0x20,
	RX_FIS_D2H_REG		= 0x40,	/* offset of D2H Register FIS data */
	RX_FIS_SDB		= 0x58, /* offset of SDB FIS data */
	RX_FIS_UNK		= 0x60, /* offset of Unknown FIS data */
	
	board_atp8620		= 0,
	board_atp8624		= 1,
//	board_atp_atp8628	= 2,	
	/* global controller registers */
	HOST_CAP		= 0x00, /* host capabilities */
	HOST_CTL		= 0x04, /* global host control */
	HOST_IRQ_STAT		= 0x08, /* interrupt status */
	HOST_PORTS_IMPL		= 0x0c, /* bitmap of implemented ports */
	HOST_VERSION		= 0x10, /* atp862x spec. version compliancy */
	HOST_EM_LOC		= 0x1c, /* Enclosure Management location */
	HOST_EM_CTL		= 0x20, /* Enclosure Management Control */

	/* HOST_CTL bits */
	HOST_RESET		= (1 << 0),  /* reset controller; self-clear */
	HOST_IRQ_EN		= (1 << 1),  /* global IRQ enable */
	HOST_ATP862X_EN		= (1 << 31), /* atp862x enabled */

	/* HOST_CAP bits */
	HOST_CAP_EMS		= (1 << 6),  /* Enclosure Management support */
	HOST_CAP_SSC		= (1 << 14), /* Slumber capable */
	HOST_CAP_PMP		= (1 << 17), /* Port Multiplier support */
	HOST_CAP_CLO		= (1 << 24), /* Command List Override support */
	HOST_CAP_ALPM		= (1 << 26), /* Aggressive Link PM support */
	HOST_CAP_SSS		= (1 << 27), /* Staggered Spin-up */
	HOST_CAP_SNTF		= (1 << 29), /* SNotification register */
	HOST_CAP_NCQ		= (1 << 30), /* Native Command Queueing */
	HOST_CAP_64		= (1 << 31), /* PCI DAC (64-bit DMA) support */
	/*global ide reg*/
	IDE_DMA_START_STOP	= 0,
	IDE_IRQ_MASK		= 1,
	IDE_IRQ_STATUS		= 2,
	IDE_PIO_MASTER_SPEED	= 4,
	IDE_PIO_SLAVE_SPEED	= 5,
	IDE_PIO_PORT_SPEED	= 6,
	IDE_UDMA_SPEED		= 7,
	IDE_DMA_TABLE_OFS_L	= 8,	
	IDE_DMA_TABLE_OFS_H	= 0xC,
	/*ide irq*/
	IDE_IRQ_CHECK		= 0x100,
	IDE_SRST		= (1 << 0x3),
//	IDE_DMA_STATUS		= 11,
	/* registers for each SATA port */
	PORT_LST_ADDR		= 0x00, /* command list DMA addr */
	PORT_LST_ADDR_HI	= 0x04, /* command list DMA addr hi */
	PORT_FIS_ADDR		= 0x08, /* FIS rx buf addr */
	PORT_FIS_ADDR_HI	= 0x0c, /* FIS rx buf addr hi */
	PORT_IRQ_STAT		= 0x10, /* interrupt status */
	PORT_IRQ_MASK		= 0x14, /* interrupt enable/disable mask */
	PORT_CMD		= 0x18, /* port command */
	PORT_TFDATA		= 0x20,	/* taskfile data */
	PORT_SIG		= 0x24,	/* device TF signature */
	PORT_CMD_ISSUE		= 0x38, /* command issue */
	PORT_SCR_STAT		= 0x28, /* SATA phy register: SStatus */
	PORT_SCR_CTL		= 0x2c, /* SATA phy register: SControl */
	PORT_SCR_ERR		= 0x30, /* SATA phy register: SError */
	PORT_SCR_ACT		= 0x34, /* SATA phy register: SActive */
	PORT_SCR_NTF		= 0x3c, /* SATA phy register: SNotification */
	PORT_SDB_FLAG		= 0X44,
	PORT_IRQA_STAT		= 0x70, /* SATA addtional interrupt status*/
	/* PORT_SCR_ERR bits*/
	PORT_SERR_N		= (1 << 16),
	/* PORT_IRQ_{STAT,MASK} bits */
	PORT_IRQ_COLD_PRES	= (1 << 31), /* cold presence detect */
	PORT_IRQ_TF_ERR		= (1 << 30), /* task file error */
	PORT_IRQ_HBUS_ERR	= (1 << 29), /* host bus fatal error */
	PORT_IRQ_HBUS_DATA_ERR	= (1 << 28), /* host bus data error */
	PORT_IRQ_IF_ERR		= (1 << 27), /* interface fatal error */
	PORT_IRQ_IF_NONFATAL	= (1 << 26), /* interface non-fatal error */
	PORT_IRQ_OVERFLOW	= (1 << 24), /* xfer exhausted available S/G */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
	PORT_IRQ_BAD_PMP	= (1 << 23), /* incorrect port multiplier */
#else
	PORT_IRQ_BAD_PMP	= 0,
#endif
	PORT_IRQ_PHYRDY		= (1 << 22), /* PhyRdy changed */
	PORT_IRQ_NOTICE		= (1 << 8), /* PM port PhyRdy changed */
	PORT_IRQ_DEV_ILCK	= (1 << 7), /* device interlock */
	PORT_IRQ_CONNECT	= (1 << 6), /* port connect change status */
	PORT_IRQ_SG_DONE	= (1 << 5), /* descriptor processed */
	PORT_IRQ_UNK_FIS	= (1 << 4), /* unknown FIS rx'd */
	PORT_IRQ_SDB_FIS	= (1 << 3), /* Set Device Bits FIS rx'd */
	PORT_IRQ_DMAS_FIS	= (1 << 2), /* DMA Setup FIS rx'd */
	PORT_IRQ_PIOS_FIS	= (1 << 1), /* PIO Setup FIS rx'd */
	PORT_IRQ_D2H_REG_FIS	= (1 << 0), /* D2H Register FIS rx'd */

	PORT_IRQ_FREEZE		= PORT_IRQ_HBUS_ERR |
				  PORT_IRQ_IF_ERR |
				  PORT_IRQ_CONNECT |
				  PORT_IRQ_PHYRDY |
				  PORT_IRQ_UNK_FIS |
				  PORT_IRQ_BAD_PMP,
	PORT_IRQ_ERROR		= PORT_IRQ_FREEZE |
				  PORT_IRQ_TF_ERR |
				  PORT_IRQ_HBUS_DATA_ERR,
	DEF_PORT_IRQ		= PORT_IRQ_ERROR | PORT_IRQ_SG_DONE |
				  PORT_IRQ_SDB_FIS | PORT_IRQ_DMAS_FIS |
				  PORT_IRQ_PIOS_FIS | PORT_IRQ_D2H_REG_FIS |
				  PORT_IRQ_NOTICE,

	/* PORT_CMD bits */
	PORT_CMD_ASP		= (1 << 27), /* Aggressive Slumber/Partial */
	PORT_CMD_ALPE		= (1 << 26), /* Aggressive Link PM enable */
	PORT_CMD_ATAPI		= (1 << 24), /* Device is ATAPI */
	PORT_CMD_PMP		= (1 << 17), /* PMP attached */
	PORT_CMD_LIST_ON	= (1 << 15), /* cmd list DMA engine running */
	PORT_CMD_FIS_ON		= (1 << 14), /* FIS DMA engine running */
	PORT_CMD_FIS_RX		= (1 << 4), /* Enable FIS receive DMA engine */
	PORT_CMD_CLO		= (1 << 3), /* Command list override */
	PORT_CMD_POWER_ON	= (1 << 2), /* Power up device */
	PORT_CMD_SPIN_UP	= (1 << 1), /* Spin up device */
	PORT_CMD_START		= (1 << 0), /* Enable port DMA engine */

	PORT_CMD_ICC_MASK	= (0xf << 28), /* i/f ICC state mask */
	PORT_CMD_ICC_ACTIVE	= (0x1 << 28), /* Put i/f in active state */
	PORT_CMD_ICC_PARTIAL	= (0x2 << 28), /* Put i/f in partial state */
	PORT_CMD_ICC_SLUMBER	= (0x6 << 28), /* Put i/f in slumber state */

	/* hpriv->flags bits */
	ATP862X_HFLAG_NO_NCQ		= (1 << 0),
	ATP862X_HFLAG_IGN_IRQ_IF_ERR	= (1 << 1), /* ignore IRQ_IF_ERR */
	ATP862X_HFLAG_IGN_SERR_INTERNAL	= (1 << 2), /* ignore SERR_INTERNAL */
	ATP862X_HFLAG_32BIT_ONLY	= (1 << 3), /* force 32bit */
	ATP862X_HFLAG_ATP_PATA		= (1 << 4), /* PATA port */
	ATP862X_HFLAG_NO_MSI		= (1 << 5), /* no PCI MSI */
	ATP862X_HFLAG_NO_PMP		= (1 << 6), /* no PMP */
	ATP862X_HFLAG_NO_HOTPLUG	= (1 << 7), /* ignore PxSERR.DIAG.N */
	ATP862X_HFLAG_SECT255		= (1 << 8), /* max 255 sectors */
	ATP862X_HFLAG_YES_NCQ		= (1 << 9), /* force NCQ cap on */
	ATP862X_FLAG_IGN_SERR_INTERNAL	= (1 << 27), /* ignore SERR_INTERNAL */
#ifdef kernel_ver_2625
	ATP862X_LFLAG_COMMON		= ATA_LFLAG_SKIP_D2H_BSY,
#endif
	/* ap->flags bits */

	ATP862X_FLAG_COMMON		= ATA_FLAG_SATA | ATA_FLAG_NO_LEGACY |
					  ATA_FLAG_MMIO | ATA_FLAG_PIO_DMA |
					  ATA_FLAG_ACPI_SATA  
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
					  | ATA_FLAG_SKIP_D2H_BSY
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,24)
					  | ATA_FLAG_IPM | ATA_FLAG_AN
#endif
					  ,
	ICH_MAP				= 0x90, /* ICH MAP register */

	/* em_ctl bits */
	EM_CTL_RST			= (1 << 9), /* Reset */
	EM_CTL_TM			= (1 << 8), /* Transmit Message */
	EM_CTL_ALHD			= (1 << 26), /* Activity LED */
	/*compatable flag*/
	ATP862X_FLAG_PMP		= (1 << 19),
	ATP862X_PFLAG_RESETTING		= (1 << 8),
};
typedef struct _MailBox 
{
	unsigned int	BufAddress;
	unsigned int	BufAddressU;
	unsigned int	Reserved;
	unsigned int	DataLength;
} T_MailBox,*P_MailBox;
struct atp862x_cmd_hdr {
	__le32			opts;
	__le32			status;
	__le32			tbl_addr;
	__le32			tbl_addr_hi;
	__le32			reserved[4];
};

struct atp862x_sg {
	__le32			addr;
	__le32			addr_hi;
	__le32			reserved;
	__le32			flags_size;
};
#ifdef kernel_ver_2628
struct atp862x_em_priv {
	enum sw_activity blink_policy;
	struct timer_list timer;
	unsigned long saved_activity;
	unsigned long activity;
	unsigned long led_state;
};
#endif
struct atp862x_host_priv {
	unsigned int		flags;		/* atp862x_HFLAG_* */
	u32			cap;		/* cap to use */
	u32			port_map;	/* port map to use */
	u32			saved_cap;	/* saved initial cap */
	u32			saved_port_map;	/* saved initial port_map */
#ifdef kernel_ver_2628
	u32 			em_loc; /* enclosure management location */
#endif
};

struct atp862x_port_priv {
	struct ata_link		*active_link;
	struct atp862x_cmd_hdr	*cmd_slot;
	dma_addr_t		cmd_slot_dma;
	void			*cmd_tbl;
	dma_addr_t		cmd_tbl_dma;
	void			*rx_fis;
	dma_addr_t		rx_fis_dma;
	/* for NCQ spurious interrupt analysis */
	unsigned int		ncq_saw_d2h:1;
	unsigned int		ncq_saw_dmas:1;
	unsigned int		ncq_saw_sdb:1;
	u32 			intr_mask;	/* interrupts to enable */
#ifdef kernel_ver_2628
	struct atp862x_em_priv	em_priv[MAX_SLOTS];/* enclosure management info
					 	 * per PM slot */	
#endif
};
/*ide function*/
static void atp8620_init_addrs(struct ata_port *ap);
static void atp8620_sff_irq_clear(struct ata_port *ap);
static void atp8620_bmdma_setup(struct ata_queued_cmd *qc);
static void atp8620_load_pio_mode (struct ata_port *ap, struct ata_device *adev, unsigned int pio);
static void atp8620_set_pio_mode(struct ata_port *ap, struct ata_device *adev);
static void atp8620_set_dma_mode (struct ata_port *ap, struct ata_device *adev);
static unsigned int ide_fill_sg(struct ata_queued_cmd *qc);
static void atp8620_sff_qc_prep(struct ata_queued_cmd *qc);
static u8 atp8620_sff_irq_on(struct ata_port *ap);
static int atp8620_port_start(struct ata_port *ap);
static int atp8620_sff_port_start(struct ata_port *ap);
//ata scsi config
/*
static void atp8620_scsi_sdev_config(struct scsi_device *sdev);
static int atp8620_scsi_slave_config(struct scsi_device *sdev);
static int atp8620_scsi_dev_config(struct scsi_device *sdev,
			       struct ata_device *dev);
static struct ata_device *__atp8620_scsi_find_dev(struct ata_port *ap,
					      const struct scsi_device *scsidev);*/
static int atp8620_cable_detect(struct ata_port *ap);
/*============================================*/
static irqreturn_t atp862x_interrupt(int irq, void *dev_instance);
static int atp8624_prepare_host(struct pci_dev *pdev, struct ata_host **r_host, struct ata_port_info *pi);
static int atp8620_prepare_host(struct pci_dev *pdev, struct ata_host **r_host, struct ata_port_info *pi);
#ifdef kernel_ver_2628
static int atp862x_scr_read(struct ata_link *link, unsigned int sc_reg, u32 *val);
static int atp862x_scr_write(struct ata_link *link, unsigned int sc_reg, u32 val);
#else
static int atp862x_scr_read(struct ata_port *ap, unsigned int sc_reg, u32 *val);
static int atp862x_scr_write(struct ata_port *ap, unsigned int sc_reg, u32 val);
#endif
static int atp862x_init_one(struct pci_dev *pdev, const struct pci_device_id *ent);
static unsigned int atp862x_qc_issue(struct ata_queued_cmd *qc);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
static int atp862x_softreset(struct ata_port *ap, unsigned int *class);
static int atp862x_hardreset(struct ata_port *ap, unsigned int *class);
#  else
static int atp862x_softreset(struct ata_port *ap, unsigned int *class,
  			  unsigned long deadline);
static int atp862x_hardreset(struct ata_port *ap, unsigned int *class,
			  unsigned long deadline);
#  endif
static void atp862x_postreset(struct ata_port *ap, unsigned int *class);
#else
static int atp862x_softreset(struct ata_link *link, unsigned int *class,
			  unsigned long deadline);
static int atp862x_hardreset(struct ata_link *link, unsigned int *class,
			  unsigned long deadline);
static void atp862x_postreset(struct ata_link *link, unsigned int *class);
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25)
static bool atp862x_qc_fill_rtf(struct ata_queued_cmd *qc);
#else
static void atp862x_tf_read(struct ata_port *ap, struct ata_taskfile *tf);
#endif
static int atp862x_port_start(struct ata_port *ap);
static void atp862x_port_stop(struct ata_port *ap);
static void atp862x_qc_prep(struct ata_queued_cmd *qc);
static void atp862x_freeze(struct ata_port *ap);
static void atp862x_thaw(struct ata_port *ap);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
static void atp862x_dev_config(struct ata_device *dev);
static void atp862x_pmp_attach(struct ata_port *ap);
static void atp862x_pmp_detach(struct ata_port *ap);
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
static u32 atp862x_scr_read2(struct ata_port *ap, unsigned int sc_reg);
static void atp862x_scr_write2(struct ata_port *ap, unsigned int sc_reg,u32 val);
#endif
static void atp862x_error_handler(struct ata_port *ap);
static void atp862x_post_internal_cmd(struct ata_queued_cmd *qc);
static int atp862x_port_resume(struct ata_port *ap);
static unsigned int atp_fill_sg(struct ata_queued_cmd *qc, void *cmd_tbl);
static void atp862x_fill_cmd_slot(struct atp862x_port_priv *pp, unsigned int tag,
			       u32 opts);
#ifdef CONFIG_PM
static int atp862x_port_suspend(struct ata_port *ap, pm_message_t mesg);
static int atp862x_pci_device_suspend(struct pci_dev *pdev, pm_message_t mesg);
static int atp862x_pci_device_resume(struct pci_dev *pdev);
#endif
/*
#ifdef kernel_ver_2628
static ssize_t atp862x_activity_show(struct ata_device *dev, char *buf);
static ssize_t atp862x_activity_store(struct ata_device *dev,
				   enum sw_activity val);
static void atp862x_init_sw_activity(struct ata_link *link);
#endif
*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
//================================================================================
#if LINUX_VERSION_CODE >KERNEL_VERSION(2,6,23)
static void sata_pmp_error_handler(struct ata_port *ap);
#endif
static u8 atp862x_check_status(struct ata_port *ap);
static void atp862x_irq_clear(struct ata_port *ap);
//===ide==========================================================================
static void atp8620_error_handler(struct ata_port *ap);

//================================================================================
/*
static struct class_device_attribute *atp8620_shost_attrs[] = {
	&class_device_attr_link_power_management_policy,
	NULL
};*/
#else
static struct device_attribute *atp862x_shost_attrs[] = {
	&dev_attr_link_power_management_policy,
#ifdef kernel_ver_2628
	&dev_attr_em_message_type,
	&dev_attr_em_message,
	NULL
};

static struct device_attribute *atp862x_sdev_attrs[] = {
	&dev_attr_sw_activity,
	&dev_attr_unload_heads,
#endif
	NULL
};
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
static struct scsi_host_template atp862x_sht = {
	.module			= THIS_MODULE,
	.name			= DRV_NAME,
	.ioctl			= ata_scsi_ioctl,
	.queuecommand		= ata_scsi_queuecmd,
	.change_queue_depth	= ata_scsi_change_queue_depth,
	.can_queue		= ATP862X_MAX_CMDS - 1,
	.this_id		= ATA_SHT_THIS_ID,
	.sg_tablesize		= ATP862X_MAX_SG,
	.cmd_per_lun		= ATA_SHT_CMD_PER_LUN,
	.emulated		= ATA_SHT_EMULATED,
	.use_clustering		= ATP862X_USE_CLUSTERING,
	.proc_name		= DRV_NAME,
	.dma_boundary		= ATA_DMA_BOUNDARY,
	.slave_configure	= ata_scsi_slave_config,
	.slave_destroy		= ata_scsi_slave_destroy,
	.bios_param		= ata_std_bios_param,
//	.shost_attrs		= atp8620_shost_attrs,
};
#else
static struct scsi_host_template atp862x_sht = {
	ATA_NCQ_SHT(DRV_NAME),
#ifndef ATP8620_IDE_PORT
	.can_queue		= ATP862X_MAX_CMDS - 1,
#endif
	.sg_tablesize		= ATP862X_MAX_SG,
	.dma_boundary		= ATP862X_DMA_BOUNDARY,
//	.slave_configure	= atp8620_scsi_slave_config,
	.shost_attrs		= atp862x_shost_attrs,
#ifdef kernel_ver_2628
	.sdev_attrs		= atp862x_sdev_attrs,	
#endif
};
#endif


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
static const struct ata_port_operations atp8620_pata_ops = {
	.set_piomode		= atp8620_set_pio_mode,
	.set_dmamode		= atp8620_set_dma_mode,

	.tf_load		= ata_tf_load,
	.tf_read		= ata_tf_read,
	.check_status		= ata_check_status,
	.exec_command		= ata_exec_command,
	.dev_select		= ata_std_dev_select,

	.freeze			= ata_bmdma_freeze,
	.thaw			= ata_bmdma_thaw,
	.error_handler		= atp8620_error_handler,
	.post_internal_cmd 	= ata_bmdma_post_internal_cmd,
#  if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,21)
	.cable_detect		= atp8620_cable_detect,
#  endif
	.bmdma_setup		= atp8620_bmdma_setup,
	.bmdma_start		= ata_bmdma_start,
	.bmdma_stop		= ata_bmdma_stop,
	.bmdma_status		= ata_bmdma_status,
	.qc_prep		= atp8620_sff_qc_prep,
	.qc_issue		= ata_qc_issue_prot,
	.data_xfer		= ata_data_xfer,

	.irq_handler		= atp862x_interrupt,//ata_interrupt,
	.irq_clear		= atp8620_sff_irq_clear,
	.irq_on			= atp8620_sff_irq_on,
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	.port_disable		= ata_port_disable,
	.irq_ack		= ata_irq_ack,
#  endif
	.port_start		= atp8620_sff_port_start,
};
#else
static struct ata_port_operations atp8620_base_ops = {
	.inherits		= &ata_bmdma_port_ops,
	.qc_prep		= atp8620_sff_qc_prep,
	.sff_irq_on		= atp8620_sff_irq_on,
	.sff_irq_clear		= atp8620_sff_irq_clear,
//	.sff_check_status	= atp8620_sff_check_status,
	.port_start		= atp8620_sff_port_start,	
};
static struct ata_port_operations atp8620_pata_ops = {
	.inherits		= &atp8620_base_ops,
	.cable_detect		= atp8620_cable_detect,
	.set_piomode		= atp8620_set_pio_mode,
	.set_dmamode		= atp8620_set_dma_mode,
	.bmdma_setup		= atp8620_bmdma_setup,
};
#endif
static struct ata_port_operations atp862x_ops = {
	
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25)
	.qc_fill_rtf		= atp862x_qc_fill_rtf,
	.inherits		= &sata_pmp_port_ops,	
	.softreset		= atp862x_softreset,
	.hardreset		= atp862x_hardreset,
	.postreset		= atp862x_postreset,
	.pmp_softreset		= atp862x_softreset,		
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	.port_disable		= ata_port_disable,
	.irq_on			= ata_dummy_irq_on,
	.irq_ack		= ata_dummy_irq_ack,
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
	.irq_handler		= atp862x_interrupt,
#endif
	.check_status		= atp862x_check_status,
	.check_altstatus	= atp862x_check_status,
	.dev_select		= ata_noop_dev_select,
	.irq_clear		= atp862x_irq_clear,
	.tf_read		= atp862x_tf_read,
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
	.qc_defer		= sata_pmp_qc_defer_cmd_switch,
	.pmp_attach		= atp862x_pmp_attach,
	.pmp_detach		= atp862x_pmp_detach,
	.dev_config		= atp862x_dev_config,
#endif
	.qc_prep		= atp862x_qc_prep,
	.qc_issue		= atp862x_qc_issue,
	

	.freeze			= atp862x_freeze,
	.thaw			= atp862x_thaw,

	.error_handler		= atp862x_error_handler,
	.post_internal_cmd	= atp862x_post_internal_cmd,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
	.scr_read		= atp862x_scr_read2,
	.scr_write		= atp862x_scr_write2,
#else
	.scr_read		= atp862x_scr_read,
	.scr_write		= atp862x_scr_write,
#endif
	
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
/*
	.enable_pm		= atp862x_enable_alpm,
	.disable_pm		= atp862x_disable_alpm,
*/
#endif
#ifdef kernel_ver_2628
/*G.C.C. 2009 0506
	.em_show		= atp862x_led_show,
	.em_store		= atp862x_led_store,
	.sw_activity_show	= atp862x_activity_show,
	.sw_activity_store	= atp862x_activity_store,*/
#endif
#ifdef CONFIG_PM
	.port_suspend		= atp862x_port_suspend,
	.port_resume		= atp862x_port_resume,
#endif
	.port_start		= atp862x_port_start,
	.port_stop		= atp862x_port_stop,
};

#define ATP862X_HFLAGS(flags)	.private_data	= (void *)(flags)
static const struct ata_port_info atp862x_port_info[] = {
	/* board_atp8620 */
	{		
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
		.sht		= &atp862x_sht,
#endif
		.flags		= ATP862X_FLAG_COMMON,
#ifdef kernel_ver_2625
		.link_flags	= ATP862X_LFLAG_COMMON,
#endif
		.pio_mask	= 0x07,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &atp862x_ops,
	},
		/* board_atp8624 */
	{
		.flags		= ATP862X_FLAG_COMMON,
		.pio_mask	= 0x07,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &atp862x_ops,
	},
};
static const struct ata_port_info atp862x_pata_port_info[] = {
	/* board_atp862x_pata */
	{
		ATP862X_HFLAGS	(ATP862X_HFLAG_NO_NCQ | ATP862X_HFLAG_NO_MSI |
				 ATP862X_HFLAG_ATP_PATA | ATP862X_HFLAG_NO_PMP),
		.flags		= ATA_FLAG_SLAVE_POSS | ATA_FLAG_PIO_POLLING,
		.pio_mask	= 0x07,
		.mwdma_mask	= 0x7,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &atp8620_pata_ops,
	},
};
static const struct pci_device_id atp862x_pci_tbl[] = {	
	/*ACARD*/
	{ PCI_VDEVICE(ARTOP, 0x000d), board_atp8620 },//ACARD
	{ PCI_VDEVICE(ARTOP, 0x0014), board_atp8624 },//ACARD
//	{ PCI_VDEVICE(ARTOP, 0x0018), board_atp_atp8628 },//ACARD

	{ }	/* terminate list */
};


static struct pci_driver atp862x_pci_driver = {
	.name			= DRV_NAME,
	.id_table		= atp862x_pci_tbl,
	.probe			= atp862x_init_one,
	.remove			= ata_pci_remove_one,
#ifdef CONFIG_PM
	.suspend		= atp862x_pci_device_suspend,
	.resume			= atp862x_pci_device_resume,
#endif
};
#ifdef kernel_ver_2628
static int atp862x_em_messages = 1;
module_param(atp862x_em_messages, int, 0444);
/* add other LED protocol types when they become supported */
MODULE_PARM_DESC(atp862x_em_messages,
	"Set atp862x Enclosure Management Message type (0 = disabled, 1 = LED");
/*
#if defined(CONFIG_PATA_MARVELL) || defined(CONFIG_PATA_MARVELL_MODULE)
static int marvell_enable;
#else
static int marvell_enable = 1;
#endif
module_param(marvell_enable, int, 0644);
MODULE_PARM_DESC(marvell_enable, "Marvell SATA via atp862x (1 = enabled)");
*/
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
static void atp8620_error_handler(struct ata_port *ap)
{
	ata_bmdma_drive_eh(ap, ata_std_prereset,
				    ata_std_softreset, NULL,
				    ata_std_postreset);
}
#endif

static inline int atp862x_nr_ports(u32 cap)
{
	return (cap & 0x1f) + 1;
}
/*
static void atp8620_scsi_sdev_config(struct scsi_device *sdev)
{
	sdev->use_10_for_rw = 1;
	sdev->use_10_for_ms = 1;
	
	sdev->max_device_blocked = 1;
}*/
/*
static int atp8620_atapi_drain_needed(struct request *rq)
{
	if (likely(!blk_pc_request(rq)))
		return 0;

	if (!rq->data_len || (rq->cmd_flags & REQ_RW))
		return 0;

	return atapi_cmd_type(rq->cmd[0]) == ATAPI_MISC;
}*/
/*
static struct ata_device * atp8620_scsi_find_dev(struct ata_port *ap, const struct scsi_device *scsidev)
{
	struct ata_device *dev = __atp8620_scsi_find_dev(ap, scsidev);

	if (unlikely(!dev || !ata_dev_enabled(dev)))
		return NULL;

	return dev;
}*/

static int atp8620_cable_detect(struct ata_port *ap)
{	
	return ATA_CBL_PATA80;
}
static int atp8620_port_start(struct ata_port *ap)
{
	struct device *dev = ap->dev;
	int rc;
	ap->prd = dmam_alloc_coherent(dev, ATP8620_IDE_PRD_TBL_SZ, &ap->prd_dma,
				      GFP_KERNEL);
	if (!ap->prd)
		return -ENOMEM;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
	rc = ata_pad_alloc(ap, dev);
	if (rc)
		return rc;
#endif
	return 0;
}

static int atp8620_sff_port_start(struct ata_port *ap)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	atp8620_port_start(ap);
#else
	if (ap->ioaddr.bmdma_addr)
		return atp8620_port_start(ap);
#endif
	return 0;
}
static void atp8620_sff_qc_prep(struct ata_queued_cmd *qc)
{
	if (!(qc->flags & ATA_QCFLAG_DMAMAP))
		return;	

	ide_fill_sg(qc);
}

static void atp8620_set_dma_mode (struct ata_port *ap, struct ata_device *adev)
{	
	unsigned int pio	= adev->pio_mode - XFER_PIO_0;
//	struct pci_dev *pdev	= to_pci_dev(ap->host->dev);
	void __iomem *mmio 	= ap->ioaddr.bmdma_addr;
	u8 ultra;

	
//	printk("atp8620_set_dma_mode %p\n",mmio);
	if (adev->dma_mode == XFER_MW_DMA_0)
		pio = 1;
	else
		pio = 4;

	/* Load the PIO timing active/recovery bits */
	atp8620_load_pio_mode(ap, adev, pio);

	/* Add ultra DMA bits if in UDMA mode */
//	pci_read_config_byte(pdev, 0x44 + ap->port_no, &ultra);
	ultra = ioread8(mmio + IDE_UDMA_SPEED);
	ultra &= ~(7 << (4  * adev->devno));	/* One nibble per drive */
	if (adev->dma_mode >= XFER_UDMA_0) {
		u8 mode = adev->dma_mode - XFER_UDMA_0 + 1;
		if (mode == 0)
			mode = 1;
		ultra |= (mode << (4 * adev->devno));
	}
	iowrite8(ultra,mmio + IDE_UDMA_SPEED);
//	pci_write_config_byte(pdev, 0x44 + ap->port_no, ultra);
}
static void atp8620_bmdma_setup(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	unsigned int rw = (qc->tf.flags & ATA_TFLAG_WRITE);
	u8 dmactl;

	dmactl = ioread8(ap->ioaddr.bmdma_addr + IDE_DMA_START_STOP);
//	printk("dmactl %x\n",dmactl);
	dmactl &= ~(ATA_DMA_WR | ATA_DMA_START);	
	if (!rw)
		dmactl |= ATA_DMA_WR;
	iowrite8(dmactl, ap->ioaddr.bmdma_addr + IDE_DMA_START_STOP);

	/* load PRD table addr. */
	mb();	/* make sure PRD table writes are visible to controller */
	iowrite32(ap->prd_dma, ap->ioaddr.bmdma_addr + IDE_DMA_TABLE_OFS_L);	
//	printk("prd_dma %x dma %x\n",ap->prd_dma,ioread32(ap->ioaddr.bmdma_addr + IDE_DMA_TABLE_OFS_L));
//	printk("qc->tf->device %x device_no %x\n",qc->tf.device,qc->dev->devno);	
//	iowrite32(0, ap->ioaddr.bmdma_addr + IDE_DMA_TABLE_OFS_H);
	/* specify data direction, triple-check start bit is clear */	

	/* issue r/w command */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	ap->ops->exec_command(ap, &qc->tf);
#else
	ap->ops->sff_exec_command(ap, &qc->tf);
#endif
}

static void atp8620_load_pio_mode (struct ata_port *ap, struct ata_device *adev, unsigned int pio)
{
//	struct pci_dev *pdev	= to_pci_dev(ap->host->dev);
//	int dn = adev->devno;
	void __iomem *mmio 	= ap->ioaddr.bmdma_addr;
	u8	PioM,PioS;
	const u8 timing[2][5] = {
		{ 0x6e, 0x58, 0x44, 0x33, 0x31 },
		{ 0x70, 0x7A, 0x78, 0x43, 0x41 }

	};
//	printk("atp8620_load_pio_mode %p\n",mmio);
	/* Load the PIO timing active/recovery bits */
//	pci_write_config_byte(pdev, 0x40 + dn, timing[0][pio]);
	
	if(adev->devno) { //slave				
		iowrite8(timing[0][pio],mmio + IDE_PIO_SLAVE_SPEED);
	} else {			
		iowrite8(timing[0][pio],mmio + IDE_PIO_MASTER_SPEED);
	}
	PioM = ioread8(mmio + IDE_PIO_MASTER_SPEED);
	PioS = ioread8(mmio + IDE_PIO_SLAVE_SPEED);
	if (PioM < PioS) {
		PioM = PioS;
	}	
	iowrite8(PioM,mmio + IDE_PIO_PORT_SPEED);
}
static void atp8620_set_pio_mode(struct ata_port *ap, struct ata_device *adev)
{
//	struct pci_dev *pdev	= to_pci_dev(ap->host->dev);
	void __iomem *mmio = ap->ioaddr.bmdma_addr;
	int dn = adev->devno;
	u8 ultra;

//	adev->flags |= ATA_DFLAG_NCQ_OFF;
//	printk("atp8620_set_pio_mode %p\n",mmio);
	atp8620_load_pio_mode(ap, adev, adev->pio_mode - XFER_PIO_0);

	/* Clear the UDMA mode bits (set_dmamode will redo this if needed) */
	ultra = ioread8(mmio + IDE_UDMA_SPEED);
//	pci_read_config_byte(pdev, 0x54, &ultra);
	ultra &= ~(7 << (4 * dn));	
	iowrite32(ultra,mmio + IDE_UDMA_SPEED); //G.C.C. disable ultra dma and enable multiword dma
//	pci_write_config_byte(pdev, 0x54, ultra);
}
static void atp8620_sff_irq_clear(struct ata_port *ap)
{
	void __iomem *mmio = ap->ioaddr.bmdma_addr;
	u8	status;
	
	if (!mmio)
		return;
	status = ioread8(mmio + IDE_IRQ_STATUS) | 0x4;
//	printk("atp8620_sff_irq_clear %p irq status %x\n",mmio,status);
	iowrite8(status, mmio + IDE_IRQ_STATUS);
}

static u8 atp8620_sff_irq_on(struct ata_port *ap)
{
	struct ata_ioports *ioaddr = &ap->ioaddr;
	void __iomem *mmio;	
//	struct atp862x_host_priv *hpriv = ap->host->private_data;	
	u8 tmp,irq_status;	
	ap->ctl &= ~ATA_NIEN;
	ap->last_ctl = ap->ctl;

	if (ioaddr->ctl_addr)
		iowrite8(ap->ctl, ioaddr->ctl_addr);
	tmp = ata_wait_idle(ap);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	ap->ops->irq_clear(ap);
#else
	ap->ops->sff_irq_clear(ap);
#endif
//	if (hpriv->flags & ATP862X_HFLAG_ATP_PATA) {		
	mmio = ap->host->iomap[ATP862X_PCI_BAR0];		
	irq_status = ioread8(mmio + 0x91);
	iowrite8(irq_status|0x08, mmio + 0x91);
//	}
	return tmp;
}

//=========================================================================
static inline void __iomem *__atp862x_port_base(struct ata_host *host,
					     unsigned int port_no)
{
	void __iomem *mmio;
//	u8	port_num = 0;
	mmio = 0;
		
	mmio = host->iomap[ATP862X_PCI_BAR]+0x100;
/*	if(port_no <= 1) { 	
	//	printk("port %x\n",port_no);	
		mmio = host->iomap[ATP862X_PCI_BAR1];
		port_num = port_no;
	} else if (port_no > 1 || port_no <= 3){
	//	printk("port %x\n",port_no);
		mmio = host->iomap[ATP862X_PCI_BAR2];
		port_num = port_no - 2 ;
	}*/
//	printk("mmio %x\n",(unsigned int)(mmio + (port_no * 0x80)));
	return mmio + (port_no * 0x80);
}

static inline void __iomem *atp862x_port_base(struct ata_port *ap)
{
	return __atp862x_port_base(ap->host, ap->port_no);
}
//=================================kernel 2.6.25==================
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
static u8 atp862x_check_status(struct ata_port *ap)
{	
	void __iomem *port_mmio = atp862x_port_base(ap);			
	return ioread32(port_mmio + PORT_TFDATA) & 0xFF;
}
static void atp862x_irq_clear(struct ata_port *ap)
{	
	/* TODO */
}
#endif

//=================================================================
static void atp862x_enable_atp862x(void __iomem *mmio)
{
	int i;
	u32 tmp;		
	/* turn on atp862x_EN */
				
	tmp = ioread32(mmio + HOST_CTL);
//	printk("HOST_ATP862X_EN %x\n",tmp);
	if (tmp & HOST_ATP862X_EN)
		return;	
	/* Some controllers need atp862x_EN to be written multiple times.
	 * Try a few times before giving up.
	 */
	for (i = 0; i < 5; i++) {
		tmp |= HOST_ATP862X_EN;
		iowrite32(tmp, mmio + HOST_CTL);
		tmp = ioread32(mmio + HOST_CTL);	/* flush && sanity check */
		if (tmp & HOST_ATP862X_EN)
			return;
	//	msleep(10);
		msleep(10);
	}
	
	WARN_ON(1);
}

/**
 *	atp862x_save_initial_config - Save and fixup initial config values
 *	@pdev: target PCI device
 *	@hpriv: host private area to store config values
 *
 *	Some registers containing configuration info might be setup by
 *	BIOS and might be cleared on reset.  This function saves the
 *	initial values of those registers into @hpriv such that they
 *	can be restored after controller reset.
 *
 *	If inconsistent, config values are fixed up by this function.
 *
 *	LOCKING:
 *	None.
 */
static void atp862x_save_initial_config(struct pci_dev *pdev,
				     struct atp862x_host_priv *hpriv)
{
	void __iomem *mmio = pcim_iomap_table(pdev)[ATP862X_PCI_BAR];
	u32 cap, port_map;
	int i;		
//	int mv;	
	/* make sure atp862x mode is enabled before accessing CAP */
	atp862x_enable_atp862x(mmio);
	/* Values prefixed with saved_ are written back to host after
	 * reset.  Values without are used for driver operation.
	 */		
	hpriv->saved_cap = cap = ioread32(mmio + HOST_CAP) & 0xDFFFFFFF;	
	hpriv->saved_port_map = port_map = ioread32(mmio + HOST_PORTS_IMPL);	
	/* some chips have errata preventing 64bit use */
	if ((cap & HOST_CAP_64) && (hpriv->flags & ATP862X_HFLAG_32BIT_ONLY)) {
		dev_printk(KERN_INFO, &pdev->dev,
			   "controller can't do 64bit DMA, forcing 32bit %x flag %x\n",cap,hpriv->flags);
		cap &= ~HOST_CAP_64;
	}

	if ((cap & HOST_CAP_NCQ) && (hpriv->flags & ATP862X_HFLAG_NO_NCQ)) {
		dev_printk(KERN_INFO, &pdev->dev,
			   "controller can't do NCQ, turning off CAP_NCQ\n");
		cap &= ~HOST_CAP_NCQ;
	}

	if (!(cap & HOST_CAP_NCQ) && (hpriv->flags & ATP862X_HFLAG_YES_NCQ)) {
		dev_printk(KERN_INFO, &pdev->dev,
			   "controller can do NCQ, turning on CAP_NCQ\n");
		cap |= HOST_CAP_NCQ;
	}

	if ((cap & HOST_CAP_PMP) && (hpriv->flags & ATP862X_HFLAG_NO_PMP)) {
		dev_printk(KERN_INFO, &pdev->dev,
			   "controller can't do PMP, turning off CAP_PMP\n");
		cap &= ~HOST_CAP_PMP;
	}

	if (pdev->vendor == PCI_VENDOR_ID_JMICRON && pdev->device == 0x2361 &&
	    port_map != 1) {
		dev_printk(KERN_INFO, &pdev->dev,
			   "JMB361 has only one port, port_map 0x%x -> 0x%x\n",
			   port_map, 1);
		port_map = 1;
	}

	/* cross check port_map and cap.n_ports */
	if (port_map) {
		int map_ports = 0;

		for (i = 0; i < ATP862X_MAX_PORTS; i++)
			if (port_map & (1 << i))
				map_ports++;

		/* If PI has more ports than n_ports, whine, clear
		 * port_map and let it be generated from n_ports.
		 */
		if (map_ports > atp862x_nr_ports(cap)) {
			dev_printk(KERN_WARNING, &pdev->dev,
				   "implemented port map (0x%x) contains more "
				   "ports than nr_ports (%u), using nr_ports\n",
				   port_map, atp862x_nr_ports(cap));
			port_map = 0;
		}		
	}
	
	/* fabricate port_map from cap.nr_ports */
	if (!port_map) {
		port_map = (1 << atp862x_nr_ports(cap)) - 1;
		dev_printk(KERN_WARNING, &pdev->dev,
			   "forcing PORTS_IMPL to 0x%x\n", port_map);

		/* write the fixed up value to the PI register */
		hpriv->saved_port_map = port_map;
	}

	/* record values to use during operation */
	hpriv->cap = cap;
	hpriv->port_map = port_map;
	
}

/**
 *	atp862x_restore_initial_config - Restore initial config
 *	@host: target ATA host
 *
 *	Restore initial config stored by atp862x_save_initial_config().
 *
 *	LOCKING:
 *	None.
 */
static void atp862x_restore_initial_config(struct ata_host *host)
{
	struct atp862x_host_priv *hpriv = host->private_data;
	void __iomem *mmio = host->iomap[ATP862X_PCI_BAR];	
				
	iowrite32(hpriv->saved_cap, mmio + HOST_CAP);
	iowrite32(hpriv->saved_port_map, mmio + HOST_PORTS_IMPL);
	(void) ioread32(mmio + HOST_PORTS_IMPL);	/* flush */
}

static unsigned atp862x_scr_offset(struct ata_port *ap, unsigned int sc_reg)
{	
	static const int offset[] = {
		[SCR_STATUS]		= PORT_SCR_STAT,
		[SCR_CONTROL]		= PORT_SCR_CTL,
		[SCR_ERROR]		= PORT_SCR_ERR,
		[SCR_ACTIVE]		= PORT_SCR_ACT,
		[SCR_NOTIFICATION]	= PORT_SCR_NTF,
	};
	struct atp862x_host_priv *hpriv = ap->host->private_data;	
	if (sc_reg < ARRAY_SIZE(offset) &&
	    (sc_reg != SCR_NOTIFICATION || (hpriv->cap & HOST_CAP_SNTF)))
		return offset[sc_reg];	
	return 0;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
static u32 atp862x_scr_read2(struct ata_port *ap, unsigned int sc_reg)
{
	void __iomem *port_mmio = atp862x_port_base(ap);	
	int offset = atp862x_scr_offset(ap, sc_reg);
	if (offset) {		
		return ioread32(port_mmio + offset);
	}	
	return 0xffffffffU;
}
static void atp862x_scr_write2(struct ata_port *ap, unsigned int sc_reg,u32 val)
{
	void __iomem *port_mmio = atp862x_port_base(ap);	
	int offset = atp862x_scr_offset(ap, sc_reg);

	if (offset) {
		iowrite32(val, port_mmio + offset);		
	}

	return;
}
#endif

#ifdef kernel_ver_2628
static int atp862x_scr_read(struct ata_link *link, unsigned int sc_reg, u32 *val)
{
	void __iomem *port_mmio = atp862x_port_base(link->ap);			
	int offset = atp862x_scr_offset(link->ap, sc_reg);
#else
static int atp862x_scr_read(struct ata_port *ap, unsigned int sc_reg, u32 *val)
{
	void __iomem *port_mmio = atp862x_port_base(ap);				
	int offset = atp862x_scr_offset(ap, sc_reg);	
#endif
	
	if (offset) {
		*val = ioread32(port_mmio + offset);
		return 0;
	}	
	return -EINVAL;
}
#ifdef kernel_ver_2628
static int atp862x_scr_write(struct ata_link *link, unsigned int sc_reg, u32 val)
{
	void __iomem *port_mmio = atp862x_port_base(link->ap);		
	int offset = atp862x_scr_offset(link->ap, sc_reg);
#else
static int atp862x_scr_write(struct ata_port *ap, unsigned int sc_reg, u32 val)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	int offset = atp862x_scr_offset(ap, sc_reg);
#endif		
	if (offset) {
		iowrite32(val, port_mmio + offset);
		return 0;
	}

	return -EINVAL;
}
/*
static int atp862x_release_port(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 tmp;
	int rc = 0;
	u8 status	= ioread8(port_mmio + PORT_CMD);	
	iowrite8(status & 0xFE, port_mmio + PORT_CMD);
	iowrite8(status | 0x1, port_mmio + PORT_CMD);
	iowrite8(status | PORT_CMD_CLO, port_mmio + PORT_CMD);
	tmp = ata_wait_register(port_mmio + PORT_CMD,
				PORT_CMD_CLO, PORT_CMD_CLO, 1, 500);
	if (tmp & PORT_CMD_CLO)
		rc = -EIO;
	iowrite8(status | 0x2, port_mmio + PORT_CMD);
	return rc;
}*/
static void atp862x_start_engine(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 tmp;		
	/* start DMA */	
	tmp = ioread32(port_mmio + PORT_CMD);
	tmp |= PORT_CMD_START;
	iowrite32(tmp, port_mmio + PORT_CMD);
	ioread32(port_mmio + PORT_CMD); /* flush */
	iowrite8(tmp | PORT_CMD_CLO, port_mmio + PORT_CMD);
	tmp = ata_wait_register(port_mmio + PORT_CMD,
				PORT_CMD_CLO, PORT_CMD_CLO, 1, 500);		
}

static int atp862x_stop_engine(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 tmp;	
	tmp = ioread32(port_mmio + PORT_CMD);	
	/* check if the HBA is idle */
	if ((tmp & (PORT_CMD_START | PORT_CMD_LIST_ON)) == 0)
		return 0;

	/* setting HBA to idle */
	tmp &= ~PORT_CMD_START;
	iowrite32(tmp, port_mmio + PORT_CMD);

	/* wait for engine to stop. This could be as long as 500 msec */
	tmp = ata_wait_register(port_mmio + PORT_CMD,
				PORT_CMD_LIST_ON, PORT_CMD_LIST_ON, 1, 500);
	if (tmp & PORT_CMD_LIST_ON) {	
		
		return -EIO;
	}
	
	return 0;
}

static void atp862x_start_fis_rx(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	struct atp862x_host_priv *hpriv = ap->host->private_data;
	struct atp862x_port_priv *pp = ap->private_data;
	u32 tmp;	
	
	/* set FIS registers */
	if (hpriv->cap & HOST_CAP_64)
		iowrite32((pp->cmd_slot_dma >> 16) >> 16,
		       port_mmio + PORT_LST_ADDR_HI);
	iowrite32(pp->cmd_slot_dma & 0xffffffff, port_mmio + PORT_LST_ADDR);

	if (hpriv->cap & HOST_CAP_64)
		iowrite32((pp->rx_fis_dma >> 16) >> 16,
		       port_mmio + PORT_FIS_ADDR_HI);
	iowrite32(pp->rx_fis_dma & 0xffffffff, port_mmio + PORT_FIS_ADDR);

	/* enable FIS reception */
	tmp = ioread32(port_mmio + PORT_CMD);
	tmp |= PORT_CMD_FIS_RX;
	iowrite32(tmp, port_mmio + PORT_CMD);	
	/* flush */
	ioread32(port_mmio + PORT_CMD);
}

static int atp862x_stop_fis_rx(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 tmp;
	
	/* disable FIS reception */
	tmp = ioread32(port_mmio + PORT_CMD);
	tmp &= ~PORT_CMD_FIS_RX;
	iowrite32(tmp, port_mmio + PORT_CMD);

	/* wait for completion, spec says 500ms, give it 1000 */
	tmp = ata_wait_register(port_mmio + PORT_CMD, PORT_CMD_FIS_ON,
				PORT_CMD_FIS_ON, 10, 1000);
	
	if (tmp & PORT_CMD_FIS_ON)
		return -EBUSY;

	return 0;
}

static void atp862x_power_up(struct ata_port *ap)
{
	struct atp862x_host_priv *hpriv = ap->host->private_data;
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 cmd;
	
	cmd = ioread32(port_mmio + PORT_CMD) & ~PORT_CMD_ICC_MASK;

	/* spin up device */
	if (hpriv->cap & HOST_CAP_SSS) {
		cmd |= PORT_CMD_SPIN_UP;
		iowrite32(cmd, port_mmio + PORT_CMD);
	}

	/* wake up link */
	iowrite32(cmd | PORT_CMD_ICC_ACTIVE, port_mmio + PORT_CMD);
	
}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
/*
static void atp862x_disable_alpm(struct ata_port *ap)
{
	struct atp862x_host_priv *hpriv = ap->host->private_data;
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 cmd;
	struct atp862x_port_priv *pp = ap->private_data;
	
	
	cmd = ioread32(port_mmio + PORT_CMD);

	
//	cmd &= ~PORT_CMD_ASP; not support set (g.c.c.20090203)
//	cmd &= ~PORT_CMD_ALPE; not support set (g.c.c.20090203)

	
	cmd |= PORT_CMD_ICC_ACTIVE;

	
	iowrite32(cmd, port_mmio + PORT_CMD);
	cmd = ioread32(port_mmio + PORT_CMD);

	
	msleep(10);

	
	iowrite32(PORT_IRQ_PHYRDY, port_mmio + PORT_IRQ_STAT);

	
#ifdef kernel_ver_2628
	atp862x_scr_write(&ap->link, SCR_ERROR, ((1 << 16) | (1 << 18)));
#else
	atp862x_scr_write(ap, SCR_ERROR, ((1 << 16) | (1 << 18)));
#endif

	
	hpriv->flags &= ~ATP862X_HFLAG_NO_HOTPLUG;


	pp->intr_mask |= PORT_IRQ_PHYRDY;
	iowrite32(pp->intr_mask, port_mmio + PORT_IRQ_MASK);
	
	
}

static int atp862x_enable_alpm(struct ata_port *ap,
	enum link_pm policy)
{
	struct atp862x_host_priv *hpriv = ap->host->private_data;
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 cmd;
	struct atp862x_port_priv *pp = ap->private_data;
//	u32 asp;
	
	
	if (!(hpriv->cap & HOST_CAP_ALPM))
		return -EINVAL;

	switch (policy) {
	case MAX_PERFORMANCE:
	case NOT_AVAILABLE:
		// if we came here with NOT_AVAILABLE,
 		// it just means this is the first time we
 		 // have tried to enable - default to max performance,
 		 // and let the user go to lower power modes on request.
 		
		atp862x_disable_alpm(ap);
		return 0;
	case MIN_POWER:
		// configure HBA to enter SLUMBER 
		asp = PORT_CMD_ASP;
		break;
	case MEDIUM_POWER:
		// configure HBA to enter PARTIAL 
		asp = 0;
		break;
	default:
		return -EINVAL;
	}
	
	
	pp->intr_mask &= ~PORT_IRQ_PHYRDY;
	iowrite32(pp->intr_mask, port_mmio + PORT_IRQ_MASK);

	hpriv->flags |= ATP862X_HFLAG_NO_HOTPLUG;

	
	cmd = ioread32(port_mmio + PORT_CMD);

	
//	cmd |= asp;

	
	cmd |= PORT_CMD_ALPE;


	iowrite32(cmd, port_mmio + PORT_CMD);
	cmd = ioread32(port_mmio + PORT_CMD);
	
	
	return 0;
}
*/
#endif
#ifdef CONFIG_PM
static void atp862x_power_down(struct ata_port *ap)
{
	struct atp862x_host_priv *hpriv = ap->host->private_data;
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 cmd, scontrol;
	
	if (!(hpriv->cap & HOST_CAP_SSS))
		return;

	/* put device into listen mode, first set PxSCTL.DET to 0 */
	scontrol = ioread32(port_mmio + PORT_SCR_CTL);
	scontrol &= ~0xf;
	iowrite32(scontrol, port_mmio + PORT_SCR_CTL);

	/* then set PxCMD.SUD to 0 */
	cmd = ioread32(port_mmio + PORT_CMD) & ~PORT_CMD_ICC_MASK;
	cmd &= ~PORT_CMD_SPIN_UP;
	iowrite32(cmd, port_mmio + PORT_CMD);
	
}
#endif

static void atp862x_start_port(struct ata_port *ap)
{
#ifdef kernel_ver_2628
	struct atp862x_port_priv *pp = ap->private_data;
	struct ata_link *link;
	struct atp862x_em_priv *emp;
#endif
	
	/* enable FIS reception */
	atp862x_start_fis_rx(ap);

	/* enable DMA */
	atp862x_start_engine(ap);

	/* turn on LEDs */
/* G.C.C 2009 05-06
#ifdef kernel_ver_2628
	if (ap->flags & ATA_FLAG_EM) {
#ifdef kernel_ver_2629
		ata_for_each_link(link, ap, EDGE) {
#else
		ata_port_for_each_link(link, ap) {
#endif
			emp = &pp->em_priv[link->pmp];
			atp862x_transmit_led_message(ap, emp->led_state, 4);
		}
	}
*/
/*
	if (ap->flags & ATA_FLAG_SW_ACTIVITY)
#ifdef kernel_ver_2629
		ata_for_each_link(link, ap, EDGE) 
#else
		ata_port_for_each_link(link, ap)
#endif
			atp862x_init_sw_activity(link);
			
#endif
*/
}

static int atp862x_deinit_port(struct ata_port *ap, const char **emsg)
{
	int rc;
	
	/* disable DMA */
	rc = atp862x_stop_engine(ap);
	if (rc) {
		*emsg = "failed to stop engine";
		return rc;
	}

	/* disable FIS reception */
	rc = atp862x_stop_fis_rx(ap);
	if (rc) {
		*emsg = "failed stop FIS RX";
		return rc;
	}
	
	return 0;
}

static int atp862x_reset_controller(struct ata_host *host)
{
//	struct pci_dev *pdev = to_pci_dev(host->dev);
//	struct atp862x_host_priv *hpriv = host->private_data;
	void __iomem *mmio = host->iomap[ATP862X_PCI_BAR];
	u32 tmp;
		
	/* we must be in atp862x mode, before using anything
	 * atp862x-specific, such as HOST_RESET.
	 */
//	atp862x_enable_atp862x(mmio);	
	/* global controller reset */
	if (!atp862x_skip_host_reset) {
		
		tmp = ioread32(mmio + HOST_CTL);
		if ((tmp & HOST_RESET) == 0) {
			iowrite32(tmp | HOST_RESET, mmio + HOST_CTL);
			ioread32(mmio + HOST_CTL); /* flush */
		}

		/*
		 * to perform host reset, OS should set HOST_RESET
		 * and poll until this bit is read to be "0".
		 * reset must complete within 1 second, or
		 * the hardware should be considered fried.
		 */
		tmp = ata_wait_register(mmio + HOST_CTL, HOST_RESET,
					HOST_RESET, 10, 1000);

		if (tmp & HOST_RESET) {
			dev_printk(KERN_ERR, host->dev,
				   "controller reset failed (0x%x)\n", tmp);
			return -EIO;
		}
		mdelay(1400);
		/* turn on atp862x mode */		
		atp862x_enable_atp862x(mmio);

		/* Some registers might be cleared on reset.  Restore
		 * initial values.
		 */
		atp862x_restore_initial_config(host);
	} else
		dev_printk(KERN_INFO, host->dev,
			   "skipping global host reset\n");
/*
	if (pdev->vendor == PCI_VENDOR_ID_INTEL) {
		u16 tmp16;

		// configure PCS 
		pci_read_config_word(pdev, 0x92, &tmp16);
		if ((tmp16 & hpriv->port_map) != hpriv->port_map) {
			tmp16 |= hpriv->port_map;
			pci_write_config_word(pdev, 0x92, tmp16);
		}
	}*/
	atp862x_enable_atp862x(mmio);
	return 0;
}
/*G.C.C. 2009 0506
#ifdef kernel_ver_2628
static void atp862x_sw_activity(struct ata_link *link)
{
	struct ata_port *ap = link->ap;
	struct atp862x_port_priv *pp = ap->private_data;
	struct atp862x_em_priv *emp = &pp->em_priv[link->pmp];
	
	if (!(link->flags & ATA_LFLAG_SW_ACTIVITY))
		return;

	emp->activity++;
	if (!timer_pending(&emp->timer))
		mod_timer(&emp->timer, jiffies + msecs_to_jiffies(10));
	
}

static void atp862x_sw_activity_blink(unsigned long arg)
{
	struct ata_link *link = (struct ata_link *)arg;
	struct ata_port *ap = link->ap;
	struct atp862x_port_priv *pp = ap->private_data;
	struct atp862x_em_priv *emp = &pp->em_priv[link->pmp];
	unsigned long led_message = emp->led_state;
	u32 activity_led_state;
	unsigned long flags;
	
	led_message &= EM_MSG_LED_VALUE;
	led_message |= ap->port_no | (link->pmp << 8);

	// check to see if we've had activity.  If so,
	// toggle state of LED and reset timer.  If not,
	// turn LED to desired idle state.
	//
	spin_lock_irqsave(ap->lock, flags);
	if (emp->saved_activity != emp->activity) {
		emp->saved_activity = emp->activity;
		// get the current LED state 
		activity_led_state = led_message & EM_MSG_LED_VALUE_ON;

		if (activity_led_state)
			activity_led_state = 0;
		else
			activity_led_state = 1;

		// clear old state 
		led_message &= ~EM_MSG_LED_VALUE_ACTIVITY;

		// toggle state 
		led_message |= (activity_led_state << 16);
		mod_timer(&emp->timer, jiffies + msecs_to_jiffies(100));
	} else {
		// switch to idle 
		led_message &= ~EM_MSG_LED_VALUE_ACTIVITY;
		if (emp->blink_policy == BLINK_OFF)
			led_message |= (1 << 16);
	}
	spin_unlock_irqrestore(ap->lock, flags);
	atp862x_transmit_led_message(ap, led_message, 4);
	
}

static void atp862x_init_sw_activity(struct ata_link *link)
{
	struct ata_port *ap = link->ap;
	struct atp862x_port_priv *pp = ap->private_data;
	struct atp862x_em_priv *emp = &pp->em_priv[link->pmp];
	
	// init activity stats, setup timer 
	emp->saved_activity = emp->activity = 0;
	setup_timer(&emp->timer, atp862x_sw_activity_blink, (unsigned long)link);

	// check our blink policy and set flag for link if it's enabled 
	if (emp->blink_policy)
		link->flags |= ATA_LFLAG_SW_ACTIVITY;
	
}

static int atp862x_reset_em(struct ata_host *host)
{
	void __iomem *mmio = host->iomap[ATP862X_PCI_BAR0];
	u32 em_ctl;
	
	em_ctl = ioread32(mmio + HOST_EM_CTL);
	if ((em_ctl & EM_CTL_TM) || (em_ctl & EM_CTL_RST))
		return -EINVAL;

	iowrite32(em_ctl | EM_CTL_RST, mmio + HOST_EM_CTL);
	
	return 0;
}

static ssize_t atp862x_transmit_led_message(struct ata_port *ap, u32 state,
					ssize_t size)
{
	struct atp862x_host_priv *hpriv = ap->host->private_data;
	struct atp862x_port_priv *pp = ap->private_data;
	void __iomem *mmio = ap->host->iomap[ATP862X_PCI_BAR0];
	u32 em_ctl;
	u32 message[] = {0, 0};
	unsigned long flags;
	int pmp;
	struct atp862x_em_priv *emp;
	
	// get the slot number from the message 
	pmp = (state & EM_MSG_LED_PMP_SLOT) >> 8;
	if (pmp < MAX_SLOTS)
		emp = &pp->em_priv[pmp];
	else
		return -EINVAL;

	spin_lock_irqsave(ap->lock, flags);

	//
	//  if we are still busy transmitting a previous message,
	//  do not allow
	 //
	em_ctl = ioread32(mmio + HOST_EM_CTL);
	if (em_ctl & EM_CTL_TM) {
		spin_unlock_irqrestore(ap->lock, flags);
		return -EINVAL;
	}

	//
	//  create message header - this is all zero except for
	//  the message size, which is 4 bytes.
	//
	message[0] |= (4 << 8);

	// ignore 0:4 of byte zero, fill in port info yourself 
	message[1] = ((state & ~EM_MSG_LED_HBA_PORT) | ap->port_no);

	// write message to EM_LOC 
	iowrite32(message[0], mmio + hpriv->em_loc);
	iowrite32(message[1], mmio + hpriv->em_loc+4);

	// save off new led state for port/slot 
	emp->led_state = message[1];

	//  tell hardware to transmit the message
	 //
	iowrite32(em_ctl | EM_CTL_TM, mmio + HOST_EM_CTL);

	spin_unlock_irqrestore(ap->lock, flags);
	
	return size;
}

static ssize_t atp862x_led_show(struct ata_port *ap, char *buf)
{
	struct atp862x_port_priv *pp = ap->private_data;
	struct ata_link *link;
	struct atp862x_em_priv *emp;
	int rc = 0;
	
#ifdef kernel_ver_2629
	ata_for_each_link(link, ap, EDGE) {
#else
	ata_port_for_each_link(link, ap) {
#endif
		emp = &pp->em_priv[link->pmp];
		rc += sprintf(buf, "%lx\n", emp->led_state);
	}

	return rc;
}

static ssize_t atp862x_led_store(struct ata_port *ap, const char *buf,
				size_t size)
{
	int state;
	int pmp;
	struct atp862x_port_priv *pp = ap->private_data;
	struct atp862x_em_priv *emp;
	
	state = simple_strtoul(buf, NULL, 0);

	// get the slot number from the message 
	pmp = (state & EM_MSG_LED_PMP_SLOT) >> 8;
	if (pmp < MAX_SLOTS)
		emp = &pp->em_priv[pmp];
	else
		return -EINVAL;

	// mask off the activity bits if we are in sw_activity
	 // mode, user should turn off sw_activity before setting
	 //activity led through em_message
	 //
	if (emp->blink_policy)
		state &= ~EM_MSG_LED_VALUE_ACTIVITY;
	
	return atp862x_transmit_led_message(ap, state, size);
}

static ssize_t atp862x_activity_store(struct ata_device *dev, enum sw_activity val)
{
	struct ata_link *link = dev->link;
	struct ata_port *ap = link->ap;
	struct atp862x_port_priv *pp = ap->private_data;
	struct atp862x_em_priv *emp = &pp->em_priv[link->pmp];
	u32 port_led_state = emp->led_state;
	
	// save the desired Activity LED behavior 
	if (val == OFF) {
		// clear LFLAG 
		link->flags &= ~(ATA_LFLAG_SW_ACTIVITY);

		// set the LED to OFF 
		port_led_state &= EM_MSG_LED_VALUE_OFF;
		port_led_state |= (ap->port_no | (link->pmp << 8));
		atp862x_transmit_led_message(ap, port_led_state, 4);
	} else {
		link->flags |= ATA_LFLAG_SW_ACTIVITY;
		if (val == BLINK_OFF) {
			// set LED to ON for idle 
			port_led_state &= EM_MSG_LED_VALUE_OFF;
			port_led_state |= (ap->port_no | (link->pmp << 8));
			port_led_state |= EM_MSG_LED_VALUE_ON; 
			atp862x_transmit_led_message(ap, port_led_state, 4);
		}
	}
	emp->blink_policy = val;
	
	return 0;
}

static ssize_t atp862x_activity_show(struct ata_device *dev, char *buf)
{
	struct ata_link *link = dev->link;
	struct ata_port *ap = link->ap;
	struct atp862x_port_priv *pp = ap->private_data;
	struct atp862x_em_priv *emp = &pp->em_priv[link->pmp];
	
	// display the saved value of activity behavior for this
	// disk.
	
	
	return sprintf(buf, "%d\n", emp->blink_policy);
}
#endif
*/
static void atp862x_port_init(struct pci_dev *pdev, struct ata_port *ap,
			   int port_no, void __iomem *mmio,
			   void __iomem *port_mmio)
{
	const char *emsg = NULL;
	int rc;
	u32 tmp;
		
	/* make sure port is not active */
	rc = atp862x_deinit_port(ap, &emsg);
	if (rc)
		dev_printk(KERN_WARNING, &pdev->dev,
			   "%s (%d)\n", emsg, rc);

	/* clear SError */
	tmp = ioread32(port_mmio + PORT_SCR_ERR);	
	VPRINTK("PORT_SCR_ERR 0x%x\n", tmp);
	iowrite32(tmp, port_mmio + PORT_SCR_ERR);
	tmp = ioread32(port_mmio + 0x28);	
	/* clear port IRQ */
	tmp = ioread32(port_mmio + PORT_IRQ_STAT);
	VPRINTK("PORT_IRQ_STAT 0x%x\n", tmp);	
	if (tmp)
		iowrite32(tmp, port_mmio + PORT_IRQ_STAT);

	iowrite32(1 << port_no, mmio + HOST_IRQ_STAT);	
}

static void atp862x_init_controller(struct ata_host *host)
{
//	struct atp862x_host_priv *hpriv = host->private_data;	
	struct pci_dev *pdev = to_pci_dev(host->dev);
	void __iomem *mmio = host->iomap[ATP862X_PCI_BAR];
	int i;
	void __iomem *port_mmio;
	u32 tmp;

	for (i = 0; i < host->n_ports; i++) {		
		struct ata_port *ap = host->ports[i];
		if ((pdev->device == 0x000d) && (i == 2)) {
		//	printk("atp8620 port 2 and don't use ahci to access\n");
			continue;		
		}
		port_mmio = atp862x_port_base(ap);

		if (ata_port_is_dummy(ap))
			continue;
		
		atp862x_port_init(pdev, ap, i, mmio, port_mmio);		
	}

	tmp = ioread32(mmio + HOST_CTL);
	VPRINTK("HOST_CTL 0x%x\n", tmp);
	iowrite32(tmp | HOST_IRQ_EN, mmio + HOST_CTL);
	tmp = ioread32(mmio + HOST_CTL);
	VPRINTK("HOST_CTL 0x%x\n", tmp);
	
}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
static void atp862x_dev_config(struct ata_device *dev)
{
	struct atp862x_host_priv *hpriv = dev->link->ap->host->private_data;		
	if (hpriv->flags & ATP862X_HFLAG_SECT255) {
		dev->max_sectors = 255;
		ata_dev_printk(dev, KERN_INFO,
			       "atp862x: limiting to 255 sectors per cmd\n");
	}
	
}
#endif
static unsigned int atp862x_dev_classify(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	struct ata_taskfile tf;
	u32 tmp;
	
	tmp = ioread32(port_mmio + PORT_SIG);
	tf.lbah		= (tmp >> 24)	& 0xff;
	tf.lbam		= (tmp >> 16)	& 0xff;
	tf.lbal		= (tmp >> 8)	& 0xff;
	tf.nsect	= (tmp)		& 0xff;
	
	return ata_dev_classify(&tf);
}

static void atp862x_fill_cmd_slot(struct atp862x_port_priv *pp, unsigned int tag,
			       u32 opts)
{
	dma_addr_t cmd_tbl_dma;
	
	cmd_tbl_dma = pp->cmd_tbl_dma + tag * ATP862X_CMD_TBL_SZ;
	
	pp->cmd_slot[tag].opts = cpu_to_le32(opts);
	pp->cmd_slot[tag].status = 0;
	pp->cmd_slot[tag].tbl_addr = cpu_to_le32(cmd_tbl_dma & 0xffffffff);
	pp->cmd_slot[tag].tbl_addr_hi = cpu_to_le32((cmd_tbl_dma >> 16) >> 16);
	
}

static int atp862x_kick_engine(struct ata_port *ap, int force_restart)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	struct atp862x_host_priv *hpriv = ap->host->private_data;
	u8 status = ioread32(port_mmio + PORT_TFDATA) & 0xFF;
	u32 tmp;
	int busy, rc;
	
	/* do we need to kick the port? */
	busy = status & (ATA_BUSY | ATA_DRQ);
	if (!busy && !force_restart)
		return 0;

	/* stop engine */
	rc = atp862x_stop_engine(ap);
	if (rc)
		goto out_restart;

	/* need to do CLO? */
	if (!busy) {
		rc = 0;
		goto out_restart;
	}

	if (!(hpriv->cap & HOST_CAP_CLO)) {
		rc = -EOPNOTSUPP;		
		goto out_restart;
	}

	/* perform CLO */
	tmp = ioread32(port_mmio + PORT_CMD);
	tmp |= PORT_CMD_CLO;
	iowrite32(tmp, port_mmio + PORT_CMD);
	
	rc = 0;
	tmp = ata_wait_register(port_mmio + PORT_CMD,
				PORT_CMD_CLO, PORT_CMD_CLO, 1, 500);
	if (tmp & PORT_CMD_CLO)
		rc = -EIO;

	/* restart engine */
 out_restart:
	atp862x_start_engine(ap);	
	
	return rc;
}

static int atp862x_exec_polled_cmd(struct ata_port *ap, int pmp,
				struct ata_taskfile *tf, int is_cmd, u16 flags,
				unsigned long timeout_msec)
{
	const u32 cmd_fis_len = 5; /* five dwords */
	struct atp862x_port_priv *pp = ap->private_data;
	void __iomem *port_mmio = atp862x_port_base(ap);
	u8 *fis = pp->cmd_tbl;
	u32 tmp;
	
	/* prep the command */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
	ata_tf_to_fis(tf, fis, 0);
#else
	ata_tf_to_fis(tf, pmp, is_cmd, fis);
#endif
	atp862x_fill_cmd_slot(pp, 0, cmd_fis_len | flags | (pmp << 12));

	/* issue & wait */
	iowrite32(1, port_mmio + PORT_CMD_ISSUE);

	if (timeout_msec) {
		tmp = ata_wait_register(port_mmio + PORT_CMD_ISSUE, 0x1, 0x1,
					1, timeout_msec);
		if (tmp & 0x1) {
			atp862x_kick_engine(ap, 1);
			return -EBUSY;
		}
	} else {
		ioread32(port_mmio + PORT_CMD_ISSUE);	/* flush */		
	}
	
	return 0;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
static int atp862x_do_softreset(struct ata_port *ap, unsigned int *class,
			     int pmp, unsigned long deadline)
{
#else

# if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
static int atp862x_do_softreset(struct ata_link *link, unsigned int *class,
			     int pmp, unsigned long deadline)	
{
	struct ata_port *ap = link->ap;
# else
static int atp862x_do_softreset(struct ata_link *link, unsigned int *class,
			     int pmp, unsigned long deadline,
			     int (*check_ready)(struct ata_link *link))
{

	struct ata_port *ap = link->ap;
# endif

#endif		
	const char *reason = NULL;
	unsigned long msecs;
	struct ata_taskfile tf;
	int rc;
	u32 tmp;
	u8 status74;
	void __iomem *port_mmio = atp862x_port_base(ap);
		
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
#  if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)	
	if (ata_link_offline(link)) {	
		*class = ATA_DEV_NONE;
		return 0;
	}
#  else
	if (ata_port_offline(ap)) {
		DPRINTK("PHY reports no device\n");		
		*class = ATA_DEV_NONE;
		return 0;
	}
#  endif
#endif
	status74 = ioread8(port_mmio + 0x74);
	iowrite8(0x4, port_mmio + 0x74);
	DPRINTK("ENTER\n");
	
	/* prepare for SRST (atp862x-1.1 10.4.1) */
	rc = atp862x_kick_engine(ap, 1);
//	rc = atp862x_release_port(ap);

	if (rc && rc != -EOPNOTSUPP)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
		ata_port_printk(ap, KERN_WARNING,
				"failed to reset engine (errno=%d)", rc);
	ata_tf_init(ap->device, &tf);
#else
		ata_link_printk(link, KERN_WARNING,
				"failed to reset engine (errno=%d)\n", rc);
	ata_tf_init(link->device, &tf);
#endif
	/* issue the first D2H Register FIS */
	msecs = 0;

	if (time_after(deadline, jiffies))
		msecs = jiffies_to_msecs(deadline - jiffies);
	tf.ctl = ATA_SRST;
//	printk("msecs %x\n",msecs);
//	
	
//	msecs = 700;
	if (atp862x_exec_polled_cmd(ap, pmp, &tf, 0,
				 ATP862X_CMD_RESET | ATP862X_CMD_CLR_BUSY, msecs)) {
		rc = -EIO;
		reason = "1st FIS failed";
		goto fail;
	}

	/* spec says at least 5us, but be generous and sleep for 1ms */
	msleep(1);

	/* issue the second D2H Register FIS */
	tf.ctl &= ~ATA_SRST;
	atp862x_exec_polled_cmd(ap, pmp, &tf, 0, 0, msecs);

	/* wait for link to become ready */

		
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	msleep(ATP862X_WAIT_AF_RESET);
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)	
	if (ata_port_online(ap)) {
		if (ata_busy_sleep(ap, ATA_TMOUT_BOOT_QUICK, ATA_TMOUT_BOOT)) {
			rc = -EIO;
			reason = "device not ready";
			goto fail;
		}		
	}
#  else	
	rc = ata_wait_ready(ap, deadline);
#  endif
#else
	rc = ata_wait_after_reset(link, deadline, check_ready);
#endif
	/* link occupied, -ENODEV too is an error */
	if (rc) {
		reason = "device not ready";		
		goto fail;
	}
	*class = atp862x_dev_classify(ap);

	tmp = ioread32(port_mmio + PORT_SIG);
	
	DPRINTK("EXIT, class=%u\n", *class);
	iowrite8(status74, port_mmio + 0x74);
	
	return 0;

 fail:
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	ata_port_printk(ap, KERN_ERR, "softreset failed (%s)\n", reason);
#else
	ata_link_printk(link, KERN_ERR, "softreset failed (%s)\n", reason);
#endif
	return rc;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,24)
static int atp862x_check_ready(struct ata_link *link)
{
	void __iomem *port_mmio = atp862x_port_base(link->ap);
	u8 status = ioread32(port_mmio + PORT_TFDATA) & 0xFF;

	
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	return 0;
#else
	return ata_check_ready(status);
#endif
}
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
static int atp862x_softreset(struct ata_port *ap, unsigned int *class)
#  else
static int atp862x_softreset(struct ata_port *ap, unsigned int *class,
  			  unsigned long deadline)
#  endif
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
	return atp862x_do_softreset(ap, class, 0, 700);
#else	
	return atp862x_do_softreset(ap, class, 0, deadline);
#endif
}
#else
static int atp862x_softreset(struct ata_link *link, unsigned int *class,
			  unsigned long deadline)
{

#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	int pmp = 0;
	if (link->ap->flags & ATP862X_FLAG_PMP)
		pmp = SATA_PMP_CTRL_PORT;
#  else
	int pmp = sata_srst_pmp(link);
#  endif
		
	DPRINTK("ENTER\n");
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	return atp862x_do_softreset(link, class, pmp, deadline);
# else
	return atp862x_do_softreset(link, class, pmp, deadline,atp862x_check_ready);
# endif
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
static int atp862x_hardreset(struct ata_port *ap, unsigned int *class)
#  else
static int atp862x_hardreset(struct ata_port *ap, unsigned int *class,
			  unsigned long deadline)
#  endif
{
#else
static int atp862x_hardreset(struct ata_link *link, unsigned int *class,
			  unsigned long deadline)
{	
#  if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25)
	const unsigned long *timing = sata_ehc_deb_timing(&link->eh_context);
#  endif
	struct ata_port *ap = link->ap;

#endif
	struct atp862x_port_priv *pp = ap->private_data;
	u8 *d2h_fis = pp->rx_fis + RX_FIS_D2H_REG;
	struct ata_taskfile tf;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
	bool online;
#endif
	int rc;	
	DPRINTK("ENTER\n");		

	atp862x_stop_engine(ap);

	/* clear D2H reception area to properly wait for D2H FIS */
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	ata_tf_init(ap->device, &tf);
#  else
	ata_tf_init(link->device, &tf);
#  endif
	tf.command = 0x80;
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
	ata_tf_to_fis(&tf, d2h_fis, 0);
#  else
	ata_tf_to_fis(&tf, 0, 0, d2h_fis);
#  endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
		
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#    if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
	rc = sata_std_hardreset(ap, class);
#    else
	rc = sata_std_hardreset(ap, class, deadline);
#endif
	atp862x_start_engine(ap);
	if (rc == 0 && ata_port_online(ap)) {
#  else
	rc = sata_std_hardreset(link, class, deadline);
	atp862x_start_engine(ap);
	if (rc == 0 && ata_link_online(link)) {
#  endif
		*class = atp862x_dev_classify(ap);
		
	}
	if (rc != -EAGAIN && *class == ATA_DEV_UNKNOWN) {
		*class = ATA_DEV_NONE;
		
	}	
#else
	rc = sata_link_hardreset(link, timing, deadline, &online,
				 atp862x_check_ready);

	atp862x_start_engine(ap);	
	if (online)
		*class = atp862x_dev_classify(ap);
#endif
	DPRINTK("EXIT, rc=%d, class=%u\n", rc, *class);	
	return rc;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
static void atp862x_postreset(struct ata_port *ap, unsigned int *class)
{
#else
static void atp862x_postreset(struct ata_link *link, unsigned int *class)
{
	struct ata_port *ap = link->ap;
#endif
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 new_tmp, tmp;
	
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	ata_std_postreset(ap, class);
#else
	ata_std_postreset(link, class);
#endif
	/* Make sure port's ATAPI bit is set appropriately */
	new_tmp = tmp = ioread32(port_mmio + PORT_CMD);
	if (*class == ATA_DEV_ATAPI)
		new_tmp |= PORT_CMD_ATAPI;
	else
		new_tmp &= ~PORT_CMD_ATAPI;
	if (new_tmp != tmp) {
		iowrite32(new_tmp, port_mmio + PORT_CMD);
		ioread32(port_mmio + PORT_CMD); /* flush */
	}
	
}
static unsigned int ide_fill_sg(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	struct scatterlist *sg_list;
	P_MailBox 	pmailbox = (P_MailBox)ap->prd;
	unsigned int 	si = 0;
	
	
	VPRINTK("ENTER\n");
	/*
	 * Next, the S/G list.
	 */
# if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
	ata_for_each_sg(sg_list,qc) {
#else
	for_each_sg(qc->sg, sg_list, qc->n_elem, si) {
#endif
		dma_addr_t 	sg_addr	=  sg_dma_address(sg_list);
		u32 		sg_len	=  sg_dma_len(sg_list);	
		while (sg_len > 0x00010000) {
			pmailbox->BufAddressU =  0;
#if __LITTLE_ENDIAN
			pmailbox->BufAddress = sg_addr;
			pmailbox->DataLength = 0x00010000;
#else
			pmailbox->BufAddress = cpu_to_le32(sg_addr);
			pmailbox->DataLength = cpu_to_le32(0x00010000);
#endif
			sg_addr += 0x00010000;
			sg_len -= 0x00010000;
			pmailbox++;
		}
		pmailbox->BufAddressU =  0;
#if __LITTLE_ENDIAN
		pmailbox->BufAddress = sg_addr;
		pmailbox->DataLength = sg_len;
#else
		pmailbox->BufAddress = cpu_to_le32(sg_addr);
		pmailbox->DataLength = cpu_to_le32(sg_len);
#endif
	//	printk("pmailbox[%x] addr %x len %x\n",si,pmailbox->BufAddress,pmailbox->DataLength);
		pmailbox++;
# if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
		si++;
#endif

/*			sg_list = sg_next(sg_list);*/
		//	sg_list = sg_next(sg_list);
	}
	pmailbox--;
#if __LITTLE_ENDIAN
	pmailbox->DataLength |= 0x80000000;
#else
	pmailbox->DataLength |= 0x00000080;
#endif
	return si;
}
static unsigned int atp_fill_sg(struct ata_queued_cmd *qc, void *pCommandTable)
{
	struct scatterlist *sg_list;
	P_MailBox 	pmailbox = pCommandTable + ATP862X_CMD_TBL_HDR_SZ;
	unsigned int 	si = 0;
	
	VPRINTK("ENTER\n");
	/*
	 * Next, the S/G list.
	 */
# if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
	ata_for_each_sg(sg_list, qc) {
#else
	for_each_sg(qc->sg, sg_list, qc->n_elem, si) {
#endif
		dma_addr_t 	sg_addr	=  sg_dma_address(sg_list);
		u32 		sg_len	=  sg_dma_len(sg_list);	
		while (sg_len > 0x00010000) {
			pmailbox->BufAddressU =  0;
#if __LITTLE_ENDIAN
			pmailbox->BufAddress = sg_addr;
			pmailbox->DataLength = 0x00010000;
#else
			pmailbox->BufAddress = cpu_to_le32(sg_addr);
			pmailbox->DataLength = cpu_to_le32(0x00010000);
#endif
			sg_addr += 0x00010000;
			sg_len -= 0x00010000;
			pmailbox++;
		} 
		pmailbox->BufAddressU =  0;
#if __LITTLE_ENDIAN
		pmailbox->BufAddress = sg_addr;
		pmailbox->DataLength = sg_len;
#else
		pmailbox->BufAddress = cpu_to_le32(sg_addr);
		pmailbox->DataLength = cpu_to_le32(sg_len);
#endif
		pmailbox++;
# if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
		si++;
#endif

/*			sg_list = sg_next(sg_list);*/
		//	sg_list = sg_next(sg_list);
	}
	pmailbox--;
#if __LITTLE_ENDIAN
	pmailbox->DataLength |= 0x80000000;
#else
	pmailbox->DataLength |= 0x00000080;
#endif
	return si;
}

static void atp862x_qc_prep(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	struct atp862x_port_priv *pp = ap->private_data;
# if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
	int is_atapi = is_atapi_taskfile(&qc->tf);
#else
	int is_atapi = ata_is_atapi(qc->tf.protocol);
#endif	
	void *cmd_tbl;
	u8 * cmd_tbl_;
	u32 opts;
	const u32 cmd_fis_len = 5; /* five dwords */
	unsigned int n_elem;
//	void __iomem *port_mmio = atp862x_port_base(ap);
//	void __iomem *mmio = ap->host->iomap[ATP862X_PCI_BAR];	

	/* Make sure port's ATAPI bit is set appropriately */	 
	/*
	 * Fill in command table information.  First, the header,
	 * a SATA Register - Host to Device command FIS.
	 */
		
	cmd_tbl = pp->cmd_tbl + qc->tag * ATP862X_CMD_TBL_SZ;
	cmd_tbl_ = (u8 *)cmd_tbl;
# if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
	ata_tf_to_fis(&qc->tf, cmd_tbl, 0);
#  else
	ata_tf_to_fis(&qc->tf, 0, 1, cmd_tbl);
#  endif
#else
	ata_tf_to_fis(&qc->tf, qc->dev->link->pmp, 1, cmd_tbl);
#endif	
	if (is_atapi) {
	//	printk("is_atapi cmd %x\n",qc->cdb[0]);
		memset(cmd_tbl + ATP862X_CMD_TBL_CDB, 0, 32);		
		memcpy(cmd_tbl + ATP862X_CMD_TBL_CDB, qc->cdb, qc->dev->cdb_len);		
	}

	n_elem = 0;
	if (qc->flags & ATA_QCFLAG_DMAMAP) {	
		if (is_atapi) {
			cmd_tbl_[3] = 0x1; //do dma
		}
		n_elem = atp_fill_sg(qc, cmd_tbl);
	}
	/*
	 * Fill in command slot information.
	 */
# if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	opts = cmd_fis_len;
#else
	opts = cmd_fis_len/* | n_elem << 16*/ | (qc->dev->link->pmp << 12);
#endif
	if (qc->tf.flags & ATA_TFLAG_WRITE)
		opts |= ATP862X_CMD_WRITE;
	if (is_atapi) {
	//	printk("fill atapi commandhead \n");
		opts |= ATP862X_CMD_ATAPI /*| ATP862X_CMD_PREFETCH*/;
		cmd_tbl_[5] = 0xFF;
		cmd_tbl_[6] = 0xFF;	
	}
	atp862x_fill_cmd_slot(pp, qc->tag, opts);
	
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
static void atp862x_error_intr(struct ata_port *ap, u32 irq_stat)
{
	struct atp862x_port_priv *pp = ap->private_data;
	struct ata_eh_info *ehi = &ap->eh_info;
	unsigned int err_mask = 0, action = 0;
	struct ata_queued_cmd *qc;
	u32 serror = 0;
		
	ata_ehi_clear_desc(ehi);

	/* AHCI needs SError cleared; otherwise, it might lock up */
	atp862x_scr_read(ap, SCR_ERROR, &serror);
	atp862x_scr_write(ap, SCR_ERROR, serror);
	
	/* analyze @irq_stat */
	ata_ehi_push_desc(ehi, "irq_stat 0x%08x", irq_stat);

	/* some controllers set IRQ_IF_ERR on device errors, ignore it */
	if (ap->flags & ATP862X_HFLAG_IGN_IRQ_IF_ERR)
		irq_stat &= ~PORT_IRQ_IF_ERR;

	if (irq_stat & PORT_IRQ_TF_ERR) {
				
		err_mask |= AC_ERR_DEV;
		if (ap->flags & ATP862X_FLAG_IGN_SERR_INTERNAL)
			serror &= ~SERR_INTERNAL;
	}

	if (irq_stat & (PORT_IRQ_HBUS_ERR | PORT_IRQ_HBUS_DATA_ERR)) {
		
		err_mask |= AC_ERR_HOST_BUS;
		action |= ATA_EH_SOFTRESET;
	}

	if (irq_stat & PORT_IRQ_IF_ERR) {
		
		err_mask |= AC_ERR_ATA_BUS;
		action |= ATA_EH_SOFTRESET;
		ata_ehi_push_desc(ehi, "interface fatal error");
	}

	if (irq_stat & (PORT_IRQ_CONNECT | PORT_IRQ_PHYRDY)) {
		
		ata_ehi_hotplugged(ehi);
		ata_ehi_push_desc(ehi, "%s", irq_stat & PORT_IRQ_CONNECT ?
			"connection status changed" : "PHY RDY changed");
	}

	if (irq_stat & PORT_IRQ_UNK_FIS) {
		u32 *unk = (u32 *)(pp->rx_fis + RX_FIS_UNK);
		
		err_mask |= AC_ERR_HSM;
		action |= ATA_EH_SOFTRESET;
		ata_ehi_push_desc(ehi, "unknown FIS %08x %08x %08x %08x",
				  unk[0], unk[1], unk[2], unk[3]);
	}

	/* okay, let's hand over to EH */
	ehi->serror |= serror;
	ehi->action |= action;

	qc = ata_qc_from_tag(ap, ap->active_tag);
	if (qc)
		qc->err_mask |= err_mask;
	else
		ehi->err_mask |= err_mask;

	if (irq_stat & PORT_IRQ_FREEZE)
		ata_port_freeze(ap);
	else
		ata_port_abort(ap);
}
#else
static void atp862x_error_intr(struct ata_port *ap, u32 irq_stat)
{
	struct atp862x_host_priv *hpriv = ap->host->private_data;
	struct atp862x_port_priv *pp = ap->private_data;
	struct ata_eh_info *host_ehi = &ap->link.eh_info;
	struct ata_link *link = NULL;
	struct ata_queued_cmd *active_qc;
	struct ata_eh_info *active_ehi;
	u32 serror;
	
	/* determine active link */
#ifdef kernel_ver_2629
	ata_for_each_link(link, ap, EDGE)
#else
	ata_port_for_each_link(link, ap)
#endif
		if (ata_link_active(link))
			break;
	if (!link)
		link = &ap->link;

	active_qc = ata_qc_from_tag(ap, link->active_tag);
	active_ehi = &link->eh_info;

	/* record irq stat */

	ata_ehi_clear_desc(host_ehi);
	ata_ehi_push_desc(host_ehi, "irq_stat 0x%08x", irq_stat);
	/* atp862x needs SError cleared; otherwise, it might lock up */
#ifdef kernel_ver_2628
	atp862x_scr_read(&ap->link, SCR_ERROR, &serror);
	atp862x_scr_write(&ap->link, SCR_ERROR, serror);
#else
	atp862x_scr_read(ap, SCR_ERROR, &serror);
	atp862x_scr_write(ap, SCR_ERROR, serror);
#endif
	host_ehi->serror |= serror;

	/* some controllers set IRQ_IF_ERR on device errors, ignore it */
	if (hpriv->flags & ATP862X_HFLAG_IGN_IRQ_IF_ERR) {		
		irq_stat &= ~PORT_IRQ_IF_ERR;
	}
	
	if (irq_stat & PORT_IRQ_TF_ERR) {
		/* If qc is active, charge it; otherwise, the active
		 * link.  There's no active qc on NCQ errors.  It will
		 * be determined by EH by reading log page 10h.
		 */	
		
		if (active_qc) {
			active_qc->err_mask |= AC_ERR_DEV;		
		} else {
			active_ehi->err_mask |= AC_ERR_DEV;			
		}
		if (hpriv->flags & ATP862X_HFLAG_IGN_SERR_INTERNAL) {			
			host_ehi->serror &= ~SERR_INTERNAL;
		}
	}

	if (irq_stat & PORT_IRQ_UNK_FIS) {
		u32 *unk = (u32 *)(pp->rx_fis + RX_FIS_UNK);
		
		active_ehi->err_mask |= AC_ERR_HSM;
		active_ehi->action |= ATA_EH_RESET;
		ata_ehi_push_desc(active_ehi,
				  "unknown FIS %08x %08x %08x %08x" ,
				  unk[0], unk[1], unk[2], unk[3]);
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	if (ap->nr_pmp_links && (irq_stat & PORT_IRQ_BAD_PMP)) {
#else
	if (sata_pmp_attached(ap) && (irq_stat & PORT_IRQ_BAD_PMP)) {
#endif		
		active_ehi->err_mask |= AC_ERR_HSM;
		active_ehi->action |= ATA_EH_RESET;
		ata_ehi_push_desc(active_ehi, "incorrect PMP");
	}
	if (irq_stat & (PORT_IRQ_HBUS_ERR | PORT_IRQ_HBUS_DATA_ERR)) {
			
		host_ehi->err_mask |= AC_ERR_HOST_BUS;
		host_ehi->action |= ATA_EH_RESET;
		ata_ehi_push_desc(host_ehi, "host bus error");
	}

	if (irq_stat & PORT_IRQ_IF_ERR) {	
		
		host_ehi->err_mask |= AC_ERR_ATA_BUS;
		host_ehi->action |= ATA_EH_RESET;
		ata_ehi_push_desc(host_ehi, "interface fatal error");
	}

	if (irq_stat & (PORT_IRQ_CONNECT | PORT_IRQ_PHYRDY)) {	
			
		ata_ehi_hotplugged(host_ehi);
		ata_ehi_push_desc(host_ehi, "%s",
			irq_stat & PORT_IRQ_CONNECT ?
			"connection status changed" : "PHY RDY changed");
	}

	/* okay, let's hand over to EH */

	if (irq_stat & PORT_IRQ_FREEZE)
		ata_port_freeze(ap);
	else
		ata_port_abort(ap);
	
	
}
#endif
static void atp862x_his_flush_cache(void __iomem *mmio_base)
{	
	int int_config;

	ioread32(mmio_base);
	int_config = hi3520_bridge_ahb_readl(CPU_ISTATUS)|0x0f000000;
	hi3520_bridge_ahb_writel(CPU_ISTATUS, 0x1000000);
}
static void atp862x_port_intr(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	struct ata_eh_info *ehi = &ap->eh_info;
#else
	struct ata_eh_info *ehi = &ap->link.eh_info;
	int resetting = !!(ap->pflags & ATP862X_PFLAG_RESETTING);
#endif
	struct atp862x_port_priv *pp = ap->private_data;
	struct atp862x_host_priv *hpriv = ap->host->private_data;
	
	u16	Notice;
	u32 status, qc_active;
	int rc,D2H_status,interrupt_status;
	u32 CompleteMapSDBFG;
	u8 *pReciveFIS;
	
	status = ioread32(port_mmio + PORT_IRQ_STAT);
	interrupt_status	= ioread32(port_mmio + PORT_IRQ_STAT);
	D2H_status	= ioread32(port_mmio + PORT_IRQA_STAT);
	D2H_status	= (D2H_status & ATP862X_REG70_MASK) | (interrupt_status & ATP862X_REG10_MASK); //D2H int flag & D2H Reg FIS int
	
	iowrite32(status, port_mmio + PORT_IRQ_STAT);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	if (ap->sactive) {
#else
	if (ap->qc_active && pp->active_link->sactive) {
#endif				
		qc_active = ioread32(port_mmio + PORT_SCR_ACT);
	} else {
		qc_active = ioread32(port_mmio + PORT_CMD_ISSUE);
	//	printk("qc_active %x ap->qc_active %x\n",qc_active,ap->qc_active);

	}	

	/* ignore BAD_PMP while resetting */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
	if (unlikely(resetting)) {
		status &= ~PORT_IRQ_BAD_PMP;
	
	}
#endif
	/* If we are getting PhyRdy, this is
 	 * just a power state change, we should
 	 * clear out this, plus the PhyRdy/Comm
 	 * Wake bits from Serror
 	 */
	if ((hpriv->flags & ATP862X_HFLAG_NO_HOTPLUG) &&
		(status & PORT_IRQ_PHYRDY)) {
		
		status &= ~PORT_IRQ_PHYRDY;
#ifdef kernel_ver_2628
		atp862x_scr_write(&ap->link, SCR_ERROR, ((1 << 16) | (1 << 18)));
#else
		atp862x_scr_write(ap, SCR_ERROR, ((1 << 16) | (1 << 18)));
#endif
	}
	
	if (interrupt_status & 0x0001FE08) {
		// SET DEVICE BITS FIS , Queue command complete 	
		if (interrupt_status & 0x0001FE00) {
			
			
		} else {
			pReciveFIS = pp->rx_fis + RX_FIS_SDB;
			if(pReciveFIS[2] & 0x1) {
				
				status |= PORT_IRQ_TF_ERR;
			}				
		}
	} else if(D2H_status & ATP862X_RX_FIS_D2H) {			
		pReciveFIS = pp->rx_fis + RX_FIS_D2H_REG;
		if(pReciveFIS[2] & 0x1) {		
			
			status |= PORT_IRQ_TF_ERR;		
		}
	} else if(D2H_status & ATP862X_RX_FIS_PIO) {
		pReciveFIS = pp->rx_fis + RX_FIS_PIO;
		if(pReciveFIS[2] & 0x1) {
					
			status |= PORT_IRQ_TF_ERR;	
		}
	}
	if (unlikely(status & PORT_IRQ_ERROR)) {	
		
		atp862x_error_intr(ap, status);	
		return;
	}	
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)	
	if (/*(status & PORT_IRQ_SDB_FIS) ||*/ status & PORT_IRQ_NOTICE ) {								
		if (hpriv->cap & HOST_CAP_SNTF){
			sata_async_notification(ap);	
		} else {
		
			const __le32 *f = pp->rx_fis + RX_FIS_SDB;
			u32 f0 = le32_to_cpu(f[0]);		
			if (f0 & (1 << 15)) {	
				sata_async_notification(ap);
			}
		}		
		if (status & PORT_IRQ_NOTICE) {	
			iowrite32(0x10000, port_mmio + PORT_SCR_ERR);
			Notice = ioread16(port_mmio + PORT_SCR_NTF);
			iowrite16(Notice,port_mmio + PORT_SCR_NTF);	
		}
	}
#else
	if (/*(status & PORT_IRQ_SDB_FIS) ||*/ status & PORT_IRQ_NOTICE ) {
		const __le32 *f = pp->rx_fis + RX_FIS_SDB;
		
		if (le32_to_cpu(f[1])) {
			/* SDB FIS containing spurious completions
			 * might be dangerous, whine and fail commands
			 * with HSM violation.  EH will turn off NCQ
			 * after several such failures.
			 */
			ata_ehi_push_desc(ehi,
				"spurious completions during NCQ "
				"issue=0x%x SAct=0x%x FIS=%08x:%08x",
				readl(port_mmio + PORT_CMD_ISSUE),
				readl(port_mmio + PORT_SCR_ACT),
				le32_to_cpu(f[0]), le32_to_cpu(f[1]));
			ehi->err_mask |= AC_ERR_HSM;
			ehi->action |= ATA_EH_SOFTRESET;
			ata_port_freeze(ap);
		} else {
			if (!pp->ncq_saw_sdb)
				ata_port_printk(ap, KERN_INFO,
					"spurious SDB FIS %08x:%08x during NCQ, "
					"this message won't be printed again\n",
					le32_to_cpu(f[0]), le32_to_cpu(f[1]));
			pp->ncq_saw_sdb = 1;
		}
		if (status & PORT_IRQ_NOTICE) {	
			iowrite32(0x10000, port_mmio + PORT_SCR_ERR);
			atp862x_his_flush_cache(port_mmio + PORT_SCR_ERR);
			Notice = ioread16(port_mmio + PORT_SCR_NTF);
			iowrite16(Notice,port_mmio + PORT_SCR_NTF);	
		}				
	}
#endif
	if (interrupt_status & 0x0001FE08) {		
		if (interrupt_status & 0x0001FE00) {
		//	printk("error:no sdb recived\n");
			if (D2H_status & ATP862X_REG70_MASK) {
					// clear interrupt flag for PM1 - PM7
				iowrite32((D2H_status & 0xFFFFFFF0),port_mmio + PORT_IRQA_STAT);
				atp862x_his_flush_cache(port_mmio + 0x10);
			}
			if (D2H_status & ATP862X_REG10_MASK) {				
				// clear interrupt flage for PM0
				iowrite32(0xEEEEEEE3,port_mmio + PORT_IRQ_STAT);
				atp862x_his_flush_cache(port_mmio + 0x10);
			}
		} else {		
			CompleteMapSDBFG = ioread32(port_mmio + PORT_SDB_FLAG);				
			iowrite32(CompleteMapSDBFG,port_mmio + PORT_SDB_FLAG);
			iowrite32(0xFFFE01F6,port_mmio + 0x0010);
			atp862x_his_flush_cache(port_mmio + 0x10);			
		}
	} else if(D2H_status) {
		if (D2H_status & ATP862X_REG70_MASK) {			
				// clear interrupt flag for PM1 - PM7
			iowrite32((D2H_status & 0xFFFFFFF0),port_mmio + PORT_IRQA_STAT);
			atp862x_his_flush_cache(port_mmio + 0x10);
		}
		if (D2H_status & ATP862X_REG10_MASK) {			
			// clear interrupt flage for PM0
			iowrite32(0xEEEEEEE3,port_mmio + PORT_IRQ_STAT);
			atp862x_his_flush_cache(port_mmio + 0x10);
		}
	}
	/* pp->active_link is valid iff any command is in flight */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
/*	struct ata_queued_cmd *qc;
	u32 done_mask,i;
	done_mask = ap->qc_active ^ qc_active;
	printk("done_mask %x\n",done_mask);
	for (i = 0; i < ATA_MAX_QUEUE; i++) {
		struct ata_queued_cmd *qc;

		if (!(done_mask & (1 << i)))
			continue;

		if ((qc = ata_qc_from_tag(ap, i))) {
			printk("before fill result_tf %p\n",qc);
			test_fill_result_tf(qc);
			printk("after fill result_tf\n");
		}
	}
*/	

	rc = ata_qc_complete_multiple(ap, qc_active, NULL);

#else
	rc = ata_qc_complete_multiple(ap, qc_active);
#endif
	/* while resetting, invalid completions are expected */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	if (rc < 0) {
		ehi->err_mask |= AC_ERR_HSM;
		ehi->action |= ATA_EH_SOFTRESET;
		ata_port_freeze(ap);
		return;
	}
#else	
	if (unlikely(rc < 0 && !resetting)) {
		ehi->err_mask |= AC_ERR_HSM;
		ehi->action |= ATA_EH_RESET;
		ata_port_freeze(ap);
	}
#endif
		
}

static irqreturn_t atp862x_interrupt(int irq, void *dev_instance)
{
	struct ata_host *host = dev_instance;
	struct atp862x_host_priv *hpriv;
	unsigned int i, handled = 0;
	void __iomem *mmio;
	struct ata_port *ap;
	u32 irq_stat, irq_masked;
	u8  Dma_status;	
	VPRINTK("ENTER\n");	
	hpriv = host->private_data;
	mmio = host->iomap[ATP862X_PCI_BAR];	
	/* sigh.  0xffffffff is a valid return from h/w */
	spin_lock(&host->lock);	
	hi3520_bridge_ahb_writel(CPU_ISTATUS, 0x1000000);	
	irq_stat = ioread32(mmio + HOST_IRQ_STAT);
//	printk("irq_stat02 addr 0x%x 0x%x\n",(unsigned int)(mmio + HOST_IRQ_STAT),irq_stat);
	if (!irq_stat) {
		spin_unlock(&host->lock);
		return IRQ_RETVAL(0);
	}
	if(irq_stat & IDE_IRQ_CHECK ) {	
	//	printk("ide intr\n");	
		
		ap = host->ports[2];
		if (ap && !(ap->flags & ATA_FLAG_DISABLED)) {
			struct ata_queued_cmd *qc;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
			qc = ata_qc_from_tag(ap, ap->active_tag);
#else
			qc = ata_qc_from_tag(ap, ap->link.active_tag);
#endif
			if (qc && (!(qc->tf.flags & ATA_TFLAG_POLLING)) &&
			(qc->flags & ATA_QCFLAG_ACTIVE)) {
			//	printk("clear ide cmd ide %x qc_dev_no %x\n",ap->port_no,qc->dev->devno);				
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
				handled |= ata_host_intr(ap, qc);				
#else
				handled |= ata_sff_host_intr(ap, qc);
#endif				
				ioread8(mmio + 0x87); //read port 7				
			} else {
			//	atp8620_qc_set_polling(ap);
				Dma_status = ioread8(mmio + 0x92) | 0x4;
				iowrite8(Dma_status,mmio + 0x92); 
				Dma_status = ioread8(mmio + 0x87); //read port 7
			//	printk("no one clean intr and ata not disabled\n");
			}
		} else {
			Dma_status = ioread8(mmio + 0x92) | 0x4;
			iowrite8(Dma_status,mmio + 0x92); 
			Dma_status = ioread8(mmio + 0x87); //read port 7			
		//	printk("no one clean intr\n");
		}
		

	} else {	
		irq_masked = irq_stat & hpriv->port_map;		
		
	
		for (i = 0; i < host->n_ports; i++) {
	
			if (!(irq_masked & (1 << i)))
				continue;
	
			ap = host->ports[i];
			if (ap) {
				atp862x_port_intr(ap);				
				VPRINTK("port %u\n", i);
			} else {
				VPRINTK("port %u (no irq)\n", i);
				if (ata_ratelimit())
					dev_printk(KERN_WARNING, host->dev,
						"interrupt on disabled port %u\n", i);
			}
	
			handled = 1;
		}
	}
	/* HOST_IRQ_STAT behaves as level triggered latch meaning that
	 * it should be cleared after all the port events are cleared;
	 * otherwise, it will raise a spurious interrupt after each
	 * valid one.  Please read section 10.6.2 of atp862x 1.1 for more
	 * information.
	 *
	 * Also, use the unmasked value to clear interrupt as spurious
	 * pending event on a dummy port might cause screaming IRQ.
	 */
	iowrite32(irq_stat, mmio + HOST_IRQ_STAT);

	spin_unlock(&host->lock);
	
	VPRINTK("EXIT\n");
	
	return IRQ_RETVAL(handled);
}

static unsigned int atp862x_qc_issue(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	void __iomem *port_mmio = atp862x_port_base(ap);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
	struct atp862x_port_priv *pp = ap->private_data;
#endif
	u32 	ci_map;
	u8	status;	
	/* Keep track of the currently active link.  It will be used
	 * in completion path to determine whether NCQ phase is in
	 * progress.
	 */
	
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#else	
	pp->active_link = qc->dev->link;
#endif
	status = ioread8(port_mmio + 0x74)&0xFE;
	ci_map = 1 << qc->tag;
	if (qc->tf.protocol == ATA_PROT_NCQ) {
		iowrite8(status|0x1, port_mmio + 0x74);
		iowrite32(ci_map, port_mmio + PORT_SCR_ACT);	
	//	printk("tag %x cmd %x\n",qc->tag,qc->tf.command);
		
	} else {
	//	printk("nonq tag %x cmd %x\n",qc->tag,qc->tf.command);
		iowrite32(status & 0xFE, port_mmio + 0x74);
	}
	status = ioread32(port_mmio + PORT_CMD_ISSUE);

	iowrite32(ci_map, port_mmio + PORT_CMD_ISSUE);


/*G.C.C. 2009 0506
#ifdef kernel_ver_2628
	atp862x_sw_activity(qc->dev->link);
#endif
*/	
	return 0;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
static void atp862x_tf_read(struct ata_port *ap, struct ata_taskfile *tf)
{
	struct atp862x_port_priv *pp = ap->private_data;	
	u8 *d2h_fis = pp->rx_fis + RX_FIS_D2H_REG;	
	
	ata_tf_from_fis(d2h_fis, tf);
}
#else
static bool atp862x_qc_fill_rtf(struct ata_queued_cmd *qc)
{
	struct atp862x_port_priv *pp = qc->ap->private_data;
	u8 *d2h_fis = pp->rx_fis + RX_FIS_D2H_REG;		
	ata_tf_from_fis(d2h_fis, &qc->result_tf);
	
	return true;
}
#endif


static void atp862x_freeze(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	
	/* turn IRQ off */
	iowrite32(0, port_mmio + PORT_IRQ_MASK);
}

static void atp862x_thaw(struct ata_port *ap)
{
	void __iomem *mmio = ap->host->iomap[ATP862X_PCI_BAR];
	void __iomem *port_mmio = atp862x_port_base(ap);
	u32 tmp;
	struct atp862x_port_priv *pp = ap->private_data;
	
	/* clear IRQ */
	tmp = ioread32(port_mmio + PORT_IRQ_STAT);
	iowrite32(tmp, port_mmio + PORT_IRQ_STAT);
	iowrite32(1 << ap->port_no, mmio + HOST_IRQ_STAT);

	/* turn IRQ back on */
	iowrite32(pp->intr_mask, port_mmio + PORT_IRQ_MASK);
	
}

static void atp862x_error_handler(struct ata_port *ap)
{	
	
	if (!(ap->pflags & ATA_PFLAG_FROZEN)) {
		/* restart engine */	
		atp862x_stop_engine(ap);
		atp862x_start_engine(ap);		
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	ata_do_eh(ap, ata_std_prereset, atp862x_softreset, atp862x_hardreset,
		  atp862x_postreset);
#else
	sata_pmp_error_handler(ap);
#endif
	
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
#  if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
static int atp862x_pmp_softreset(struct ata_link *link, unsigned int *class,
			      unsigned long deadline)
{	
	return atp862x_do_softreset(link, class, link->pmp, deadline);	

}
static void sata_pmp_error_handler(struct ata_port *ap)
{
	
	sata_pmp_do_eh(ap, ata_std_prereset, atp862x_softreset,
		       atp862x_hardreset, atp862x_postreset,
		       sata_pmp_std_prereset, atp862x_pmp_softreset,
		       sata_pmp_std_hardreset, sata_pmp_std_postreset);
}
# endif
#endif

static void atp862x_post_internal_cmd(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;	
	
	/* make DMA engine forget about the failed command */
	if (qc->flags & ATA_QCFLAG_FAILED) {	
		
		atp862x_kick_engine(ap, 1);
	}
	
	
}
#  if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
/*20100119@GCC add*/
static void atp862x_port_pmp_attach(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	struct atp862x_port_priv *pp = ap->private_data;
	u32 cmd;
	
	cmd = ioread32(port_mmio + PORT_CMD);
	cmd |= PORT_CMD_PMP;
	iowrite32(cmd, port_mmio + PORT_CMD);
	cmd = ioread32(port_mmio + PORT_CMD);
	pp->intr_mask |= PORT_IRQ_BAD_PMP;

}

static void atp862x_port_pmp_detach(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	struct atp862x_port_priv *pp = ap->private_data;
	u32 cmd;
	
	cmd = ioread32(port_mmio + PORT_CMD);
	cmd &= ~PORT_CMD_PMP;
	iowrite32(cmd, port_mmio + PORT_CMD);
	cmd = ioread32(port_mmio + PORT_CMD);
	pp->intr_mask &= ~PORT_IRQ_BAD_PMP;

}
/*20100119@GCC add end*/
static void atp862x_pmp_attach(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	struct atp862x_port_priv *pp = ap->private_data;
	u32 cmd;
	
	cmd = ioread32(port_mmio + PORT_CMD);
	cmd |= PORT_CMD_PMP;
	iowrite32(cmd, port_mmio + PORT_CMD);
	cmd = ioread32(port_mmio + PORT_CMD);
	pp->intr_mask |= PORT_IRQ_BAD_PMP;
	iowrite32(pp->intr_mask, port_mmio + PORT_IRQ_MASK);
	
}

static void atp862x_pmp_detach(struct ata_port *ap)
{
	void __iomem *port_mmio = atp862x_port_base(ap);
	struct atp862x_port_priv *pp = ap->private_data;
	u32 cmd;
	
	cmd = ioread32(port_mmio + PORT_CMD);
	cmd &= ~PORT_CMD_PMP;
	iowrite32(cmd, port_mmio + PORT_CMD);
	cmd = ioread32(port_mmio + PORT_CMD);
	pp->intr_mask &= ~PORT_IRQ_BAD_PMP;
	iowrite32(pp->intr_mask, port_mmio + PORT_IRQ_MASK);
	
}
#endif
static int atp862x_resume_port(struct ata_port *ap)
{	
	
	atp862x_power_up(ap);
	atp862x_start_port(ap);

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
//======================follow was not support with 2.6.23 and under=================================
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	if (ap->nr_pmp_links)
#  else
	if (sata_pmp_attached(ap))
#  endif
		atp862x_port_pmp_attach(ap);	
	else 
		atp862x_port_pmp_detach(ap);
//=======================================================
#endif			
	return 0;
}
static int atp862x_port_resume(struct ata_port *ap)
{	
	
	atp862x_power_up(ap);
	atp862x_start_port(ap);

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
//======================follow was not support with 2.6.23 and under=================================
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	if (ap->nr_pmp_links)
#  else
	if (sata_pmp_attached(ap))
#  endif
		atp862x_pmp_attach(ap);	
	else 
		atp862x_pmp_detach(ap);
//=======================================================
#endif			
	return 0;
}

#ifdef CONFIG_PM
static int atp862x_port_suspend(struct ata_port *ap, pm_message_t mesg)
{
	const char *emsg = NULL;
	int rc;
	
	rc = atp862x_deinit_port(ap, &emsg);
	if (rc == 0)
		atp862x_power_down(ap);
	else {
		ata_port_printk(ap, KERN_ERR, "%s (%d)\n", emsg, rc);
		atp862x_start_port(ap);
	}
	
	return rc;
}

static int atp862x_pci_device_suspend(struct pci_dev *pdev, pm_message_t mesg)
{
	struct ata_host *host = dev_get_drvdata(&pdev->dev);
	void __iomem *mmio = host->iomap[ATP862X_PCI_BAR];
	u32 ctl;
	
	if (mesg.event & PM_EVENT_SLEEP) {
		
		ctl = ioread32(mmio + HOST_CTL);
		ctl &= ~HOST_IRQ_EN;
		iowrite32(ctl, mmio + HOST_CTL);
		ioread32(mmio + HOST_CTL); /* flush */
	}
	
	return ata_pci_device_suspend(pdev, mesg);
}

static int atp862x_pci_device_resume(struct pci_dev *pdev)
{
	struct ata_host *host = dev_get_drvdata(&pdev->dev);
	int rc;
	
	rc = ata_pci_device_do_resume(pdev);
	if (rc)
		return rc;

	if (pdev->dev.power.power_state.event == PM_EVENT_SUSPEND) {
		rc = atp862x_reset_controller(host);
		if (rc)
			return rc;

		atp862x_init_controller(host);
	}

	ata_host_resume(host);
	
	return 0;
}
#endif

static int atp862x_port_start(struct ata_port *ap)
{
	struct device *dev = ap->host->dev;
	struct atp862x_port_priv *pp;
	void *mem;
	dma_addr_t mem_dma;
	int rc;
	
	pp = devm_kzalloc(dev, sizeof(*pp), GFP_KERNEL);
	if (!pp)
		return -ENOMEM;

	mem = dmam_alloc_coherent(dev, ATP862X_PORT_PRIV_DMA_SZ, &mem_dma,
				  GFP_KERNEL);
	if (!mem)
		return -ENOMEM;
	
	memset(mem, 0, ATP862X_PORT_PRIV_DMA_SZ);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
	rc = ata_pad_alloc(ap, dev);
	if (rc)
		return rc;
#endif
	/*
	 * First item in chunk of DMA memory: 32-slot command table,
	 * 32 bytes each in size
	 */
	pp->cmd_slot = mem;
	pp->cmd_slot_dma = mem_dma;

	mem += ATP862X_CMD_SLOT_SZ;
	mem_dma += ATP862X_CMD_SLOT_SZ;

	/*
	 * Second item: Received-FIS area
	 */
	pp->rx_fis = mem;
	pp->rx_fis_dma = mem_dma;

	mem += ATP862X_RX_FIS_SZ;
	mem_dma += ATP862X_RX_FIS_SZ;

	/*
	 * Third item: data area for storing a single command
	 * and its scatter-gather table
	 */
	pp->cmd_tbl = mem;
	pp->cmd_tbl_dma = mem_dma;

	/*
	 * Save off initial list of interrupts to be enabled.
	 * This could be changed later
	 */
	pp->intr_mask = DEF_PORT_IRQ;

	ap->private_data = pp;
	
	/* engage engines, captain */
/*20100119@GCC modify:ahci fllow bug*/
	return atp862x_resume_port(ap);
//	return atp862x_port_resume(ap);
}

static void atp862x_port_stop(struct ata_port *ap)
{
	const char *emsg = NULL;
	int rc;
	
	/* de-initialize port */
	rc = atp862x_deinit_port(ap, &emsg);
	if (rc)
		ata_port_printk(ap, KERN_WARNING, "%s (%d)\n", emsg, rc);
	
}

static int atp862x_configure_dma_masks(struct pci_dev *pdev, int using_dac)
{
	int rc;
	
	if (using_dac &&
	    !pci_set_dma_mask(pdev, DMA_64BIT_MASK)) {
		rc = pci_set_consistent_dma_mask(pdev, DMA_64BIT_MASK);
		if (rc) {
			rc = pci_set_consistent_dma_mask(pdev, DMA_32BIT_MASK);
			if (rc) {
				dev_printk(KERN_ERR, &pdev->dev,
					   "64-bit DMA enable failed\n");
				return rc;
			}
		}
	} else {
		rc = pci_set_dma_mask(pdev, DMA_32BIT_MASK);
		if (rc) {
			dev_printk(KERN_ERR, &pdev->dev,
				   "32-bit DMA enable failed\n");
			return rc;
		}
		rc = pci_set_consistent_dma_mask(pdev, DMA_32BIT_MASK);
		if (rc) {
			dev_printk(KERN_ERR, &pdev->dev,
				   "32-bit consistent DMA enable failed\n");
			return rc;
		}
	}
	rc = pci_set_dma_mask(pdev, DMA_32BIT_MASK);
	if (rc) {
		dev_printk(KERN_ERR, &pdev->dev,
				"32-bit DMA enable failed\n");
		return rc;
	}
	rc = pci_set_consistent_dma_mask(pdev, DMA_32BIT_MASK);
	if (rc) {
		dev_printk(KERN_ERR, &pdev->dev,
				"32-bit consistent DMA enable failed\n");
		return rc;
	}
	
	return 0;
}
static void atp862x_print_info(struct ata_host *host)
{
	struct atp862x_host_priv *hpriv = host->private_data;
	struct pci_dev *pdev = to_pci_dev(host->dev);
	void __iomem *mmio = host->iomap[ATP862X_PCI_BAR];
	u32 vers, cap, impl, speed;
	const char *speed_s;
	u16 cc;
	const char *scc_s;
	
	vers = ioread32(mmio + HOST_VERSION);
	cap = hpriv->cap;
	impl = hpriv->port_map;

	speed = (cap >> 20) & 0xf;
	if (speed == 1)
		speed_s = "1.5";
	else if (speed == 2)
		speed_s = "3";
	else
		speed_s = "?";

	pci_read_config_word(pdev, 0x0a, &cc);
	if (cc == PCI_CLASS_STORAGE_IDE)
		scc_s = "IDE";
	else if (cc == PCI_CLASS_STORAGE_SATA)
		scc_s = "SATA";
	else if (cc == PCI_CLASS_STORAGE_RAID)
		scc_s = "RAID";
	else
		scc_s = "unknown";

	dev_printk(KERN_INFO, &pdev->dev,
		"atp862x %02x%02x.%02x%02x "
		"%u slots %u ports %s Gbps 0x%x impl %s mode\n"
		,

		(vers >> 24) & 0xff,
		(vers >> 16) & 0xff,
		(vers >> 8) & 0xff,
		vers & 0xff,

		((cap >> 8) & 0x1f) + 1,
		(cap & 0x1f) + 1,
		speed_s,
		impl,
		scc_s);

	dev_printk(KERN_INFO, &pdev->dev,
		"flags: "
		"%s%s%s%s%s%s%s"
		"%s%s%s%s%s%s%s"
		"%s\n"
		,

		cap & (1 << 31) ? "64bit " : "",
		cap & (1 << 30) ? "ncq " : "",
		cap & (1 << 29) ? "sntf " : "",
		cap & (1 << 28) ? "ilck " : "",
		cap & (1 << 27) ? "stag " : "",
		cap & (1 << 26) ? "pm " : "",
		cap & (1 << 25) ? "led " : "",

		cap & (1 << 24) ? "clo " : "",
		cap & (1 << 19) ? "nz " : "",
		cap & (1 << 18) ? "only " : "",
		cap & (1 << 17) ? "pmp " : "",
		cap & (1 << 15) ? "pio " : "",
		cap & (1 << 14) ? "slum " : "",
		cap & (1 << 13) ? "part " : "",
		cap & (1 << 6) ? "ems ": ""
		);
	
}

/* On ASUS P5W DH Deluxe, the second port of PCI device 00:1f.2 is
 * hardwired to on-board SIMG 4726.  The chipset is ICH8 and doesn't
 * support PMP and the 4726 either directly exports the device
 * attached to the first downstream port or acts as a hardware storage
 * controller and emulate a single ATA device (can be RAID 0/1 or some
 * other configuration).
 *
 * When there's no device attached to the first downstream port of the
 * 4726, "Config Disk" appears, which is a pseudo ATA device to
 * configure the 4726.  However, ATA emulation of the device is very
 * lame.  It doesn't send signature D2H Reg FIS after the initial
 * hardreset, pukes on SRST w/ PMP==0 and has bunch of other issues.
 *
 * The following function works around the problem by always using
 * hardreset on the port and not depending on receiving signature FIS
 * afterward.  If signature FIS isn't received soon, ATA class is
 * assumed without follow-up softreset.
 */

static int atp862x_init_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	static int printed_version;
	unsigned int board_id = ent->driver_data;
	struct ata_port_info pi = atp862x_port_info[board_id];
//	const struct ata_port_info *ppi[ATP862X_MAX_PORTS];
	struct device *dev = &pdev->dev;
	struct atp862x_host_priv *hpriv;	
	int /*n_ports, */i, rc;
	struct ata_host *host;
	void __iomem *mmio;		
	u32 cmd;//GCC

	VPRINTK("ENTER\n");	
	WARN_ON(ATA_MAX_QUEUE > ATP862X_MAX_CMDS);

	if (!printed_version++)
		dev_printk(KERN_DEBUG, &pdev->dev, "version " DRIVE_VERSION "\n");

	/* acquire resources */
	rc = pcim_enable_device(pdev);
	if (rc)
		return rc;

	/* atp862x controllers often implement SFF compatible interface.
	 * Grab all PCI BARs just in case.
	 */
//	rc = pcim_iomap_regions_request_all(pdev, 1 << ATP862X_PCI_BAR, DRV_NAME);
/*	rc = pcim_iomap_regions(pdev, 1 << ATP862X_PCI_BAR0, DRV_NAME);
	if (rc == -EBUSY)
		pcim_pin_device(pdev);	
	if (rc)
		return rc;
	rc = pcim_iomap_regions(pdev, 1 << ATP862X_PCI_BAR1, DRV_NAME);	
	if (rc == -EBUSY)
		pcim_pin_device(pdev);	
	if (rc)
		return rc;
	rc = pcim_iomap_regions(pdev, 1 << ATP862X_PCI_BAR2, DRV_NAME);	
	if (rc == -EBUSY)
		pcim_pin_device(pdev);	
	if (rc)
		return rc;*/

	rc = pcim_iomap_regions(pdev, 1 << ATP862X_PCI_BAR, DRV_NAME);
	if (rc == -EBUSY)
		pcim_pin_device(pdev);	
	if (rc)
		return rc;
	mmio = pcim_iomap_table(pdev)[ATP862X_PCI_BAR];
	
/*	mmio = pcim_iomap_table(pdev)[ATP862X_PCI_BAR0];
	printk("mmio0 %p\n",mmio);
	mmio = pcim_iomap_table(pdev)[ATP862X_PCI_BAR1];	
	printk("mmio1 %p\n",mmio);
	mmio = pcim_iomap_table(pdev)[ATP862X_PCI_BAR2];
	printk("mmio2 %p\n",mmio);*/

	// added by GCC
	cmd=0x403f;
	pci_write_config_dword(pdev, PCI_CACHE_LINE_SIZE, cmd);
	cmd=0x00;
	pci_read_config_dword(pdev, PCI_CACHE_LINE_SIZE, &cmd);
	printk("0x%x: latency timer and cache line value !\n",cmd);
	pci_read_config_dword(pdev, PCI_BASE_ADDRESS_5, &cmd);
	printk("*************** bar5: %x\n",cmd);

//	mmio = host->iomap[ATP862X_PCI_BAR];
	//mmio_base = ioremap_nocache(cmd,0x1000);
//	printk("!!!!!!!! mmio_base phy: %x\n",virt_to_phys(mmio));

	hpriv = devm_kzalloc(dev, sizeof(*hpriv), GFP_KERNEL);
	if (!hpriv)
		return -ENOMEM;
	hpriv->flags |= (unsigned long)pi.private_data;
//	hpriv->flags |= ATP862X_HFLAG_NO_MSI;	

	if ((hpriv->flags & ATP862X_HFLAG_NO_MSI) || pci_enable_msi(pdev))
		pci_intx(pdev, 1);

	/* save initial config */	
	atp862x_save_initial_config(pdev, hpriv);

	/* prepare host */
#if	NCQ_ENABLE
	if (hpriv->cap & HOST_CAP_NCQ)
		pi.flags |= ATA_FLAG_NCQ;
#endif
	if (hpriv->cap & HOST_CAP_PMP)
		pi.flags |= ATP862X_FLAG_PMP;
/*
#ifdef kernel_ver_2628
	if (atp862x_em_messages && (hpriv->cap & HOST_CAP_EMS)) {		
		u8 messages;
		void __iomem *mmio = pcim_iomap_table(pdev)[ATP862X_PCI_BAR0 ];
//		printk("mmio2 %p\n",mmio);
		u32 em_loc = ioread32(mmio + HOST_EM_LOC);
		u32 em_ctl = ioread32(mmio + HOST_EM_CTL);

		messages = (em_ctl & EM_CTRL_MSG_TYPE) >> 16;

	//	 we only support LED message type right now
 
		if ((messages & 0x01) && (atp862x_em_messages == 1)) {
//			 store em_loc 
			hpriv->em_loc = ((em_loc >> 16) * 4);
			pi.flags |= ATA_FLAG_EM;
			if (!(em_ctl & EM_CTL_ALHD))
				pi.flags |= ATA_FLAG_SW_ACTIVITY;
		}
	}
#endif*/
	/* CAP.NP sometimes indicate the index of the last enabled
	 * port, at other times, that of the last possible port, so
	 * determining the maximum port number requires looking at
	 * both CAP.NP and port_map.
	 */
	
	switch (board_id) {
	case board_atp8620:
		
		rc = atp8620_prepare_host(pdev, &host,&pi);
		
	//	printk("atp8620_prepare_host\n");
		if (rc)
			return rc;
		break;	
	case board_atp8624:		
		rc = atp8624_prepare_host(pdev, &host,&pi);
		host->iomap = pcim_iomap_table(pdev);
		
//		n_ports = max(atp862x_nr_ports(hpriv->cap), fls(hpriv->port_map));
		if (rc)
			return rc;
		
		break;
	default:
		rc = -EBUSY;
		return rc;
		
	}	
//	printk("be atp862x_configure_dma_masks\n");
	rc = atp862x_configure_dma_masks(pdev, hpriv->cap & HOST_CAP_64);
	if (rc)
		return rc;
//	printk("af atp862x_configure_dma_masks\n");			
	
	host->private_data = hpriv;


	
/*
	if (pi.flags & ATA_FLAG_EM)
		atp862x_reset_em(host);
*/	
	for (i = 0; i < host->n_ports; i++) {
		struct ata_port *ap = host->ports[i];
		void __iomem *port_mmio = atp862x_port_base(ap);
# if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
		ata_port_pbar_desc(ap, ATP862X_PCI_BAR, -1, "abar");
#endif
		switch (board_id) {
		case board_atp8620:			
			if(i == 2) {
#ifdef	ATP8620_IDE_PORT		
				hpriv->flags |= ATP862X_HFLAG_ATP_PATA;	
# if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)	
				ata_port_pbar_desc(ap, ATP862X_PCI_BAR0, 0x90, "port");
# endif
#else
				ap->ops = &ata_dummy_port_ops;
#endif
			} else {				
				ap->ioaddr.cmd_addr = port_mmio;
				ata_port_pbar_desc(ap, ATP862X_PCI_BAR1,
							ap->port_no * 0x80, "port");
			}
			break;			
		case board_atp8624:		
		/*	if(i <= 1) {
				ata_port_pbar_desc(ap, ATP862X_PCI_BAR1,
						ap->port_no * 0x80, "port");
			} else if (i > 1 || i <=3) {
				ata_port_pbar_desc(ap, ATP862X_PCI_BAR2,
						(ap->port_no-2) * 0x80, "port");
			}
			ap->ioaddr.cmd_addr = port_mmio;*/
			ata_port_pbar_desc(ap, ATP862X_PCI_BAR+0x100,
						ap->port_no * 0x80, "port");			
			ap->ioaddr.cmd_addr = port_mmio;
			break;
		}
		/* set initial link pm policy */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,23)
		ap->pm_policy = NOT_AVAILABLE;
#endif
		/* set enclosure management message type */
#ifdef kernel_ver_2628
		if (ap->flags & ATA_FLAG_EM)
			ap->em_message_type = atp862x_em_messages;
#endif
		/* disabled/not-implemented port */
	/*	if (!(hpriv->port_map & (1 << i)))
			ap->ops = &ata_dummy_port_ops;*/
	}
	/* initialize adapter */
	rc = atp862x_reset_controller(host);
	if (rc) {			
		return rc;
	}		
	atp862x_init_controller(host);
	atp862x_print_info(host);

	pci_set_master(pdev);
	/*enable intr*/
	printk("pdev->irq is %x\n",pdev->irq);//gcc for his
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,21)		
	return ata_host_activate(host, pdev->irq, atp862x_interrupt, IRQF_SHARED,
				 &atp862x_sht);
#endif
}

static int atp8624_prepare_host(struct pci_dev *pdev, struct ata_host **r_host, struct ata_port_info *pi)
{
	const struct ata_port_info *ppi[] = { pi, NULL, NULL, NULL };
	struct ata_host *host;
	
	*r_host = host = ata_host_alloc_pinfo(&pdev->dev, ppi, ARRAY_SIZE(ppi));
	if (!host) {
		dev_printk(KERN_ERR, &pdev->dev, "failed to allocate host\n");
		return -ENOMEM;
	}	

//	host->n_ports = ARRAY_SIZE(ppi);

	return 0;
}
static int atp8620_prepare_host(struct pci_dev *pdev, struct ata_host **r_host, struct ata_port_info *pi)
{
	const struct ata_port_info *ppi[] =
		{ pi, pi, atp862x_pata_port_info };
	struct ata_host *host;		
	int rc;
	
	if (!devres_open_group(&pdev->dev, NULL, GFP_KERNEL))
		return -ENOMEM;

	*r_host = host = ata_host_alloc_pinfo(&pdev->dev, ppi, ARRAY_SIZE(ppi));
	if (!host) {
		dev_printk(KERN_ERR, &pdev->dev, "failed to allocate host\n");
		rc = -ENOMEM;
		goto err_out;
		
	}		
	host->iomap = pcim_iomap_table(pdev);
	atp8620_init_addrs(host->ports[2]);
	
	devres_remove_group(&pdev->dev, NULL);
//	host->n_ports = ARRAY_SIZE(ppi);
	
/*
	rc = pci_set_dma_mask(pdev, ATA_DMA_MASK);
	if (rc)
		return rc;
	rc = pci_set_consistent_dma_mask(pdev, ATA_DMA_MASK);
	if (rc)
		return rc;
*/
	return 0;
 err_out:
	devres_release_group(&pdev->dev, NULL);
	return rc;
}
static void atp8620_init_addrs(struct ata_port *ap)
{
	void __iomem * const * iomap = ap->host->iomap;
	void __iomem *reg_addr = iomap[ATP862X_PCI_BAR] + 0x80;
	void __iomem *bmdma_addr = reg_addr + 0x10;
	struct ata_ioports *ioaddr = &ap->ioaddr;

//	printk("atp8620_init_addrs io %x\n",(u32)reg_addr);
	ioaddr->cmd_addr = reg_addr;
	ioaddr->altstatus_addr =
	ioaddr->ctl_addr = (void __iomem *)
		((unsigned long)(reg_addr + 0xE));
	ioaddr->bmdma_addr = bmdma_addr;
//	ioaddr->scr_addr = vt6421_scr_addr(iomap[5], ap->port_no); no scr
//	printk("before ata_sff_std_ports\n");
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	ata_std_ports(ioaddr);
#else
	ata_sff_std_ports(ioaddr);
#endif
//	printk("after ata_sff_std_ports\n");
//	ata_port_desc(ap, "cmd 0x%llx ctl 0x%llx",
//			(unsigned long long)ioaddr->altstatus_addr,
//			(unsigned long long)ioaddr->ctl_add);
//	ata_port_pbar_desc(ap, ap->port_no, -1, "port");
//	ata_port_pbar_desc(ap, 4, ap->port_no * 8, "bmdma");
}
static int __init atp862x_init(void)
{
	return pci_register_driver(&atp862x_pci_driver);
}

static void __exit atp862x_exit(void)
{
	pci_unregister_driver(&atp862x_pci_driver);
}


MODULE_AUTHOR("G.C.C.");
MODULE_DESCRIPTION("atp862x SATA low-level driver");
MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(pci, atp862x_pci_tbl);
MODULE_VERSION(DRIVE_VERSION);

module_init(atp862x_init);
module_exit(atp862x_exit);
