# Instruction Set


## Instructions

| Instruction | Opcode | Layout          |
|-------------|--------|-----------------|
| `NOP`       | `0x00` |                 |
| `ADD`       | `0x01` | rd, r1, r2      |
| `SUB`       | `0x02` | rd, r1, r2      |
| `ADDI`      | `0x03` | rd, r1, imm16   |
| `SUBI`      | `0x04` | rd, r1, imm16   |
| `LB`        | `0x05` | rd, r1, imm16   |
| `LH`        | `0x06` | rd, r1, imm16   |
| `LW`        | `0x07` | rd, r1, imm16   |
| `LBU`       | `0x08` | rd, r1, imm16   |
| `LHU`       | `0x09` | rd, r1, imm16   |
| `SB`        | `0x0A` | rd, r1, imm16   |
| `SH`        | `0x0B` | rd, r1, imm16   |
| `SW`        | `0x0C` | rd, r1, imm16   |
| `LI`        | N/A    | rd, imm32       |
| `LUI`       | `0x0D` | rd, imm20       |
| `MV`        | N/A    | rd, r1          |
| `NEG`       | N/A    | rd, r1          |
| `AND`       | `0x0E` | rd, r1, r2      |
| `ANDI`      | `0x0F` | rd, r1, imm16   |
| `OR`        | `0x10` | rd, r1, r2      |
| `ORI`       | `0x11` | rd, r1, imm16   |
| `XOR`       | `0x12` | rd, r1, r2      |
| `XORI`      | `0x13` | rd, r1, imm16   |
| `NOT`       | N/A    | rd, r1          |
| `BCLR`      | `0x14` | rd, r1, r2      |
| `BCLRI`     | `0x15` | rd, r1, imm5    |
| `BSET`      | `0x16` | rd, r1, r2      |
| `BSETI`     | `0x17` | rd, r1, imm5    |
| `BINV`      | `0x18` | rd, r1, r2      |
| `BINVI`     | `0x19` | rd, r1, imm5    |
| `BEXT`      | `0x1A` | rd, r1, r2      |
| `BEXTI`     | `0x1B` | rd, r1, imm5    |
| `SLL`       | `0x1C` | rd, r1, r2      |
| `SLLI`      | `0x1D` | rd, r1, imm5    |
| `SRL`       | `0x1E` | rd, r1, r2      |
| `SRLI`      | `0x1F` | rd, r1, imm5    |
| `SRA`       | `0x20` | rd, r1, r2      |
| `SRAI`      | `0x21` | rd, r1, imm5    |
| `J`         | N/A    | label           |
| `JAL`       | `0x22` | rd, imm20       |
| `JR`        | N/A    | label           |
| `JALR`      | `0x23` | rd, r1, imm16   |
| `AUIPC`     | `0x24` | rd, imm20       |
| `CALL`      | N/A    | label           |
| `TAIL`      | N/A    | label           |
| `RET`       | N/A    |                 |
| `BEQ`       | `0x25` | rs1, rs2, label |
| `BNE`       | `0x26` | rs1, rs2, label |
| `BLT`       | `0x27` | rs1, rs2, label |
| `BGT`       | N/A    | rs1, rs2, label |
| `BGE`       | `0x28` | rs1, rs2, label |
| `BLE`       | N/A    | rs1, rs2, label |
| `BLTU`      | `0x29` | rs1, rs2, label |
| `BGTU`      | N/A    | rs1, rs2, label |
| `BGEU`      | `0x2A` | rs1, rs2, label |
| `BLEU`      | N/A    | rs1, rs2, label |