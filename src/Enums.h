#ifndef ENUMS_H
#define ENUMS_H

enum SectionLabel
{
    OTHER = 0,
    TEXT,
    DATA,
    BSS,
    START,
    UND
};
enum TokenType
{
    LABEL,
    SECTION,
    EXT,
    GLB,
    INSTRUCTION,
    INCORECT,
    DIRECTIVE,
    SYMBOL,
    OP_DEC,
    OP_SYM_VALUE,
    EXP,
    END
};
enum Scope
{
    LOCAL,
    GLOBAL
};
enum AddressingType
{
    IMM,
    REG_DIR,
    REG_IND,
    REG_IND_POM8,
    REG_IND_POM16,
    MEM,
    PC_POM,
    
};
enum RelType{
    R_386_32,R_386_PC32
};
#endif