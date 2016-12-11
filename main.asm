TODO:

;REM_TO_BE_1024KB_MAX
;REM_IS_128_ROWS_OF_CODES

* = $0500
ENIGV1 = $4F   ;ONE_LESS
ENIGVA = $50
RINGST = $53   ;REM_ROTOR
LRRING = $54   
ROTORS = $57   ;REM_REFLECTOR
LROTOR = $58   
KEYIN  = $5B
KEYOUT = $5C 

TIMEO1 = $5D
TMP01  = $5E
TMP02  = $5F
TMP03  = $60
TMP04  = $61
TMP05  = $62
TMP06  = $63

DISP3  = $F9

GETKEY = $1F6A
SCANS  = $1F1F

JMP INIT

DEFVAL
.BYTE $01
.BYTE $02
.BYTE $03
.BYTE $01
.BYTE $01
.BYTE $01
.BYTE $01
.BYTE $04
.BYTE $03
.BYTE $02
.BYTE $01

ROTIDX
.BYTE $1E
.BYTE $00
.BYTE $0A
.BYTE $14

FWRTR1
.BYTE $9
.BYTE $6
.BYTE $4
.BYTE $1
.BYTE $8
.BYTE $2
.BYTE $7
.BYTE $0
.BYTE $3
.BYTE $5
FWRTR2
.BYTE $2
.BYTE $5
.BYTE $8
.BYTE $4
.BYTE $1
.BYTE $0
.BYTE $9
.BYTE $7
.BYTE $6
.BYTE $3
FWRTR3
.BYTE $4
.BYTE $3
.BYTE $5
.BYTE $8
.BYTE $1
.BYTE $6
.BYTE $2
.BYTE $0
.BYTE $7
.BYTE $9
REFLEC
.BYTE $2
.BYTE $5
.BYTE $0
.BYTE $7
.BYTE $9
.BYTE $1
.BYTE $8
.BYTE $3
.BYTE $6
.BYTE $4
BWRTR1
.BYTE $7
.BYTE $3
.BYTE $5
.BYTE $8
.BYTE $2
.BYTE $9
.BYTE $1
.BYTE $6
.BYTE $4
.BYTE $0
BWRTR2
.BYTE $5
.BYTE $4
.BYTE $0
.BYTE $9
.BYTE $3
.BYTE $1
.BYTE $8
.BYTE $7
.BYTE $2
.BYTE $6
BWRTR3
.BYTE $7
.BYTE $4
.BYTE $6
.BYTE $1
.BYTE $0
.BYTE $2
.BYTE $5
.BYTE $8
.BYTE $3
.BYTE $9

KEYTBL
.BYTE $10 ;INC_V1
.BYTE $0C ;DEV_V1
.BYTE $11 ;INC_V2
.BYTE $0D ;DEV_V2
.BYTE $14 ;INC_V3
.BYTE $0E ;DEV_V3
.BYTE $12 ;INC_V4
.BYTE $0F ;DEV_V4

INIT

CLD
LDA #$0     ;REM_ROTOR_CONFIG
CMP *ENIGVA
BNE INITOK

LDA #$0B    ;REM_11_PARAMS
STA *TIMEO1
LDX #$0

CPYINI
LDA DEFVAL,X
STA *ENIGVA,X
INX
DEC *TIMEO1
BNE CPYINI

INITOK

START

JSR TIMEOU

JSR SHOWEN

JSR SCANS
JSR GETKEY

CMP #$15   
BCS START  ;REM_IF_ABOVE_15

CMP #$0A
BCC SKIPMR ;REM_IF_9_OR_LESS

CMP #$0A
BEQ KPUSHA

CMP #$0B
BEQ KPUSHB

CMP #$13
BEQ KPUSHG

JSR MOVROT
JMP START

SKIPMR
STA *KEYIN
JSR STEPRT
JSR ENIGMA

LDA #$50
STA *TIMEO1

JMP START

KPUSHA
JMP START
KPUSHB
JMP START
KPUSHG
JMP START

;REM_SUB_TIMEOUT
TIMEOU

LDA #$0
CMP *TIMEO1
BEQ TIMEOS
DEC *TIMEO1
BNE TIMEOS

