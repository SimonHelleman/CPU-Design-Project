library ieee;
use ieee.std_logic_1164.all;
USE WORK.opcodes.all;
use work.aluops.all;

ENTITY ControlLogic_vhdl IS
PORT (
    CLK,RESET, N,Z                  : IN STD_LOGIC;
    IR                              : IN STD_LOGIC_VECTOR (15 DOWNTO 0);
    
    RD, WR, LD_MAR, OE_MAR          : OUT STD_LOGIC;
    OE_SP, LD_SP, PD_SP, PI_SP      : OUT STD_LOGIC;
    OE_FP, LD_FP, OE_XP, LD_XP      : OUT STD_LOGIC;
    INC_XP, DEC_XP                  : OUT STD_LOGIC;
    LD_PC, CNT_PC, OE_PC            : OUT STD_LOGIC;
    LD_MDR, OE_MDR, LD_IR, OE_IRL   : OUT STD_LOGIC;
    LD_A, CLR_A, DEC_A, INC_A       : OUT STD_LOGIC;
    OE_AA, OE_AD                    : OUT STD_LOGIC;
    ALU_OP                          : OUT STD_LOGIC_VECTOR (2 DOWNTO 0)
);
END ControlLogic_vhdl;

ARCHITECTURE RTL of ControlLogic_vhdl IS
    TYPE State_type IS 
    (
        Zero, Fetch, Decode,
        Inst_ADDA1, Inst_ADDA2, Inst_SUBA1, Inst_SUBA2,
        Inst_CMPA1, Inst_CMPA2, Inst_CLRA , Inst_SHRA,
        Inst_SHRL, Inst_INCA, Inst_DECA, Inst_INCX,
        Inst_DECX, Inst_LDAI, Inst_LDAD1, Inst_LDAD2,
        Inst_LDAF1, Inst_LDAF2, Inst_LDAX1, Inst_LDAX2,
        Inst_STAD1, Inst_STAD2, Inst_STAF1, Inst_STAF2,
        Inst_STAX1, Inst_STAX2, Inst_PUSHA, Inst_POPA,
        Inst_PUSHX1, Inst_PUSHX2, Inst_POPX, Inst_NOP,
        Inst_JSR1, Inst_JSR2, Inst_JSR3, Inst_JSR4,
        Inst_RTS, Inst_LINK1, Inst_LINK2, Inst_LINK3,
        Inst_UNLINK1, Inst_UNLINK2, Inst_BEQ, Inst_BLT,
        Inst_BLE, Inst_BRA, Inst_HALT
    );
    SIGNAL CLUState : State_type;
