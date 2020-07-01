#include "ktcc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 分岐ラベルを作成するための通し番号
static unsigned long branch_serial_no = 0;

static void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->lvar->offset);
  printf("  push rax\n");
}

static void gen_gval(Node *node) {
  if (node->kind != ND_GVAR) {
    error("代入の左辺値が変数ではありません");
  }

  char label[MAX_VARIABLE_NAME_LENGTH];
  snprintf(label, node->gvar->len + 1, "%s", node->gvar->name);

  printf("  lea rax, %s[rip]\n", label);
  printf("  push rax\n");
}

static void gen_str_literal(Node *node) {
  printf("  lea rax, %s[rip]\n", node->str_literal->label);
  printf("  push rax\n");
}

// ローカル変数に対する加算
static void add_lvar(Node *node) {
  switch (node->lhs->lvar->ty->kind) {
  case CHAR:
    // fallthrough
  case INT:
    printf("  add rax, rdi\n");
    break;
  case ARRAY:
    // fallthrough
  case PTR:
    printf("  imul rdi, %d\n", alloc_size(node->lhs->lvar->ty->ptr_to));
    printf("  add rax, rdi\n");
    break;
  default:
    error("不明な型に対する+演算です");
  }
}

// ローカル変数に対する減算
static void sub_lvar(Node *node) {
  switch (node->lhs->lvar->ty->kind) {
  case CHAR:
    // fallthrough
  case INT:
    printf("  sub rax, rdi\n");
    break;
  case ARRAY:
    // fallthrough
  case PTR:
    printf("  imul rdi, %d\n", alloc_size(node->lhs->lvar->ty->ptr_to));
    printf("  sub rax, rdi\n");
    break;
  default:
    error("不明な型に対する-演算です");
  }
}

