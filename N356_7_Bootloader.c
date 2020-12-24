#define TSMP_IMPL
#include "TSMaster.h"
#include "MPLibrary.h"
#include "Database.h"
#include "Test.h"

// Timers defintions
TMPTimerMS Task;
TMPTimerMS tester_time;
TMPTimerMS tp_cf_dly;

// Function Prorotypes
s32 reflash_guide(void);
s32 can_msg_init(void);
s32 start_timers(void);
s32 variables_init(void);
s32 Notification_Function(void);
s32 Start_ReFlash_Execution(void);
s32 TP_Transmit(void);
s32 task_tester_req(void);
s32 task_tester_req_val(void);
s32 task_ext_sess_req(void);
s32 task_ext_sess_val(void);
s32 task_disable_faultcode_req(void);
s32 task_disable_faultcode_val(void);
s32 task_disable_normal_msg_tx_req(void);
s32 task_disable_normal_msg_tx_val(void);
s32 task_prg_sess_req(void);
s32 task_prg_sess_val(void);
s32 Security_Seed_Request(void);
s32 task_seed_req_val(void);
s32 fbl_key_Generate_Bytes(u8 aceessSeed[]);
s32 fbl_key_generate(void);
s32 Sec_Key_Validation(void);
s32 task_erase_req_rom(void);
s32 task_erase_req_val_rom(void);
s32 task_req_dwnload_rom(void);
s32 task_req_dwnload_rom_val(void);
s32 task_trsfr_data_rom(void);
s32 task_trsfr_data_rom_val(void);
s32 task_trsfr_exit_rom(void);
s32 task_trsfr_exit_rom_val(void);
s32 task_checksum_rom(void);
s32 task_checksum_rom_val(void);
s32 task_erase_req_hf1(void);
s32 task_erase_req_val_hf1(void);
s32 task_req_dwnload_hf1(void);
s32 task_req_dwnload_hf1_val(void);
s32 task_trsfr_data_hf1(void);
s32 task_trsfr_data_hf1_val(void);
s32 task_trsfr_exit_hf1(void);
s32 task_trsfr_exit_hf1_val(void);
s32 task_checksum_hf1(void);
s32 task_checksum_hf1_val(void);
s32 ChecksumRoutine(u8 chksum_start_add[], u32 length);
s32 Update_Checksum_Byte(u32 arg_fbl_calc_chksum);
s32 task_pwron_reset_req(void);
s32 task_pwron_reset_req_val(void);
s32 task_idle(void);

// Global definitions

typedef u8 byte;
typedef u16 word;
typedef u32 dword;

//global variables
u32 KEY_MASK = 0x8EC8BB8E;
u32 seedKey = 0;
u8 bitLoop = 0;

u8 SecureAccessSeed[4];
u8 seedkey_success = 0;

/*N356 Inch7 Program Specific - start*/
const ADDR_AND_LENGTH_SIZE_IN_BYTES = 0x44;
const NORMAL_BLOCK = 1;
const LAST_BLOCK = 2;
u8 ChecksumRoutine_Flag = NORMAL_BLOCK;
u8 CHECKSUM_AREA_SIZE = 4;
u8 Sec_Error_Count = 0;

u32 fbl_calc_chksum = 0;
const TRUE = 1;
const FALSE = 0;

u8 Task_State;
u8 diag_resp_rcvd;
u8 diag_resp_sf[8];

u8 tp_first = 0;
u8 tp_first_frame = 0;
u8 diag_req_buff[4095];
u8 chk_buff[4095];

//u32 rom_handle, hf1_handle, chk1;
FILE* rom_handle;
FILE* hf1_handle;
FILE* chk1;

int diag_req_len;
int i = 0;
int tp_data_len_txd;
u8 diag_req_cmnd;

const ROM_AND_HF = 1;
const ROM_ONLY = 2;
const HF_ONLY = 3;

/*Enter Idel Task when flashing is done*/
const IDLE = 1;

/*Request Tester Present and Validate tester present Response */
const TESTER_MSG_REQ = 2;
const TESTER_MSG_VAL = 3;

/*1*/ /*Request Extended Diagnostic Session and Validate Extended diagnostic session*/
const EXT_SESS_REQ = 4;
const EXT_SESS_VAL = 5;

/*2*/ /*Request Disable DTC and Validate Disable DTC Response*/
const DIS_FAULTCOD_REQ = 6;
const DIS_FAULTCOD_VAL = 7;
/*3*/ /*Request Disable Normal message and validate disable normal message response*/
const DIS_NRMSGTX_REQ = 8;
const DIS_NRMSGTX_VAL = 9;

/*4*/ /*Request Programming Diagnostic Session and Validate Programming diagnostic session*/
const PRG_SESS_REQ = 10;
const PRG_SESS_VAL = 11;

/*5*/ /*Request SEED & KEY and validate SEED and KEY response*/
const SEC_SEED_REQ = 12;
const SEC_SEED_VAL = 13;
const SEC_KEY_VAL = 14;

/*6.1*/ /*Erase Internal Application Flash*/
const MEM_ERASE_ROM_REQ = 16;
const MEM_ERASE_ROM_VAL = 17;
/*7.1*/ /*Request Download and validate Request Download Response*/
const REQ_DWNLOAD_ROM = 18;
const REQ_DWNLOAD_ROMVAL = 19;
/*8.1*/ /*Data Transfer and validate data transfer Response*/
const TRSFR_DATA_ROM = 20;
const TRSFR_DATA_ROMVAL = 21;
/*9.1*/ /*Transfer data exit and validate Transfer data exit Response*/
const TRSFR_EXIT_ROM = 22;
const TRSFR_EXIT_ROM_VAL = 23;
/*10.1*/ /*Request routine control and validate Routine control Response*/
const CHECKSUM_ROM = 24;
const CHECKSUM_ROM_VAL = 25;

/*6.2*/ /*Erase Internal Application Flash*/
const MEM_ERASE_HF1_REQ = 26;
const MEM_ERASE_HF1_VAL = 27;
/*7.2*/ /*Request Download and validate Request Download Response*/
const REQ_DWNLOAD_HF1 = 28;
const REQ_DWNLOAD_HF1VAL = 29;
/*8.2*/ /*Data Transfer and validate data transfer Response*/
const TRSFR_DATA_HF1 = 30;
const TRSFR_DATA_HF1VAL = 31;
/*9.2*/ /*Transfer data exit and validate Transfer data exit Response*/
const TRSFR_EXIT_HF1 = 32;
const TRSFR_EXIT_HF1_VAL = 33;
/*10.2*/ /*Request routine control and validate Routine control Response*/
const CHECKSUM_HF1 = 34;
const CHECKSUM_HF1_VAL = 35;

