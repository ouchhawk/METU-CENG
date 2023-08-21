list P=18F8722

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

paddle_left equ 0x21
paddle_right equ 0x22
ball_vertical equ 0x23
temp equ 0x29
temp2 equ 0x31
temp3 equ 0x32
tpaddle_left equ 0x41
tpaddle_right equ 0x42
tball_vertical equ 0x43
ttemp equ 0x49
ttemp2 equ 0x41
ttemp3 equ 0x42
twreg equ 0x43
lscore equ 0x3A
rscore equ 0x3B
counter udata 0x24
counter
counter2 udata 0x3C
 counter2
 counter3 udata 0x3D
 counter3
w_temp  udata 0x23
w_temp
status_temp udata 0x25
status_temp
pclath_temp udata 0x26
pclath_temp
portb_var   udata 0x27
portb_var
state udata 0x28
state
direction equ 0x30
time res 0
time

UDATA_ACS
  t1	res 1	
  t2	res 1	
  t3	res 1   

ORG 0x00
GOTO MAIN

ORG 0x08
GOTO ISR
	
TABLE:
    RLNCF   WREG, W ; 
    ADDWF   PCL, F  ; 
    RETLW b'00111111' 
    RETLW b'00000110'
    RETLW b'01011011' 
    RETLW b'01001111' 
    RETLW b'01100110' 
    RETLW b'01101101' 
    
LEFTSCORES:
    movlw   b'00000010'
    movwf PORTH
    movlw   d'0'
    movwf PORTJ
    movff lscore, WREG
    call TABLE
    movwf PORTJ
    return
    
RIGHTSCORES:
    movlw   b'00001000'
    movwf PORTH
    movlw   d'0'
    movwf PORTJ
    movff rscore, WREG
    call TABLE
    movwf PORTJ
    return
    
    
MAIN:
 CALL INIT_PORTS
 CALL INIT_INTS
  
ROUND

 CALL CHECK_PADDLES
 GOTO ROUND

INIT_PORTS:

MOVLW 1C
MOVWF paddle_left
MOVWF paddle_right

MOVLW 3F
MOVWF ADCON1

CLRF LATA
CLRF LATB
CLRF LATC
CLRF LATD
CLRF LATE
CLRF LATF
CLRF LATG

CLRF INTCON
CLRF INTCON2
    
CLRF lscore
CLRF rscore
    
MOVLW 1F
MOVWF TRISG 

MOVLW 0
MOVWF TRISA
MOVWF TRISB
MOVWF TRISC
MOVWF TRISD
MOVWF TRISE
MOVWF TRISF
 
MOVLW 0
MOVWF TRISH
MOVWF TRISJ
    
START:

MOVLW 5
CPFSEQ lscore
GOTO C2
;RIGHT/P2 WINS
 goto END_
C2
MOVLW 5
CPFSEQ rscore
GOTO C3
;LEFT/P1 WINS   
 goto END_
C3
   
    
MOVLW b'00001000'
MOVWF LATD

LFSR FSR0, PORTD

MOVLW b'00001000'
MOVWF ball_vertical

MOVLW 0             ;set direction 0
MOVWF direction

CALL LIGHT_BALL

RETURN
    
INIT_INTS
    
BCF INTCON2,7  ;DISABLE PRIORITIES

MOVLW b'11000001'
MOVWF T0CON
    
MOVLW b'11000001'
MOVWF T1CON
    
MOVLW b'11100000'
MOVWF INTCON

BSF T0CON, 7
BSF T1CON, 7
RETURN
    
    
LIGHT_PADDLES:
    MOVFF paddle_left, LATA
    MOVFF paddle_right, LATF
    RETURN

LIGHT_BALL:
    MOVFF ball_vertical, INDF0
    CALL LIGHT_PADDLES
    RETURN
    
    
    
;INTERRUPT SERVICE ROUTINE
ISR:
;call    save_registers  ;Save current content of STATUS and PCLATH registers to be able to restore them later

T_INT:
    
    CALL LEFTSCORES
    
    
    incf counter, f 
    MOVLW 20 
    CPFSEQ counter
    GOTO T_EXIT    ;No, then exit from interrupt service routine
    GOTO PRE                ;Yes, then clear count variable
    
    
    PRE:
    incf counter2
    clrf counter
    
    MOVLW 5C 
    CPFSEQ counter2
    GOTO T_EXIT    ;No, then exit from interrupt service routine
    GOTO CONT     
    
    ;call restore_registers
    
    CONT:
    clrf counter2
    CALL BALL_NEXT
    CALL CHECK_GOAL
    GOTO T_EXIT
    
T_EXIT:
	CALL RIGHTSCORES
	bcf     INTCON, 2           ;Clear TMROIF
	retfie


R_INT:
RETFIE

;PADDLE MOVEMENT;
CHECK_PADDLES
BTFSC PORTG,0
CALL P2_DOWN

BTFSC PORTG,1
CALL P2_UP

