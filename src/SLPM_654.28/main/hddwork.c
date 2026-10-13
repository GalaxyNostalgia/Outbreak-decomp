#include "common.h"
#include "options.h"
#include "cri.h"
#include "mcjob.h"

/* Work area for the boot-time HDD/network setup screens. */
typedef struct {
    u8 unk0;
    s8 state;           /* 0x01 */
    s8 step;            /* 0x02 */
    u8 unk3[5];
    s16 held;           /* 0x08 */
    u16 pressed;        /* 0x0A */
    s16 trigger;        /* 0x0C */
    u8 unkE[2];
    s32 unk10;
    s32 unk14;
    u8 unk18[4];
    s32 kind;           /* 0x1C */
    s32 mode;           /* 0x20 */
    s32 unk24;
    s32 unk28;
    s32 msg;            /* 0x2C: message shown */
    s32 yesNo;          /* 0x30: a yes/no choice is shown */
    s32 error;          /* 0x34 */
    s32 cursor;         /* 0x38: 1 yes, 0 no */
    s32 unk3C;
    s32 loaded;         /* 0x40 */
    u8 unk44[0xD4 - 0x44];
    s32 x;              /* 0xD4 */
    s32 y;              /* 0xD8 */
    s32 wide;           /* 0xDC */
    s32 unkE0;
    s32 buf0;           /* 0xE4: 0x100 bytes */
    s32 buf1;           /* 0xE8: 0x40000 bytes */
    s32 buf2;           /* 0xEC: 0x40000 bytes */
    s32 buf3;           /* 0xF0 */
} HddWork;

extern HddWork *D_0023BBE0;
extern s32 D_003065DC[];        /* (Options) work memory */




void *memset(void *dst, s32 c, u32 n);

void func_001BE6C0(s32 layout) {
    D_00308D80.layout = layout;
    D_00308D80.step = 0;
    D_00308D80.sub = 0;
    D_00308D80.job = 0;
}

void func_001C0940(s32 mode, s32 kind) {
    if (D_00306590.unk0) {
        D_0023BBE0 = (HddWork *)(D_003065DC[0] + 0xA0000);
    } else {
        D_0023BBE0 = (HddWork *)(D_003065DC[0] + 0x2BF000);
    }
    memset(D_0023BBE0, 0, sizeof(HddWork));
    D_0023BBE0->buf0 = (s32)D_0023BBE0;
    D_0023BBE0->buf1 = D_0023BBE0->buf0 + 0x100;
    D_0023BBE0->buf2 = D_0023BBE0->buf1 + 0x40000;
    D_0023BBE0->buf3 = D_0023BBE0->buf2 + 0x40000;
    D_0023BBE0->mode = mode;
    D_0023BBE0->kind = kind;
    func_001BE6C0(0);
    switch (D_0023BBE0->mode) {
    case 0:
        switch (D_0023BBE0->kind) {
        case 0:
            D_0023BBE0->x = 0x30;
            D_0023BBE0->y = 0x120;
            break;
        case 1:
        case 2:
            D_0023BBE0->wide = 1;
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        case 3:
        case 4:
            break;
        }
        break;
    case 1:
        switch (D_0023BBE0->kind) {
        case 0:
        case 4:
            break;
        case 3:
        case 1:
        case 2:
            D_0023BBE0->wide = 1;
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        }
        break;
    case 2:
        switch (D_0023BBE0->kind) {
        case 0:
        case 3:
        case 4:
            break;
        case 1:
        case 2:
            D_0023BBE0->wide = 1;
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        }
        break;
    case 3:
        switch (D_0023BBE0->kind) {
        case 0:
        case 3:
            break;
        case 1:
        case 2:
        case 4:
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        }
        break;
    case 4:
        switch (D_0023BBE0->kind) {
        case 0:
        case 3:
        case 4:
            break;
        case 1:
        case 2:
            D_0023BBE0->wide = 1;
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        }
        break;
    }
}

/* Loads a whole file from the disc through ADXF. */
extern s32 func_001BF610(s32 *pack);

void func_001C3B00(HddWork *w, s32 buf, char *name) {
    ADXF adxf;

    while ((adxf = ADXF_Open(name, 0)) == 0) {
    }
    ADXF_ReadNw(adxf, ADXF_GetFsizeSct(adxf), (void *)buf);
    while (ADXF_GetStat(adxf) != 3) {
    }
    ADXF_Close(adxf);
    w->loaded = 1;
    w->unk28 = func_001BF610((s32 *)buf);
}

