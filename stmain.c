


/**
 **   $Log: stmain.c,v $
 **   Revision 1.22.8.1.6.16.2.2.2.2  2020/02/14 05:17:19  LPPL253
 **   2nd Test Version With corrections
 **
 **   Revision 1.22.8.1.6.16.2.2.2.1  2017/08/14 10:56:58  LPPL253
 **   -Addition on flexcan
 **
 **   Revision 1.22.8.1.6.16.2.2  2017/08/14 12:12:32  LPPL253
 **   -Correction in static ip
 **   -Correction in SNMP Walk
 **
 **   Revision 1.22.8.1.6.16.2.1  2017/08/14 12:27:12  LPPL253
 **   -IN02 Fixed for Smoke Fire alarm
 **
 **   Revision 1.22.8.1.6.16  2017/08/14 10:34:43  PPSPL232
 **   -If ac is available the consider highest rectifier voltage as plant voltage
 **   -get_max_batt_chrg_current changed to unsigned to signed
 **
 **   Revision 1.22.8.1.6.15  2017/08/14 10:02:52  LPPL265
 **   - Removed DC1,SCAP from bcgal.
 **   - Solved vision non communication issue.
 **
 **   Revision 1.22.8.1.6.14  2017/08/14 07:25:17  LPPL265
 **   -Clearing the rxd buffer after reading to respective parameters not present in 4N5 but present in 4N3.
 **   -Fixing rectifier output voltage to BLVD reconnect voltage for first 30s after AC retrival to avoid rectifiers going to faulty mode.
 **   -"REVERSED BATTERY" text changed to "BATT REVERSE".
 **   -LION Alarm issue fixed which was introduced after optimization.
 **   -Made compatable with both 64k and 256k flash IC's.
 **
 **   Revision 1.22.8.1.6.13  2017/08/14 06:53:19  LPPL265
 **   -Adaptive charging logic added.
 **   -Given attribute to read voltage sensed across battery fuse.
 **   -Vision 100Ah batteries added and protocol changed for existing vision 75Ah.
 **   -Minimum step increment or decrement made 0.2v to avoid issues araised due to calibration difference.
 **   -Generation of FANFAIL alarm based on digital input.
 **   -Battery type strings changed based on RIL requirement.
 **   -Shifted to current method.
 **   -Generation of FANFAIL alarm based on digital input.
 **
 **   Revision 1.22.8.1.6.12  2017/08/14 06:14:16  LPPL265
 **    - Overwriting RCL based on battery selected for the first time.
 **
 **   Revision 1.22.8.1.6.11  2017/08/14 08:29:42  LPPL265
 **   -Increased max number of OID's accepted in single GET or GETBULK command to 34 numbers.
 **   -Neglecting CRC and reply length for LINKDATA part number command(Non standard) as per RIL requirement.
 **
 **   Revision 1.22.8.1.6.10  2017/01/04 05:50:53  LPPL239
 **   	- Shifted of default from Flash to SerialFlashMemory to access via ethernet
 **   	- BDC alarm bug clearence
 **   	- IPV6 address representation and configuration is made into standard format.
 **   	- Added new BLVD reconnect and disconnect voltage while mains are not available.
 **   	- Added new current charging logic in plrcl.c
 **
 **   Revision 1.22.8.1.6.9  2016/09/30 04:17:50  LPPL239
 **   -VRLA btsoc is modified and tested.
 **   -Rect ON Battery discharge logice is made based on rectifier current
 **   -AL_MNBD name changed to AL_AACFBD and added logic based on rectifier current.
 **   -IPv6 address can be editable from display by adding HEX type.
 **   -In Smoke fire alarm, LVD trip is removed and LVD is forcly closed at bootup for 5min .
 **   -SAFT timer is modified to individula battery timer.
 **   -For SAFT battery Float set point modified to 56.5v to 56v.
 **   -Rectifers sequencial start is removed and starting all with 0.1v less the plant voltage.
 **   -Minimum battery voltage from all batteries is made as plant voltage
 **   -For Web ala repeated increment is removed and check for wrong alarm is given.
 **   -IA address is added in dhcpv6 request for juniper.
 **
 **   Revision 1.22.8.1.6.8  2016/05/28 13:09:17  LPPL239
 **   VRLA Battery soc logic added and logo file removed from web
 **
 **   Revision 1.22.8.1.6.7  2016/05/09 06:16:32  LPPL239
 **   FIxing his.csv and read in web,saving in serial flash
 **   tested web in reliance on 6 may 2016
 **
 **   Revision 1.22.8.1.6.6  2016/04/19 09:57:54  LPPL239
 **   	- Added preprocessors to optimize the code.
 **   	- Warnings were removed and code optimised
 **   	- Added SNMPv3 with NOAuth,fault log his.csv
 **   	- Added new Fnet,dhcpv6,webpages in external.
 **
 **   Revision 1.22.8.1.6.4  2015/11/07 07:04:43  LPPL239
 **   - assigned MJ,Min and Ro for alarms
 **   - Preprossers added to BCGAL,RIL_PBUS,RIL_OTA,MODEM
 **
 **   Revision 1.22.8.1.6.3  2015/10/28 14:46:00  LPPL239
 **   -- VRLA parameters range addtion
 **   -- Coslight data loss com fail alarm addtion
 **   -- equalization in vrla
 **   -- dg commfail modificaition
 **
 **   Revision 1.22.8.1.6.2  2015/10/24 08:05:37  LPPL239
 **   	- Added DG comfail alarm
 **   	- Extend display and communication to 24 modbus devices
 **   	- Add VRLA config parameters in display,com and Mib.
 **
 **   Revision 1.22.8.1.6.1  2015/10/19 09:14:12  LPPL239
 **   -Added Features
 **   	- Extend number of Main to 24 and slaves and remote devices to 8 each
 **   	- Extend display and communication to 16 modbus devices
 **   	- Extend alarms to 16 batteries and mib for 16 batteries
 **   BugFix
 **   	--compensation for Battery fuse fail logic
 **   	--ACEM config config update for Latest code change.
 **
 **   Revision 1.22.8.1  2015/08/03 10:32:11  LPPL239
 **   - MERGING FROM GBM 0.1.0.ci3.2 (tag - "GBM-0_1_0_CI_3-2")
 **   - New Features from the MERGE
 **   -- DG MODBUS Device (Primary & Secondary MODBUS port)
 **   -- Active Alarm Sync on SNMP ("PS1,ALST")
 **   -- SNMP MIB modifications(for the above).
 **   - Bugs/Fixes from the MERGE
 **   -- Alarm debounce for battery comm alarms and a few more (LICOF1.., BDC1..)
 **
 **   Revision 1.22.10.1  2015/07/15 10:31:04  LPPL239
 **   -- Address LCM (Display) Jitter (Display SPI freq reduced from 8MHz to 0.8MHz.
 **
 **   Revision 1.22  2014/03/12 13:47:12  fr003137
 **   - MERGING 0.0.28.c10 into head, latest tag on head at the time: 0.0.28
 **
 **   Revision 1.21.10.3  2014/03/06 14:15:57  fr003137
 **   - New event AL_SYST (to notify sytem start) created.
 **
 **   Revision 1.21.10.2  2014/03/03 07:07:12  fr003253
 **   - 0.0.28_LC_1 (config.gal) merged to 0.0.28_C branch
 **
 **   Revision 1.21.10.1  2014/02/21 11:29:05  fr003232
 **   - V1_LVD_VTG_BULD_SEC_TIMEOUT and LVD_POWERUP_TIMEOUT increased to 30 seconds
 **   -  BD state is not handled till voltage is build up
 **   - Sens Voltage Fail, Battery on Discharge (Based on Voltage), Very Low Voltage alarms delayed for Voltage build up time (30 Seconds)
 **
 **   Revision 1.21.8.1  2014/01/29 06:57:08  fr003253
 **   - added config_gal schedular
 **
 **   Revision 1.21  2013/08/16 15:25:37  fr003232
 **   - Remote Master support added
 **
 **   Revision 1.20  2013/08/13 17:47:55  fr003137
 **   - SPI critical functions handled at more resolution.
 **
 **   Revision 1.19  2013/08/07 15:07:38  fr003137
 **   - New Feature:- STFFS (Mini mock flash file system) and remote firmware upgrade via TFTP
 **
 **   Revision 1.18  2013/07/16 09:24:14  fr003232
 **   - lv_init_lvd_states(); moved just before while(1) loop to give proper power up time
 **
 **   Revision 1.17  2013/07/12 13:55:42  fr003137
 **   - Implementing sysUpTime for SNMP rfc1213.
 **
 **   Revision 1.16  2013/07/05 12:58:13  fr003232
 **   - BUILD_CNFG_REL_GBM name replaced by BUILD_CNFG_RIL_PP
 **
 **   Revision 1.15  2013/07/01 15:59:12  fr003232
 **   - eq_handler double definition removed
 **   - saft modbus configuration corrected
 **   - ACEM default configuration corrected
 **   - modbus assigning null device corrected
 **
 **   Revision 1.14  2013/06/19 07:56:41  fr003137
 **   - Function relocation (moving mac id converter from st_main to nt.c) as nt was found to be
 **     natural host to the function.
 **
 **   Revision 1.13  2013/06/17 15:41:44  fr003253
 **   - Enabled watchdog
 **
 **   Revision 1.12  2013/06/17 05:06:08  fr003232
 **   - Ac Energy meter support added
 **
 **   Revision 1.11  2013/05/31 16:46:27  fr003232
 **   - Coslight Battery Handler Added
 **
 **   Revision 1.10  2013/05/30 19:04:39  fr003232
 **   - Alarm Section for Lithium Ion Batteries Corrected
 **   - AC Fail and Battery on Discharge based on Current alarms added
 **   - Modbus Master Initialization added
 **   - Batery Defaults Corrected
 **
 **   Revision 1.9  2013/05/30 00:01:02  fr003137
 **   - Mac-ID from Factory connected to FNET now!
 **   - Network (nt) and SNMP (sn) application modules added, removing the old UDP Sock Module.
 **     - "net1" and "snt1" objects created in this regard.
 **
 **   Revision 1.8  2013/05/29 21:14:10  fr003232
 **   - Old RCL Handler added as a optional handler for Regular Batteries
 **   - Li-on Defaults Corrected
 **
 **   Revision 1.7  2013/05/29 09:17:36  fr003232
 **   - Number of Alarms Increased to two byte value
 **   - Lithum-Ion Batteries selection added
 **   - Lithium Ion Batteries Support added
 **   - Rectifer Current Limit after Communication Loss added as a configurable parameter
 **
 **   Revision 1.6  2013/05/28 06:13:30  fr003253
 **   - Commented FNET temporarily as MAC is Dummy for it
 **
 **   Revision 1.5  2013/05/21 13:21:44  fr003137
 **   - Relocation alarm history to external flash (from EEPROM).
 **   - correcting "warmboot"ing location!
 **
 **   Revision 1.4  2013/05/20 13:24:23  fr003137
 **   ------------------------------------------------------------
 **   LARGE SCALE CHANGES OWING TO FNET & INTERNICHE STACK
 **              INTEGRATION with APPLICATION - FIRST CUT
 **   ------------------------------------------------------------
 **   0.0.6, 20-May-2013, "rvk"
 **   - CodeWarrior IDE platform changed to 7.2 (from 7.1)
 **   - Project compilation warning:- BUILD_ALL:691, XRAM1M - Not Compiled at this time, STD:691.
 **   - Adding back "STD" build target.
 **   - Using Internal Flash for executing code memory
 **   - Using both Internal as well as extern SRAM for data memory.
 **   - LCF, mem and cfg file affected accordingly.
 **   ------------------------------------------------------------
 **   ------------------------------------------------------------
 **
 **   Revision 1.3  2013/05/15 17:32:43  fr003253
 **   - Rectifier initial chattering resolved
 **
 **   Revision 1.2  2013/05/15 09:22:56  fr003253
 **   - Warmboot to external app
 **   - For REL_GBM comment some portion of code
 **   - mutex between external flash and LCD operation
 **
 **   Revision 1.1  2013/05/06 15:33:34  fr003137
 **   0.0.1, 6-May-2013 "rvk"
 **   - Moving all the files from Pluto+ to PlutoPlus CVS repository!
 **   - Changes only in who.c and my_readme
 **
 **   Revision 1.1  2013/05/02 05:09:40  fr003137
 **   - All New.
 **
 **
 **/