BTFSC PORTG,2
CALL P1_DOWN

BTFSC PORTG,3
CALL P1_UP

RETURN

P1_UP
MOVLW b'00000111'
CPFSEQ paddle_left
RRNCF paddle_left
		CALL LIGHT_PADDLES
		L2
		BTFSC PORTG,3
		GOTO L2
RETURN
		
P1_DOWN
MOVLW b'00111000'
CPFSEQ paddle_left
RLNCF paddle_left
		CALL LIGHT_PADDLES
		L1
		BTFSC PORTG,2
		GOTO L1
RETURN   
  
P2_UP
MOVLW b'00000111'
CPFSEQ paddle_right
RRNCF paddle_right
		CALL LIGHT_PADDLES
		L4
		BTFSC PORTG,1
		GOTO L4
RETURN      
		
P2_DOWN
MOVLW b'00111000'
CPFSEQ paddle_right
RLNCF paddle_right
		CALL LIGHT_PADDLES
		L3
		BTFSC PORTG,0
		GOTO L3
RETURN
    
	
;BALL MOVEMENT;
BALL_NEXT

MOVLW b'00000011'
MOVWF temp
		
MOVF FSR0L,W
		
ANDWF temp, 1

Check_if_Upwards:
MOVLW 1
CPFSEQ temp
GOTO Check_if_Downwards
GOTO GO_UP

Check_if_Downwards:
MOVLW 2
CPFSEQ temp
GOTO GO_STR
GOTO GO_DOWN
RETURN


GO_STR:
MOVLW 0
CPFSEQ direction
CALL RIGHT_
CALL LEFT_
CALL LIGHT_BALL
RETURN

GO_DOWN:
RRNCF ball_vertical
CALL LIGHT_BALL
MOVLW 0
CPFSEQ direction
CALL RIGHT_
CALL LEFT_
CALL LIGHT_BALL
RETURN
    
GO_UP:
RLNCF ball_vertical
MOVLW 0
CPFSEQ direction
CALL RIGHT_
CALL LEFT_
CALL LIGHT_BALL
RETURN
    
RIGHT_
CLRF POSTINC0
CLRF POSTINC0
RETURN

LEFT_
CLRF POSTDEC0
RETURN


;Register handling for proper operation of main program
save_registers:
MOVFF paddle_left, tpaddle_left
MOVFF paddle_right, tpaddle_right 
MOVFF ball_vertical, tball_vertical 
MOVFF temp, ttemp 
MOVFF temp2, ttemp2
MOVFF temp3, ttemp3
MOVF W, twreg
 RETURN
 
restore_registers:
MOVFF tpaddle_left, paddle_left
MOVFF tpaddle_right, paddle_right 
MOVFF tball_vertical, ball_vertical 
MOVFF ttemp, temp 
MOVFF ttemp2, temp2
MOVFF ttemp3, temp3
MOVFF twreg, W
   RETURN

delay_300ms:                          ; Time Delay Routine
	; Time Delay Routine with 3 nested loops
    MOVLW 82	; Copy desired value to W
    MOVWF t3	; Copy W into t3

    _loop3f:
	MOVLW 0x78  ; Copy desired value to W
	MOVWF t2    ; Copy W into t2
	_loop2f:
	    MOVLW 0x9F	; Copy desired value to W
	    MOVWF t1	; Copy W into t1
	    _loop1f:

		DECFSZ t1 ; Decrement t1. If 0 Skip next instruction
		GOTO _loop1f ; ELSE Keep counting down
		DECFSZ t2 ; Decrement t2. If 0 Skip next instruction
		GOTO _loop2f ; ELSE Keep counting down
		DECFSZ t3 ; Decrement t3. If 0 Skip next instruction
		GOTO _loop3f ; ELSE Keep counting down
		RETURN

;CHECK IF GOAL;		
CHECK_GOAL
		
MOVLW 0x80
CPFSEQ 0xFE9
GOTO CHECK_RIGHT_GOAL
GOTO LEFT_GOAL 
    
CHECK_RIGHT_GOAL          
MOVLW 0x85
CPFSEQ 0xFE9
RETURN 
GOTO RIGHT_GOAL  

LEFT_GOAL    

MOVF paddle_left, WREG       ;COLLISION
ANDWF ball_vertical, 0
MOVWF temp2

MOVLW 0                   
CPFSEQ temp2	   
GOTO LEFT_REFLECT
INCF lscore		
GOTO START ;GOAL 
			   		   
LEFT_REFLECT
MOVLW 1
MOVWF direction

RETURN			  
    
RIGHT_GOAL    
		   
MOVF paddle_right, WREG       ;COLLISION
ANDWF ball_vertical, 0
MOVWF temp2
			   
MOVLW 0  
CPFSEQ temp2               
GOTO RIGHT_REFLECT
INCF rscore
GOTO START	                 ;GOAL 
			   		   
RIGHT_REFLECT
MOVLW 0
MOVWF direction
			   
RETURN	
  
    END_
END