/* Boot-time save-data screens: one step per frame, returns 0 when finished. */
extern s32 D_003063E0[];
extern s32 D_003063E8[];
extern s32 D_003063F8[];
extern u8 D_00306646[];

s32 func_001BF5B0(void);
void func_001BE6F0(void);
s32 func_001C0FD0(HddWork *w);
s32 func_001C1400(HddWork *w);
s32 func_001C1840(HddWork *w);
s32 func_001C1BF0(HddWork *w);
s32 func_001C1F00(HddWork *w);
s32 func_001C25F0(HddWork *w);
s32 func_001C2BC0(HddWork *w);
s32 func_001C2EE0(HddWork *w);
s32 func_001C3020(HddWork *w);

s32 func_001C0C20(void) {
    HddWork *w = D_0023BBE0;

    w->held = D_003063E8[0];
    w->pressed = D_003063E0[0];
    w->trigger = D_003063F8[0] | w->pressed;
    switch (w->state) {
    case 0:
        switch (w->mode) {
        case 0:
            switch (w->kind) {
            case 0:
                w->state = 1;
                break;
            default:
                return 0;
            }
            break;
        case 1:
            switch (w->kind) {
            case 1:
                if (D_00306646[0] == 0) {
                    w->state = 2;
                } else {
                    w->state = 4;
                }
                break;
            case 2:
                w->state = 3;
                break;
            case 3:
                w->state = 5;
                break;
            default:
                return 0;
            }
            break;
        case 2:
            switch (w->kind) {
            case 1:
                if (D_00306646[0] == 0) {
                    w->state = 2;
                } else {
                    w->state = 4;
                }
                break;
            case 2:
                w->state = 3;
                break;
            default:
                return 0;
            }
            break;
        case 3:
            switch (w->kind) {
            case 1:
                if (D_00306646[0] == 0) {
                    w->state = 2;
                } else {
                    w->state = 4;
                }
                break;
            case 2:
                w->state = 6;
                break;
            case 4:
                w->state = 7;
                break;
            default:
                return 0;
            }
            break;
        case 4:
            switch (w->kind) {
            case 1:
                if (D_00306646[0] == 0) {
                    w->state = 2;
                } else {
                    w->state = 4;
                }
                break;
            case 2:
                w->state = 3;
                break;
            default:
                return 0;
            }
            break;
        }
        {
            char *name = "MCICON.BIN";
            w->unk10 = 0;
            w->unk14 = 0;
            w->unk24 = 0;
            func_001C3B00(w, w->buf3, name);
        }
        break;
    case 1:
        if (func_001C0FD0(w) == 0) {
            return 0;
        }
        break;
    case 2:
        if (func_001C1F00(w) == 0) {
        save_end:
            if (w->wide == 0) {
                return 0;
            }
            if (func_001BF5B0() == 0 && w->unkE0 == 0) {
                return 0;
            }
            w->state = 8;
            w->step = 0;
        }
        break;
    case 3:
        if (func_001C1400(w) == 0) {
            if (w->wide == 0) {
                return 0;
            }
            if (func_001BF5B0() == 0 && w->unkE0 == 0) {
                return 0;
            }
            w->state = 9;
            w->step = 0;
        }
        break;
    case 4:
        if (func_001C2BC0(w) == 0) {
            goto save_end;
        }
        break;
    case 5:
        if (func_001C25F0(w) == 0) {
            return 0;
        }
        break;
    case 6:
        if (func_001C1BF0(w) == 0) {
            return 0;
        }
        break;
    case 7:
        if (func_001C1840(w) == 0) {
            return 0;
        }
        break;
    case 8:
        if (func_001C2EE0(w) == 0) {
            return 0;
        }
        break;
    case 9:
        if (func_001C3020(w) == 0) {
            return 0;
        }
        break;
    }
    func_001BE6F0();
    return 1;
}

s32 func_001BF5B0(void) {
    return D_00308D80.result;
}


/* Takes the file offsets/sizes from a packed file (count, then {offset, size} pairs) into the
   current layout's files 1-4 and returns the save's size in clusters. */