/*11*/ /*Request Power on reset and validate power on reset Response*/
const PON_RESET_REQ = 36;
const PON_RESET_VAL = 37;

u8 block_seq_counter = 0;
u32 app_size_counter = 0;
u16 app_size_counter_val = 0;
u16 app_size_counter_bal = 0;

u8 Flashing_Sequence_Type = 0;
u8 Flashing_Section_Type = 0;
u32 ROM_NO_OF_BLOCK                     = 0x00;
u32 ROM_NO_OF_BLOCK_FOR_VALIDATION      = 0x00; 
u16 ROM_NO_OF_REMAINING_BYTES           = 0x00;
u32 ROM_DATA_APP_SIZE_COUNTER_LIMIT     = 0x00;
u32 ROM_DATA_VAL_APP_SIZE_COUNTER_LIMIT = 0x00;

u32 HF1_NO_OF_BLOCK                     = 0x00;
u32 HF1_NO_OF_BLOCK_FOR_VALIDATION      = 0x00;
u16 HF1_NO_OF_REMAINING_BYTES           = 0x00;
u32 HF1_DATA_APP_SIZE_COUNTER_LIMIT     = 0x00;
u32 HF1_DATA_VAL_APP_SIZE_COUNTER_LIMIT = 0x00;

/*Configure the below for no of bytes in a block*/  
u16  NO_OF_BYTES_IN_A_BLOCK              = 0x100;

/*Configure the below 3 variable for ROM*/  
u32 ROM_FILE_START_ADDRESS              = 0x01A00000;
//u32 ROM_FILE_LENGTH                     = 0x1000;  
u32 ROM_FILE_LENGTH                     = 0x200000;
u8  ROM_COMPRESS_DATA                   = 0x00;  /*compress - 0x10   Non-Compress - 0x00*/
/*Configure the below 3 variable for HF1*/        
u32 HF1_FILE_START_ADDRESS              = 0x48000000;
u32 HF1_FILE_LENGTH                     = 0x1E27916;
u8  HF1_COMPRESS_DATA                   = 0x10;  /*compress - 0x10   Non-Compress - 0x00*/

/*Configure this value to test fail case of seed and key*/
/*Valid only for N356 Inch7 */ 
u8 NFFA_COUNT                           = 0x00; 

u8  ROM_File_Name[100]                  = "C:\\temp\\PROGRAM_COMPRESS_ONLY.BIN";
u8  HF1_File_Name[100]                  = "C:\\temp\\PROGRAM_COMPRESS_ONLY_CAPL.bin";  
			
//messages
TTester_DiagReqToIC_1 diag_req;
TTester_DiagFuncReq_1 tester_req;
TIC_DiagResp_1 diag_res;




// Main step function being executed every 5 ms
void step(void) { // interval = 5 ms

}

// On CAN message reception handler "On_0x738" for identifier = 0x738
void on_can_rx_On_0x738(const PCAN ACAN) { // for identifier = 0x738

	diag_resp_sf[0] = ACAN->FData[0];
	diag_resp_sf[1] = ACAN->FData[1];
	diag_resp_sf[2] = ACAN->FData[2];
	diag_resp_sf[3] = ACAN->FData[3];
	diag_resp_sf[4] = ACAN->FData[4];
	diag_resp_sf[5] = ACAN->FData[5];
	diag_resp_sf[6] = ACAN->FData[6];
	diag_resp_sf[7] = ACAN->FData[7];

	diag_resp_rcvd = TRUE;

	if((diag_resp_sf[0] == 0x30)&&(tp_first_frame == 1))
	{
		tp_cf_dly.stop();
		tp_cf_dly.start();
		tp_first = 1;
		tp_first_frame = 0;
	}
	if((diag_resp_sf[0] == 0x10) || (diag_resp_sf[0] == 0x11))
	{
		diag_req.FCAN.FData[0] = 0x30;
		diag_req.FCAN.FData[1] = 0x00;      
		diag_req.FCAN.FData[2] = 0x00;      
		diag_req.FCAN.FData[3] = 0x00;             
		diag_req.FCAN.FData[4] = 0x00;
		diag_req.FCAN.FData[5] = 0x00;
		diag_req.FCAN.FData[6] = 0x00;
		diag_req.FCAN.FData[7] = 0x00;

		com.transmit_can_async(&diag_req.FCAN);
	}
}

