keysyms = [
    [0x12, "XK_Shift_L", None], #  left shift
    [0x59, "XK_Shift_R", None], #  left shift
    [13, "XK_Tab", None], #  Tab
    [14, "XK_grave", "XK_asciitilde"],
    [21, "XK_q", "XK_Q"],
    [22, "XK_1", "XK_exclam"],
    [26, "XK_z", "XK_Z"],
    [27, "XK_s", "XK_S"],
    [28, "XK_a", "XK_A"],
    [29, "XK_w", "XK_W"],
    [30, "XK_2", 'XK_at'],
    [33, "XK_c", "XK_C"],
    [34, "XK_x", "XK_X"],
    [35, "XK_d", "XK_D"],
    [36, "XK_e", "XK_E"],
    [37, "XK_4", 'XK_dollar'],
    [38, "XK_3", 'XK_numbersign'],
    [42, "XK_v", "XK_V"],
    [43, "XK_f", "XK_F"],
    [44, "XK_t", "XK_T"],
    [45, "XK_r", "XK_R"],
    [46, "XK_5", 'XK_percent'],
    [49, "XK_n", "XK_N"],
    [50, "XK_b", "XK_B"],
    [51, "XK_h", "XK_H"],
    [52, "XK_g", "XK_G"],
    [53, "XK_y", "XK_Y"],
    [54, "XK_6", 'XK_asciicircum'],
    [58, "XK_m", "XK_M"],
    [59, "XK_j", "XK_J"],
    [60, "XK_u", "XK_U"],
    [61, "XK_7", 'XK_ampersand'],
    [62, "XK_8", 'XK_asterisk'],
    [65, "XK_comma", 'XK_less'],
    [66, "XK_k", "XK_K"],
    [67, "XK_i", "XK_I"],
    [68, "XK_o", "XK_O"],
    [69, "XK_0", 'XK_parenright'],
    [70, "XK_9", 'XK_parenleft'],
    [73, 'XK_period', 'XK_greater'],
    [74, 'XK_slash', 'XK_question'],
    [75, "XK_l", "XK_L"],
    [76, 'XK_semicolon', 'XK_colon'],
    [77, "XK_p", "XK_P"],
    [78, 'XK_minus', 'XK_underscore'],
    [82, "XK_apostrophe", 'XK_quotedbl'], #  Single quote
    [84, 'XK_bracketleft', 'XK_braceleft'],
    [85, 'XK_equal', 'XK_plus'],
    [90, "XK_Return", None], #  Enter
    [91, 'XK_bracketright', 'XK_braceright'],
    [93, "XK_backslash", 'XK_bar'], #  Backslash
    [102, "XK_BackSpace", None], #  Backspace
    [118, "XK_Escape", None], #  Escape
]

print """
std::map<int, unsigned char> rfb_keys_to_AT_keycodes;

void populate_keycode_map()
{
"""

for (code, nothing, shifted) in keysyms: 
    print "    rfb_keys_to_AT_keycodes[%s] = 0x%02X;" % (nothing, code);
    if shifted:
        print "    rfb_keys_to_AT_keycodes[%s] = 0x%02X;" % (shifted, code);

print "}"