#ifdef NON_MATCHING   /* register allocation differs (the original reuses pack's register for the second loop) */
s32 func_001BF610(s32 *pack) {
    SaveDesc *d = D_00217560[D_00308D80.layout];
    s32 n, blocks, i, j;
    s32 base;

    if (pack != NULL) {
        n = pack[1];
        base = (s32)pack;
        pack += 2;
        for (i = 1; i < 5; i++) {
            if (n <= 0) {
                break;
            }
            if (d->files[i].used) {
                n--;
                d->files[i].data = pack[0];
                d->files[i].data += base;
                d->files[i].size = pack[1];
                pack += 2;
            }
        }
    }
    n = 0;
    blocks = 0;
    for (i = 0; i < 5; i++) {
        if (d->files[i].used) {
            blocks += (d->files[i].size + 0x3FF) / 0x400;
            n++;
        }
    }
    blocks += (n + 1) / 2;
    return d->blocks = blocks + 2;
}
#endif

/* Memory card job: one step of the current operation per frame. */
typedef void (*McStep)(McJob *mc);
extern McStep D_00217570[];
void func_001AAA60(void);
void func_001AAAB0(void);

void func_001BE6F0(void) {
    func_001AAA60();
    D_00217570[D_00308D80.job](&D_00308D80);
    func_001AAAB0();
}

/* Job 0: idle. */
void func_001BE760(McJob *mc) {
}

/* Save-data screen, mode 0 (boot): check the memory card in slot 1. */
extern s32 D_0023BA18;
void func_001BEA60(s32 a, s32 buf, s32 c);
void func_001BE830(s32 a, s32 buf);
void func_001BF5E0(void);
void func_001C3150(HddWork *w);
void func_001C33F0(HddWork *w);
s32 func_001C37F0(HddWork *w);
void func_001C3BB0(void);
void func_001C3BC0(void);

extern u8 D_002F88D2;

#ifdef NON_MATCHING   /* first draft from m2c */
/* Save-data screen, mode 6 (from the select screen): load the save from slot 1. */
s32 func_001C1BF0(HddWork *w) {
    s32 r;

    switch (w->step) {
    case 0:
        w->step++;
        w->msg = 8;
        D_002F88D2 = 0;
        func_001BEA60(w->unk24, w->buf1, 1);
        w->unk10 = 0x3C;
    case 1:
        switch (r = func_001BF5B0()) {
        case -0xFE:
        case -0xFD:
            w->msg = 7;
            return 0;
        case -0x100:
            w->msg = 9;
            w->step = 0x63;
            w->error = 1;
            break;
        case -1:
            break;
        case -0xFB:
            w->msg = 9;
            w->step = 0x63;
            w->error = 1;
            break;
        case 0:
            func_001BE830(w->unk24, w->buf2);
            w->step++;
            w->msg = 0x26;
            func_001BF5E0();
            break;
        default:
            w->error = r & 0xFF;
            switch (w->error) {
            case 4:
            case 2:
            case 1:
            case 3:
                return 0;
            }
            break;
        }
        break;
    case 2:
        switch (r = func_001BF5B0()) {
        case 0:
            if (func_001C37F0(w) == 0) {
                w->msg = 0x1A;
                return 0;
            }
            D_00306646[0] = 0;
            w->msg = 0x34;
            w->error = 5;
            w->error = 1;
            w->step = 0x63;
        case -1:
            w->unk14 = D_0023BA18;
            break;
        case -0x100:
            w->msg = 0x34;
            w->error = 1;
            w->step = 0x63;
            w->unk14 = D_0023BA18;
            break;
        default:
            w->step = 0x63;
            w->error = r & 0xFF;
            switch (w->error) {
            case 4:
                w->msg = 0x17;
                break;
            case 3:
                w->msg = 7;
                break;
            case 2:
                w->msg = 6;
                break;
            case 1:
                w->msg = 5;
                break;
            }
            w->unk14 = D_0023BA18;
            break;
        }
        break;
    case 0x63:
        if (w->pressed & 0xFFF0) {
            D_00306646[0] = 0;
            func_001C3BB0();
            return 0;
        }
        break;
    }
    func_001C3150(w);
    func_001C33F0(w);
    return 1;
}
#endif