// Timer event handler "On_Task" for Timer Task
void on_timer_On_Task(void) { // timer = Task
//1ms 



	//Settimer(Task,0, 300*1000);

	/* Main Routine Start based on task state */
	switch(Task_State)
	{
		/*Enter Idel Task when flashing is done*/
		case IDLE:              task_idle();                      break; 
		/*Request Tester Present and Validate tester present Response */        
		case TESTER_MSG_REQ:    task_tester_req();                break;
		case TESTER_MSG_VAL:    task_tester_req_val();            break;

		/*1*//*Request Extended Diagnostic Session and Validate Extended diagnostic session*/
		case EXT_SESS_REQ:      task_ext_sess_req();              break;
		case EXT_SESS_VAL:      task_ext_sess_val();              break; 

		/*2*//*Request Disable DTC and Validate Disable DTC Response*/
		case DIS_FAULTCOD_REQ:  task_disable_faultcode_req();     break;
		case DIS_FAULTCOD_VAL:  task_disable_faultcode_val();     break; 

		/*3*//*Request Disable Normal message and validate disable normal message response*/
		case DIS_NRMSGTX_REQ:   task_disable_normal_msg_tx_req(); break;
		case DIS_NRMSGTX_VAL:   task_disable_normal_msg_tx_val(); break;  

		/*4*//*Request Programming Diagnostic Session and Validate Programming diagnostic session*/
		case PRG_SESS_REQ:      task_prg_sess_req();              break;
		case PRG_SESS_VAL:      task_prg_sess_val();              break;

		/*5*//*Request SEED & KEY and validate SEED and KEY response*/    
		case SEC_SEED_REQ:      Security_Seed_Request();          break;
		case SEC_SEED_VAL:      task_seed_req_val();              break;
		case SEC_KEY_VAL:       Sec_Key_Validation();             break;   

		/*6.1*//*Erase Internal Application Flash*/
		case MEM_ERASE_ROM_REQ: task_erase_req_rom();             break; 
		case MEM_ERASE_ROM_VAL: task_erase_req_val_rom();         break;
		/*7.1*//*Request Download and validate Request Download Response*/
		case REQ_DWNLOAD_ROM:   task_req_dwnload_rom ();          break;
		case REQ_DWNLOAD_ROMVAL:task_req_dwnload_rom_val();       break;
		/*8.1*//*Data Transfer and validate data transfer Response*/
		case TRSFR_DATA_ROM:    task_trsfr_data_rom();            break;
		case TRSFR_DATA_ROMVAL: task_trsfr_data_rom_val();        break;
		/*9.1*//*Transfer data exit and validate Transfer data exit Response*/
		case TRSFR_EXIT_ROM:    task_trsfr_exit_rom();            break;
		case TRSFR_EXIT_ROM_VAL:task_trsfr_exit_rom_val();        break; 
		/*10.1*//*Request routine control and validate Routine control Response*/
		case CHECKSUM_ROM:      task_checksum_rom();              break; 
		case CHECKSUM_ROM_VAL:  task_checksum_rom_val();          break;

		/*6.2*//*Erase Internal Application Flash*/
		case MEM_ERASE_HF1_REQ: task_erase_req_hf1();             break;
		case MEM_ERASE_HF1_VAL: task_erase_req_val_hf1();         break;
		/*7.2*//*Request Download and validate Request Download Response*/
		case REQ_DWNLOAD_HF1:   task_req_dwnload_hf1();           break;
		case REQ_DWNLOAD_HF1VAL:task_req_dwnload_hf1_val();       break;
		/*8.2*//*Data Transfer and validate data transfer Response*/
		case TRSFR_DATA_HF1:    task_trsfr_data_hf1();            break;
		case TRSFR_DATA_HF1VAL: task_trsfr_data_hf1_val();        break;
		/*9.2*//*Transfer data exit and validate Transfer data exit Response*/
		case TRSFR_EXIT_HF1:    task_trsfr_exit_hf1();            break;
		case TRSFR_EXIT_HF1_VAL:task_trsfr_exit_hf1_val();        break;
		/*10.2*//*Request routine control and validate Routine control Response*/
		case CHECKSUM_HF1:      task_checksum_hf1();              break;
		case CHECKSUM_HF1_VAL:  task_checksum_hf1_val();          break;

		/*11*//*Request Power on reset and validate power on reset Response*/
		case PON_RESET_REQ:     task_pwron_reset_req();           break;
		case PON_RESET_VAL:     task_pwron_reset_req_val();       break;

		/*13*//*Request Power on reset and validate power on reset Response*/    
		default:break;

	}
	
}

// Timer event handler "On_tester_time" for Timer tester_time
void on_timer_On_tester_time(void) { // timer = tester_time
//2000ms
	tester_req.FCAN.FData[0]=0x02;
	tester_req.FCAN.FData[1]=0x3E;
	tester_req.FCAN.FData[2]=0x02;
	//tester_req.dlc=0x08;
	tester_time.start();
}

// Timer event handler "On_tp_cf_dly" for Timer tp_cf_dly
void on_timer_On_tp_cf_dly(void) { // timer = tp_cf_dly
//1ms
	u8 i =0;

	if(tp_first == 1)
	{
		diag_req.FCAN.FData[0] = 0x21;
		tp_first = 2;
	}
	else
	{
		diag_req.FCAN.FData[0]++;
		diag_req.FCAN.FData[0] &= 0x2F;
	}

	if(diag_req_len > 7)
	{
		diag_req_len -= 7;
		for (i = 0; i < 7; i++)
		{
			diag_req.FCAN.FData[i+1] = diag_req_buff[tp_data_len_txd];
			tp_data_len_txd++;
		}
		com.transmit_can_async(&diag_req.FCAN);
		//settimer(tp_cf_dly,0, 300*1000);
		tp_cf_dly.start();
	}
	else
	{
		for (i = 0; i < diag_req_len; i++)
		{
			diag_req.FCAN.FData[i+1] = diag_req_buff[i+tp_data_len_txd];
		}
		for (i = diag_req_len; i < 10; i++)
		{
			diag_req.FCAN.FData[i+1] = 0x00;
		}

		tp_cf_dly.stop();
		diag_req_len = 0;
		com.transmit_can_async(&diag_req.FCAN);
	}
}

// On start handler "On_Start"
void on_start_On_Start(void) { // on start event
	/*******************************************************************************************
	********************************************************************************************
		Created Date : 2020-12-16
		Created From : jchen57
		
    Procedure to be followed
		========================
		Step 1 :: Start the TSMaster software 
		STEP 2 :: Press any Two key as mentioned below to do the respective operation
		Press Key 'P' to start from Programming Session
		|
		|
		|-->Press Key '1' to Erase and Re-Flash(Internal ROM Memory and Hyper Flash Memory )
		|-->Press Key '2' to Erase and Re-Flash(Internal ROM Memory )
		|-->Press Key '3' to Erase and Re-Flash(Hyper Flash Memory )
		  
		Press Key 'E' to start from Extended Diagnostic Session
		|
		|
		|-->Press Key '1' to Erase and Re-Flash(Internal ROM Memory and Hyper Flash Memory )
		|-->Press Key '2' to Erase and Re-Flash(Internal ROM Memory )
		|-->Press Key '3' to Erase and Re-Flash(Hyper Flash Memory )
		  
		Flashing Completed.
		
		To Restart the flasing again , Stop the TSMaster software if it is running. Then follow from STEP 1
		
	********************************************************************************************		
	********************************************************************************************/

	reflash_guide();
	variables_init();
	can_msg_init();
	start_timers();



}

// On stop handler "On_Stop"
void on_stop_On_Stop(void) { // on stop event
  fclose(rom_handle);
  fclose(hf1_handle);  
  fclose(chk1);
    
}

// On shortcut "AnyKey" with shortcut = ANY
void on_shortcut_AnyKey(const s32 AShortcut) { // on shortcut = ANY
  //(void)AShortcut;
  s32 key = AShortcut;
  
  switch (AShortcut)
  {
      case 'P':	
           printf("N356 Inch7 : Boot Mode Re-Flash Sequence Started From Programmming Session...");
           Flashing_Sequence_Type = 1;
           Notification_Function();
           break;
      case 'E':	
           printf("N356 Inch7 : App Mode  Re-Flash Sequence Started from Extended Diagnistic Session...");
           Flashing_Sequence_Type = 2;
           Notification_Function();
           break;
      case '1':	
           if(Flashing_Sequence_Type != FALSE)
           {
               Flashing_Section_Type = ROM_AND_HF;
               Start_ReFlash_Execution();
           }
           break;
      case '2':	
	         if(Flashing_Sequence_Type != FALSE)
	         {
	             Flashing_Section_Type = ROM_ONLY;
	             Start_ReFlash_Execution();
           }
           break;
      case '3':	
	         if(Flashing_Sequence_Type != FALSE)
	         {
		           Flashing_Section_Type = HF_ONLY;
	            Start_ReFlash_Execution();
           }
           break;    
      default:	
           printf("%c pressed\n", AShortcut);
           break;         
  }
}