#include "CPU.h"
#include "TI1.h"

#include "al.h"
#include "an.h"
#include "ac.h"
#include "bc.h"
#include "bd.h"
#include "bo.h"
#include "bt.h"
#include "ck.h"
#include "cnfg.h"
#include "di.h"
#include "dt.h"
#include "ee.h"
#include "eq.h"
#include "fl.h"
#include "flc.h"
#include "gltext.h"
#include "gp.h"
#include "i2c.h"
#include "ld.h"
#include "lv.h"
#include "pbus.h"
#include "pl.h"
#include "pt.h"
#include "rg.h"
#include "re.h"
#include "sc.h"
#include "st.h"
#include "sysdef.h"
#include "xf.h"
#include "ss.h"
#include "mbms1.h"
#include "mbsl1.h"
#include "mo.h"
#include "to.h"
#include "ota.h"
#include "AS0.h"
#include "string.h"
#include "v1.h"
#include "MB_TIMER.h"
#include "nt.h"
#include "sn.h"
#include "dg.h"
#define WATCHDOG
#ifndef WATCHDOG
  #warning PUT THE WATCHDOG BACK PUT THE WATCHDOG BACK PUT THE WATCHDOG BACK PUT THE WATCHDOG BACK
#endif

#define V1_LVD_VTG_BULD_SEC_TIMEOUT 30

