#include "p18f8722.inc"
; CONFIG1H
  CONFIG  OSC = HSPLL, FCMEN = OFF, IESO = OFF
; CONFIG2L
  CONFIG  PWRT = OFF, BOREN = OFF, BORV = 3
; CONFIG2H
  CONFIG  WDT = OFF, WDTPS = 32768
; CONFIG3L
  CONFIG  MODE = MC, ADDRBW = ADDR20BIT, DATABW = DATA16BIT, WAIT = OFF
; CONFIG3H
  CONFIG  CCP2MX = PORTC, ECCPMX = PORTE, LPT1OSC = OFF, MCLRE = ON
; CONFIG4L
  CONFIG  STVREN = ON, LVP = OFF, BBSIZ = BB2K, XINST = OFF
; CONFIG5L
  CONFIG  CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF, CP4 = OFF, CP5 = OFF
  CONFIG  CP6 = OFF, CP7 = OFF
; CONFIG5H
  CONFIG  CPB = OFF, CPD = OFF
; CONFIG6L
  CONFIG  WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF, WRT4 = OFF
  CONFIG  WRT5 = OFF, WRT6 = OFF, WRT7 = OFF
; CONFIG6H
  CONFIG  WRTC = OFF, WRTB = OFF, WRTD = OFF
; CONFIG7L
  CONFIG  EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF, EBTR4 = OFF
  CONFIG  EBTR5 = OFF, EBTR6 = OFF, EBTR7 = OFF
; CONFIG7H
  CONFIG  EBTRB = OFF

;*******************************************************************************
; Variables & Constants
;*******************************************************************************
UDATA_ACS
  t1	res 1	; used in delay
  t2	res 1	; used in delay
  t3	res 1	; used in delay
  count res 1

  state res 1	; controlled by RB0 button
;*******************************************************************************
; Reset Vector
;*******************************************************************************

RES_VECT  CODE    0x0000            ; processor reset vector
    GOTO    START                   ; go to beginning of program

;*******************************************************************************
; MAIN PROGRAM
;*******************************************************************************

MAIN_PROG CODE	; let linker place main program

 
START
    CALL INIT	; initialize variables and ports
     

MAIN
    
BTFSC PORTA,4    ;RA4 BASILDI MI
    CALL WAIT_RA4
    
MOVLW 0FF
CPFSEQ state
    CALL STATE_2
    
MOVLW 00
CPFSEQ state    
    CALL STATE_1

CALL MAIN
    
    
STATE_1   
DECF count
CALL LIGHT  
RETURN
    
STATE_2
INCF count
CALL LIGHT     
RETURN  
    
LIGHT 
S12:
MOVLW 0C
CPFSEQ count   
    GOTO S11
CALL POS_1
    BTFSC PORTA,4 
    CALL WAIT_RA4
    MOVLW 0FF            
    CPFSEQ state
    CALL WAIT_RB5
    
RETURN
    
S11:
MOVLW 0B
CPFSEQ count
    GOTO S10
CALL POS_2    
RETURN
    
S10:
MOVLW 0A
CPFSEQ count   
    GOTO S9
CALL POS_3
BTFSC PORTA,4 
    CALL WAIT_RA4    
MOVLW 00            
CPFSEQ state
    CALL WAIT_RB5
RETURN
    
S9:
MOVLW 9
CPFSEQ count   
    GOTO S8
CALL POS_4
 BTFSC PORTA,4   
    CALL WAIT_RA4   
    MOVLW 0FF            
    CPFSEQ state
    CALL WAIT_RB5
RETURN
    
S8:
MOVLW 8
CPFSEQ count   
    GOTO S7
    CALL POS_5   
    RETURN  

S7:
   MOVLW 7
CPFSEQ count   
    GOTO S6
    CALL POS_6 
BTFSC PORTA,4   
    CALL WAIT_RA4    
    MOVLW 00            
    CPFSEQ state
    CALL WAIT_RB5
    RETURN

S6:
    MOVLW 6
CPFSEQ count   
    GOTO S5
    CALL POS_7 
 BTFSC PORTA,4  
    CALL WAIT_RA4   
    MOVLW 0FF            
    CPFSEQ state
    CALL WAIT_RB5
    RETURN