void gen(Node *node) {
  unsigned long end_no = branch_serial_no++;
  unsigned long else_no = branch_serial_no++;
  unsigned long while_no = branch_serial_no++;
  unsigned long for_no = branch_serial_no++;
  List *l;

  switch (node->kind) {
  case ND_NUM:
    debug_comment("ND_NUM");
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    debug_comment("ND_LVAR");
    gen_lval(node);
    switch (node->lvar->ty->kind) {
    case CHAR:
      printf("  pop rax\n");
      printf("  movsx rax, BYTE PTR [rax]\n");
      printf("  push rax\n");
      break;
    case INT:
      // fallthrough
    case PTR:
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      break;
    case ARRAY:
      // 配列ならスタックに配列の先頭のアドレスを入れたままにしておく
      break;
    default:
      error("不明な型の変数が見つかりました");
      break;
    }
    return;
  case ND_GVAR:
    debug_comment("ND_GVAR");
    gen_gval(node);

    switch (node->gvar->ty->kind) {
    case CHAR:
      printf("  pop rax\n");
      printf("  movsx rax, BYTE PTR [rax]\n");
      printf("  push rax\n");
      break;
    case INT:
    case PTR:
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      break;
    case ARRAY:
      // 配列ならスタックに配列の先頭のアドレスを入れたままにしておく
      break;
    default:
      error("不明な型の変数が見つかりました");
      break;
    }
    return;
  case ND_STR_LITERAL:
    debug_comment("ND_STR_LITERAL");
    gen_str_literal(node);
    return;
  case ND_ASSIGN:
    debug_comment("ND_ASSIGN");
    if (node->lhs->kind == ND_DEREF) {
      gen(node->lhs->lhs);
    } else if (node->lhs->kind == ND_LVAR) {
      gen_lval(node->lhs);
    } else if (node->lhs->kind == ND_GVAR) {
      gen_gval(node->lhs);
    } else {
      gen_str_literal(node->lhs);
    }
    gen(node->rhs);

    Type *ty = type_of(node->lhs);
    if (!ty) {
      error("不正な代入式ですよ");
    }
    switch (ty->kind) {
    case CHAR:
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], dil\n");
      printf("  push rdi\n");
      break;
    case INT:
      // fallthrough
    case ARRAY:
      // fallthrough
    case PTR:
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      break;
    default:
      error("不正な代入式です");
      break;
    }
    return;
  case ND_RETURN:
    debug_comment("ND_RETURN");
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_IF:
    debug_comment("ND_IF");
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
    debug_comment("ND_WHILE");
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
    debug_comment("ND_FOR");
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
    debug_comment("ND_BLOCK");
    l = node->stmts;
    while (l) {
      gen(l->data);
      l = l->next;
    }
    return;
  case ND_FUNC:
    debug_comment("ND_FUNC");
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
    debug_comment("ND_FUNC_DEF");
    printf("%s:\n", node->func_def_name);

    // ベースポインタを変更
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");

    // localsを差し替える
    LVar *l = locals;
    locals = node->locals;

    // 引数の数を調べる
    int nargs = 0;
    for (LVar *p = locals; p; p = p->next) {
      if (p->is_arg) {
        nargs++;
      }
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

    // 変数領域を確保しておく
    unsigned int lvar_size = 0;
    for (LVar *p = locals; p; p = p->next) {
      switch (p->ty->kind) {
      case CHAR:
        // fallthrough
      case INT:
        // fallthrough
      case PTR:
        lvar_size += 8;
        break;
      case ARRAY:
        lvar_size += p->ty->array_size * 8;
        break;
      default:
        error("不明な型が見つかりました");
        break;
      }
    }
    // スタックポインタは16の倍数でなければならないので調整する
    lvar_size += lvar_size % 16;
    printf("  sub rsp, %d\n", lvar_size);

    // 関数のコードを生成
    gen(node->func_body);

    // localsを戻す
    locals = l;

    // ベースポインタを戻す
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");

    return;
  case ND_ADDR:
    debug_comment("ND_ADDR");
    gen_lval(node->lhs);
    return;
  case ND_DEREF:
    debug_comment("ND_DEREF");
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    debug_comment("ND_ADD");
    if (node->lhs->kind == ND_LVAR) {
      add_lvar(node);
    } else {
      printf("  add rax, rdi\n");
    }
    break;
  case ND_SUB:
    debug_comment("ND_SUB");
    if (node->lhs->kind == ND_LVAR) {
      sub_lvar(node);
    } else {
      printf("  sub rax, rdi\n");
    }
    break;
  case ND_MUL:
    debug_comment("ND_MUL");
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    debug_comment("ND_DIV");
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    debug_comment("ND_EQ");
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    debug_comment("ND_NE");
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    debug_comment("ND_LT");
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    debug_comment("ND_LE");
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}

void gen_global(GVar *globals) {
  if (!globals) {
    return;
  }

  if (!globals->initv) {
    char label[MAX_VARIABLE_NAME_LENGTH];
    snprintf(label, globals->len + 1, "%s", globals->name);

    printf("%s:\n", label);
    printf("  .zero %d\n", alloc_size(globals->ty));
  }

  gen_global(globals->next);
  return;
}

void gen_init_global(GVar *globals) {
  if (!globals) {
    return;
  }

  if (globals->initv) {
    char label[MAX_VARIABLE_NAME_LENGTH];
    snprintf(label, globals->len + 1, "%s", globals->name);

    printf("%s:\n", label);
    printf("  .long %d\n", globals->initv);
  }

  gen_init_global(globals->next);
  return;
}

void gen_str_literal_data(StrLiteral *literals, int count) {
  if (!literals) {
    return;
  }

  // 16 = ".LC" + count(max 12 chars) + NULL
  literals->label = calloc(1, 16);
  snprintf(literals->label, 16, ".LC%d", count);

  char *val = calloc(1, literals->len + 1);
  memcpy(val, literals->str, literals->len);
  val[literals->len] = '\0';

  printf("%s:\n", literals->label);
  printf("  .string \"%s\"\n", val);

  return gen_str_literal_data(literals->next, count + 1);
}
