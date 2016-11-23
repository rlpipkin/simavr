/* THIS FILE IS AUTOGENERATED, DO NOT EDIT */
#ifndef __SIM_CORE_JIT_H__
#define __SIM_CORE_JIT_H__
	// JIT_EXTRACT_START <-- don't change this line,
	// it's there to help the script extract the opcode state machine
	jit_literal_flush();

	switch (opcode & 0xf000) {
		case 0x0000: {
			switch (opcode) {
				case 0x0000:  {	// NOP
jit_generate(opcode, "STATE(\"nop\\n\");\n"
);
				}	break;
				default: {
					switch (opcode & 0xfc00) {
						case 0x0400:  {	// CPC -- Compare with carry -- 0000 01rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint8_t res = vd - vr - SREG_BIT(S_C);\n"
"STATE(\"cpc %s[%02x], %s[%02x] = %02x\\n\", avr_regname(d), vd, avr_regname(r), vr, res);\n"
"_avr_flags_sub_Rzns(avr, res, vd, vr);\n"
"SREG();\n"
);
						}	break;
						case 0x0c00:  {	// ADD -- Add without carry -- 0000 11rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint8_t res = vd + vr;\n"
"if (r == d) {\n"
"STATE(\"lsl %s[%02x] = %02x\\n\", avr_regname(d), vd, res & 0xff);\n"
"} else {\n"
"STATE(\"add %s[%02x], %s[%02x] = %02x\\n\", avr_regname(d), vd, avr_regname(r), vr, res);\n"
"}\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_add_zns(avr, res, vd, vr);\n"
"SREG();\n"
);
						}	break;
						case 0x0800:  {	// SBC -- Subtract with carry -- 0000 10rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint8_t res = vd - vr - SREG_BIT(S_C);\n"
"STATE(\"sbc %s[%02x], %s[%02x] = %02x\\n\", avr_regname(d), avr_data[d], avr_regname(r), avr_data[r], res);\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_sub_Rzns(avr, res, vd, vr);\n"
"SREG();\n"
);
						}	break;
						default:
							switch (opcode & 0xff00) {
								case 0x0100:  {	// MOVW -- Copy Register Word -- 0000 0001 dddd rrrr
jit_generate(opcode, "uint8_t d = ((opcode >> 4) & 0xf) << 1;\n"
"uint8_t r = ((opcode) & 0xf) << 1;\n"
"STATE(\"movw %s:%s, %s:%s[%02x%02x]\\n\", avr_regname(d), avr_regname(d+1), avr_regname(r), avr_regname(r+1), avr_data[r+1], avr_data[r]);\n"
"uint16_t vr = avr_data[r] | (avr_data[r + 1] << 8);\n"
"_avr_set_r16le(avr, d, vr);\n"
);
								}	break;
								case 0x0200:  {	// MULS -- Multiply Signed -- 0000 0010 dddd rrrr
jit_generate(opcode, "int8_t r = 16 + (opcode & 0xf);\n"
"int8_t d = 16 + ((opcode >> 4) & 0xf);\n"
"int16_t res = ((int8_t)avr_data[r]) * ((int8_t)avr_data[d]);\n"
"STATE(\"muls %s[%d], %s[%02x] = %d\\n\", avr_regname(d), ((int8_t)avr_data[d]), avr_regname(r), ((int8_t)avr_data[r]), res);\n"
"_avr_set_r16le(avr, 0, res);\n"
"SREG_SETBIT(S_I, (res >> 15) & 1);\n"
"SREG_SETBIT(S_Z, res == 0);\n"
"cycle++;\n"
"SREG();\n"
);
								}	break;
								case 0x0300:  {	// MUL -- Multiply -- 0000 0011 fddd frrr
jit_generate(opcode, "int8_t r = 16 + (opcode & 0x7);\n"
"int8_t d = 16 + ((opcode >> 4) & 0x7);\n"
"int16_t res = 0;\n"
"uint8_t c = 0;\n"
"T(const char * name = \"\";)\n"
"switch (opcode & 0x88) {\n"
"case 0x00: 	// MULSU -- Multiply Signed Unsigned -- 0000 0011 0ddd 0rrr\n"
"res = ((uint8_t)avr_data[r]) * ((int8_t)avr_data[d]);\n"
"c = (res >> 15) & 1;\n"
"T(name = \"mulsu\";)\n"
"break;\n"
"case 0x08: 	// FMUL -- Fractional Multiply Unsigned -- 0000 0011 0ddd 1rrr\n"
"res = ((uint8_t)avr_data[r]) * ((uint8_t)avr_data[d]);\n"
"c = (res >> 15) & 1;\n"
"res <<= 1;\n"
"T(name = \"fmul\";)\n"
"break;\n"
"case 0x80: 	// FMULS -- Multiply Signed -- 0000 0011 1ddd 0rrr\n"
"res = ((int8_t)avr_data[r]) * ((int8_t)avr_data[d]);\n"
"c = (res >> 15) & 1;\n"
"res <<= 1;\n"
"T(name = \"fmuls\";)\n"
"break;\n"
"case 0x88: 	// FMULSU -- Multiply Signed Unsigned -- 0000 0011 1ddd 1rrr\n"
"res = ((uint8_t)avr_data[r]) * ((int8_t)avr_data[d]);\n"
"c = (res >> 15) & 1;\n"
"res <<= 1;\n"
"T(name = \"fmulsu\";)\n"
"break;\n"
"}\n"
"cycle++;\n"
"STATE(\"%s %s[%d], %s[%02x] = %d\\n\", name, avr_regname(d), ((int8_t)avr_data[d]), avr_regname(r), ((int8_t)avr_data[r]), res);\n"
"_avr_set_r16le(avr, 0, res);\n"
"SREG_SETBIT(S_I, c);\n"
"SREG_SETBIT(S_Z, res == 0);\n"
"SREG();\n"
);
								}	break;
								default: _avr_invalid_opcode(avr);
							}
					}
				}
			}
		}	break;

		case 0x1000: {
			switch (opcode & 0xfc00) {
				case 0x1800:  {	// SUB -- Subtract without carry -- 0001 10rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint8_t res = vd - vr;\n"
"STATE(\"sub %s[%02x], %s[%02x] = %02x\\n\", avr_regname(d), vd, avr_regname(r), vr, res);\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_sub_zns(avr, res, vd, vr);\n"
"SREG();\n"
);
				}	break;
				case 0x1000:  {	// CPSE -- Compare, skip if equal -- 0001 00rd dddd rrrr
					/* This code is deduped like this to allow emmiting a 'constant'
					 * piece of code, without having to read the next instruction in case
					 * of JIT. In normal mode, we lose a few cycles but at least the code is
					 * clear */
					const int skip = 1 + _avr_is_instruction_32_bits(avr, new_pc);
					 {
						jit_generate_literal("const int skip = %d", skip);
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint16_t res = vd == vr;\n"
"STATE(\"cpse %s[%02x], %s[%02x]\\t; Will%s skip\\n\", avr_regname(d), avr_data[d], avr_regname(r), avr_data[r], res ? \"\":\" not\");\n"
"if (res) {\n"
"new_pc += 2 * skip; cycle += skip;\n"
"TRACE_JUMP();\n"
"}\n"
);
					} break;
				}	break;
				case 0x1400:  {	// CP -- Compare -- 0001 01rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint8_t res = vd - vr;\n"
"STATE(\"cp %s[%02x], %s[%02x] = %02x\\n\", avr_regname(d), vd, avr_regname(r), vr, res);\n"
"_avr_flags_sub_zns(avr, res, vd, vr);\n"
"SREG();\n"
);
				}	break;
				case 0x1c00:  {	// ADD -- Add with carry -- 0001 11rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint8_t res = vd + vr + SREG_BIT(S_C);\n"
"if (r == d) {\n"
"STATE(\"rol %s[%02x] = %02x\\n\", avr_regname(d), avr_data[d], res);\n"
"} else {\n"
"STATE(\"addc %s[%02x], %s[%02x] = %02x\\n\", avr_regname(d), avr_data[d], avr_regname(r), avr_data[r], res);\n"
"}\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_add_zns(avr, res, vd, vr);\n"
"SREG();\n"
);
				}	break;
				default: _avr_invalid_opcode(avr);
			}
		}	break;

		case 0x2000: {
			switch (opcode & 0xfc00) {
				case 0x2000:  {	// AND -- Logical AND -- 0010 00rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint8_t res = vd & vr;\n"
"if (r == d) {\n"
"STATE(\"tst %s[%02x]\\n\", avr_regname(d), avr_data[d]);\n"
"} else {\n"
"STATE(\"and %s[%02x], %s[%02x] = %02x\\n\", avr_regname(d), vd, avr_regname(r), vr, res);\n"
"}\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_znv0s(avr, res);\n"
"SREG();\n"
);
				}	break;
				case 0x2400:  {	// EOR -- Logical Exclusive OR -- 0010 01rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint8_t res = vd ^ vr;\n"
"if (r==d) {\n"
"STATE(\"clr %s[%02x]\\n\", avr_regname(d), avr_data[d]);\n"
"} else {\n"
"STATE(\"eor %s[%02x], %s[%02x] = %02x\\n\", avr_regname(d), vd, avr_regname(r), vr, res);\n"
"}\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_znv0s(avr, res);\n"
"SREG();\n"
);
				}	break;
				case 0x2800:  {	// OR -- Logical OR -- 0010 10rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint8_t res = vd | vr;\n"
"STATE(\"or %s[%02x], %s[%02x] = %02x\\n\", avr_regname(d), vd, avr_regname(r), vr, res);\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_znv0s(avr, res);\n"
"SREG();\n"
);
				}	break;
				case 0x2c00:  {	// MOV -- 0010 11rd dddd rrrr
jit_generate(opcode, "get_d5_vr5(opcode);\n"
"uint8_t res = vr;\n"
"STATE(\"mov %s, %s[%02x] = %02x\\n\", avr_regname(d), avr_regname(r), vr, res);\n"
"_avr_set_r(avr, d, res);\n"
);
				}	break;
				default: _avr_invalid_opcode(avr);
			}
		}	break;

		case 0x3000:  {	// CPI -- Compare Immediate -- 0011 kkkk hhhh kkkk
jit_generate(opcode, "get_vh4_k8(opcode);\n"
"uint8_t res = vh - k;\n"
"STATE(\"cpi %s[%02x], 0x%02x\\n\", avr_regname(h), vh, k);\n"
"_avr_flags_sub_zns(avr, res, vh, k);\n"
"SREG();\n"
);
		}	break;

		case 0x4000:  {	// SBCI -- Subtract Immediate With Carry -- 0100 kkkk hhhh kkkk
jit_generate(opcode, "get_vh4_k8(opcode);\n"
"uint8_t res = vh - k - SREG_BIT(S_C);\n"
"STATE(\"sbci %s[%02x], 0x%02x = %02x\\n\", avr_regname(h), vh, k, res);\n"
"_avr_set_r(avr, h, res);\n"
"_avr_flags_sub_Rzns(avr, res, vh, k);\n"
"SREG();\n"
);
		}	break;

		case 0x5000:  {	// SUBI -- Subtract Immediate -- 0101 kkkk hhhh kkkk
jit_generate(opcode, "get_vh4_k8(opcode);\n"
"uint8_t res = vh - k;\n"
"STATE(\"subi %s[%02x], 0x%02x = %02x\\n\", avr_regname(h), vh, k, res);\n"
"_avr_set_r(avr, h, res);\n"
"_avr_flags_sub_zns(avr, res, vh, k);\n"
"SREG();\n"
);
		}	break;

		case 0x6000:  {	// ORI aka SBR -- Logical OR with Immediate -- 0110 kkkk hhhh kkkk
jit_generate(opcode, "get_vh4_k8(opcode);\n"
"uint8_t res = vh | k;\n"
"STATE(\"ori %s[%02x], 0x%02x\\n\", avr_regname(h), vh, k);\n"
"_avr_set_r(avr, h, res);\n"
"_avr_flags_znv0s(avr, res);\n"
"SREG();\n"
);
		}	break;

		case 0x7000:  {	// ANDI	-- Logical AND with Immediate -- 0111 kkkk hhhh kkkk
jit_generate(opcode, "get_vh4_k8(opcode);\n"
"uint8_t res = vh & k;\n"
"STATE(\"andi %s[%02x], 0x%02x\\n\", avr_regname(h), vh, k);\n"
"_avr_set_r(avr, h, res);\n"
"_avr_flags_znv0s(avr, res);\n"
"SREG();\n"
);
		}	break;

		case 0xa000:
		case 0x8000: {
			/*
			 * Load (LDD/STD) store instructions
			 *
			 * 10q0 qqsd dddd yqqq
			 * s = 0 = load, 1 = store
			 * y = 16 bits register index, 1 = Y, 0 = X
			 * q = 6 bit displacement
			 */
			switch (opcode & 0xd008) {
				case 0xa000:
				case 0x8000:  {	// LD (LDD) -- Load Indirect using Z -- 10q0 qqsd dddd yqqq
jit_generate(opcode, "uint16_t v = avr_data[R_ZL] | (avr_data[R_ZH] << 8);\n"
"get_d5_q6(opcode);\n"
"if (opcode & 0x0200) {\n"
"STATE(\"st (Z+%d[%04x]), %s[%02x]\\n\", q, v+q, avr_regname(d), avr_data[d]);\n"
"_avr_set_ram(avr, v+q, avr_data[d]);\n"
"} else {\n"
"STATE(\"ld %s, (Z+%d[%04x])=[%02x]\\n\", avr_regname(d), q, v+q, avr_data[v+q]);\n"
"_avr_set_r(avr, d, _avr_get_ram(avr, v+q));\n"
"}\n"
"cycle += 1; // 2 cycles, 3 for tinyavr\n"
);
				}	break;
				case 0xa008:
				case 0x8008:  {	// LD (LDD) -- Load Indirect using Y -- 10q0 qqsd dddd yqqq
jit_generate(opcode, "uint16_t v = avr_data[R_YL] | (avr_data[R_YH] << 8);\n"
"get_d5_q6(opcode);\n"
"if (opcode & 0x0200) {\n"
"STATE(\"st (Y+%d[%04x]), %s[%02x]\\n\", q, v+q, avr_regname(d), avr_data[d]);\n"
"_avr_set_ram(avr, v+q, avr_data[d]);\n"
"} else {\n"
"STATE(\"ld %s, (Y+%d[%04x])=[%02x]\\n\", avr_regname(d), q, v+q, avr_data[d+q]);\n"
"_avr_set_r(avr, d, _avr_get_ram(avr, v+q));\n"
"}\n"
"cycle += 1; // 2 cycles, 3 for tinyavr\n"
);
				}	break;
				default: _avr_invalid_opcode(avr);
			}
		}	break;

		case 0x9000: {
			/* this is an annoying special case, but at least these lines handle all the SREG set/clear opcodes */
			if ((opcode & 0xff0f) == 0x9408) {
				 {
jit_generate(opcode, "get_sreg_bit(opcode);\n"
"STATE(\"%s%c\\n\", opcode & 0x0080 ? \"cl\" : \"se\", _sreg_bit_name[b]);\n"
"SREG_SETBIT(b, (opcode & 0x0080) == 0);\n"
"SREG();\n"
);
				} break;
			} else switch (opcode) {
				case 0x9588:  { // SLEEP -- 1001 0101 1000 1000
jit_generate(opcode, "STATE(\"sleep\\n\");\n"
"/* Don't sleep if there are interrupts about to be serviced.\n"
"* Without this check, it was possible to incorrectly enter a state\n"
"* in which the cpu was sleeping and interrupts were disabled. For more\n"
"* details, see the commit message. */\n"
"if (!avr_has_pending_interrupts(avr) || !SREG_BIT(S_I)) {\n"
"avr_state = cpu_Sleeping;\n"
"CORE_SLEEP();\n"
"}\n"
);
				}	break;
				case 0x9598:  { // BREAK -- 1001 0101 1001 1000
jit_generate(opcode, "STATE(\"break\\n\");\n"
"if (avr->gdb) {\n"
"// if gdb is on, we break here as in here\n"
"// and we do so until gdb restores the instruction\n"
"// that was here before\n"
"avr_state = cpu_StepDone;\n"
"new_pc = avr->pc;\n"
"cycle = 0;\n"
"}\n"
);
				}	break;
				case 0x95a8:  { // WDR -- Watchdog Reset -- 1001 0101 1010 1000
jit_generate(opcode, "STATE(\"wdr\\n\");\n"
"avr_ioctl(avr, AVR_IOCTL_WATCHDOG_RESET, 0);\n"
);
				}	break;
				case 0x95e8:  { // SPM -- Store Program Memory -- 1001 0101 1110 1000
jit_generate(opcode, "STATE(\"spm\\n\");\n"
"avr_ioctl(avr, AVR_IOCTL_FLASH_SPM, 0);\n"
);
				}	break;
				case 0x9409:   // IJMP -- Indirect jump -- 1001 0100 0000 1001
				case 0x9419:   // EIJMP -- Indirect jump -- 1001 0100 0001 1001   bit 4 is "indirect"
				case 0x9509:   // ICALL -- Indirect Call to Subroutine -- 1001 0101 0000 1001
				case 0x9519: { // EICALL -- Indirect Call to Subroutine -- 1001 0101 0001 1001   bit 8 is "push pc"
					int e = opcode & 0x10;
					int p = opcode & 0x100;
					if (e && !avr_eind)
						_avr_invalid_opcode(avr);
					 {
						jit_generate_literal("const int e = %d", e);
						jit_generate_literal("const int p = %d", p);
jit_generate(opcode, "uint32_t z = avr_data[R_ZL] | (avr_data[R_ZH] << 8);\n"
"if (e)\n"
"z |= avr_data[avr_eind] << 16;\n"
"STATE(\"%si%s Z[%04x]\\n\", e?\"e\":\"\", p?\"call\":\"jmp\", z << 1);\n"
"if (p)\n"
"cycle += _avr_push_addr(avr, new_pc) - 1;\n"
"new_pc = z << 1;\n"
"cycle++;\n"
"TRACE_JUMP();\n"
);
					}	break;
				}	break;
				case 0x9518: 		// RETI -- Return from Interrupt -- 1001 0101 0001 1000
				case 0x9508:  {	// RET -- Return -- 1001 0101 0000 1000
jit_generate(opcode, "if (opcode == 0x9518) {\n"
"SREG_SETBIT(S_I, 1);\n"
"avr_interrupt_reti(avr);\n"
"}\n"
"new_pc = _avr_pop_addr(avr);\n"
"cycle += 1 + avr_address_size;\n"
"STATE(\"ret%s\\n\", opcode & 0x10 ? \"i\" : \"\");\n"
"TRACE_JUMP();\n"
"STACK_FRAME_POP();\n"
);
				}	break;
				case 0x95c8:  {	// LPM -- Load Program Memory R0 <- (Z) -- 1001 0101 1100 1000
jit_generate(opcode, "uint16_t z = avr_data[R_ZL] | (avr_data[R_ZH] << 8);\n"
"STATE(\"lpm %s, (Z[%04x]) = %02x\\n\", avr_regname(0), z, avr_flash[z]);\n"
"cycle += 2; // 3 cycles\n"
"_avr_set_r(avr, 0, avr_flash[z]);\n"
);
				}	break;
				case 0x95d8: {	// ELPM -- Load Program Memory R0 <- (Z) -- 1001 0101 1101 1000
					if (!avr_rampz)
						_avr_invalid_opcode(avr);
					 {
jit_generate(opcode, "uint32_t z = avr_data[R_ZL] | (avr_data[R_ZH] << 8) | (avr_data[avr_rampz] << 16);\n"
"STATE(\"elpm %s, (Z[%02x:%04x])\\n\", avr_regname(0), z >> 16, z & 0xffff);\n"
"_avr_set_r(avr, 0, avr_flash[z]);\n"
"cycle += 2; // 3 cycles\n"
);
					}	break;
				}	break;
				default:  {
					switch (opcode & 0xfe0f) {
						case 0x9000:  {	// LDS -- Load Direct from Data Space, 32 bits -- 1001 0000 0000 0000
jit_generate(opcode, "get_d5(opcode);\n"
"uint16_t x = _avr_flash_read16le(avr, new_pc);\n"
"new_pc += 2;\n"
"STATE(\"lds %s[%02x], 0x%04x\\n\", avr_regname(d), avr_data[d], x);\n"
"_avr_set_r(avr, d, _avr_get_ram(avr, x));\n"
"cycle++; // 2 cycles\n"
);
						}	break;
						case 0x9005:
						case 0x9004:  {	// LPM -- Load Program Memory -- 1001 000d dddd 01oo
jit_generate(opcode, "get_d5(opcode);\n"
"uint16_t z = avr_data[R_ZL] | (avr_data[R_ZH] << 8);\n"
"int op = opcode & 1;\n"
"STATE(\"lpm %s, (Z[%04x]%s) = %02x\\n\", avr_regname(d), z, op ? \"+\" : \"\", avr_flash[z]);\n"
"_avr_set_r(avr, d, avr_flash[z]);\n"
"if (op)\n"
"_avr_set_r16le_hl(avr, R_ZL, z + 1);\n"
"cycle += 2; // 3 cycles\n"
);
						}	break;
						case 0x9006:
						case 0x9007: {	// ELPM -- Extended Load Program Memory -- 1001 000d dddd 01oo
							if (!avr_rampz)
								_avr_invalid_opcode(avr);
							 {
jit_generate(opcode, "uint32_t z = avr_data[R_ZL] | (avr_data[R_ZH] << 8) | (avr_data[avr_rampz] << 16);\n"
"get_d5(opcode);\n"
"int op = opcode & 1;\n"
"STATE(\"elpm %s, (Z[%02x:%04x]%s)\\n\", avr_regname(d), z >> 16, z & 0xffff, op ? \"+\" : \"\");\n"
"_avr_set_r(avr, d, avr_flash[z]);\n"
"if (op) {\n"
"z++;\n"
"_avr_set_r(avr, avr_rampz, z >> 16);\n"
"_avr_set_r16le_hl(avr, R_ZL, z);\n"
"}\n"
"cycle += 2; // 3 cycles\n"
);
							}	break;
						}	break;
						/*
						 * Load store instructions
						 *
						 * 1001 00sr rrrr iioo
						 * s = 0 = load, 1 = store
						 * ii = 16 bits register index, 11 = X, 10 = Y, 00 = Z
						 * oo = 1) post increment, 2) pre-decrement
						 */
						case 0x900c:
						case 0x900d:
						case 0x900e:  {	// LD -- Load Indirect from Data using X -- 1001 000d dddd 11oo
jit_generate(opcode, "int op = opcode & 3;\n"
"get_d5(opcode);\n"
"uint16_t x = (avr_data[R_XH] << 8) | avr_data[R_XL];\n"
"STATE(\"ld %s, %sX[%04x]%s\\n\", avr_regname(d), op == 2 ? \"--\" : \"\", x, op == 1 ? \"++\" : \"\");\n"
"cycle++; // 2 cycles (1 for tinyavr, except with inc/dec 2)\n"
"if (op == 2) x--;\n"
"uint8_t vd = _avr_get_ram(avr, x);\n"
"if (op == 1) x++;\n"
"_avr_set_r16le_hl(avr, R_XL, x);\n"
"_avr_set_r(avr, d, vd);\n"
);
						}	break;
						case 0x920c:
						case 0x920d:
						case 0x920e:  {	// ST -- Store Indirect Data Space X -- 1001 001d dddd 11oo
jit_generate(opcode, "int op = opcode & 3;\n"
"get_vd5(opcode);\n"
"uint16_t x = (avr_data[R_XH] << 8) | avr_data[R_XL];\n"
"STATE(\"st %sX[%04x]%s, %s[%02x] \\n\", op == 2 ? \"--\" : \"\", x, op == 1 ? \"++\" : \"\", avr_regname(d), vd);\n"
"cycle++; // 2 cycles, except tinyavr\n"
"if (op == 2) x--;\n"
"_avr_set_ram(avr, x, vd);\n"
"if (op == 1) x++;\n"
"_avr_set_r16le_hl(avr, R_XL, x);\n"
);
						}	break;
						case 0x9009:
						case 0x900a:  {	// LD -- Load Indirect from Data using Y -- 1001 000d dddd 10oo
jit_generate(opcode, "int op = opcode & 3;\n"
"get_d5(opcode);\n"
"uint16_t y = (avr_data[R_YH] << 8) | avr_data[R_YL];\n"
"STATE(\"ld %s, %sY[%04x]%s\\n\", avr_regname(d), op == 2 ? \"--\" : \"\", y, op == 1 ? \"++\" : \"\");\n"
"cycle++; // 2 cycles, except tinyavr\n"
"if (op == 2) y--;\n"
"uint8_t vd = _avr_get_ram(avr, y);\n"
"if (op == 1) y++;\n"
"_avr_set_r16le_hl(avr, R_YL, y);\n"
"_avr_set_r(avr, d, vd);\n"
);
						}	break;
						case 0x9209:
						case 0x920a:  {	// ST -- Store Indirect Data Space Y -- 1001 001d dddd 10oo
jit_generate(opcode, "int op = opcode & 3;\n"
"get_vd5(opcode);\n"
"uint16_t y = (avr_data[R_YH] << 8) | avr_data[R_YL];\n"
"STATE(\"st %sY[%04x]%s, %s[%02x]\\n\", op == 2 ? \"--\" : \"\", y, op == 1 ? \"++\" : \"\", avr_regname(d), vd);\n"
"cycle++;\n"
"if (op == 2) y--;\n"
"_avr_set_ram(avr, y, vd);\n"
"if (op == 1) y++;\n"
"_avr_set_r16le_hl(avr, R_YL, y);\n"
);
						}	break;
						case 0x9200:  {	// STS -- Store Direct to Data Space, 32 bits -- 1001 0010 0000 0000
jit_generate(opcode, "get_vd5(opcode);\n"
"uint16_t x = _avr_flash_read16le(avr, new_pc);\n"
"new_pc += 2;\n"
"STATE(\"sts 0x%04x, %s[%02x]\\n\", x, avr_regname(d), vd);\n"
"cycle++;\n"
"_avr_set_ram(avr, x, vd);\n"
);
						}	break;
						case 0x9001:
						case 0x9002:  {	// LD -- Load Indirect from Data using Z -- 1001 000d dddd 00oo
jit_generate(opcode, "int op = opcode & 3;\n"
"get_d5(opcode);\n"
"uint16_t z = (avr_data[R_ZH] << 8) | avr_data[R_ZL];\n"
"STATE(\"ld %s, %sZ[%04x]%s\\n\", avr_regname(d), op == 2 ? \"--\" : \"\", z, op == 1 ? \"++\" : \"\");\n"
"cycle++;; // 2 cycles, except tinyavr\n"
"if (op == 2) z--;\n"
"uint8_t vd = _avr_get_ram(avr, z);\n"
"if (op == 1) z++;\n"
"_avr_set_r16le_hl(avr, R_ZL, z);\n"
"_avr_set_r(avr, d, vd);\n"
);
						}	break;
						case 0x9201:
						case 0x9202:  {	// ST -- Store Indirect Data Space Z -- 1001 001d dddd 00oo
jit_generate(opcode, "int op = opcode & 3;\n"
"get_vd5(opcode);\n"
"uint16_t z = (avr_data[R_ZH] << 8) | avr_data[R_ZL];\n"
"STATE(\"st %sZ[%04x]%s, %s[%02x] \\n\", op == 2 ? \"--\" : \"\", z, op == 1 ? \"++\" : \"\", avr_regname(d), vd);\n"
"cycle++; // 2 cycles, except tinyavr\n"
"if (op == 2) z--;\n"
"_avr_set_ram(avr, z, vd);\n"
"if (op == 1) z++;\n"
"_avr_set_r16le_hl(avr, R_ZL, z);\n"
);
						}	break;
						case 0x900f:  {	// POP -- 1001 000d dddd 1111
jit_generate(opcode, "get_d5(opcode);\n"
"_avr_set_r(avr, d, _avr_pop8(avr));\n"
"T(uint16_t sp = _avr_sp_get(avr);)\n"
"STATE(\"pop %s (@%04x)[%02x]\\n\", avr_regname(d), sp, avr_data[sp]);\n"
"cycle++;\n"
);
						}	break;
						case 0x920f:  {	// PUSH -- 1001 001d dddd 1111
jit_generate(opcode, "get_vd5(opcode);\n"
"_avr_push8(avr, vd);\n"
"T(uint16_t sp = _avr_sp_get(avr);)\n"
"STATE(\"push %s[%02x] (@%04x)\\n\", avr_regname(d), vd, sp);\n"
"cycle++;\n"
);
						}	break;
						case 0x9400:  {	// COM -- One’s Complement -- 1001 010d dddd 0000
jit_generate(opcode, "get_vd5(opcode);\n"
"uint8_t res = 0xff - vd;\n"
"STATE(\"com %s[%02x] = %02x\\n\", avr_regname(d), vd, res);\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_znv0s(avr, res);\n"
"SREG_SETBIT(S_I, 1);\n"
"SREG();\n"
);
						}	break;
						case 0x9401:  {	// NEG -- Two’s Complement -- 1001 010d dddd 0001
jit_generate(opcode, "get_vd5(opcode);\n"
"uint8_t res = 0x00 - vd;\n"
"STATE(\"neg %s[%02x] = %02x\\n\", avr_regname(d), vd, res);\n"
"_avr_set_r(avr, d, res);\n"
"SREG_SETBIT(S_H, ((res >> 3) | (vd >> 3)) & 1);\n"
"SREG_SETBIT(S_V, res == 0x80);\n"
"SREG_SETBIT(S_I, res != 0);\n"
"_avr_flags_zns(avr, res);\n"
"SREG();\n"
);
						}	break;
						case 0x9402:  {	// SWAP -- Swap Nibbles -- 1001 010d dddd 0010
jit_generate(opcode, "get_vd5(opcode);\n"
"uint8_t res = (vd >> 4) | (vd << 4) ;\n"
"STATE(\"swap %s[%02x] = %02x\\n\", avr_regname(d), vd, res);\n"
"_avr_set_r(avr, d, res);\n"
);
						}	break;
						case 0x9403:  {	// INC -- Increment -- 1001 010d dddd 0011
jit_generate(opcode, "get_vd5(opcode);\n"
"uint8_t res = vd + 1;\n"
"STATE(\"inc %s[%02x] = %02x\\n\", avr_regname(d), vd, res);\n"
"_avr_set_r(avr, d, res);\n"
"SREG_SETBIT(S_V, res == 0x80);\n"
"_avr_flags_zns(avr, res);\n"
"SREG();\n"
);
						}	break;
						case 0x9405:  {	// ASR -- Arithmetic Shift Right -- 1001 010d dddd 0101
jit_generate(opcode, "get_vd5(opcode);\n"
"uint8_t res = (vd >> 1) | (vd & 0x80);\n"
"STATE(\"asr %s[%02x]\\n\", avr_regname(d), vd);\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_zcnvs(avr, res, vd);\n"
"SREG();\n"
);
						}	break;
						case 0x9406:  {	// LSR -- Logical Shift Right -- 1001 010d dddd 0110
jit_generate(opcode, "get_vd5(opcode);\n"
"uint8_t res = vd >> 1;\n"
"STATE(\"lsr %s[%02x]\\n\", avr_regname(d), vd);\n"
"_avr_set_r(avr, d, res);\n"
"SREG_SETBIT(S_N, 0);\n"
"_avr_flags_zcvs(avr, res, vd);\n"
"SREG();\n"
);
						}	break;
						case 0x9407:  {	// ROR -- Rotate Right -- 1001 010d dddd 0111
jit_generate(opcode, "get_vd5(opcode);\n"
"uint8_t res = (SREG_BIT(S_C) ? 0x80 : 0) | vd >> 1;\n"
"STATE(\"ror %s[%02x]\\n\", avr_regname(d), vd);\n"
"_avr_set_r(avr, d, res);\n"
"_avr_flags_zcnvs(avr, res, vd);\n"
"SREG();\n"
);
						}	break;
						case 0x940a:  {	// DEC -- Decrement -- 1001 010d dddd 1010
jit_generate(opcode, "get_vd5(opcode);\n"
"uint8_t res = vd - 1;\n"
"STATE(\"dec %s[%02x] = %02x\\n\", avr_regname(d), vd, res);\n"
"_avr_set_r(avr, d, res);\n"
"SREG_SETBIT(S_V, res == 0x7f);\n"
"_avr_flags_zns(avr, res);\n"
"SREG();\n"
);
						}	break;
						case 0x940c:
						case 0x940d:  {	// JMP -- Long Call to sub, 32 bits -- 1001 010a aaaa 110a
jit_generate(opcode, "avr_flashaddr_t a = ((opcode & 0x01f0) >> 3) | (opcode & 1);\n"
"uint16_t x = _avr_flash_read16le(avr, new_pc);\n"
"a = (a << 16) | x;\n"
"STATE(\"jmp 0x%06x\\n\", a);\n"
"new_pc = a << 1;\n"
"cycle += 2;\n"
"TRACE_JUMP();\n"
);
						}	break;
						case 0x940e:
						case 0x940f:  {	// CALL -- Long Call to sub, 32 bits -- 1001 010a aaaa 111a
jit_generate(opcode, "avr_flashaddr_t a = ((opcode & 0x01f0) >> 3) | (opcode & 1);\n"
"uint16_t x = _avr_flash_read16le(avr, new_pc);\n"
"a = (a << 16) | x;\n"
"STATE(\"call 0x%06x\\n\", a);\n"
"new_pc += 2;\n"
"cycle += 1 + _avr_push_addr(avr, new_pc);\n"
"new_pc = a << 1;\n"
"TRACE_JUMP();\n"
"STACK_FRAME_PUSH();\n"
);
						}	break;

						default: {
							switch (opcode & 0xff00) {
								case 0x9600:  {	// ADIW -- Add Immediate to Word -- 1001 0110 KKpp KKKK
jit_generate(opcode, "get_vp2_k6(opcode);\n"
"uint16_t res = vp + k;\n"
"STATE(\"adiw %s:%s[%04x], 0x%02x\\n\", avr_regname(p), avr_regname(p + 1), vp, k);\n"
"_avr_set_r16le_hl(avr, p, res);\n"
"SREG_SETBIT(S_V, ((~vp & res) >> 15) & 1);\n"
"SREG_SETBIT(S_I, ((~res & vp) >> 15) & 1);\n"
"_avr_flags_zns16(avr, res);\n"
"SREG();\n"
"cycle++;\n"
);
								}	break;
								case 0x9700:  {	// SBIW -- Subtract Immediate from Word -- 1001 0111 KKpp KKKK
jit_generate(opcode, "get_vp2_k6(opcode);\n"
"uint16_t res = vp - k;\n"
"STATE(\"sbiw %s:%s[%04x], 0x%02x\\n\", avr_regname(p), avr_regname(p + 1), vp, k);\n"
"_avr_set_r16le_hl(avr, p, res);\n"
"SREG_SETBIT(S_V, ((vp & ~res) >> 15) & 1);\n"
"SREG_SETBIT(S_I, ((res & ~vp) >> 15) & 1);\n"
"_avr_flags_zns16(avr, res);\n"
"SREG();\n"
"cycle++;\n"
);
								}	break;
								case 0x9800:  {	// CBI -- Clear Bit in I/O Register -- 1001 1000 AAAA Abbb
jit_generate(opcode, "get_io5_b3mask(opcode);\n"
"uint8_t res = _avr_get_ram(avr, io) & ~mask;\n"
"STATE(\"cbi %s[%04x], 0x%02x = %02x\\n\", avr_regname(io), avr_data[io], mask, res);\n"
"_avr_set_ram(avr, io, res);\n"
"cycle++;\n"
);
								}	break;
								case 0x9900: {
									const int skip = 1 + _avr_is_instruction_32_bits(avr, new_pc);
									 {	// SBIC -- Skip if Bit in I/O Register is Cleared -- 1001 1001 AAAA Abbb
										jit_generate_literal("const int skip = %d", skip);
jit_generate(opcode, "get_io5_b3mask(opcode);\n"
"uint8_t res = _avr_get_ram(avr, io) & mask;\n"
"STATE(\"sbic %s[%04x], 0x%02x\\t; Will%s branch\\n\", avr_regname(io), avr_data[io], mask, !res?\"\":\" not\");\n"
"if (!res) {\n"
"new_pc += 2 * skip; cycle += skip;\n"
"TRACE_JUMP();\n"
"}\n"
);
									}	break;
								}	break;
								case 0x9a00:  {	// SBI -- Set Bit in I/O Register -- 1001 1010 AAAA Abbb
jit_generate(opcode, "get_io5_b3mask(opcode);\n"
"uint8_t res = _avr_get_ram(avr, io) | mask;\n"
"STATE(\"sbi %s[%04x], 0x%02x = %02x\\n\", avr_regname(io), avr_data[io], mask, res);\n"
"_avr_set_ram(avr, io, res);\n"
"cycle++;\n"
);
								}	break;
								case 0x9b00: {
									const int skip = 1 + _avr_is_instruction_32_bits(avr, new_pc);
									 {	// SBIS -- Skip if Bit in I/O Register is Set -- 1001 1011 AAAA Abbb
										jit_generate_literal("const int skip = %d", skip);
jit_generate(opcode, "get_io5_b3mask(opcode);\n"
"uint8_t res = _avr_get_ram(avr, io) & mask;\n"
"STATE(\"sbis %s[%04x], 0x%02x\\t; Will%s branch\\n\", avr_regname(io), avr_data[io], mask, res?\"\":\" not\");\n"
"if (res) {\n"
"new_pc += 2 * skip; cycle += skip;\n"
"TRACE_JUMP();\n"
"}\n"
);
									}	break;
								}	break;
								default:
									switch (opcode & 0xfc00) {
										case 0x9c00:  {	// MUL -- Multiply Unsigned -- 1001 11rd dddd rrrr
jit_generate(opcode, "get_vd5_vr5(opcode);\n"
"uint16_t res = vd * vr;\n"
"STATE(\"mul %s[%02x], %s[%02x] = %04x\\n\", avr_regname(d), vd, avr_regname(r), vr, res);\n"
"cycle++;\n"
"_avr_set_r16le(avr, 0, res);\n"
"SREG_SETBIT(S_Z, res == 0);\n"
"SREG_SETBIT(S_I, (res >> 15) & 1);\n"
"SREG();\n"
);
										}	break;
										default: _avr_invalid_opcode(avr);
									}
							}
						}	break;
					}
				}	break;
			}
		}	break;

		case 0xb000: {
			switch (opcode & 0xf800) {
				case 0xb800:  {	// OUT A,Rr -- 1011 1AAd dddd AAAA
jit_generate(opcode, "get_d5_a6(opcode);\n"
"STATE(\"out %s, %s[%02x]\\n\", avr_regname(A), avr_regname(d), avr_data[d]);\n"
"_avr_set_ram(avr, A, avr_data[d]);\n"
);
				}	break;
				case 0xb000:  {	// IN Rd,A -- 1011 0AAd dddd AAAA
jit_generate(opcode, "get_d5_a6(opcode);\n"
"STATE(\"in %s, %s[%02x]\\n\", avr_regname(d), avr_regname(A), avr_data[A]);\n"
"_avr_set_r(avr, d, _avr_get_ram(avr, A));\n"
);
				}	break;
				default: _avr_invalid_opcode(avr);
			}
		}	break;

		case 0xc000:  {	// RJMP -- 1100 kkkk kkkk kkkk
jit_generate(opcode, "get_o12(opcode);\n"
"STATE(\"rjmp .%d [%04x]\\n\", o >> 1, new_pc + o);\n"
"new_pc = (new_pc + o) % (avr_flashend + 1);\n"
"cycle++;\n"
"TRACE_JUMP();\n"
);
		}	break;

		case 0xd000:  {	// RCALL -- 1101 kkkk kkkk kkkk
jit_generate(opcode, "get_o12(opcode);\n"
"STATE(\"rcall .%d [%04x]\\n\", o >> 1, new_pc + o);\n"
"cycle += _avr_push_addr(avr, new_pc);\n"
"new_pc = (new_pc + o) % (avr_flashend + 1);\n"
"// 'rcall .1' is used as a cheap \"push 16 bits of room on the stack\"\n"
"if (o != 0) {\n"
"TRACE_JUMP();\n"
"STACK_FRAME_PUSH();\n"
"}\n"
);
		}	break;

		case 0xe000:  {	// LDI Rd, K aka SER (LDI r, 0xff) -- 1110 kkkk dddd kkkk
jit_generate(opcode, "get_h4_k8(opcode);\n"
"STATE(\"ldi %s, 0x%02x\\n\", avr_regname(h), k);\n"
"_avr_set_r(avr, h, k);\n"
);
		}	break;

		case 0xf000: {
			switch (opcode & 0xfe00) {
				case 0xf000:
				case 0xf200:
				case 0xf400:
				case 0xf600:  {	// BRXC/BRXS -- All the SREG branches -- 1111 0Boo oooo osss
jit_generate(opcode, "int16_t o = ((int16_t)(opcode << 6)) >> 9; // offset\n"
"uint8_t s = opcode & 7;\n"
"int set = (opcode & 0x0400) == 0;		// this bit means BRXC otherwise BRXS\n"
"int branch = (SREG_BIT(s) && set) || (!SREG_BIT(s) && !set);\n"
"#ifdef CONFIG_SIMAVR_TRACE\n"
"if (brxc_names[set][s]) {\n"
"STATE(\"%s .%d [%04x]\\t; Will%s branch\\n\", brxc_names[set][s], o, new_pc + (o << 1), branch ? \"\":\" not\");\n"
"} else {\n"
"STATE(\"%s%c .%d [%04x]\\t; Will%s branch\\n\", set ? \"brbs\" : \"brbc\", _sreg_bit_name[s], o, new_pc + (o << 1), branch ? \"\":\" not\");\n"
"}\n"
"#endif\n"
"if (branch) {\n"
"cycle++; // 2 cycles if taken, 1 otherwise\n"
"new_pc = new_pc + (o << 1);\n"
"TRACE_JUMP();\n"
"}\n"
);
				}	break;
				case 0xf800:
				case 0xf900:  {	// BLD -- Bit Store from T into a Bit in Register -- 1111 100d dddd 0bbb
jit_generate(opcode, "get_vd5_s3_mask(opcode);\n"
"uint8_t v = (vd & ~mask) | (SREG_BIT(S_T) ? mask : 0);\n"
"STATE(\"bld %s[%02x], 0x%02x = %02x\\n\", avr_regname(d), vd, mask, v);\n"
"_avr_set_r(avr, d, v);\n"
);
				}	break;
				case 0xfa00:
				case 0xfb00:  {	// BST -- Bit Store into T from bit in Register -- 1111 101d dddd 0bbb
jit_generate(opcode, "get_vd5_s3(opcode);\n"
"STATE(\"bst %s[%02x], 0x%02x\\n\", avr_regname(d), vd, 1 << s);\n"
"SREG_SETBIT(S_T, (vd >> s) & 1);\n"
"SREG();\n"
);
				}	break;
				case 0xfc00:
				case 0xfe00: {
					const int skip = 1 + _avr_is_instruction_32_bits(avr, new_pc);
					 {	// SBRS/SBRC -- Skip if Bit in Register is Set/Clear -- 1111 11sd dddd 0bbb
						jit_generate_literal("const int skip = %d", skip);
jit_generate(opcode, "get_vd5_s3_mask(opcode);\n"
"int set = (opcode & 0x0200) != 0;\n"
"int branch = ((vd & mask) && set) || (!(vd & mask) && !set);\n"
"STATE(\"%s %s[%02x], 0x%02x\\t; Will%s branch\\n\", set ? \"sbrs\" : \"sbrc\", avr_regname(d), vd, mask, branch ? \"\":\" not\");\n"
"if (branch) {\n"
"new_pc += 2 * skip; cycle += skip;\n"
"TRACE_JUMP();\n"
"}\n"
);
					}	break;
				}	break;
				default: _avr_invalid_opcode(avr);
			}
		}	break;

		default: _avr_invalid_opcode(avr);

	}
#endif /*__SIM_CORE_JIT_H__*/