s32 func_001C0FD0(HddWork *w) {
    s32 r;

    switch (w->step) {
    case 0:
        w->step = 1;
        w->msg = 8;
        func_001BEA60(w->unk24, w->buf1, 0);
    case 1:
        switch (r = func_001BF5B0()) {
        case -0xFD:
        case -0xFE:
            w->msg = 0;
            w->step = 4;
            w->unk10 = 1;
            break;
        case -0x100:
            w->msg = 0x36;
            w->step = 3;
            w->unk10 = 5;
            w->yesNo = 1;
            w->cursor = 1;
            break;
        case 0:
            func_001BE830(w->unk24, w->buf2);
            w->step = 2;
            w->msg = 3;
            break;
        case -0xFB:
            w->msg = 0x36;
            w->step = 3;
            w->unk10 = 5;
            w->yesNo = 1;
            w->cursor = 1;
            break;
        case -1:
            break;
        default:
            w->step = 3;
            w->error = r & 0xFF;
            switch (w->error) {
            case 1:
                w->msg = 0x1C;
                w->unk10 = 5;
                w->yesNo = 1;
                w->cursor = 1;
                break;
            case 2:
                w->msg = 0;
                w->step = 4;
                w->unk10 = 1;
                break;
            case 4:
                w->msg = 0x1D;
                w->unk10 = 5;
                w->yesNo = 1;
                w->cursor = 1;
                break;
            case 3:
                w->msg = 0;
                w->step = 4;
                w->unk10 = 1;
                break;
            }
            break;
        }
        break;
    case 2:
        switch (r = func_001BF5B0()) {
        case 0:
            if (func_001C37F0(w) == 0) {
                w->msg = 0x1A;
                func_001BF5E0();
                w->step = 4;
                w->error = 0;
                w->unk10 = 0x3C;
            } else {
                D_00306646[0] = 0;
                w->msg = 0x39;
                w->step = 3;
                w->unk10 = 5;
                w->yesNo = 1;
                w->cursor = 1;
            }
            break;
        case -0x100:
            w->step = 3;
            w->msg = 0x39;
            w->unk10 = 5;
            w->yesNo = 1;
            w->cursor = 1;
            break;
        case -1:
            break;
        default:
            w->step = 3;
            w->error = r & 0xFF;
            switch (w->error) {
            case 1:
                w->msg = 0x1C;
                w->unk10 = 5;
                w->yesNo = 1;
                w->cursor = 1;
                break;
            case 2:
            case 3:
                w->msg = 0;
                w->step = 4;
                w->unk10 = 1;
                break;
            case 4:
                w->msg = 0x1D;
                w->unk10 = 5;
                w->yesNo = 1;
                w->cursor = 1;
                break;
            }
            break;
        }
        w->unk14 = D_0023BA18;
        break;
    case 3:
        if (w->pressed & 0x20) {
            func_001C3BB0();
            w->yesNo = 0;
            if (w->cursor == 1) {
                w->step = 0;
            } else {
                w->msg = 0;
                w->step = 4;
                w->unk10 = 1;
            }
        } else if (w->unk10 != 0) {
            w->unk10--;
        } else if (((w->pressed & 8) && w->cursor == 0) || ((w->pressed & 4) && w->cursor == 1)) {
            func_001C3BC0();
            w->cursor ^= 1;
            w->unk10 = 5;
        }
        break;
    case 4:
        if (--w->unk10 == 0) {
            return 0;
        }
        break;
    case 5:
        if (w->pressed & 0xFFF0) {
            func_001C3BB0();
            return 0;
        }
        break;
    }
    func_001C3150(w);
    func_001C33F0(w);
    return 1;
}

s32 sprintf(char *buf, const char *fmt, ...);
extern char D_00232370[];   /* "%s/%s" */

/* Starts job 3: read the save into buf. */
void func_001BEA60(s32 port, s32 buf, s32 flags) {
    SaveDesc *d = D_00217560[D_00308D80.layout];

    D_00308D80.job = 3;
    D_00308D80.result = -1;
    D_00308D80.step = 0;
    D_00308D80.sub = 0;
    D_00308D80.port = port;
    D_00308D80.buf = buf;
    D_00308D80.flags = flags;
    sprintf(D_00308D80.path, D_00232370, d->files[0].name, d->files[0].name);
    D_00308D80.size = d->files[0].size;
}

/* Starts job 2. */
void func_001BE830(s32 port, s32 buf) {
    SaveDesc *d = D_00217560[D_00308D80.layout];

    D_00308D80.job = 2;
    D_00308D80.result = -1;
    D_00308D80.step = 0;
    D_00308D80.sub = 0;
    D_00308D80.port = port;
    D_00308D80.buf = buf;
    sprintf(D_00308D80.path, D_00232370, d->files[0].name, d->files[0].name);
    D_00308D80.size = d->files[0].size;
}

