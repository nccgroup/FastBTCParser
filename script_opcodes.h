#ifndef SCRIPT_OPCODE_H
#define SCRIPT_OPCODE_H

#include <cstdint>

enum opCodes : uint8_t {
OP_0 = 0U,
OP_DATA_1,
OP_DATA_2,
OP_DATA_3,
OP_DATA_4,
OP_DATA_5,
OP_DATA_6,
OP_DATA_7,
OP_DATA_8,
OP_DATA_9,
OP_DATA_10,
OP_DATA_11,
OP_DATA_12,
OP_DATA_13,
OP_DATA_14,
OP_DATA_15,
OP_DATA_16,
OP_DATA_17,
OP_DATA_18,
OP_DATA_19,
OP_DATA_20,
OP_DATA_21,
OP_DATA_22,
OP_DATA_23,
OP_DATA_24,
OP_DATA_25,
OP_DATA_26,
OP_DATA_27,
OP_DATA_28,
OP_DATA_29,
OP_DATA_30,
OP_DATA_31,
OP_DATA_32,
OP_DATA_33,
OP_DATA_34,
OP_DATA_35,
OP_DATA_36,
OP_DATA_37,
OP_DATA_38,
OP_DATA_39,
OP_DATA_40,
OP_DATA_41,
OP_DATA_42,
OP_DATA_43,
OP_DATA_44,
OP_DATA_45,
OP_DATA_46,
OP_DATA_47,
OP_DATA_48,
OP_DATA_49,
OP_DATA_50,
OP_DATA_51,
OP_DATA_52,
OP_DATA_53,
OP_DATA_54,
OP_DATA_55,
OP_DATA_56,
OP_DATA_57,
OP_DATA_58,
OP_DATA_59,
OP_DATA_60,
OP_DATA_61,
OP_DATA_62,
OP_DATA_63,
OP_DATA_64,
OP_DATA_65,
OP_DATA_66,
OP_DATA_67,
OP_DATA_68,
OP_DATA_69,
OP_DATA_70,
OP_DATA_71,
OP_DATA_72,
OP_DATA_73,
OP_DATA_74,
OP_DATA_75,
OP_PUSHDATA1, //next byte holds size of data
OP_PUSHDATA2, //next 2 bytes hold size of data
OP_PUSHDATA4, //next 4 bytes hold size of data
OP_1NEGATE,
OP_RESERVED, //transaction invalid unless in unexecuted OP_IF branch
OP_1,
OP_2,
OP_3,
OP_4,
OP_5,
OP_6,
OP_7,
OP_8,
OP_9,
OP_10,
OP_11,
OP_12,
OP_13,
OP_14,
OP_15,
OP_16,
OP_NOP,
OP_VER, //transaction invalid unless in unexecuted OP_IF branch
OP_IF,
OP_NOTIF,
OP_VERIF, //101 transaction invalid ALWAYS
OP_VERNOTIF, //102 transaction invalid ALWAYS
OP_ELSE,
OP_ENDIF,
OP_VERIFY,
OP_RETURN,
OP_TOALTSTACK, //to alt stack
OP_FROMALTSTACK,
OP_2DROP,
OP_2DUP,
OP_3DUP,
OP_2OVER,
OP_2ROT,
OP_2SWAP,
OP_IFDUP,
OP_DEPTH,
OP_DROP,
OP_DUP,
OP_NIP,
OP_OVER,
OP_PICK,
OP_ROLL,
OP_ROT,
OP_SWAP,
OP_TUCK,
OP_CAT, //disabled
OP_SUBSTR, //disabled
OP_LEFT, //disabled
OP_RIGHT, //disabled
OP_SIZE,
OP_INVERT,
OP_AND,
OP_OR,
OP_XOR,
OP_EQUAL,
OP_EQUALVERIFY,
OP_RESERVED1, //transaction invalid unless in unexecuted OP_IF branch
OP_RESERVED2, //transaction invalid unless in unexecuted OP_IF branch
OP_1ADD,
OP_1SUB,
OP_2MUL, //disabled
OP_2DIV, //disabled
OP_NEGATE,
OP_ABS,
OP_NOT,
OP_0NOTEQUAL,
OP_ADD,
OP_SUB,
OP_MUL, //disabled
OP_DIV, //disabled
OP_MOD, //disabled
OP_LSHIFT, //disabled
OP_RSHIFT, //disabled
OP_BOOLAND,
OP_BOOLOR,
OP_NUMEQUAL,
OP_NUMEQUALVERIFY,
OP_NUMNOTEQUAL,
OP_LESSTHAN,
OP_GREATERTHAN,
OP_LESSTHANOREQUAL,
OP_GREATERTHANOREQUAL,
OP_MIN,
OP_MAX,
OP_WITHIN,
OP_RIPEMD160,
OP_SHA1,
OP_SHA256,
OP_HASH160,
OP_HASH256,
OP_CODESEPARATOR,
OP_CHECKSIG,
OP_CHECKSIGVERIFY,
OP_CHECKMULTISIG,
OP_CHECKMULTISIGVERIFY,
OP_NOP1,
OP_CHECKLOCKTIMEVERIFY, //previously OP_NOP2
OP_CHECKSEQUENCEVERIFY, //previously OP_NOP3
OP_NOP4,
OP_NOP5,
OP_NOP6,
OP_NOP7,
OP_NOP8,
OP_NOP9,
OP_NOP10,
OP_PUBKEYHASH = 253U, //invalid if used in script
OP_PUBKEY = 254U, //invalid if used in script
OP_INVALIDOPCODE = 255U //invalid if used in script
};