UINT8 rsr_reg_val;
volatile UINT8 spi_busy_flag;
char mac_id_as_fnet_str[MAC_ID_NUM_LEN*2];
UINT32 st_sec_sys_up_timer;

static void st_enable_watchdog(void);
static void st_disable_watchdog(void);
fnet_ip6_addr_t ipv6_linklocal_add =	{0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x00,0x00,0x00};
fnet_mac_addr_t macaddr = {0x00,0x11,0x22,0x33,0x44,0x55};//mac address
/*****************************************************************************
*****************************************************************************/
void st_main(void)
{
	rsr_reg_val = RSR;
	/************************************************************************************/
	SPI_ENTER_CRITICAL();
	/************************************************************************************/
	#ifndef WATCHDOG
	st_disable_watchdog();
	#endif
	st_enable_watchdog();
	st_kick_watchdog();                          // Reload Watchdog
	startup_flag = CLR_STARTUP_FLAG;
	#ifdef USE_SSPI
	ss_init_intermicro();
	#endif
	st_init_timer();
	st_arm_sec_timer(LVD_SPI_CMD_CNTR_SEC_TIMER,300);
	//pt_putchar('0');
	st_spi_init();
	/* Do not change the init sequence below. */
	/****************************************************************/
	fl_init();
	i2c_init();
	ee_init();
	xf_init();
	st_kick_watchdog();                          // Reload Watchdog
	//pt_putchar('1');
	st_load_factory_config();
	st_kick_watchdog();                          // Reload Watchdog
	//pt_putchar('2');
	ee_mngr_init();
	ee_validate_data();
	st_init_clock();
	st_kick_watchdog();                          // Reload Watchdog
	//pt_putchar('3');
	bc_init();
	
	st_kick_watchdog();                          // Reload Watchdog
	/****************************************************************/
	//pt_putchar('4');
	pm_library_init();                             // Port Library Initialization
	an_init_adc();
	ld_init();
	ld_led_init();
	pl_init();
	pl_init_rcl();
	pt_init();
	re_init();
	bt_init_reserve();
	bt_init_soc();
	gp_init();
	pt_comm_init(PT_57600, 0);

	st_ms_delay(200);                              // the clock chip requires this
	ck_drvr_init();
	st_kick_watchdog();                          // Reload Watchdog
	//pt_putchar('5');
	al_init_alarms();
	bd_init();
	bd_init_trace();
	boost_init();
	dt_init();
	st_kick_watchdog();                          // Reload Watchdog
	//pt_putchar('6');
	sc_init();
	sc_init_usage();
	st_kick_watchdog();                          // Reload Watchdog
	//pt_putchar('7');
	di_drvr_init();
	di_canvas_init();
	di_fill_image(0x00);
	di_clear();
	ac_mngr_init();
	st_kick_watchdog();                          // Reload Watchdog
	//pt_putchar('8');
	st_kick_watchdog();                          // Reload Watchdog
	//pt_putchar('9');
	v1_com_init();
	nt_init();
	mb_ms1_init();                                 // Modbus Master Application1 Initalization
	mb_rm1_init();                                 // Modbus Remote Master Application1 Initalization
	stffs_init();
	eq_init();
	pms_init();
	pt_puts(gl_product_str);
	st_ms_delay(200);                              // delay to display to product string.
	cnfg_reset = 0;                                // this used to signal all modules only during a reset.
	dcdc_dataCheck();
	/************************************************************************************/
	SPI_EXIT_CRITICAL();
	/************************************************************************************/
	//  Shifted here to give proper power up time
	lv_init_lvd_states();
	st_arm_sec_timer(V1_LVD_VTG_BULD_TIMER,V1_LVD_VTG_BULD_SEC_TIMEOUT);
	
	// System Up Event is handled only here.
	al_set_alarm(AL_SYST);
	al_clear_alarm_active_bitfield(AL_SYST);
	clear_data(); 
	
	if(cnfg.bt_type[LITHIUM_TYPE]==BT_MB_PAN_JBOX || 
	   cnfg.bt_type[LITHIUM_TYPE]==BT_MB_PAN_DCB)
	{
		if(cnfg.bt_type[LITHIUM_TYPE]==BT_MB_PAN_JBOX)
			bt_set_string_cap(52,LITHIUM_TYPE);
		else
			bt_set_string_cap(56,LITHIUM_TYPE);
		pl_set_rcl_amps(&cnfg.pl_rcl[LITHIUM_TYPE],10,LITHIUM_TYPE);//25 Amps for 100Ah
	}
	else if(cnfg.bt_type[LITHIUM_TYPE] == BT_COSLIGHT_100 || 
					cnfg.bt_type[LITHIUM_TYPE] == BT_MB_VISION_100|| 
					cnfg.bt_type[LITHIUM_TYPE] == BT_MB_EXICOM_100|| 
					cnfg.bt_type[LITHIUM_TYPE] == BT_MB_SAFT_100)
	{
		bt_set_string_cap(DFLT_CAP_100AH,LITHIUM_TYPE);
		pl_set_rcl_amps(&cnfg.pl_rcl[LITHIUM_TYPE],DFLT_CLT_100AH,LITHIUM_TYPE);//25 Amps for 100Ah
	}
	else if(cnfg.bt_type[LITHIUM_TYPE] == BT_MB_SAFT)
	{
		bt_set_string_cap(77,LITHIUM_TYPE);
		pl_set_rcl_amps(&cnfg.pl_rcl[LITHIUM_TYPE],18,LITHIUM_TYPE);//25 Amps for 100Ah
	}
	else
	{
		bt_set_string_cap(75,LITHIUM_TYPE);
		pl_set_rcl_amps(&cnfg.pl_rcl[LITHIUM_TYPE],18,LITHIUM_TYPE);//25 Amps for 100Ah
	}
  st_arm_sec_timer(CONTROLLER_RESET_SEC_TIMER, 10*60);

  fnet_str_to_mac( mac_id_as_fnet_str, macaddr);  
  fnet_memcpy(&(ipv6_linklocal_add.addr[8]), macaddr,  3u);
  fnet_memcpy(&(ipv6_linklocal_add.addr[13]), &macaddr[3],  3u);
  ipv6_linklocal_add.addr[8] ^= 0x02u;
  st_arm_sec_timer(REC_STNDBY_SEC_TIMER,60);
  st_arm_sec_timer(OPTIBRTV_SEC_TIMER,30);
	st_arm_sec_timer(OPTIBRPV_SEC_TIMER,30);
  //cnfg.megmeet_rectifier =1;
  //pt_puts("10:43");
  while (1)
  {
   	i2c_handler(); 
    ee_handler(); 
    fl_handler(); 
    st_kick_watchdog(); 
    an_adc_handler(); 
    st_get_ck_update();
    nt_app_handler();
    stffs_handler();
    stffs_bc_gal_handler();
    xf_handle_chip();
		if(v1_handle_f == 0)
    {
    	v1_handle_f = 1;
    	v1_handle_chip();
    	v1_handle_f = 0;
    }
    //ST_ROUND_ROBIN_FACTORY();
    pt_manager();
    pt_ui_manager();
    if(v1_handle_f == 0)
    {
    	v1_handle_f = 1;
    	v1_handle_chip();
    	v1_handle_f = 0;
    }    
    gp_message_handler();   // This needs to be here to handle GP debug commands (like "gpl"
    if(v1_handle_f == 0)
    {
    	v1_handle_f = 1;
    	v1_handle_chip();
    	v1_handle_f = 0;
    }
    v1_reg_req_handler();

    if (pt_login_type != PT_FACTORY_TEST_TYPE)
    {
      //ST_ROUND_ROBIN_APP();
      ee_manager();
      ac_manager();
      bd_manager();
      bd_trace_discharge();
      bo_handler();
      bt_reserve_handler();
      ld_led_handler();
      al_handle_buzzer();
      al_contacts_handler();
      sc_handler();
      sc_ee_handler();
      bc_handler();
      al_fl_handler();
      ld_handler();
      al_scan();
      pms_handler();
			eq_handler();
			dt_discharge_test_handler();
			bt_soc_handler(sel_batt_type);
			bt_soc_lihandler();

	  	if((cnfg.bt_type[LITHIUM_TYPE] < BT_COSLIGHT) && (sel_batt_type == VRLA_TYPE))
				read_batt = VRLA_TYPE;
	  	else
	  		read_batt = LITHIUM_TYPE;
      pl_rcl_handler();         				/* Lithium Ion Handler */
			pl_voltage_handler();
      re_alarm_handler();
      re_restart_handler(RESTART_NORMAL_ACTION);
      mb_ms1_handler();
      coslight_com_handler();
     	mb_rm1_handler();
      lvd_handler();
      stffs_his_handler();// sagar fills data	
      stffs_sum_handler();// sagar fills data	
      flexcan_handler();
      //state_machine();
    }
  }
    //-HEARTBEAT_TOGGLE();
}