// Custom Function "reflash_guide"
s32 reflash_guide(void) { // custom function

	printf("*********************************************\n");
	printf("N356 Inch7 CAN Re-Flash Tool\n");
	printf("Please select the Re-flash Options\n");
	printf("Press Either Key 'P' or 'E' For N356 Inch7 Re-flash Sequence to initiate\n");
	printf("Key 'P' or 'p' from Boot Mode / Programmming Session\n");
	printf("Key 'E' or 'e' from Application Mode / Extended Diagnistic Session\n");
	printf("*********************************************");

	return 0;
}

// Custom Function "can_msg_init"
s32 can_msg_init(void) { // custom function
	//CAN message init
	diag_req.init(); // execute this init function before use
	tester_req.init(); // execute this init function before use
	diag_res.init(); // execute this init function before use 

	return 0;
}

// Custom Function "start_timers"
s32 start_timers(void) { // custom function
	//timers start
	Task.start(); //Settimer(Task, 0,2000*1000);

	return 0;
}

// Custom Function "variables_init"
s32 variables_init(void) { // custom function

	ROM_NO_OF_BLOCK			          = (ROM_FILE_LENGTH/0x100);
	ROM_NO_OF_REMAINING_BYTES		  = (ROM_FILE_LENGTH%0x100);

	HF1_NO_OF_BLOCK			          = (HF1_FILE_LENGTH/0x100);
	HF1_NO_OF_REMAINING_BYTES		  = (HF1_FILE_LENGTH%0x100);

	ROM_NO_OF_BLOCK_FOR_VALIDATION = ROM_NO_OF_BLOCK;
	HF1_NO_OF_BLOCK_FOR_VALIDATION = HF1_NO_OF_BLOCK;


	if(ROM_NO_OF_REMAINING_BYTES != 0)
	ROM_NO_OF_BLOCK_FOR_VALIDATION += 1;
	if(HF1_NO_OF_REMAINING_BYTES != 0)
	HF1_NO_OF_BLOCK_FOR_VALIDATION += 1;

	Task_State      = IDLE;
	diag_resp_rcvd  = FALSE;

	return 0;
}

// Custom Function "Notification_Function"
s32 Notification_Function(void) { // custom function
	printf("Press Key '1' - Erase and ReProgram ROM(Internal Flash) and Hyper Flash(External Flash Memory)");
	printf("Press Key '2' - Erase and ReProgram ROM(Internal Flash) Only");
	printf("Press Key '3' - Erase and ReProgram Hyper Flash(External Flash Memory) Only");   

	return 0;
}

// Custom Function "Start_ReFlash_Execution"
s32 Start_ReFlash_Execution(void) { // custom function
	switch(Flashing_Sequence_Type)
	{
		case 1:Task_State = PRG_SESS_REQ;break;
		case 2:Task_State = EXT_SESS_REQ;break;
		default:break;
	}

	return 0;
}

// Custom Function "TP_Transmit"
s32 TP_Transmit(void) { // custom function
	u8 i;
	u8 tx_valid = TRUE;
	//diag_req.CAN = 1;
	//diag_req.DLC = 8;    

	if((diag_req_len < 8) && (diag_req_len > 0))
	{
		diag_req.FCAN.FData[0] = diag_req_len;
		for(i = 0; i < diag_req_len; i++)
		{
			diag_req.FCAN.FData[i+1] = diag_req_buff[i];
		}
		for(i = diag_req_len; i < 7; i++)
		{
			diag_req.FCAN.FData[i+1] = 0x00;
		}
	}
	else
	{
		if(diag_req_len > 4095)
		{ 
			tx_valid = FALSE;
		}
		else
		{
			diag_req.FCAN.FData[0] = (0x10 | (diag_req_len >> 8));
			diag_req.FCAN.FData[1] = (diag_req_len);
			tp_data_len_txd  = 0;
			diag_req.FCAN.FData[2] = diag_req_cmnd;

			tp_first_frame = 1;

			for(i = 0; i < 5; i++)
			{
				diag_req.FCAN.FData[i+3] = diag_req_buff[i];
				tp_data_len_txd++;
			}
			diag_req_len -= 6;
		}
	}

	diag_resp_rcvd = FALSE;

	if(tx_valid == TRUE)
	{   
		com.transmit_can_async(&diag_req.FCAN);
	}
	else
	{}



	return 0;
}

// Custom Function "task_tester_req"
s32 task_tester_req(void) { // custom function
	diag_req_buff[0] = 0x3E;
	diag_req_buff[1] = 0x00;

	diag_req_len = 2;

	TP_Transmit(); 
	Task_State = TESTER_MSG_VAL;
	printf("ECU: Tester Present : Sent..");
 
	return 0;
}

// Custom Function "task_tester_req_val"
s32 task_tester_req_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x7E) && (diag_resp_sf[2] == 0x00))
	{
		printf("ECU: Tester Present : Ok");
		Task_State = PRG_SESS_REQ;
	}
 
 	return 0;
}

// Custom Function "task_ext_sess_req"
s32 task_ext_sess_req(void) { // custom function
	diag_req_buff[0] = 0x10;
	diag_req_buff[1] = 0x03;

	diag_req_len = 2;

	TP_Transmit();

	Task_State = EXT_SESS_VAL;

	printf("ECU: Extended Session Request : Sent..");  
 
 
 	return 0;
}

// Custom Function "task_ext_sess_val"
s32 task_ext_sess_val(void) { // custom function
	if((diag_resp_rcvd==TRUE) && (diag_resp_sf[1] == 0x50) && (diag_resp_sf[2] == 0x03))
	{
		printf("ECU: Extended Session Request : Ok");
		Task_State = DIS_FAULTCOD_REQ;
	}
 
 	return 0;
}

