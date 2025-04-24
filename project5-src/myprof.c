#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct { unsigned long pos; int span; } TimePoint;
typedef struct { char label[256]; unsigned long pos; char cat; } Symbol;
typedef struct { char label[256]; unsigned long entry; unsigned long exit; int span; } Method;
typedef struct { unsigned long pos; int span; char code[256]; } AsmInst;

FILE* safe_fopen(const char* path, const char* mode) {
    FILE* f = fopen(path, mode);
    if (!f) { perror("File open error"); exit(1); }
    return f;
}

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) { perror("Memory allocation error"); exit(1); }
    return ptr;
}

char* build_path(char* buf, size_t size, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, size, fmt, args);
    va_end(args);
    return buf;
}

int by_time_desc(const void* x, const void* y) {
    return ((Method*)y)->span - ((Method*)x)->span;
}

TimePoint* read_timeline(const char* prog, int* count, int limit_hist) {
    char path[1024];
    TimePoint* data = safe_malloc(limit_hist * sizeof(TimePoint));
    *count = 0;
    
    FILE* src = safe_fopen(build_path(path, sizeof(path), "%s.hist", prog), "r");
    char buf[256];
    unsigned long p;
    int t;
    
    while (*count < limit_hist && fgets(buf, sizeof(buf), src)) {
        if (sscanf(buf, "0x%lx\t%dms", &p, &t) == 2) {
            data[*count].pos = p;
            data[*count].span = t;
            (*count)++;
        }
    }
    fclose(src);
    return data;
}

Symbol* extract_symbols(const char* prog, int* count, int limit_sym) {
    char cmd[1024];
    build_path(cmd, sizeof(cmd), "nm -v %s > nm.out", prog);
    if (system(cmd) != 0) { fprintf(stderr, "nm failed\n"); exit(1); }
    
    Symbol* syms = safe_malloc(limit_sym * sizeof(Symbol));
    *count = 0;
    FILE* src = safe_fopen("nm.out", "r");
    char buf[256];
    
    while (*count < limit_sym && fgets(buf, sizeof(buf), src)) {
        unsigned long p;
        char t, n[256];
        if (sscanf(buf, "%lx %c %s", &p, &t, n) == 3) {
            syms[*count].pos = p;
            syms[*count].cat = t;
            strncpy(syms[*count].label, n, 255);
            (*count)++;
        }
    }
    fclose(src);
    return syms;
}

Method* build_methods(Symbol* syms, int sym_count, int* method_count, int limit_fn) {
    Method* methods = safe_malloc(limit_fn * sizeof(Method));
    *method_count = 0;
    
    for (int i = 0; i < sym_count && *method_count < limit_fn; i++) {
        if (syms[i].cat == 'T' || syms[i].cat == 't') {
            methods[*method_count].entry = syms[i].pos;
            strncpy(methods[*method_count].label, syms[i].label, 255);
            methods[*method_count].span = 0;
            (*method_count)++;
        }
    }
    
    int last = *method_count - 1;
    while (--last >= 0) {
        methods[last].exit = methods[last + 1].entry;
    }
    methods[*method_count - 1].exit = 0xffffffffffffffff;
    
    return methods;
}

void process_timeline(Method* methods, int method_count, TimePoint* timeline, int time_count) {
    for (TimePoint* tp = timeline; tp < timeline + time_count; tp++) {
        for (Method* m = methods; m < methods + method_count; m++) {
            if (tp->pos >= m->entry && tp->pos < m->exit) {
                m->span += tp->span;
                break;
            }
        }
    }
}

AsmInst* analyze_method(const char* prog, Method* m, int* inst_count, int idx, int limit_asm) {
    char cmd[1024], path[256];
    build_path(cmd, sizeof(cmd),
               "objdump -d --start-address=0x%lx --stop-address=0x%lx %s > method_%d.out",
               m->entry, m->exit - 1, prog, idx);
    
    if (system(cmd) != 0) { 
        fprintf(stderr, "objdump failed for %s\n", m->label);
        *inst_count = 0;
        return NULL;
    }
    
    build_path(path, sizeof(path), "method_%d.out", idx);
    FILE* src = safe_fopen(path, "r");
    AsmInst* insts = safe_malloc(limit_asm * sizeof(AsmInst));
    *inst_count = 0;
    char buf[256];
    
    while (*inst_count < limit_asm && fgets(buf, sizeof(buf), src)) {
        unsigned long p;
        char code[256];
        if (sscanf(buf, " %lx:\t%[^\n]", &p, code) == 2) {
            insts[*inst_count].pos = p;
            insts[*inst_count].span = 0;
            strncpy(insts[*inst_count].code, code, 255);
            (*inst_count)++;
        }
    }
    fclose(src);
    remove(path);
    return insts;
}

void map_time_to_instructions(AsmInst* insts, int inst_count, Method* m, TimePoint* timeline, int time_count) {
    while (time_count--) {
        if (timeline->pos >= m->entry && timeline->pos < m->exit) {
            for (AsmInst* inst = insts; inst < insts + inst_count; inst++) {
                if (inst->pos == timeline->pos) {
                    inst->span += timeline->span;
                    break;
                }
            }
        }
        timeline++;
    }
}

void print_method_summary(Method* m, int idx, double total_time) {
    double pct = m->span * 100.0 / total_time;
    printf("%d: %s      %dms        %.2f%%\n\n", idx + 1, m->label, m->span, pct);
}

void print_instruction_times(AsmInst* insts, int count) {
    while (count-- != 0) {
        insts->span ? printf("  %4dms   0x%lx:\t%s\n",
                            insts->span, insts->pos, insts->code) : 0;
        insts++;
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int limit_fn = 10000, limit_hist = 100000, limit_sym = 100000, limit_asm = 100000;
    char* prog = (argc != 2) ? (fprintf(stderr, "Usage: %s prog\n", argv[0]), exit(1), NULL) : argv[1];
    
    int time_count, sym_count, method_count;
    TimePoint* timeline = read_timeline(prog, &time_count, limit_hist);
    Symbol* symbols = extract_symbols(prog, &sym_count, limit_sym);
    Method* methods = build_methods(symbols, sym_count, &method_count, limit_fn);
    
    process_timeline(methods, method_count, timeline, time_count);
    qsort(methods, method_count, sizeof(Method), by_time_desc);
    
    int total_time = 0;
    for (Method* m = methods; m < methods + method_count; m++) {
        total_time += m->span;
    }
    
    printf("Top 10 functions:\n\n");
    printf("ith   Function               Time(ms)        (%%)\n");
    
    int show_count = method_count < 10 ? method_count : 10;
    for (int i = 0; i < show_count; i++) {
        double pct = methods[i].span * 100.0 / total_time;
        printf("%-5d %-20s %6dms      %.2f%%\n", i + 1, methods[i].label, methods[i].span, pct);
    }
    
    printf("\nTop 10 functions Assembly\n\n");
    
    for (int i = 0; i < show_count; i++) {
        int inst_count;
        AsmInst* insts = analyze_method(prog, &methods[i], &inst_count, i, limit_asm);
        if (!insts) continue;
        
        map_time_to_instructions(insts, inst_count, &methods[i], timeline, time_count);
        print_method_summary(&methods[i], i, total_time);
        print_instruction_times(insts, inst_count);

        free(insts);
    }
    
    free(methods);
    free(symbols);
    free(timeline);
    return 0;
}
