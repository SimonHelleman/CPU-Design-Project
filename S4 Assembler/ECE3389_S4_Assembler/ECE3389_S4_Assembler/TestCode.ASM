

Dest	EqU $4000	; Global variable definition
Result	EQU $7F90	; Global variable definition

		ORG $00
I		EQU -2		; I is a local on the stack
		LINK -2
		CLRA
		STAF I
LOOP1	LDAI Source
		PUSHA
		LDAF I
		ADDA
		PUSHA
		POPX
		LDAX 0
		PUSHA
		LDAI Dest
		PUSHA
		LDAF I
		ADDA
		PUSHA
		POPX
		POPA
		STAX 0
		LDAF I
		PUSHA
		LDAI 3
		CMPA
		BLE #(END1)
		LDAF I
		INCA
		STAF I
		BRA #(LOOP1)
END1	LDAI Dest
		PUSHA
		LDAI 3
		PUSHA
		LDAI 1
		PUSHA
		PUSHA
		JSR Average
		POPA
		STAD Result
		POPA
		POPA
		POPA
		HALT

		ORG $40
Average_i	EQU	-1
Average_sum	EQU -2
Average_arr	EQU 5
Average_size EQU 4
Average_shift EQU 3
Average_retval EQU 2
Average	LINK -2
		PUSHA
		CLRA
		STAF Average_i
		STAF Average_sum
LOOP2	LDAF Average_sum
		PUSHA
		LDAF Average_i
		PUSHA
		LDAF Average_arr
		ADDA
		PUSHA
		POPX
		LDAX 0
		ADDA
		STAF Average_sum
		LDAF Average_i
		PUSHA
		LDAF Average_size
		CMPA
		BLE #(END2)
		LDAF Average_i
		INCA
		STAF Average_i
		BRA #(LOOP2)
END2	CLRA
		STAF Average_i
LOOP3	LDAF Average_sum
		SHRA	;Shift RIGHT as ordered
		STAF Average_sum
		LDAF Average_i
		PUSHA
		LDAF Average_shift
		CMPA
		BLE #(END3)
		LDAF Average_i
		INCA
		STAF Average_i
		BRA #(LOOP3)
END3	LDAF Average_sum
		STAF Average_retval
		POPA
		UNLINK
		RTS

		ORG $70
Source	DCW 5,3,2,6


