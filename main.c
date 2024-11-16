#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xis.h"

int debug = 0;

int labelC = 0;

const char *x_tr_reg(int reg)
{
    const char *x86_registers[] = {
        "%rax", "%rbx", "%rcx", "%rdx",
        "%rsi", "%rdi", "%r8", "%r9",
        "%r10", "%r11", "%r12", "%r13",
        "%r14", "%rbp", "%rsp"};
    if (reg >= 0 && reg < 15)
    {
        return x86_registers[reg];
    }
    return "%r15";
}

void translate_ret();
void translate_cld();
void translate_std();

void translate_zero(unsigned char opcode)
{
    switch (opcode)
    {
    case 0x01:
        translate_ret();
        break;
    case 0x02:
        translate_cld();
        break;
    case 0x03:
        translate_std();
        break;
    default:
        break;
    }
}

void translate_ret()
{
    printf("    pop %%rbp\n");
    printf("    ret\n");
}

void translate_cld()
{
    debug = 0;
}

void translate_std()
{
    debug = 1;
}

void translate_one_operand(unsigned char opcode, unsigned char operand)
{
    unsigned char reg0 = operand >> 4;

    switch (opcode)
    {
    case 0x41:
        printf("    neg %s\n", x_tr_reg(reg0));
        break;
    case 0x42:
        printf("    not %s\n", x_tr_reg(reg0));
        break;
    case 0x48:
        printf("    inc %s\n", x_tr_reg(reg0));
        break;
    case 0x49:
        printf("    dec %s\n", x_tr_reg(reg0));
        break;
    case 0x43:
        printf("    push %s\n", x_tr_reg(reg0));
        break;
    case 0x44:
        printf("    pop %s\n", x_tr_reg(reg0));
        break;
    case 0x47:
        printf("    mov %s, %%rdi\n", x_tr_reg(reg0));
        printf("    mov $1, %%rax\n");
        printf("    mov $1, %%rdx\n");
        printf("    syscall\n");
        break;
    case 0x61:
        printf("    cmp $0, %%r15\n");
        printf("    jne .L%04x\n", labelC);
        break;
    case 0x62:
        printf("    jmp .L%04x:\n", labelC);
        break;
    default:
        break;
    }
}

void translate_add(unsigned char regS, unsigned char regD)
{
    printf("    add %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
}

void translate_sub(unsigned char regS, unsigned char regD)
{
    printf("    sub %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
}

void translate_mul(unsigned char regS, unsigned char regD)
{
    printf("    imul %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
}

void translate_and(unsigned char regS, unsigned char regD)
{
    printf("    and %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
}

void translate_or(unsigned char regS, unsigned char regD)
{
    printf("    or %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
}

void translate_xor(unsigned char regS, unsigned char regD)
{
    printf("    xor %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
}

void translate_test(unsigned char regS, unsigned char regD)
{
    printf("    test %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
    printf("    setnz %%r15b\n");
}

void translate_cmp(unsigned char regS, unsigned char regD)
{
    printf("    cmp %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
    printf("    setg %%r15b\n");
}

void translate_equ(unsigned char regS, unsigned char regD)
{
    printf("    cmp %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
    printf("    setz %%r15b\n");
}

void translate_mov(unsigned char regS, unsigned char regD)
{
    printf("    mov %s, %s\n", x_tr_reg(regS), x_tr_reg(regD));
}

void translate_two_operands(unsigned char opcode, unsigned char regS, unsigned char regD)
{
    switch (opcode)
    {
    case 0x81:
        translate_add(regS, regD);
        break;
    case 0x82:
        translate_sub(regS, regD);
        break;
    case 0x83:
        translate_mul(regS, regD);
        break;
    case 0x85:
        translate_and(regS, regD);
        break;
    case 0x86:
        translate_or(regS, regD);
        break;
    case 0x87:
        translate_xor(regS, regD);
        break;
    case 0x8A:
        translate_test(regS, regD);
        break;
    case 0x8B:
        translate_cmp(regS, regD);
        break;
    case 0x8C:
        translate_equ(regS, regD);
        break;
    case 0x8D:
        translate_mov(regS, regD);
        break;
    default:
        break;
    }
}

void translate_loadi(int regD, unsigned short immediate)
{
    printf("    mov $%d, %s\n", immediate, x_tr_reg(regD));
}

void translate_extended(unsigned char opcode, unsigned char operand, FILE *file)
{
    unsigned char b1;
    unsigned char b2;
    fread(&b1, 1, 1, file);
    fread(&b2, 1, 1, file);
    unsigned short tt = (b1 << 8) | b2;
    switch (opcode)
    {
    case 0xE1:
        translate_loadi(operand >> 4, tt);
        break;
    case 0xC1:
        printf("    jmp .L%04x\n", tt);
        break;
    default:
        break;
    }
}
void translate_to_asm(unsigned char b1, unsigned char b2, FILE *file)
{
    unsigned char instructionType = b1 >> 6;
    unsigned char reg1, reg2;
    unsigned char operand = b2;

    printf(".L%04x:\n", labelC);
    if (debug)
    {
        printf("    call debug\n");
    }
    switch (instructionType)
    {
    case 0:
        translate_zero(b1);
        labelC += 2;
        break;
    case 1:
        translate_one_operand(b1, operand);
        labelC += 2;
        break;
    case 2:
        reg1 = b2 >> 4;
        reg2 = b2 & 0x0F;
        translate_two_operands(b1, reg1, reg2);
        labelC += 2;
        break;
    case 3:
        translate_extended(b1, operand, file);
        labelC += 4;
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    printf(".globl test\n");
    printf("test:\n");
    printf("    push %%rbp\n");
    printf("    mov %%rsp, %%rbp\n");

    unsigned char b1;
    unsigned char b2;

    fread(&b1, 1, 1, file);
    fread(&b2, 1, 1, file);

    while (!(b1 == 0 && b2 == 0))
    {
        translate_to_asm(b1, b2, file);
        fread(&b1, 1, 1, file);
        fread(&b2, 1, 1, file);
    }
    printf(".L%04x:\n", labelC);
    if (debug)
    {
        printf("    call debug\n");
    }
    printf("    pop %%rbp\n");
    printf("    ret\n");
    fclose(file);
    return 0;
}