// Custom Function "task_disable_faultcode_req"
s32 task_disable_faultcode_req(void) { // custom function
	diag_req_buff[0] = 0x85;
	diag_req_buff[1] = 0x02;
	  
	diag_req_len = 2;       

	TP_Transmit();

	Task_State = DIS_FAULTCOD_VAL;           

	printf("ECU: Disable Fault Code Request : Sent..");
	printf("ECU: Disable Fault Code Request : Ok");
 
 	return 0;
}

// Custom Function "task_disable_faultcode_val"
s32 task_disable_faultcode_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0xC5))
	{
		printf("ECU: Disable Fault Code Request : Ok");
		Task_State = DIS_NRMSGTX_REQ;
	}
 
 	return 0;
}

// Custom Function "task_disable_normal_msg_tx_req"
s32 task_disable_normal_msg_tx_req(void) { // custom function
	diag_req_buff[0] = 0x28;
	diag_req_buff[1] = 0x03;
	diag_req_buff[2] = 0x01;

	diag_req_len = 3; 

	TP_Transmit();

	Task_State = DIS_NRMSGTX_VAL; 

	printf("ECU: Disable Normal Msg Transmission Request : Sent..");
	printf("ECU: Disable Normal Msg Transmission Request : Ok");
	
	return 0;
}

// Custom Function "task_disable_normal_msg_tx_val"
s32 task_disable_normal_msg_tx_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x68))
	{
		printf("ECU: Disable Normal Msg Transmission Request : Ok");
		Task_State = PRG_SESS_REQ;
	}
	
	return 0;
}

// Custom Function "task_prg_sess_req"
s32 task_prg_sess_req(void) { // custom function
	diag_req_buff[0] = 0x10;
	diag_req_buff[1] = 0x02;

	diag_req_len = 2;

	TP_Transmit();

	Task_State = PRG_SESS_VAL;

	printf("ECU: Programming Session Request : Sent..");
	
	return 0;
}

// Custom Function "task_prg_sess_val"
s32 task_prg_sess_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x50) && (diag_resp_sf[2] == 0x02)) 
	{
		printf("ECU: Programming Session Request : Ok");
		Task_State = SEC_SEED_REQ;
	}
	
	return 0;
}

// Custom Function "Security_Seed_Request"
s32 Security_Seed_Request(void) { // custom function
	Sec_Error_Count++;  

	diag_req_buff[0] = 0x27;
	diag_req_buff[1] = 0x09;
	diag_req_len = 2;
	TP_Transmit();  
	printf("ECU: Security Seed Request : Sent..");


	Task_State = SEC_SEED_VAL;
	
	return 0;
}

// Custom Function "task_seed_req_val"
s32 task_seed_req_val(void) { // custom function

	if(diag_resp_rcvd == TRUE)
	{
		if(diag_resp_sf[0] == 0x06)
		{
			if(diag_resp_sf[1] == 0x67)
			{
				if(diag_resp_sf[2] == 0x09)
				{
					printf("ECU: Security Seed Request : Ok");
					SecureAccessSeed[0]=diag_resp_sf[3];
					SecureAccessSeed[1]=diag_resp_sf[4];
					SecureAccessSeed[2]=diag_resp_sf[5];
					SecureAccessSeed[3]=diag_resp_sf[6];
					fbl_key_Generate_Bytes(SecureAccessSeed);
					fbl_key_generate();                     
				}
			}
		}
		else if(diag_resp_sf[0] == 0x03)
		{
			if(diag_resp_sf[1] == 0x7F)
			{
				/*If Failed then , send the SEED request again*/  
				Task_State = SEC_SEED_REQ;
			}
		}
	}
 
  return 0;
}

// Custom Function "fbl_key_Generate_Bytes"
s32 fbl_key_Generate_Bytes(u8 aceessSeed[]) { // custom function
	seedKey = (aceessSeed[0] << 24) | (aceessSeed[1] << 16) | (aceessSeed[2] << 8) | aceessSeed[3];
	
	for(bitLoop = 0; bitLoop < 35; bitLoop++)
	{
		if(seedKey & 0x80000000)
		{
			seedKey = seedKey << 1;
			seedKey = seedKey ^ KEY_MASK;
		}
		else
		{
			seedKey = seedKey << 1;
		}
	}	
	diag_req_buff[2] = (u8)(seedKey >> 24);  
	diag_req_buff[3] = (u8)(seedKey >> 16);  
	diag_req_buff[4] = (u8)(seedKey >> 8);  
	diag_req_buff[5] = (u8)(seedKey >> 0);  
	
	return 0;
}

// Custom Function "fbl_key_generate"
s32 fbl_key_generate(void) { // custom function
	if(Sec_Error_Count <= NFFA_COUNT)  
	{
		diag_req_buff[2] = (u8)Sec_Error_Count; 
	}
	printf("ECU: Security Key Validation: Sent.....");
	diag_req_buff[0] = 0x27; 
	diag_req_buff[1] = 0x0A;;
	diag_req_len = 6;
	TP_Transmit();  

	Task_State = SEC_KEY_VAL;
	
	return 0;
}

// Custom Function "Sec_Key_Validation"
s32 Sec_Key_Validation(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x67) && (diag_resp_sf[2] == 0x0A))
	{
		printf("ECU: Security Validation: Ok");
		seedkey_success =   TRUE;

		if(Flashing_Section_Type != HF_ONLY)
			Task_State      =   MEM_ERASE_ROM_REQ;
		else
			Task_State      =   MEM_ERASE_HF1_REQ;
	}	
	else if(diag_resp_sf[1]==0x7F)
	{
		printf("Mismatch in calculated key.. Trying to Resend");


		/*If Failed then , send the SEED request again*/
		Task_State = SEC_SEED_REQ; 
	}
	
	return 0;
}

// Custom Function "task_erase_req_rom"
s32 task_erase_req_rom(void) { // custom function
	/*diag_req_cmnd     = 0x31;*/
	diag_req_buff[0]  = 0x31;
	diag_req_buff[1]  = 0x01;
	diag_req_buff[2]  = 0xFF;
	diag_req_buff[3]  = 0x00;
	diag_req_buff[4]  = 0x01;

	diag_req_len = 5;     
	TP_Transmit();
	printf("ECU: ROM Erase Request : Sent..");

	Task_State = MEM_ERASE_ROM_VAL;
	
	return 0;
}

// Custom Function "task_erase_req_val_rom"
s32 task_erase_req_val_rom(void) { // custom function
	if(
		(diag_resp_rcvd == TRUE)&&
		(diag_resp_sf[1] == 0x71)&&
		(diag_resp_sf[2] == 0x01)&&
		(diag_resp_sf[3] == 0xFF)&&
		(diag_resp_sf[4] == 0x00)&&
		(diag_resp_sf[5] == 0x00)&&
		(diag_resp_sf[6] == 0x01)
		)
	{
		printf("ROM Memory Erased Successfully");
		Task_State = REQ_DWNLOAD_ROM;
	}
	
	return 0;
}

