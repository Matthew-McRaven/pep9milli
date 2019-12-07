enum Instructions {
    i_stop = 0,
    i_ret = 1,
    i_rettr = 2,
    i_movspa = 3,
    i_movflga = 4,
    i_movaflg = 5,
    i_nota = 6,
    i_notx = 7,
    i_nega = 8,
    i_negx = 9,
    i_asla = 10,
    i_aslx = 11,
    i_asra = 12,
    i_asrx = 13,
    i_rola = 14,
    i_rolx = 15,
    i_rora = 16,
    i_rorx = 17,
    i_br = 18,
    i_brle = 20,
    i_bglt = 22,
    i_breq = 24,
    i_brne = 26,
    i_brge = 28,
    i_brgt = 30,
    i_brv = 32,
    i_brc = 34,
    i_call = 36,
    i_nop0 = 38,
    i_nop1 = 39,
    i_nop = 40,
    i_deci = 48,
    i_deco = 56,
    i_hexo = 64,
    i_stro = 72,

    i_addsp = 80,
    i_subsp = 88,
    i_adda = 96,
    i_addx = 104,
    i_suba = 112,
    i_subx = 120,
    i_anda = 128,
    i_andx = 136,
    i_ora = 144,
    i_orx = 152,

    i_cpwa = 160,
    i_cpwx = 168,
    i_cpba = 176,
    i_cpbx = 184,
    i_ldwa = 192,
    i_ldwx = 200,
    i_ldba = 208,
    i_ldbx = 216,
    i_stwa = 224,
    i_stwx = 232,
    i_stba = 240,
    i_stbx = 248
};

enum Instructions instruction_array[256] =
{
    i_stop,    i_ret,     i_rettr,   i_movspa,  i_movflga, i_movaflg, i_nota,    i_notx,      // 7
    i_nega,    i_negx,    i_asla,    i_aslx,    i_asra,    i_asrx,    i_rola,    i_rolx,      // 15
    i_rora,    i_rorx,    i_br,      i_br,      i_brle,    i_brle,    i_bglt,    i_bglt,      // 23
    i_breq,    i_breq,    i_brne,    i_brne,    i_brge,    i_brge,    i_brgt,    i_brgt,      // 31
    i_brv,     i_brv,     i_brc,     i_brc,     i_call,    i_call,    i_nop0,    i_nop1,      // 39
    i_nop,     i_nop,     i_nop,     i_nop,     i_nop,     i_nop,     i_nop,     i_nop,       // 47
    i_deci,    i_deci,    i_deci,    i_deci,    i_deci,    i_deci,    i_deci,    i_deci,      // 55
    i_deco,    i_deco,    i_deco,    i_deco,    i_deco,    i_deco,    i_deco,    i_deco,      // 63
    i_hexo,    i_hexo,    i_hexo,    i_hexo,    i_hexo,    i_hexo,    i_hexo,    i_hexo,      // 71
    i_stro,    i_stro,    i_stro,    i_stro,    i_stro,    i_stro,    i_stro,    i_stro,      // 79
    i_addsp,   i_addsp,   i_addsp,   i_addsp,   i_addsp,   i_addsp,   i_addsp,   i_addsp,     // 87
    i_subsp,   i_subsp,   i_subsp,   i_subsp,   i_subsp,   i_subsp,   i_subsp,   i_subsp,     // 95
    i_adda,    i_adda,    i_adda,    i_adda,    i_adda,    i_adda,    i_adda,    i_adda,      // 103
    i_addx,    i_addx,    i_addx,    i_addx,    i_addx,    i_addx,    i_addx,    i_addx,      // 111
    i_suba,    i_suba,    i_suba,    i_suba,    i_suba,    i_suba,    i_suba,    i_suba,      // 119
    i_subx,    i_subx,    i_subx,    i_subx,    i_subx,    i_subx,    i_subx,    i_subx,      // 127
    i_adda,    i_adda,    i_adda,    i_adda,    i_adda,    i_adda,    i_adda,    i_adda,       // 135
    i_andx,    i_andx,    i_andx,    i_andx,    i_andx,    i_andx,    i_andx,    i_andx,      // 143
    i_ora,     i_ora,     i_ora,     i_ora,     i_ora,     i_ora,     i_ora,     i_ora,       // 151
    i_orx,     i_orx,     i_orx,     i_orx,     i_orx,     i_orx,     i_orx,     i_orx,       // 159
    i_cpwa,    i_cpwa,    i_cpwa,    i_cpwa,    i_cpwa,    i_cpwa,    i_cpwa,    i_cpwa,      // 167
    i_cpwx,    i_cpwx,    i_cpwx,    i_cpwx,    i_cpwx,    i_cpwx,    i_cpwx,    i_cpwx,      // 175
    i_cpba,    i_cpba,    i_cpba,    i_cpba,    i_cpba,    i_cpba,    i_cpba,    i_cpba,      // 183
    i_cpbx,    i_cpbx,    i_cpbx,    i_cpbx,    i_cpbx,    i_cpbx,    i_cpbx,    i_cpbx,      // 191
    i_ldwa,    i_ldwa,    i_ldwa,    i_ldwa,    i_ldwa,    i_ldwa,    i_ldwa,    i_ldwa,      // 199
    i_ldwx,    i_ldwx,    i_ldwx,    i_ldwx,    i_ldwx,    i_ldwx,    i_ldwx,    i_ldwx,      // 207
    i_ldba,    i_ldba,    i_ldba,    i_ldba,    i_ldba,    i_ldba,    i_ldba,    i_ldba,      // 215
    i_ldbx,    i_ldbx,    i_ldbx,    i_ldbx,    i_ldbx,    i_ldbx,    i_ldbx,    i_ldbx,      // 223
    i_stwa,    i_stwa,    i_stwa,    i_stwa,    i_stwa,    i_stwa,    i_stwa,    i_stwa,      // 231
    i_stwx,    i_stwx,    i_stwx,    i_stwx,    i_stwx,    i_stwx,    i_stwx,    i_stwx,      // 239
    i_stba,    i_stba,    i_stba,    i_stba,    i_stba,    i_stba,    i_stba,    i_stba,      // 247
    i_stbx,    i_stbx,    i_stbx,    i_stbx,    i_stbx,    i_stbx,    i_stbx,    i_stbx       // 255
};