/******************************************************************************
1 Second Update provided by the RTC 1 second interrupt.
******************************************************************************/
void st_get_ck_update(void)
{
  static UINT8 minutes;
  extern UINT8 ck_1hz_interrupt_occurred;
  if (ck_1hz_interrupt_occurred)
  {
    ck_1hz_interrupt_occurred = 0;
    st_sec_sys_up_timer++;

    if (ck_read_status() & CK_BATT_LOW_BIT) // clear alarm, read batt low
      al_set_alarm(AL_CLK_BATT_LOW);
    else
      al_clear_alarm(AL_CLK_BATT_LOW);
    ck_read_date_time();      // update date/time
    st_update_sec_min_timers(); // update second/minute timers
    bd_update_sec_min_timers();
    if (minutes != ck_time.minutes)
    {
      minutes = ck_time.minutes;
      sc_minute_roll_over = 1;
    }
  }
}

/******************************************************************************
******************************************************************************/
void st_spi_init(void)
{
  // make the QSPI port pins do GPIO - SPI (mix - match)
  PQSPAR = 0x1055;
  QMR = 0xa310;					// 8MHz
  setReg16(QDLYR, 0x4040U);
  setReg16(QWR, 0x1000U);
  setReg16(QIR, 0x0DU);
}

/******************************************************************************
******************************************************************************/
//#define SPI_NON_DSP_BAUD    0x04    // 64M/(2*baud) = 8MHz
//#define SPI_DSP_BAUD        0x28    // 64M/(2*baud) = 0.8MHz
UINT8 st_spi_write(UINT8 b, UINT16 cs)
{
  QIR = 0xd00f;     // reset done status
 	QAR = 0x20;       // point to command data
  QDR = (UINT16)(QCR_DSCK | QCR_DT | (~cs & QCR_CS_MASK));     // write cs pattern supplied to QCR0
	QAR = 0x0000;    // point to tx data
  QDR = b;         // send the data
  QWR = 0x1000;
  QDLYR |= QDLYR_SPE_BITMASK; // start
  while ((QIR & QIR_SPIF_BITMASK) == 0)
      ;
  QAR = 0x0010;    // point to rx data
  return (UINT8)(QDR & 0xff);
}