BEGIN
    -- The next state logic is in the PROCESS block
    PROCESS (CLK,RESET)
    BEGIN
        IF (RESET = '1') THEN
            CLUState <= Zero;
        
        ELSIF rising_edge(CLK) THEN
            CASE CLUState IS
                WHEN Zero =>
                    CLUState <= Fetch;
                WHEN Fetch =>
                    CLUState <= Decode;
                WHEN Decode =>
                    CASE IR(15 DOWNTO 8) IS
                        WHEN ADDA =>
                            CLUState <= Inst_ADDA1;
                        WHEN SUBA =>
                            CLUState <= Inst_SUBA1;
                        WHEN CMPA =>
                            CLUState <= Inst_CMPA1;
                        WHEN CLRA =>
                            CLUState <= Inst_CLRA;
                        WHEN SHRA =>
                            CLUState <= Inst_SHRA;
                        WHEN SHRL =>
                            CLUState <= Inst_SHRL;
                        WHEN INCA =>
                            CLUState <= Inst_INCA;
                        WHEN DECA =>
                            CLUState <= Inst_DECA;
                        WHEN INCX =>
                            CLUState <= Inst_INCX;
                        WHEN DECX =>
                            CLUState <= Inst_DECX;
                        WHEN LDAI =>
                            CLUState <= Inst_LDAI;
                        WHEN LDAD =>
                            CLUState <= Inst_LDAD1;
                        WHEN LDAF =>
                            CLUState <= Inst_LDAF1;
                        WHEN LDAX =>
                            CLUState <= Inst_LDAX1;
                        WHEN STAD =>
                            CLUState <= Inst_STAD1;
                        WHEN STAF =>
                            CLUState <= Inst_STAF1;
                        WHEN STAX =>
                            CLUState <= Inst_STAX1;
                        WHEN PUSHA =>
                            CLUState <= Inst_PUSHA;
                        WHEN POPA =>
                            CLUState <= Inst_POPA;
                        WHEN PUSHX =>
                            CLUState <= Inst_PUSHX1;
                        WHEN POPX =>
                            CLUState <= Inst_POPX;
                        WHEN NOP =>
                            CLUState <= Inst_NOP;
                        WHEN JSR =>
                            CLUState <= Inst_JSR1;
                        WHEN RTS =>
                            CLUState <= Inst_RTS;
                        WHEN LINK =>
                            CLUState <= Inst_LINK1;
                        WHEN UNLINK =>
                            CLUState <= Inst_UNLINK1;
                        WHEN BEQ =>
                            CLUState <= Inst_BEQ;
                        WHEN BLT =>
                            CLUState <= Inst_BLT;
                        WHEN BLE =>
                            CLUState <= Inst_BLE;
                        WHEN BRA =>
                            CLUState <= Inst_BRA;
                        WHEN OTHERS => -- HALT if garbage
                            CLUState <= Inst_HALT;
                    END CASE;
                WHEN Inst_ADDA1 =>
                    CLUState <= Inst_ADDA2;
                WHEN Inst_SUBA1 =>
                    CLUState <= Inst_SUBA2;
                WHEN Inst_CMPA1 =>
                    CLUState <= Inst_CMPA2;
                WHEN Inst_LDAD1 =>
                    CLUState <= Inst_LDAD2;
                WHEN Inst_LDAF1 =>
                    CLUState <= Inst_LDAF2;
                WHEN Inst_LDAX1 =>
                    CLUState <= Inst_LDAX2;
                WHEN Inst_STAD1 =>
                    CLUState <= Inst_STAD2;
                WHEN Inst_STAF1 =>
                    CLUState <= Inst_STAF2;
                WHEN Inst_STAX1 =>
                    CLUState <= Inst_STAX2;
                WHEN Inst_PUSHX1 =>
                    CLUState <= Inst_PUSHX2;
                WHEN Inst_JSR1 =>
                    CLUState <= Inst_JSR2;
                WHEN Inst_JSR2 =>
                    CLUState <= INST_JSR3;
                WHEN Inst_JSR3 =>
                    CLUState <= INST_JSR4;
                WHEN Inst_LINK1 =>
                    CLUState <= Inst_LINK2;
                WHEN Inst_LINK2 =>
                    CLUState <= Inst_LINK3;
                WHEN Inst_UNLINK1 =>
                    CLUState <= Inst_UNLINK2;
                WHEN Inst_HALT =>
                    CLUState <= Inst_HALT; -- Don't do anything again
                WHEN OTHERS =>
                    CLUState <= Fetch;
            END CASE;
        END IF;
    END PROCESS;
    
    -- Output process begins here
    PROCESS (CLUState, IR, N, Z)
    BEGIN
        -- Default values for the outputs are specified outside the CASE
        RD <= '0';
        WR <= '0';
        LD_MAR <= '0';
        OE_MAR <= '0';
        OE_SP <= '0';
        LD_SP <= '0';
        PD_SP <= '0';
        PI_SP <= '0';
        OE_FP <= '0';
        LD_FP <= '0';
        OE_XP <= '0';
        LD_XP <= '0';
        INC_XP <= '0';
        DEC_XP <= '0';
        LD_PC <= '0';
        CNT_PC <= '0';
        OE_PC <= '0';
        LD_MDR <= '0';
        OE_MDR <= '0';
        LD_IR <= '0';
        OE_IRL <= '0';
        LD_A <= '0';
        CLR_A <= '0';
        DEC_A <= '0';
        INC_A <= '0';
        OE_AA <= '0';
        OE_AD <= '0';
        ALU_OP <= ALUNOP;

        CASE CLUState IS
            WHEN Fetch =>
                OE_PC <= '1';
                RD <= '1';
                LD_IR <= '1';
                CNT_PC <= '1';
            WHEN Inst_ADDA1 =>
                -- MDR = *(++SP)
                PI_SP <= '1';
                OE_SP <= '1';
                RD <= '1';
                LD_MDR <= '1';
            WHEN Inst_ADDA2 =>
                -- A += MDR
                OE_MDR <= '1';
                OE_AA <= '1';
                ALU_OP <= ADD;
                LD_A <= '1';
            WHEN Inst_SUBA1 =>
                -- MDR = *(++SP)
                PI_SP <= '1';
                OE_SP <= '1';
                RD <= '1';
                LD_MDR <= '1';
            WHEN Inst_SUBA2 =>
                -- A -= MDR
                OE_MDR <= '1';
                OE_AA <= '1';
                ALU_OP <= SUBT;
                LD_A <= '1';
            WHEN Inst_CMPA1 =>
                -- MDR = *(++SP)
                PI_SP <= '1';
                OE_SP <= '1';
                RD <= '1';
                LD_MDR <= '1';
            WHEN Inst_CMPA2 =>
                -- A - MDR (result is discarded since we just want to update the flags)
                OE_MDR <= '1';
                OE_AA <= '1';
                ALU_OP <= SUBT;
            WHEN Inst_CLRA =>
                CLR_A <= '1';
            WHEN Inst_SHRA =>
                -- A = A >> 1
                OE_AA <= '1';
                ALU_OP <= SHR;
                LD_A <= '1';
            WHEN Inst_SHRL =>
                -- A = A << 1
                OE_AA <= '1';
                ALU_OP <= SHL;
                LD_A <= '1';
            WHEN Inst_INCA =>
                INC_A <= '1';
            WHEN Inst_DECA =>
                DEC_A <= '1';
            WHEN Inst_INCX =>
                INC_XP <= '1';
            WHEN Inst_DECX =>
                DEC_XP <= '1';
            WHEN Inst_LDAI =>
                -- A = *(PC++)
                OE_PC <= '1';
                RD <= '1';
                LD_A <= '1';
                CNT_PC <= '1';
            WHEN Inst_LDAD1 =>
                -- MAR = *(PC++)
                OE_PC <= '1';
                RD <= '1';
                LD_MAR <= '1';
                CNT_PC <= '1';
            WHEN Inst_LDAD2 =>
                -- A = *MAR
                OE_MAR <= '1';
                RD <= '1';
                LD_A <= '1';
            WHEN Inst_LDAF1 =>
                -- MAR = FP + IRL (sign extended low byte of IR)
                OE_FP <= '1';
                OE_IRL <= '1';
                ALU_OP <= ADD;
                LD_MAR <= '1';
            WHEN Inst_LDAF2 =>
                -- A = *MAR
                OE_MAR <= '1';
                RD <= '1';
                LD_A <= '1';
            WHEN Inst_LDAX1 =>
                -- MAR = XR + IRL
                OE_XP <= '1';
                OE_IRL <= '1';
                ALU_OP <= ADD;
                LD_MAR <= '1';
            WHEN Inst_LDAX2 =>
                -- A = *MAR
                OE_MAR <= '1';
                RD <= '1';
                LD_A <= '1';
            WHEN Inst_STAD1 =>
                -- MAR = *(PC++)
                OE_PC <= '1';
                RD <= '1';
                LD_MAR <= '1';
                CNT_PC <= '1';
            WHEN Inst_STAD2 =>
                -- *MAR = A
                OE_MAR <= '1';
                OE_AD <= '1';
                WR <= '1';
            WHEN Inst_STAF1 =>
                -- MAR = FP + IRL
                OE_FP <= '1';
                OE_IRL <= '1';
                ALU_OP <= ADD;
                LD_MAR <= '1';
            WHEN Inst_STAF2 =>
                -- *MAR = A
                OE_MAR <= '1';
                OE_AD <= '1';
                WR <= '1';
            WHEN Inst_STAX1 =>
                -- MAR = XR + IRL
                OE_XP <= '1';
                OE_IRL <= '1';
                ALU_OP <= ADD;
                LD_MAR <= '1';
            WHEN Inst_STAX2 =>
                -- *MAR = A
                OE_MAR <= '1';
                OE_AD <= '1';
                WR <= '1';
            WHEN Inst_PUSHA =>
                -- *(SP--) = A
                OE_SP <= '1';
                PD_SP <= '1';
                OE_AD <= '1';
                WR <= '1';
            WHEN Inst_POPA =>
                -- A = *(++SP)
                PI_SP <= '1';
                OE_SP <= '1';
                RD <= '1';
                LD_A <= '1';
            WHEN Inst_PUSHX1 =>
                -- Ideally *(SP--) = XR (in 1 cycle)
                -- but both XR and SP can 
                -- only be put on the address bus
                -- one at a time with no option for
                -- xr to go on the data bus

                -- bandaid solution using the MDR and 2 cycles
                -- i.e. MDR = XR
                OE_XP <= '1';
                ALU_OP <= PASSA;
                LD_MDR <= '1';
            WHEN Inst_PUSHX2 =>
                -- *(SP--) = MDR
                OE_SP <= '1';
                PD_SP <= '1';
                OE_MDR <= '1';
                WR <= '1';
            WHEN Inst_POPX =>
                -- XP = *(++SP)
                PI_SP <= '1';
                OE_SP <= '1';
                RD <= '1';
                LD_XP <= '1';
            WHEN Inst_JSR1 =>
                -- MAR = PC++
                OE_PC <= '1';
                ALU_OP <= PASSA;
                LD_MAR <= '1';
                CNT_PC <= '1';
            WHEN Inst_JSR2 =>
                -- MDR = PC
                OE_PC <= '1';
                ALU_OP <= PASSA;
                LD_MDR <= '1';
            WHEN Inst_JSR3 =>
                -- *(SP--) = MDR
                OE_SP <= '1';
                PD_SP <= '1';
                OE_MDR <= '1';
                WR <= '1';
            WHEN Inst_JSR4 =>
                -- PC = *MAR
                OE_MAR <= '1';
                ALU_OP <= PASSA;
                LD_PC <= '1';
            WHEN Inst_RTS =>
                -- PC = *(++SP)
                PI_SP <= '1';
                OE_SP <= '1';
                RD <= '1';
                LD_PC <= '1';
            WHEN Inst_LINK1 =>
                -- *SP = FP pt. 1 => MDR = FP
                OE_FP <= '1';
                ALU_OP <= PASSA;
                LD_MDR <= '1';
            WHEN Inst_LINK2 =>
                -- *SP = FP pt. 2 => *SP = MDR
                -- Also do FP = SP--.
                OE_SP <= '1';
                OE_MDR <= '1';
                WR <= '1';

                ALU_OP <= PASSA;
                LD_FP <= '1';
                PD_SP <= '1';
            WHEN Inst_LINK3 =>
                -- SP = SP + IRL
                OE_SP <= '1';
                OE_IRL <= '1';
                ALU_OP <= ADD;
                LD_SP <= '1';
            WHEN Inst_UNLINK1 =>
                -- SP = FP
                OE_FP <= '1';
                ALU_OP <= PASSA;
                LD_SP <= '1';
            WHEN Inst_UNLINK2 =>
                -- FP = *SP
                OE_SP <= '1';
                RD <= '1';
                LD_FP <= '1';
            WHEN Inst_BEQ =>
                -- if (Z) { PC = PC + IRL; }
                IF Z = '1' THEN
                    OE_PC <= '1';
                    OE_IRL <= '1';
                    ALU_OP <= ADD;
                    LD_PC <= '1';
                END IF;
            WHEN Inst_BLT =>
                -- if (N) { PC = PC + IRL }
                IF N = '1' THEN
                    OE_PC <= '1';
                    OE_IRL <= '1';
                    ALU_OP <= ADD;
                    LD_PC <= '1';
                END IF;
            WHEN Inst_BLE =>
                IF N = '1' OR Z = '1' THEN
                    OE_PC <= '1';
                    OE_IRL <= '1';
                    ALU_OP <= ADD;
                    LD_PC <= '1';
                END IF;
            WHEN Inst_BRA =>
                OE_PC <= '1';
                OE_IRL <= '1';
                ALU_OP <= ADD;
                LD_PC <= '1';
            WHEN OTHERS =>
        END CASE;
    END PROCESS;
END RTL;