S5:
    MOVLW 5
CPFSEQ count   
    GOTO S4
    CALL POS_8      
    RETURN

S4:
    MOVLW 4
CPFSEQ count   
    GOTO S3
    CALL POS_9 
BTFSC PORTA,4 
    CALL WAIT_RA4    
    MOVLW 00            
    CPFSEQ state
    CALL WAIT_RB5
    RETURN

S3:
    MOVLW 3
CPFSEQ count   
    GOTO S2
    CALL POS_10   
 BTFSC PORTA,4 
    CALL WAIT_RA4   
    MOVLW 0FF            
    CPFSEQ state
    CALL WAIT_RB5
    RETURN

S2:
    MOVLW 2
CPFSEQ count   
    GOTO S1
    CALL POS_11      
    RETURN

S1:
    MOVLW 1
CPFSEQ count   
    GOTO S0
    CALL POS_12
BTFSC PORTA,4  
    CALL WAIT_RA4    
    MOVLW 00            
    CPFSEQ state
    CALL WAIT_RB5
    RETURN

S0:    
    MOVLW 0D
    CPFSEQ count   
    GOTO SET13
    GOTO SET0
    
SET0:
    MOVLW 1
    MOVWF count 
    GOTO S1
    
SET13:
    MOVLW 0C
    MOVWF count 
    GOTO S12 
RETURN

POS_1
MOVLW b'00000001'
MOVWF LATA    
MOVLW b'00000001' 
MOVWF LATB    
MOVLW b'00000000' 
MOVWF LATC    
MOVLW b'00000000' 
MOVWF LATD
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
    
POS_2
MOVLW b'00000000' 
MOVWF LATA    
MOVLW b'00000001' 
MOVWF LATB    
MOVLW b'00000001' 
MOVWF LATC    
MOVLW b'00000000' 
MOVWF LATD 
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
    
POS_3
MOVLW b'00000000' 
MOVWF LATA    
MOVLW b'00000000' 
MOVWF LATB    
MOVLW b'00000001' 
MOVWF LATC    
MOVLW b'00000001' 
MOVWF LATD  
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
    
POS_4
MOVLW b'00000000' 
MOVWF LATA    
MOVLW b'00000000' 
MOVWF LATB    
MOVLW b'00000000' 
MOVWF LATC    
MOVLW b'00000011' 
MOVWF LATD   
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
    
POS_5
MOVLW b'00000000' 
MOVWF LATA    
MOVLW b'00000000' 
MOVWF LATB    
MOVLW b'00000000' 
MOVWF LATC    
MOVLW b'00000110' 
MOVWF LATD   
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
    
POS_6
MOVLW b'00000000' 
MOVWF LATA    
MOVLW b'00000000' 
MOVWF LATB    
MOVLW b'00000000' 
MOVWF LATC    
MOVLW b'00001100' 
MOVWF LATD    
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
    
POS_7
MOVLW b'00000000' 
MOVWF LATA    
MOVLW b'00000000' 
MOVWF LATB    
MOVLW b'00001000' 
MOVWF LATC    
MOVLW b'00001000' 
MOVWF LATD    
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
    
POS_8
MOVLW b'00000000' 
MOVWF LATA    
MOVLW b'00001000' 
MOVWF LATB    
MOVLW b'00001000' 
MOVWF LATC    
MOVLW b'00000000' 
MOVWF LATD    
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
   
POS_9
MOVLW b'00001000' 
MOVWF LATA    
MOVLW b'00001000' 
MOVWF LATB    
MOVLW b'00000000' 
MOVWF LATC    
MOVLW b'00000000' 
MOVWF LATD    

MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
    
POS_10
MOVLW b'00001100' 
MOVWF LATA    
MOVLW b'00000000' 
MOVWF LATB    
MOVLW b'00000000' 
MOVWF LATC    
MOVLW b'00000000' 
MOVWF LATD    
  
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN

POS_11
MOVLW b'00000110' 
MOVWF LATA    
MOVLW b'00000000' 
MOVWF LATB    
MOVLW b'00000000' 
MOVWF LATC    
MOVLW b'00000000' 
MOVWF LATD    
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
    