void func_001BF5E0(void) {
    D_00308D80.newCards = 0;
}

/* Save-data screen messages: lines of text, NULL-terminated. */
extern char **D_00217E40[];

char **func_001C01E0(s32 id) {
    return D_00217E40[id];
}

void func_001B4300(s32 size);
void func_001B4380(u32 color);
void func_001B43C0(char *s);
s32 func_001C0340(s32 x, s32 y, char *s, s32 max);
s32 func_001C0460(s32 x, s32 y, char *s, s32 max);
void func_001C0720(char *buf, s32 n);
char *strstr(const char *s, const char *find);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);

/* Draws the current message. "%s" becomes slot 1, "%a" the save's size in clusters, and "%r"
   ends the line with a "press a button" line after the message. */
#ifdef NON_MATCHING   /* the original keeps &num[2] in a register across the loop */
void func_001C3150(HddWork *w) {
    char num[0x10];
    char out[0x50];
    char src[0x50];
    char **line;
    char *p;
    char *q;
    s32 x, y;
    s32 more;
    Options *opt = &D_00306590;

    if (w->msg == 0) {
        return;
    }
    line = func_001C01E0(w->msg);
    func_001B4300(0x14);
    func_001B4380(0x80808080);
    y = w->y;
    more = 0;
    x = w->x;
    for (; *line != NULL; line++) {
        p = src;
        out[0] = 0;
        strcpy(p, *line);
        for (;;) {
            if ((q = strstr(p, "%s")) != NULL) {
                *q = 0;
                strcat(out, p);
                strcat(out, "<C2>");
                p = q + 2;
                if (opt->unk3 == 0) {
                    strcpy(num, "\x82\x50");
                    num[2] = 0;
                } else {
                    num[0] = '1';
                    num[1] = 0;
                }
                strcat(out, num);
                strcat(out, "<C7>");
            } else if ((q = strstr(p, "%a")) != NULL) {
                *q = 0;
                strcat(out, p);
                strcat(out, "<C2>");
                p = q + 2;
                if (opt->unk3 == 0) {
                    func_001C0720(num, w->unk28);
                    num[6] = 0;
                } else {
                    sprintf(num, "%d", w->unk28);
                }
                strcat(out, num);
                strcat(out, "<C7>");
            } else if ((q = strstr(p, "%r")) != NULL) {
                *q = 0;
                more = 1;
            } else {
                break;
            }
        }
        strcat(out, p);
        if (w->mode == 4) {
            func_001C0460(x, y, out, 0);
        } else {
            func_001C0340(x, y, out, 0);
        }
        y += 0x16;
    }
    if (more) {
        line = func_001C01E0(0x1F);
        if (w->mode == 4) {
            func_001C0460(x, y, line[0], 0);
        } else {
            func_001C0340(x, y, line[0], 0);
        }
    }
}
#endif

/* Draws the yes/no choice, the chosen one in red. */
void func_001C33F0(HddWork *w) {
    char **m;

    if (w->yesNo == 0) {
        return;
    }
    m = func_001C01E0(1);
    func_001B4300(0x14);
    func_001B43C0("\x81\x40");
    if (w->cursor == 0) {
        func_001B4380(0x80000080);
    } else {
        func_001B4380(0x80808080);
    }
    func_001B43C0(m[4]);
    func_001B4380(0x80808080);
    func_001B43C0(m[6]);
    if (w->cursor == 1) {
        func_001B4380(0x80000080);
    } else {
        func_001B4380(0x80808080);
    }
    func_001B43C0(m[5]);
}

/* Text with <tags>: "<Cn>" sets colour n. */
s32 func_001B5450(char *s);
void func_001B4320(s32 x, s32 y);
void func_001B4340(s32 z);
void func_001B5FB0(char *s);

/* If s starts with a tag, copies its name to tag and returns what follows it. */
char *func_001C05A0(char *s, char *tag) {
    u8 c;

    if (*s != '<') {
        return NULL;
    }
    s++;
    for (;;) {
        c = *s++;
        if (c == 0) {
            return NULL;
        }
        if (c == '>') {
            break;
        }
        *tag++ = c;
    }
    *tag = 0;
    return s;
}