// Custom Function "task_req_dwnload_rom"
s32 task_req_dwnload_rom(void) { // custom function
	diag_req_cmnd    = 0x34;
	diag_req_buff[0] = ROM_COMPRESS_DATA;             // 0x00
	diag_req_buff[1] = ADDR_AND_LENGTH_SIZE_IN_BYTES; // 0x44 

	diag_req_buff[2] = (byte)(ROM_FILE_START_ADDRESS >> 24);
	diag_req_buff[3] = (byte)(ROM_FILE_START_ADDRESS >> 16);
	diag_req_buff[4] = (byte)(ROM_FILE_START_ADDRESS >> 8);
	diag_req_buff[5] = (byte)(ROM_FILE_START_ADDRESS >> 0);

	diag_req_buff[6] = (byte)(ROM_FILE_LENGTH >> 24);
	diag_req_buff[7] = (byte)(ROM_FILE_LENGTH >> 16);
	diag_req_buff[8] = (byte)(ROM_FILE_LENGTH >> 8);
	diag_req_buff[9] = (byte)(ROM_FILE_LENGTH >> 0);

	diag_req_len = 11;
	TP_Transmit(); 
	printf("ECU: Request Download Application : Sent..");


	Task_State = REQ_DWNLOAD_ROMVAL;
	
	return 0;
}

// Custom Function "task_req_dwnload_rom_val"
s32 task_req_dwnload_rom_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x74))
	{
		printf("ECU: Request Download Application : Ok");
		app_size_counter = 0;
		rom_handle = fopen(ROM_File_Name, "rb");
		printf("ECU: Transfer Data Application : Sending..");


		Task_State = TRSFR_DATA_ROM ;

		/*TBR*/
		printf("ROM_NO_OF_BLOCK: %lX",ROM_NO_OF_BLOCK);
		printf("NO_OF_BYTES_IN_A_BLOCK: %lX",NO_OF_BYTES_IN_A_BLOCK);
		printf("ROM_NO_OF_REMAINING_BYTES: %lX",ROM_NO_OF_REMAINING_BYTES);
	}
 
	return 0;
}

// Custom Function "task_trsfr_data_rom"
s32 task_trsfr_data_rom(void) { // custom function
	diag_req_cmnd    = 0x36;

	if(rom_handle == 0)
	{
		//WriteTextColor(1,250,0,0);
		printf(">>FileName<<.bin: File Open Error!!!");

		/*incase any problem in opening the file then, go to idle task*/
		Task_State = IDLE;
	}
	else
	{ 
		if(app_size_counter < ROM_NO_OF_BLOCK)
		{
			fread(chk_buff, NO_OF_BYTES_IN_A_BLOCK, 1, rom_handle);/*Read the data from binary file and upadte it in chk_buff arrray*/

			block_seq_counter++;
			diag_req_buff[0]  =   block_seq_counter;
			for(i=0; i < NO_OF_BYTES_IN_A_BLOCK; i++)/*Copy the data in to TP buffer which needs to be transmitted*/
			{
				diag_req_buff[i+1] =  chk_buff[i];   
			}
			diag_req_len = (NO_OF_BYTES_IN_A_BLOCK+2);

			if( (ROM_NO_OF_REMAINING_BYTES == 0) && (app_size_counter == (ROM_NO_OF_BLOCK-1)) )
			{
				/*TBR*/
				printf("fbl_calc_chksum %lX",fbl_calc_chksum);
			}

			/*do additive calculation for the current block of data except last 4 byte and update the result in last 4 byte*/
			ChecksumRoutine(chk_buff,NO_OF_BYTES_IN_A_BLOCK);
			/*Do 2's Compliment by inverting it and add value 1 to it*/
			//fbl_calc_chksum = ((~fbl_calc_chksum)+1);
			/*TBR*/
			printf("Final check code value: %lX",fbl_calc_chksum);
		}
		else
		{
			printf("Entered Else");
			fread(chk_buff, ROM_NO_OF_REMAINING_BYTES, 1, rom_handle);

			block_seq_counter++;
			diag_req_buff[0]  =   block_seq_counter;
			for(i=0; i<ROM_NO_OF_REMAINING_BYTES; i++)
			{
				diag_req_buff[i+1] =  chk_buff[i];
			}  
			diag_req_len = (ROM_NO_OF_REMAINING_BYTES+2);

			ChecksumRoutine(chk_buff, ROM_NO_OF_REMAINING_BYTES);
			//fbl_calc_chksum = ((~fbl_calc_chksum)+1);   
			/*Update_Checksum_Byte(fbl_calc_chksum);*/

			printf("Final check code value: %lX",fbl_calc_chksum);
		}   
		TP_Transmit(); 

		Task_State = TRSFR_DATA_ROMVAL;         
	} 
 
	return 0;
}

// Custom Function "task_trsfr_data_rom_val"
s32 task_trsfr_data_rom_val(void) { // custom function

	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x76))
	{
		diag_resp_rcvd  = 0;        
		diag_resp_sf[1] = 0;
		diag_resp_sf[2] = 0;

		app_size_counter +=1;

		if(app_size_counter < ROM_NO_OF_BLOCK_FOR_VALIDATION) 
		{
			printf("ECU: Sending Application Block: %d",app_size_counter);

			/*If transfer data is not completed fully then send the transfer data again*/
			Task_State = TRSFR_DATA_ROM;
		}
		else
		{
			fread(chk_buff, 4, 1, rom_handle);
			printf("ECU: Transfer Data Application : Ok"); 
			/*fbl_calc_chksum = 0;*/

			/*If transfer data is successfully completed then move to the transfer Exit*/
			Task_State = TRSFR_EXIT_ROM;
			block_seq_counter = 0;
		}
	}
 
	return 0;
}

// Custom Function "task_trsfr_exit_rom"
s32 task_trsfr_exit_rom(void) { // custom function
	diag_req_buff[0] = 0x37;
	diag_req_buff[1] = 0x00;
	diag_req_buff[2] = 0x00;

	diag_req_len = 1;
			
	TP_Transmit(); 
	printf("ECU: Transfer Exit Application : Sent..");


	Task_State = TRSFR_EXIT_ROM_VAL;

	return 0;
}