POS_12
MOVLW b'00000011' 
MOVWF LATA    
MOVLW b'00000000' 
MOVWF LATB    
MOVLW b'00000000' 
MOVWF LATC    
MOVLW b'00000000' 
MOVWF LATD    
MOVLW 0FF
CPFSEQ state
    CALL DELAY_BW
    
MOVLW 000
CPFSEQ state
    CALL DELAY
RETURN
 

WAIT_RB5
BTFSC PORTA,4
    GOTO WAIT_RA4
BTFSS PORTB,5
    goto WAIT_RB5
    
_debounce1
    BTFSC PORTB,5
    goto _debounce1
    return

WAIT_RA4
BTFSC PORTA,4
    goto WAIT_RA4
    
    COMF state ;STATE DEGISTIR

RETURN     
 
    
DELAY	; Time Delay Routine with 3 nested loops
    MOVLW 61	; Copy desired value to W
    MOVWF t3	; Copy W into t3
    
    _loop3:
	MOVLW 0xA0  ; Copy desired value to W
	MOVWF t2    ; Copy W into t2
	_loop2:
	    MOVLW 0x9F	; Copy desired value to W
	    MOVWF t1	; Copy W into t1
	    _loop1:

		DECFSZ t1,F ; Decrement t1. If 0 Skip next instruction
		GOTO _loop1 ; ELSE Keep counting down
		DECFSZ t2,F ; Decrement t2. If 0 Skip next instruction
		GOTO _loop2 ; ELSE Keep counting down
		DECFSZ t3,F ; Decrement t3. If 0 Skip next instruction
		GOTO _loop3 ; ELSE Keep counting down
		RETURN

DELAY_BW	; Delay in State 2
    MOVLW 34	; Copy desired value to W
    MOVWF t3	; Copy W into t3
    
    _loop3s:
	MOVLW 0xA0  ; Copy desired value to W
	MOVWF t2    ; Copy W into t2
	_loop2s:
	    MOVLW 0x9F	; Copy desired value to W
	    MOVWF t1	; Copy W into t1
	    _loop1s:

		DECFSZ t1,F ; Decrement t1. If 0 Skip next instruction
		GOTO _loop1s ; ELSE Keep counting down
		DECFSZ t2,F ; Decrement t2. If 0 Skip next instruction
		GOTO _loop2s ; ELSE Keep counting down
		DECFSZ t3,F ; Decrement t3. If 0 Skip next instruction
		GOTO _loop3s ; ELSE Keep counting down
		RETURN
		
		
DELAY_ST	; Time Delay Routine with 3 nested loops
    MOVLW 82	; Copy desired value to W
    MOVWF t3	; Copy W into t3
    
    _loop3f:
	MOVLW 0xA0  ; Copy desired value to W
	MOVWF t2    ; Copy W into t2
	_loop2f:
	    MOVLW 0x9F	; Copy desired value to W
	    MOVWF t1	; Copy W into t1
	    _loop1f:

		DECFSZ t1,F ; Decrement t1. If 0 Skip next instruction
		GOTO _loop1f ; ELSE Keep counting down
		DECFSZ t2,F ; Decrement t2. If 0 Skip next instruction
		GOTO _loop2f ; ELSE Keep counting down
		DECFSZ t3,F ; Decrement t3. If 0 Skip next instruction
		GOTO _loop3f ; ELSE Keep counting down
		RETURN		
		
INIT
	
MOVLW d'1'
MOVWF 0x20
MOVLW d'2'
MOVWF 0x30
MOVLW d'11'
MOVWF 0x50
MOVF W,d'5',A

MOVLW 0Fh
MOVWF ADCON1

MOVLW b'11110000'
MOVWF TRISA
MOVWF TRISB

MOVLW b'00000000'
MOVWF TRISC
MOVWF TRISD
		
bsf LATC, 3, A
    
MOVLW b'00001111'
MOVWF LATA
MOVWF LATB
MOVWF LATC
MOVWF LATD
    
CALL DELAY_ST
CALL DELAY_ST
    
CLRF LATA
CLRF LATB
CLRF LATC
CLRF LATD

MOVLW 0FF
MOVWF state
		
MOVLW 0D
MOVWF count		
RETURN    
END