/* Copies text up to the next tag (whole characters) and returns where it stopped. */
char *func_001C0600(char *s, char *out) {
    s32 n, i;
    u8 c;

    while (*s != 0) {
        c = *s;
        n = func_001B5450(s) & 3;
        if (n == 1 && c == '<') {
            break;
        }
        for (i = 0; i < n; i++) {
            *out++ = *s++;
        }
    }
    *out = 0;
    return s;
}

extern u32 D_00218480[];    /* text colours */

void func_001C06A0(u8 *tag) {
    s32 i;

    switch (*tag++) {
    case 'C':
        if (*tag != 0) {
            i = (u8)(*tag - '0');
            if (i < 8) {
                func_001B4380(D_00218480[i] | 0x80000000);
            }
        }
        break;
    }
}

/* Writes n (0-999) as three full-width digits. */
extern char D_00217E20[];   /* "０１２...９" */
char *strncpy(char *dst, const char *src, u32 n);

void func_001C0720(char *buf, s32 n) {
    s32 i;
    s32 div;

    i = 0;
    div = 1000;
    do {
        n %= div;
        div /= 10;
        strncpy(buf, &D_00217E20[(n / div) * 2], 2);
        i++;
        buf += 2;
    } while (i < 3);
}

/* Draws a line of tagged text at (x, y); with max > 0 only its first max characters.
   Returns how many characters were drawn when it stopped early, else 0. */
s32 func_001C0340(s32 x, s32 y, char *s, s32 max) {
    char text[0x8C];
    char one[4];
    s32 i;
    s32 n;
    char *p;
    s32 count;
    char *q;

    count = 0;
    func_001B4320(x, y);
    while (*s != 0) {
        if ((p = func_001C05A0(s, text)) == NULL) {
            s = func_001C0600(s, text);
            if (max == 0) {
                func_001B43C0(text);
            } else {
                p = text;
                q = one;
                while ((u8)*p != 0) {
                    n = func_001B5450(p) & 3;
                    for (i = 0; i < n; i++) {
                        *q++ = *p++;
                    }
                    *q = 0;
                    func_001B43C0(one);
                    count++;
                    q = one;
                    if (count >= max) {
                        return count;
                    }
                }
            }
        } else {
            func_001C06A0((u8 *)text);
            s = p;
        }
    }
    return 0;
}

/* The same in front of everything (z -2), for mode 4. */
s32 func_001C0460(s32 x, s32 y, char *s, s32 max) {
    char text[0x8C];
    char one[4];
    s32 i;
    s32 n;
    char *p;
    s32 count;
    char *q;

    count = 0;
    func_001B4340(-2);
    func_001B4320(x, y);
    while (*s != 0) {
        if ((p = func_001C05A0(s, text)) == NULL) {
            s = func_001C0600(s, text);
            if (max == 0) {
                func_001B5FB0(text);
            } else {
                p = text;
                q = one;
                while ((u8)*p != 0) {
                    n = func_001B5450(p) & 3;
                    for (i = 0; i < n; i++) {
                        *q++ = *p++;
                    }
                    *q = 0;
                    func_001B5FB0(one);
                    count++;
                    q = one;
                    if (count >= max) {
                        return count;
                    }
                }
            }
        } else {
            func_001C06A0((u8 *)text);
            s = p;
        }
    }
    return 0;
}


extern McInfo D_00308C28;
extern McDirEntry D_00308BF0;

/* 0 when the last memory card command has finished, -1 while it runs. */
s32 func_001BDB80(McJob *mc) {
    return -(sceMc2CheckAsync(&mc->mcCmd, &mc->mcResult) == 0);
}

#ifdef NON_MATCHING   /* branch layout / scheduling differs */
/* Is there a card in the port? Returns -1 while checking, else the card state. */
s32 func_001BDBB0(McJob *mc) {
    s32 st;

    switch (mc->sub) {
    case 0:
        if (func_001BDB80(mc) < 0) {
            break;
        }
        mc->sub++;
        mc->retries = 3;
    case 1:
        if (sceMc2GetInfoAsync(D_0023BBD8[mc->port], &D_00308C28) < 0) {
            if (--mc->retries > 0) {
                break;
            }
            goto none;
        }
        mc->sub++;
        break;
    case 2:
        if (func_001BDB80(mc) < 0) {
            break;
        }
        switch ((u16)mc->mcResult) {
        case 0:
            st = 1;
        found:
            if (D_00308C28.type != 2) {
                goto none;
            }
            if (D_00308C28.formatted == 0) {
                goto retry;
            }
            mc->card[mc->port] = st;
            mc->free[mc->port] = D_00308C28.freeClusters;
            goto done;
        case 0x9003:
            mc->newCards |= 1 << mc->port;
            st = 2;
            goto found;
        case 0x2F:
        retry:
            if (--mc->retries > 0) {
                mc->sub = 1;
                return -1;
            }
            mc->card[mc->port] = 3;
            mc->free[mc->port] = 8000;
        done:
            mc->sub = 0;
            return mc->card[mc->port];
        default:
            if (--mc->retries <= 0) {
                goto none;
            }
            mc->sub = 1;
            return -1;
        }
    }
    return -1;
none:
    mc->sub = 0;
    mc->card[mc->port] = 0;
    mc->free[mc->port] = 0;
    return 0;
}
#endif