STA *KEYIN
STA *KEYOUT

TIMEOS
RTS

;REM_SUB_SHOWENIGMA
SHOWEN

LDX #$00
LDY #$03
SHOWND
LDA *ROTORS,X
ASL A
ASL A
ASL A
ASL A
STA $F8,Y
INX
LDA *ROTORS,X
ORA $F8,Y
STA $F8,Y
INX
DEY
BNE SHOWND

RTS

;REM_SUB_MOVROT
MOVROT

LDX *$00
LDY *$04
STX *TMP01

MOVRSC
CMP KEYTBL,X
BEQ MOVRIN
CMP INC_V1,X
BEQ MOVRDE
INC *TMP01
INX
INX
DEY
BNE MOVRSC
RTS

MOVRIN
LDX *TMP01
INC *ROTORS,X
JMP MOVRCK

MOVRDE
LDX *TMP01
DEC *ROTORS,X
;REM_JMP_MOVRCK

MOVRCK
LDA #$0
STA *KEYIN
STA *KEYOUT

JSR ROLLOV

RTS

;REM_SUB_ROLLOV
;REM_CALL_THIS_WITH_X
;REM_TO_CHECK_0-9_ROLLOVER
;REM_AFTER_MOVE_WHEELS
ROLLOV

LDA #$FF
CMP *ROTORS,X
BEQ ROLLO9

LDA #$0A
CMP *ROTORS,X
BEQ ROLLO0

RTS

ROLLO9
LDA #$09
STA *ROTORS,X
RTS

ROLLO0
LDA #$00
STA *ROTORS,X
RTS

;REM_SUB_STEPRT
STEPRT
LDX #$00
LDY #$03      

STEPSC
LDA *LRRING,X
CLC
ADC #$08      
CMP #$0A
BCC STEPS1
SBC #$0A
STEPS1
CMP *LROTOR,X
BNE STEPS2
LDA #$01
JMP STEPDN
STEPS2
LDA #$00
STEPDN
STA *TMP02,X
INX
DEY
BNE STEPSC

STY *TMP01
LDA #$04
STA *TMP05   ;REM_INC_RIGHT_DIGIT
STA *TMP06
LDX #$00
LDY #$00

STEPIW
LDA *TMP01,X
STY *TMP01,X
INX
CLC
ADC *TMP01,X
CMP #$00
BEQ STEPS3

DEX
INC *ROTORS,X
JSR ROLLOV
INX

STEPS3
DEC *TMP06
BNE STEPIW

RTS

;REM_SUB_ENRING
;CALL_WITH_A_KEY_TO_ENCODE
;CALL_WITH_X_OFFSET_TO_ROTOR
ENRING

SEC
ADC *ROTORS,X ;REM_PLUS_1
CMP #$0A
BCC ENRIK1  ;REM_ABOVE_9
SBC #$0A    ;REM_CARRY_IS_SET

ENRIK1
SEC
SBC *RINGST,X
BCS ENRIK2
ADC #$0A

ENRIK2
CLC
RTS

;REM_SUB_ENIGMA
ENIGMA

LDA #$FF
STA *TMP03

LDA #$00
STA *TMP04

LDY #$07
LDX #$03
LDA *KEYIN

ENIGM2
JSR ENRING

STX *TMP01

PHA
TXA
CMP #$0
BEQ ENIGXO

LDA *ENIGV1,X
TAX

ENIGXO
PLA

CLC
ADC *TMP04
ADC ROTIDX,X
TAX
LDA FWRTR1,X

PHA
LDA #$00
LDX *TMP01
JSR ENRING
STA *TMP02
PLA

SEC
SBC *TMP02
BCS ENIGM3
ADC #$0A

ENIGM3
 
STA *KEYOUT

TYA
;CMP #$05
;BNE ENIGNR
;REM_SETUP_FOR_REFLECTOR
;ENIGNR

CMP #$04
BNE ENIGNB

;REM_SETUP_TO_GO_BACK

LDA #$01
STA *TMP03

LDA #$28
STA *TMP04

ENIGNB

TXA
CLC
ADC *TMP03
TAX

LDA *KEYOUT

DEY
BNE ENIGM2

RTS