const char* opCodesNames[256] = {
"OP_0",
"OP_DATA_1",
"OP_DATA_2",
"OP_DATA_3",
"OP_DATA_4",
"OP_DATA_5",
"OP_DATA_6",
"OP_DATA_7",
"OP_DATA_8",
"OP_DATA_9",
"OP_DATA_10",
"OP_DATA_11",
"OP_DATA_12",
"OP_DATA_13",
"OP_DATA_14",
"OP_DATA_15",
"OP_DATA_16",
"OP_DATA_17",
"OP_DATA_18",
"OP_DATA_19",
"OP_DATA_20",
"OP_DATA_21",
"OP_DATA_22",
"OP_DATA_23",
"OP_DATA_24",
"OP_DATA_25",
"OP_DATA_26",
"OP_DATA_27",
"OP_DATA_28",
"OP_DATA_29",
"OP_DATA_30",
"OP_DATA_31",
"OP_DATA_32",
"OP_DATA_33",
"OP_DATA_34",
"OP_DATA_35",
"OP_DATA_36",
"OP_DATA_37",
"OP_DATA_38",
"OP_DATA_39",
"OP_DATA_40",
"OP_DATA_41",
"OP_DATA_42",
"OP_DATA_43",
"OP_DATA_44",
"OP_DATA_45",
"OP_DATA_46",
"OP_DATA_47",
"OP_DATA_48",
"OP_DATA_49",
"OP_DATA_50",
"OP_DATA_51",
"OP_DATA_52",
"OP_DATA_53",
"OP_DATA_54",
"OP_DATA_55",
"OP_DATA_56",
"OP_DATA_57",
"OP_DATA_58",
"OP_DATA_59",
"OP_DATA_60",
"OP_DATA_61",
"OP_DATA_62",
"OP_DATA_63",
"OP_DATA_64",
"OP_DATA_65",
"OP_DATA_66",
"OP_DATA_67",
"OP_DATA_68",
"OP_DATA_69",
"OP_DATA_70",
"OP_DATA_71",
"OP_DATA_72",
"OP_DATA_73",
"OP_DATA_74",
"OP_DATA_75",
"OP_PUSHDATA1", //next byte holds size of data
"OP_PUSHDATA2", //next 2 bytes hold size of data
"OP_PUSHDATA4", //next 4 bytes hold size of data
"OP_1NEGATE",
"OP_RESERVED", //transaction invalid unless in unexecuted OP_IF branch
"OP_1",
"OP_2",
"OP_3",
"OP_4",
"OP_5",
"OP_6",
"OP_7",
"OP_8",
"OP_9",
"OP_10",
"OP_11",
"OP_12",
"OP_13",
"OP_14",
"OP_15",
"OP_16",
"OP_NOP",
"OP_VER", //transaction invalid unless in unexecuted OP_IF branch
"OP_IF",
"OP_NOTIF",
"OP_VERIF", //101 transaction invalid ALWAYS
"OP_VERNOTIF", //102 transaction invalid ALWAYS
"OP_ELSE",
"OP_ENDIF",
"OP_VERIFY",
"OP_RETURN",
"OP_TOALTSTACK", //to alt stack
"OP_FROMALTSTACK",
"OP_2DROP",
"OP_2DUP",
"OP_3DUP",
"OP_2OVER",
"OP_2ROT",
"OP_2SWAP",
"OP_IFDUP",
"OP_DEPTH",
"OP_DROP",
"OP_DUP",
"OP_NIP",
"OP_OVER",
"OP_PICK",
"OP_ROLL",
"OP_ROT",
"OP_SWAP",
"OP_TUCK",
"OP_CAT", //disabled
"OP_SUBSTR", //disabled
"OP_LEFT", //disabled
"OP_RIGHT", //disabled
"OP_SIZE",
"OP_INVERT",
"OP_AND",
"OP_OR",
"OP_XOR",
"OP_EQUAL",
"OP_EQUALVERIFY",
"OP_RESERVED1", //transaction invalid unless in unexecuted OP_IF branch
"OP_RESERVED2", //transaction invalid unless in unexecuted OP_IF branch
"OP_1ADD",
"OP_1SUB",
"OP_2MUL", //disabled
"OP_2DIV", //disabled
"OP_NEGATE",
"OP_ABS",
"OP_NOT",
"OP_0NOTEQUAL",
"OP_ADD",
"OP_SUB",
"OP_MUL", //disabled
"OP_DIV", //disabled
"OP_MOD", //disabled
"OP_LSHIFT", //disabled
"OP_RSHIFT", //disabled
"OP_BOOLAND",
"OP_BOOLOR",
"OP_NUMEQUAL",
"OP_NUMEQUALVERIFY",
"OP_NUMNOTEQUAL",
"OP_LESSTHAN",
"OP_GREATERTHAN",
"OP_LESSTHANOREQUAL",
"OP_GREATERTHANOREQUAL",
"OP_MIN",
"OP_MAX",
"OP_WITHIN",
"OP_RIPEMD160",
"OP_SHA1",
"OP_SHA256",
"OP_HASH160",
"OP_HASH256",
"OP_CODESEPARATOR",
"OP_CHECKSIG",
"OP_CHECKSIGVERIFY",
"OP_CHECKMULTISIG",
"OP_CHECKMULTISIGVERIFY",
"OP_NOP1",
"OP_CHECKLOCKTIMEVERIFY", //previously OP_NOP2
"OP_CHECKSEQUENCEVERIFY", //previously OP_NOP3
"OP_NOP4",
"OP_NOP5",
"OP_NOP6",
"OP_NOP7",
"OP_NOP8",
"OP_NOP9",
"OP_NOP10",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_INVALIDOPCODE",
"OP_PUBKEYHASH", //invalid if used in script
"OP_PUBKEY", //invalid if used in script
"OP_INVALIDOPCODE" //invalid if used in script
};

#endif // SCRIPT_OPCODE_H