#ifdef NON_MATCHING   /* branch layout / scheduling differs */
/* Looks for the save's file: 0 found, 1 not there, 3 no save directory, 2 error. */
s32 func_001BDDB0(McJob *mc) {
    switch (mc->sub) {
    case 0:
        if (func_001BDB80(mc) < 0) {
            break;
        }
        if (sceMc2SearchFileAsync(D_0023BBD8[mc->port], mc->path, &D_00308BF0) < 0) {
            return 1;
        }
        mc->sub++;
        break;
    case 1:
        if (func_001BDB80(mc) < 0) {
            break;
        }
        mc->sub = 0;
        switch ((u16)mc->mcResult) {
        case 0:
            return 0;
        case 2:
            return 1;
        case 0x9002:
            return 3;
        default:
            return 2;
        }
    }
    return -1;
}
#endif

#ifdef NON_MATCHING   /* branch layout / scheduling differs */
/* Reads the save's file into mc->buf: 0 done, 1 error. */
s32 func_001BDEA0(McJob *mc) {
    switch (mc->sub) {
    case 0:
        if (func_001BDB80(mc) < 0) {
            break;
        }
        if (sceMc2ReadFileAsync(D_0023BBD8[mc->port], mc->path, (void *)mc->buf, 0, mc->size) < 0) {
            return 1;
        }
        mc->sub++;
        break;
    case 1:
        if (func_001BDB80(mc) < 0) {
            break;
        }
        mc->sub = 0;
        return mc->mcResult < 0;
    }
    return -1;
}
#endif

void func_001BE740(s32 result) {
    D_00308D80.job = 0;
    D_00308D80.unkA8 = 0;
}

void func_001BE770(McJob *mc) {
    if (mc->result != -1) {
        func_001BE740(mc->result);
    }
}


/* Job 3: load the save (with flags 0 only check it's there). */
void func_001BEB00(McJob *mc) {
    SaveDesc *d = D_00217560[mc->layout];

    switch (mc->step) {
    case 0:
        switch (func_001BDBB0(mc)) {
        case 0:
            mc->result = MC_NO_CARD;
            break;
        case 3:
            mc->result = MC_UNFORMATTED;
            break;
        case 1:
        case 2:
            mc->step++;
            break;
        }
        break;
    case 1:
        switch (func_001BDDB0(mc)) {
        case 0:
            if (D_00308BF0.size != mc->size) {
                mc->step = 0;
                mc->result = MC_BAD_SAVE;
            } else if (mc->flags == 0) {
                mc->step = 0;
                mc->result = 0;
            } else {
                mc->step++;
            }
            break;
        case 1:
            mc->step = 0;
            if (mc->free[mc->port] >= d->blocks) {
                mc->result = MC_NO_SAVE;
            } else {
                mc->result = MC_NO_ROOM;
            }
            break;
        case 2:
            mc->step = 0;
            mc->result = MC_NO_CARD;
            break;
        case 3:
            mc->step = 0;
            mc->result = MC_BROKEN;
            break;
        }
        break;
    case 2:
        switch (func_001BDEA0(mc)) {
        case 0:
            mc->step = 0;
            if (mc->mcResult == mc->size) {
                mc->result = 0;
            } else {
                mc->result = MC_BROKEN;
            }
            break;
        case 1:
            mc->step = 0;
            switch ((u16)mc->mcResult) {
            case 0x13:
                mc->result = MC_NO_CARD;
                break;
            default:
                mc->result = MC_BROKEN;
                break;
            }
            break;
        }
        break;
    }
    func_001BE770(mc);
}