// Custom Function "task_trsfr_exit_rom_val"
s32 task_trsfr_exit_rom_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x77))
	{
		printf("ECU: Transfer Exit Application : Ok ");


		Task_State = CHECKSUM_ROM;
	} 
 
  return 0;
}

// Custom Function "task_checksum_rom"
s32 task_checksum_rom(void) { // custom function
	diag_req_cmnd    = 0x31;
		
	diag_req_buff[0] = 0x01;
	diag_req_buff[1] = 0xFF;  
	diag_req_buff[2] = 0x01;
	diag_req_buff[3] = 0x01;
	diag_req_buff[4] = (u8)( (fbl_calc_chksum & 0xFF000000) >> 24 );
	diag_req_buff[5] = (u8)( (fbl_calc_chksum & 0x00FF0000) >> 16 );
	diag_req_buff[6] = (u8)( (fbl_calc_chksum & 0x0000FF00) >> 8 );
	diag_req_buff[7] = (u8)( (fbl_calc_chksum & 0x000000FF) >> 0 );

	fbl_calc_chksum   =   0;

	diag_req_len = 9;
	TP_Transmit(); 
	printf("ECU: Checksum for Application : Sent..");


	Task_State = CHECKSUM_ROM_VAL; 
	
	return 0;
}

// Custom Function "task_checksum_rom_val"
s32 task_checksum_rom_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x71) && (diag_resp_sf[2] == 0x01) &&
	(diag_resp_sf[3] == 0xFF) && (diag_resp_sf[4] == 0x01) && (diag_resp_sf[5] == 0x00) && (diag_resp_sf[6] == 0x01))
	{ 
		printf("ECU: Checksum for Application : Ok");


		if(Flashing_Section_Type != ROM_ONLY)
			Task_State      =   MEM_ERASE_HF1_REQ;
		else
			Task_State      =   PON_RESET_REQ;
	}
	else if( (diag_resp_sf[1] == 0x7F) && (diag_resp_sf[3] != 0x78) )  
	{
		printf("ECU: Checksum for Application Failed: Ok");


		/*incase any problem in opening the file then, go to idle task*/
		Task_State = IDLE;
	}

	fbl_calc_chksum   =   0;
 
 return 0;
}

// Custom Function "task_erase_req_hf1"
s32 task_erase_req_hf1(void) { // custom function
	/*diag_req_cmnd     = 0x31;*/
	diag_req_buff[0]  = 0x31;
	diag_req_buff[1]  = 0x01;
	diag_req_buff[2]  = 0xFF;
	diag_req_buff[3]  = 0x00;
	diag_req_buff[4]  = 0x02;

	diag_req_len = 5; 
	TP_Transmit();
	printf("ECU: Application Erase Request : Sent..");


	Task_State = MEM_ERASE_HF1_VAL;
 
 return 0;
}

// Custom Function "task_erase_req_val_hf1"
s32 task_erase_req_val_hf1(void) { // custom function
	if(
		(diag_resp_rcvd == TRUE)&&
		(diag_resp_sf[1] == 0x71)&&
		(diag_resp_sf[2] == 0x01)&&
		(diag_resp_sf[3] == 0xFF)&&
		(diag_resp_sf[4] == 0x00)&&
		(diag_resp_sf[5] == 0x00)&&
		(diag_resp_sf[6] == 0x02) 
		)
	{    
		printf("HF Memory Erased Successfully");


		Task_State = REQ_DWNLOAD_HF1;
	}
	
	return 0;
}

// Custom Function "task_req_dwnload_hf1"
s32 task_req_dwnload_hf1(void) { // custom function
	diag_req_cmnd    = 0x34;
	diag_req_buff[0] = HF1_COMPRESS_DATA;
	diag_req_buff[1] = ADDR_AND_LENGTH_SIZE_IN_BYTES;

	diag_req_buff[2] = (byte)(HF1_FILE_START_ADDRESS >> 24);
	diag_req_buff[3] = (byte)(HF1_FILE_START_ADDRESS >> 16);
	diag_req_buff[4] = (byte)(HF1_FILE_START_ADDRESS >> 8);
	diag_req_buff[5] = (byte)(HF1_FILE_START_ADDRESS >> 0);

	diag_req_buff[6] = (byte)(HF1_FILE_LENGTH >> 24);
	diag_req_buff[7] = (byte)(HF1_FILE_LENGTH >> 16);
	diag_req_buff[8] = (byte)(HF1_FILE_LENGTH >> 8);
	diag_req_buff[9] = (byte)(HF1_FILE_LENGTH >> 0); 

	diag_req_len = 11;

	TP_Transmit(); 
	printf("ECU: Request Download Map Image : Sent..");


	Task_State = REQ_DWNLOAD_HF1VAL;
	
	return 0;
}

// Custom Function "task_req_dwnload_hf1_val"
s32 task_req_dwnload_hf1_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x74))
	{
		printf("ECU: Request Download HF 1 Image : Ok");
		app_size_counter = 0;
		hf1_handle = fopen(HF1_File_Name, "rb");
		printf("ECU: Transfer Data Map Image : Sending..");


		Task_State = TRSFR_DATA_HF1;
	}
 
	return 0;
}

// Custom Function "task_trsfr_data_hf1"
s32 task_trsfr_data_hf1(void) { // custom function

	diag_req_cmnd    = 0x36;

	if(hf1_handle == 0)
	{
		//WriteTextColor(1,250,0,0);
		printf(">>FileName<<.bin: File Open Error!!!");


		/*incase any problem in opening the file then, go to idle task*/
		Task_State = IDLE;
	}
	else 
	{   
		if(app_size_counter < HF1_NO_OF_BLOCK)
		{
			fread(chk_buff, NO_OF_BYTES_IN_A_BLOCK, 1, hf1_handle);

			block_seq_counter++;
			diag_req_buff[0]  =   block_seq_counter;   
			for(i=0; i<NO_OF_BYTES_IN_A_BLOCK;i++)
			{
				diag_req_buff[i+1] =  chk_buff[i];  
			}
			diag_req_len = (NO_OF_BYTES_IN_A_BLOCK+2);
			ChecksumRoutine(chk_buff,NO_OF_BYTES_IN_A_BLOCK);    
			if( (HF1_NO_OF_REMAINING_BYTES == 0) && (app_size_counter == (HF1_NO_OF_BLOCK-1)) )
			{
				printf("Final check code value: %lX",fbl_calc_chksum);
			}
			printf("fbl_calc_chksum %lX",fbl_calc_chksum);
		}
		else
		{
			printf("Entered Else");
			fread(chk_buff, HF1_NO_OF_REMAINING_BYTES, 1, hf1_handle);

			block_seq_counter++;
			diag_req_buff[0]  =   block_seq_counter; 
			for(i=0; i<HF1_NO_OF_REMAINING_BYTES ;i++)
			{
				diag_req_buff[i+1] =  chk_buff[i];  
			}  
			diag_req_len = (HF1_NO_OF_REMAINING_BYTES+2); 
			ChecksumRoutine(chk_buff,HF1_NO_OF_REMAINING_BYTES);
			printf("Final check code value: %lX",fbl_calc_chksum);
		}
		TP_Transmit();  

		Task_State = TRSFR_DATA_HF1VAL;     
	}
	
	return 0;
}

