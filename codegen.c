#include "ktcc.h"
#include <stdio.h>

// 分岐ラベルを作成するための通し番号
static unsigned long branch_serial_no = 0;

static void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  unsigned long end_no = branch_serial_no++;
  unsigned long else_no = branch_serial_no++;
  unsigned long while_no = branch_serial_no++;
  unsigned long for_no = branch_serial_no++;
  List *l;

  switch (node->kind) {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_IF:
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lelse%ld\n", else_no);
    gen(node->then);
    printf("  jmp .Lend%ld\n", end_no);
    printf(".Lelse%ld:\n", else_no);
    if (node->els) {
      gen(node->els);
    }
    printf(".Lend%ld:\n", end_no);
    return;
  case ND_WHILE:
    printf(".Lwhile%ld:\n", while_no);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%ld\n", end_no);
    gen(node->then);
    printf("  jmp .Lwhile%ld\n", while_no);
    printf(".Lend%ld:\n", end_no);
    return;
  case ND_FOR:
    if (node->init) {
      gen(node->init);
    }
    printf(".Lfor%ld:\n", for_no);
    if (node->cond) {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%ld\n", end_no);
    }
    gen(node->then);
    if (node->inc) {
      gen(node->inc);
    }
    printf("  jmp .Lfor%ld\n", for_no);
    printf(".Lend%ld:\n", end_no);
    return;
  case ND_BLOCK:
    l = node->stmts;
    while (l) {
      gen(l->data);
      l = l->next;
      printf("  pop rax\n");
    }
    return;
  case ND_FUNC:
    l = node->args;
    if (l) {
      gen(l->data);
      printf("  pop rax\n");
      printf("  mov rdi, rax\n");
      l = l->next;
    }
    if (l) {
      gen(l->data);
      printf("  pop rax\n");
      printf("  mov rsi, rax\n");
      l = l->next;
    }
    if (l) {
      gen(l->data);
      printf("  pop rax\n");
      printf("  mov rdx, rax\n");
      l = l->next;
    }
    if (l) {
      gen(l->data);
      printf("  pop rax\n");
      printf("  mov rcx, rax\n");
      l = l->next;
    }
    if (l) {
      gen(l->data);
      printf("  pop rax\n");
      printf("  mov r8, rax\n");
      l = l->next;
    }
    if (l) {
      gen(l->data);
      printf("  pop rax\n");
      printf("  mov r9, rax\n");
      l = l->next;
    }
    printf("  call %s\n", node->func_name);
    printf("  push rax\n");
    return;
  case ND_FUNC_DEF:
    printf("%s:\n", node->func_def_name);

    // ベースポインタを変更
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");

    // localsを差し替える
    LVar *l = locals;
    locals = node->def_args;

    // 引数の数を調べる
    int nargs = 0;
    for (LVar *p = l; p; p = p->next) {
      nargs++;
    }

    // レジスタから引数の数だけスタックへ引数を移す
    switch (nargs) {
    case 6:
      printf("  push r9\n");
    case 5:
      printf("  push r8\n");
    case 4:
      printf("  push rcx\n");
    case 3:
      printf("  push rdx\n");
    case 2:
      printf("  push rsi\n");
    case 1:
      printf("  push rdi\n");
    default:
      break;
    }

    // 変数26個分を確保しておく
    printf("  sub rsp, 208\n"); // 26 * 8 = 208

    // 関数のコードを生成
    gen(node->func_body);

    // localsを戻す
    locals = l;

    // ベースポインタを戻す
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("	ret\n");

    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}