/******************************************************************************
******************************************************************************/
void warmboot(void)
{
  st_disable_all_interrupts();
  START_APP();
}


/******************************************************************************
This functions needs to be updated to disable every a new interrupt introduced
in the project.
******************************************************************************/
void st_disable_all_interrupts(void)
{
  an_disable_interrupt();
  TI1_Disable();
  i2c_disable_interrupt();
  #ifndef USE_SSPI
    pt_disable_interrupt();
  #endif
  gp_disable_interrupt();
  ck_disable_interrupt();
  mb_disable_interrupts();
  pm_disable_interrupts();
  AS0_Disable();
  st_disable_watchdog();
}

/******************************************************************************
******************************************************************************
void st_delay_us(UINT16 i)
{
  while(i--)
  {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
  }
}

******************************************************************************
******************************************************************************/
static void st_enable_watchdog(void)
{
  #ifdef WATCHDOG
    /* CWCR: CWE=0,CWRI=0,CWT=0,CWTA=0,CWTAVAL=1,CWTIF=1 */
    setReg8(CWCR, 0x03);
    /* CWSR: CWSR=0x55 */
    setReg8(CWSR, 0x55);
    /* CWSR: CWSR=0xAA */
    setReg8(CWSR, 0xAA);
    /* CWCR: CWE=1,CWRI=0,CWT=7,CWTA=0,CWTAVAL=0,CWTIF=0 */
    setReg8(CWCR, 0xB8);
  #endif
}
/******************************************************************************
******************************************************************************/
static void st_disable_watchdog(void)
{
  /* CWCR: CWE=0,CWRI=0,CWT=0,CWTA=0,CWTAVAL=1,CWTIF=1 */
  setReg8(CWCR, 0x03);
}

/******************************************************************************
******************************************************************************/
void st_kick_watchdog(void)
{
  #ifdef WATCHDOG
    setReg8(CWSR, 0x55);
    setReg8(CWSR, 0xAA);
  #endif
}