// Custom Function "task_trsfr_data_hf1_val"
s32 task_trsfr_data_hf1_val(void) { // custom function

	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x76))
	{

		diag_resp_rcvd  = 0;        
		diag_resp_sf[1] = 0;    
		diag_resp_sf[2] = 0;

		app_size_counter +=1;

		if(app_size_counter < HF1_NO_OF_BLOCK_FOR_VALIDATION)
		{
			printf("ECU: Sending HF1 Block: %d",app_size_counter); 


			/*If transfer data is not completed fully then send the transfer data again*/
			Task_State = TRSFR_DATA_HF1; 
		} 
		else 
		{
			fread(chk_buff, 4, 1, hf1_handle);
			printf("ECU: Transfer HF data : Ok"); 
			//fbl_calc_chksum = 0;

			Task_State = TRSFR_EXIT_HF1; 
			block_seq_counter = 0;
		}
	}
	
	return 0;
}

// Custom Function "task_trsfr_exit_hf1"
s32 task_trsfr_exit_hf1(void) { // custom function
  diag_req_buff[0] = 0x37;

  diag_req_len = 1;
  TP_Transmit();
      
  printf("ECU: Transfer Exit HF1 data : Sent..");
  
  
  Task_State = TRSFR_EXIT_HF1_VAL;
  
  return 0;
}

// Custom Function "task_trsfr_exit_hf1_val"
s32 task_trsfr_exit_hf1_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x77))
	{
		printf("ECU: Transfer Exit HF1 data : Ok ");


		Task_State = CHECKSUM_HF1; 
	}
	
	return 0;
}

// Custom Function "task_checksum_hf1"
s32 task_checksum_hf1(void) { // custom function
	diag_req_cmnd    = 0x31;

	diag_req_buff[0] = 0x01;
	diag_req_buff[1] = 0xFF;  
	diag_req_buff[2] = 0x01;
	diag_req_buff[3] = 0x02;
	diag_req_buff[4] = (byte)( (fbl_calc_chksum & 0xFF000000) >> 24 );
	diag_req_buff[5] = (byte)( (fbl_calc_chksum & 0x00FF0000) >> 16 );
	diag_req_buff[6] = (byte)( (fbl_calc_chksum & 0x0000FF00) >> 8 );
	diag_req_buff[7] = (byte)( (fbl_calc_chksum & 0x000000FF) >> 0 );

	fbl_calc_chksum   =   0;

	diag_req_len = 9;

	TP_Transmit(); 
	printf("ECU: Checksum for HF : Sent..");  


	Task_State = CHECKSUM_HF1_VAL; 
	
	return 0;
}

// Custom Function "task_checksum_hf1_val"
s32 task_checksum_hf1_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x71) && (diag_resp_sf[2] == 0x01)&& (diag_resp_sf[3]==0xFF) &&
	(diag_resp_sf[4] == 0x01) && (diag_resp_sf[5] == 0x00) && (diag_resp_sf[6] == 0x02))
	{
		printf("ECU: Checksum for Application : Ok");

		Task_State = PON_RESET_REQ;
	}
	else if( (diag_resp_sf[1] == 0x7F) && (diag_resp_sf[3] != 0x78) )     
	{
		printf("For Temporary  ECU: Checksum for HF1 : Ok"); 


		Task_State = IDLE;
	} 
	
	return 0;
}

// Custom Function "ChecksumRoutine"
s32 ChecksumRoutine(u8 chksum_start_add[], u32 length) { // custom function
	u32 chksumptr;
	u32 pos; 

	for(pos=0; pos<length; pos += 4)
	{
		/*
		xyz Software binary will look like this
				LSB      MSB                        LSB      MSB
				 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
		Address   11 22 33 44 11 11 11 11 11 11 11 11 11 11 11 11
		Before calculating do the respective changes by shifting the position of bytes
		*/    

		chksumptr   = ( (u32)(chksum_start_add[pos+3]<<24) | (u32)(chksum_start_add[pos+2]<<16) | (u32)(chksum_start_add[pos+1]<<8) | (u32)(chksum_start_add[pos+0]<<0) );




		fbl_calc_chksum	 +=	chksumptr;
		/*TBR*/
		//write("fbl_calc_chksum %lX",fbl_calc_chksum);

	}
	
	return 0;
}

// Custom Function "Update_Checksum_Byte"
s32 Update_Checksum_Byte(u32 arg_fbl_calc_chksum) { // custom function
	diag_req_buff[i-3] = (u8)((arg_fbl_calc_chksum & 0x000000FF)>>0);
	diag_req_buff[i-2] = (u8)((arg_fbl_calc_chksum & 0x0000FF00)>>8);
	diag_req_buff[i-1] = (u8)((arg_fbl_calc_chksum & 0x00FF0000)>>16);
	diag_req_buff[i-0] = (u8)((arg_fbl_calc_chksum & 0xFF000000)>>24);
	
	return 0;
}

// Custom Function "task_pwron_reset_req"
s32 task_pwron_reset_req(void) { // custom function
	printf("ECU: Power-ON Reset Request : Sent");
	diag_req_buff[0] = 0x11;
	diag_req_buff[1] = 0x01;

	diag_req_len = 2;

	TP_Transmit();


	Task_State = PON_RESET_VAL;
	
	return 0;
}

// Custom Function "task_pwron_reset_req_val"
s32 task_pwron_reset_req_val(void) { // custom function
	if((diag_resp_rcvd == TRUE) && (diag_resp_sf[1] == 0x51) && (diag_resp_sf[2] == 0x01))
	{
		printf("ECU: Power-ON Reset Request : Ok");

		Task_State = IDLE;//ENA_NRMSGTX_REQ;
	}
	
	return 0;
}

// Custom Function "task_idle"
s32 task_idle(void) { // custom function

  return 0;
}

