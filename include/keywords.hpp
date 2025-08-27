#pragma once

// ============================================================================
// PROTECTED FILE - AUTO-GENERATED - DO NOT MODIFY
// ============================================================================
// Auto-generated from DB25_SQL_GRAMMAR.ebnf
// This file is automatically regenerated when the grammar changes.
// 
// MODIFICATION RESTRICTION: Never edit manually. Use extract_keywords tool.
// To update: ./extract_keywords ../grammar/DB25_SQL_GRAMMAR.ebnf ../include/keywords.hpp
// ============================================================================

#include <string_view>
#include <array>
#include <cstdint>
#include <algorithm>

namespace db25 {

enum class Keyword : uint16_t {
    UNKNOWN = 0,
    AS = 1,
    BY = 2,
    DO = 3,
    IF = 4,
    IN = 5,
    IS = 6,
    NO = 7,
    OF = 8,
    ON = 9,
    OR = 10,
    TO = 11,
    ADD = 12,
    ALL = 13,
    AND = 14,
    ASC = 15,
    END = 16,
    FOR = 17,
    GIN = 18,
    INT = 19,
    KEY = 20,
    NOT = 21,
    ROW = 22,
    SET = 23,
    BLOB = 24,
    BOOL = 25,
    BRIN = 26,
    KW_CASE = 27,
    CAST = 28,
    CHAR = 29,
    CUBE = 30,
    DATA = 31,
    DATE = 32,
    DESC = 33,
    DROP = 34,
    EACH = 35,
    ELSE = 36,
    FROM = 37,
    FULL = 38,
    GIST = 39,
    HASH = 40,
    INTO = 41,
    JOIN = 42,
    JSON = 43,
    LAST = 44,
    LEFT = 45,
    LIKE = 46,
    NEXT = 47,
    KW_NULL = 48,
    ONLY = 49,
    OVER = 50,
    PLAN = 51,
    READ = 52,
    REAL = 53,
    ROWS = 54,
    SETS = 55,
    TEMP = 56,
    TEXT = 57,
    THEN = 58,
    TIES = 59,
    TIME = 60,
    KW_TRUE = 61,
    TYPE = 62,
    VIEW = 63,
    WHEN = 64,
    WITH = 65,
    WORK = 66,
    ZONE = 67,
    AFTER = 68,
    ALTER = 69,
    ARRAY = 70,
    BEGIN = 71,
    BTREE = 72,
    BYTEA = 73,
    CACHE = 74,
    CHAIN = 75,
    CHECK = 76,
    CROSS = 77,
    CYCLE = 78,
    DEPTH = 79,
    KW_FALSE = 80,
    FETCH = 81,
    FIRST = 82,
    FLOAT = 83,
    GROUP = 84,
    ILIKE = 85,
    INDEX = 86,
    INNER = 87,
    JSONB = 88,
    LEVEL = 89,
    LIMIT = 90,
    LOCAL = 91,
    NULLS = 92,
    ORDER = 93,
    OUTER = 94,
    OWNER = 95,
    PIVOT = 96,
    QUERY = 97,
    RANGE = 98,
    RIGHT = 99,
    START = 100,
    TABLE = 101,
    UNION = 102,
    USING = 103,
    WHERE = 104,
    WRITE = 105,
    ACTION = 106,
    ALWAYS = 107,
    ATTACH = 108,
    BEFORE = 109,
    BIGINT = 110,
    BINARY = 111,
    COLUMN = 112,
    COMMIT = 113,
    CREATE = 114,
    DELETE = 115,
    DETACH = 116,
    DOUBLE = 117,
    ESCAPE = 118,
    EXCEPT = 119,
    EXISTS = 120,
    FILTER = 121,
    GROUPS = 122,
    HAVING = 123,
    INSERT = 124,
    OFFSET = 125,
    OPTION = 126,
    OTHERS = 127,
    PRAGMA = 128,
    RENAME = 129,
    ROLLUP = 130,
    SCHEMA = 131,
    SEARCH = 132,
    SELECT = 133,
    SPGIST = 134,
    STORED = 135,
    UNIQUE = 136,
    UPDATE = 137,
    VACUUM = 138,
    VALUES = 139,
    WINDOW = 140,
    WITHIN = 141,
    ANALYZE = 142,
    BETWEEN = 143,
    BOOLEAN = 144,
    BREADTH = 145,
    CASCADE = 146,
    COLLATE = 147,
    CURRENT = 148,
    DECIMAL = 149,
    KW_DEFAULT = 150,
    EXCLUDE = 151,
    EXPLAIN = 152,
    EXTRACT = 153,
    FOREIGN = 154,
    INSTEAD = 155,
    INTEGER = 156,
    LATERAL = 157,
    NATURAL = 158,
    NOTHING = 159,
    NUMERIC = 160,
    PRIMARY = 161,
    REINDEX = 162,
    RELEASE = 163,
    REPLACE = 164,
    RESTART = 165,
    SESSION = 166,
    TRIGGER = 167,
    UNPIVOT = 168,
    VARCHAR = 169,
    VARYING = 170,
    VIRTUAL = 171,
    CASCADED = 172,
    CONFLICT = 173,
    DATABASE = 174,
    DISTINCT = 175,
    GROUPING = 176,
    INTERVAL = 177,
    MAXVALUE = 178,
    MINVALUE = 179,
    RESTRICT = 180,
    ROLLBACK = 181,
    SEQUENCE = 182,
    SMALLINT = 183,
    CHARACTER = 184,
    COMMITTED = 185,
    FOLLOWING = 186,
    GENERATED = 187,
    INCREMENT = 188,
    INTERSECT = 189,
    ISOLATION = 190,
    PARTITION = 191,
    PRECEDING = 192,
    PRECISION = 193,
    RECURSIVE = 194,
    RETURNING = 195,
    SAVEPOINT = 196,
    STATEMENT = 197,
    TEMPORARY = 198,
    TIMESTAMP = 199,
    UNBOUNDED = 200,
    CONSTRAINT = 201,
    DEFERRABLE = 202,
    REFERENCES = 203,
    REPEATABLE = 204,
    TRANSACTION = 205,
    UNCOMMITTED = 206,
    SERIALIZABLE = 207,
    AUTHORIZATION = 208
};

struct KeywordEntry {
    std::string_view text;
    uint8_t length;
    uint32_t hash;
    Keyword id;
    bool is_reserved;
};

inline constexpr std::array<KeywordEntry, 208> KEYWORDS = {{
    {"AS", 2, 0x3dd53c4d, Keyword::AS, false},
    {"BY", 2, 0x33dd7b54, Keyword::BY, true},
    {"DO", 2, 0x41ce86d4, Keyword::DO, false},
    {"IF", 2, 0x58e8ee86, Keyword::IF, false},
    {"IN", 2, 0x60e8fb1e, Keyword::IN, false},
    {"IS", 2, 0x6de90f95, Keyword::IS, false},
    {"NO", 2, 0x41e68bba, Keyword::NO, false},
    {"OF", 2, 0x88e4bce8, Keyword::OF, false},
    {"ON", 2, 0x80e4b050, Keyword::ON, false},
    {"OR", 2, 0x7ce4aa04, Keyword::OR, true},
    {"TO", 2, 0x21f563e4, Keyword::TO, false},
    {"ADD", 3, 0x7d7558d4, Keyword::ADD, false},
    {"ALL", 3, 0x55619224, Keyword::ALL, true},
    {"AND", 3, 0x91666dc6, Keyword::AND, true},
    {"ASC", 3, 0x64ad8a0a, Keyword::ASC, true},
    {"END", 3, 0xaf43920a, Keyword::END, true},
    {"FOR", 3, 0xefcd9ab0, Keyword::FOR, false},
    {"GIN", 3, 0xe1054809, Keyword::GIN, false},
    {"INT", 3, 0xd8c3957e, Keyword::INT, false},
    {"KEY", 3, 0xaccae4cc, Keyword::KEY, true},
    {"NOT", 3, 0xabee47aa, Keyword::NOT, true},
    {"ROW", 3, 0x86e8349b, Keyword::ROW, false},
    {"SET", 3, 0x490182e3, Keyword::SET, false},
    {"BLOB", 4, 0x4d76c3aa, Keyword::BLOB, false},
    {"BOOL", 4, 0xb24ea83d, Keyword::BOOL, false},
    {"BRIN", 4, 0x5636804e, Keyword::BRIN, false},
    {"CASE", 4, 0xd8776db1, Keyword::KW_CASE, true},
    {"CAST", 4, 0xe777854e, Keyword::CAST, false},
    {"CHAR", 4, 0xa59f665d, Keyword::CHAR, false},
    {"CUBE", 4, 0xa981d6e0, Keyword::CUBE, false},
    {"DATA", 4, 0xc407fae5, Keyword::DATA, false},
    {"DATE", 4, 0xc007f499, Keyword::DATE, false},
    {"DESC", 4, 0x6cbd1320, Keyword::DESC, true},
    {"DROP", 4, 0x52c16b0c, Keyword::DROP, true},
    {"EACH", 4, 0xfd96e368, Keyword::EACH, false},
    {"ELSE", 4, 0x69f37330, Keyword::ELSE, true},
    {"FROM", 4, 0x93beb475, Keyword::FROM, true},
    {"FULL", 4, 0xfccb837c, Keyword::FULL, true},
    {"GIST", 4, 0xda44865a, Keyword::GIST, false},
    {"HASH", 4, 0xcc18db11, Keyword::HASH, false},
    {"INTO", 4, 0x6ce3dc23, Keyword::INTO, false},
    {"JOIN", 4, 0x74b902f9, Keyword::JOIN, true},
    {"JSON", 4, 0xdfbd7fc3, Keyword::JSON, false},
    {"LAST", 4, 0xa1340d19, Keyword::LAST, false},
    {"LEFT", 4, 0x103c2070, Keyword::LEFT, true},
    {"LIKE", 4, 0x493a0436, Keyword::LIKE, false},
    {"NEXT", 4, 0x5fbd55e8, Keyword::NEXT, false},
    {"NULL", 4, 0x396fde24, Keyword::KW_NULL, true},
    {"ONLY", 4, 0x9af1ed37, Keyword::ONLY, false},
    {"OVER", 4, 0x2fe7860f, Keyword::OVER, false},
    {"PLAN", 4, 0xa185da12, Keyword::PLAN, false},
    {"READ", 4, 0xcc317405, Keyword::READ, false},
    {"REAL", 4, 0xd431809d, Keyword::REAL, false},
    {"ROWS", 4, 0x278b16d8, Keyword::ROWS, false},
    {"SETS", 4, 0x9d60bb10, Keyword::SETS, false},
    {"TEMP", 4, 0xc32287c7, Keyword::TEMP, false},
    {"TEXT", 4, 0x80ee447e, Keyword::TEXT, false},
    {"THEN", 4, 0xe78a4db6, Keyword::THEN, true},
    {"TIES", 4, 0x60d2e476, Keyword::TIES, false},
    {"TIME", 4, 0x22befb24, Keyword::TIME, false},
    {"TRUE", 4, 0x501b0925, Keyword::KW_TRUE, true},
    {"TYPE", 4, 0x142fe78d, Keyword::TYPE, false},
    {"VIEW", 4, 0x19968db8, Keyword::VIEW, true},
    {"WHEN", 4, 0x84f8b619, Keyword::WHEN, true},
    {"WITH", 4, 0xceb390e9, Keyword::WITH, false},
    {"WORK", 4, 0x5e01c9a0, Keyword::WORK, false},
    {"ZONE", 4, 0x9120d47, Keyword::ZONE, false},
    {"AFTER", 5, 0xa5309da9, Keyword::AFTER, false},
    {"ALTER", 5, 0xb4be5d0b, Keyword::ALTER, true},
    {"ARRAY", 5, 0x54b90286, Keyword::ARRAY, false},
    {"BEGIN", 5, 0x3f126d5e, Keyword::BEGIN, true},
    {"BTREE", 5, 0x24a13ed5, Keyword::BTREE, false},
    {"BYTEA", 5, 0xe8f6a7a, Keyword::BYTEA, false},
    {"CACHE", 5, 0x8ec6ec85, Keyword::CACHE, false},
    {"CHAIN", 5, 0xafadd76, Keyword::CHAIN, false},
    {"CHECK", 5, 0x1b3af527, Keyword::CHECK, false},
    {"CROSS", 5, 0xacd8ebb, Keyword::CROSS, true},
    {"CYCLE", 5, 0x3f9f1551, Keyword::CYCLE, false},
    {"DEPTH", 5, 0xce61558a, Keyword::DEPTH, false},
    {"FALSE", 5, 0xee597878, Keyword::KW_FALSE, true},
    {"FETCH", 5, 0x50d13a43, Keyword::FETCH, false},
    {"FIRST", 5, 0xf0eb79a1, Keyword::FIRST, false},
    {"FLOAT", 5, 0x8a18cfa5, Keyword::FLOAT, false},
    {"GROUP", 5, 0x822bfec, Keyword::GROUP, true},
    {"ILIKE", 5, 0x9e22bec7, Keyword::ILIKE, false},
    {"INDEX", 5, 0xb1744b0b, Keyword::INDEX, true},
    {"INNER", 5, 0x81480b87, Keyword::INNER, true},
    {"JSONB", 5, 0xb84fb813, Keyword::JSONB, false},
    {"LEVEL", 5, 0x7eecc6fd, Keyword::LEVEL, false},
    {"LIMIT", 5, 0x9aa4b214, Keyword::LIMIT, false},
    {"LOCAL", 5, 0x7d1bdd28, Keyword::LOCAL, false},
    {"NULLS", 5, 0xe21b3555, Keyword::NULLS, false},
    {"ORDER", 5, 0xdaf5e977, Keyword::ORDER, true},
    {"OUTER", 5, 0x455fd2d4, Keyword::OUTER, true},
    {"OWNER", 5, 0x9d318a74, Keyword::OWNER, false},
    {"PIVOT", 5, 0xb28f56b7, Keyword::PIVOT, false},
    {"QUERY", 5, 0x60984ec3, Keyword::QUERY, false},
    {"RANGE", 5, 0x6521e1b2, Keyword::RANGE, false},
    {"RIGHT", 5, 0x1b97d785, Keyword::RIGHT, true},
    {"START", 5, 0xd94a63f, Keyword::START, false},
    {"TABLE", 5, 0x14fcf13f, Keyword::TABLE, true},
    {"UNION", 5, 0x44481394, Keyword::UNION, true},
    {"USING", 5, 0xd23750a7, Keyword::USING, false},
    {"WHERE", 5, 0xcfcc69b8, Keyword::WHERE, true},
    {"WRITE", 5, 0x28c50c7c, Keyword::WRITE, false},
    {"ACTION", 6, 0x1c3ec9bf, Keyword::ACTION, false},
    {"ALWAYS", 6, 0x7c980fe4, Keyword::ALWAYS, false},
    {"ATTACH", 6, 0x19fb4b44, Keyword::ATTACH, false},
    {"BEFORE", 6, 0xcb2a7a3e, Keyword::BEFORE, false},
    {"BIGINT", 6, 0xee5998a, Keyword::BIGINT, false},
    {"BINARY", 6, 0x5eeb26fc, Keyword::BINARY, false},
    {"COLUMN", 6, 0x41e72127, Keyword::COLUMN, false},
    {"COMMIT", 6, 0xd0f73cbc, Keyword::COMMIT, true},
    {"CREATE", 6, 0xc19230dd, Keyword::CREATE, true},
    {"DELETE", 6, 0xf8718eca, Keyword::DELETE, true},
    {"DETACH", 6, 0x84c58f72, Keyword::DETACH, false},
    {"DOUBLE", 6, 0xdf980448, Keyword::DOUBLE, false},
    {"ESCAPE", 6, 0x5f8742c6, Keyword::ESCAPE, false},
    {"EXCEPT", 6, 0xa43935f0, Keyword::EXCEPT, true},
    {"EXISTS", 6, 0xe6aac0bd, Keyword::EXISTS, false},
    {"FILTER", 6, 0x11554e77, Keyword::FILTER, false},
    {"GROUPS", 6, 0x8db3d9ad, Keyword::GROUPS, false},
    {"HAVING", 6, 0xb9367e9c, Keyword::HAVING, true},
    {"INSERT", 6, 0xa4ed3768, Keyword::INSERT, true},
    {"OFFSET", 6, 0xcc049a0a, Keyword::OFFSET, false},
    {"OPTION", 6, 0x4ae76f14, Keyword::OPTION, false},
    {"OTHERS", 6, 0xc1ef0672, Keyword::OTHERS, false},
    {"PRAGMA", 6, 0xb19a29e5, Keyword::PRAGMA, false},
    {"RENAME", 6, 0x203bf453, Keyword::RENAME, false},
    {"ROLLUP", 6, 0x9c75ea53, Keyword::ROLLUP, false},
    {"SCHEMA", 6, 0x2df21036, Keyword::SCHEMA, false},
    {"SEARCH", 6, 0x36d11e29, Keyword::SEARCH, false},
    {"SELECT", 6, 0xb4293aad, Keyword::SELECT, true},
    {"SPGIST", 6, 0xbb3ae899, Keyword::SPGIST, false},
    {"STORED", 6, 0x9a16a4fe, Keyword::STORED, false},
    {"UNIQUE", 6, 0xdc0777a6, Keyword::UNIQUE, true},
    {"UPDATE", 6, 0x1c3385b4, Keyword::UPDATE, true},
    {"VACUUM", 6, 0x54e0212, Keyword::VACUUM, false},
    {"VALUES", 6, 0xd74d847b, Keyword::VALUES, false},
    {"WINDOW", 6, 0x714851dd, Keyword::WINDOW, false},
    {"WITHIN", 6, 0x9d242ea, Keyword::WITHIN, false},
    {"ANALYZE", 7, 0x6c873f3, Keyword::ANALYZE, false},
    {"BETWEEN", 7, 0xfd8646cf, Keyword::BETWEEN, false},
    {"BOOLEAN", 7, 0x880b7e9f, Keyword::BOOLEAN, false},
    {"BREADTH", 7, 0x81f37b51, Keyword::BREADTH, false},
    {"CASCADE", 7, 0x5d23ce75, Keyword::CASCADE, false},
    {"COLLATE", 7, 0x3c9dd295, Keyword::COLLATE, false},
    {"CURRENT", 7, 0xe321143a, Keyword::CURRENT, false},
    {"DECIMAL", 7, 0x48af9a2c, Keyword::DECIMAL, false},
    {"DEFAULT", 7, 0x7ce203fe, Keyword::KW_DEFAULT, false},
    {"EXCLUDE", 7, 0xd37153c9, Keyword::EXCLUDE, false},
    {"EXPLAIN", 7, 0x49de9fc, Keyword::EXPLAIN, false},
    {"EXTRACT", 7, 0xbc04de, Keyword::EXTRACT, false},
    {"FOREIGN", 7, 0xe01ba913, Keyword::FOREIGN, true},
    {"INSTEAD", 7, 0x99a78e51, Keyword::INSTEAD, false},
    {"INTEGER", 7, 0x8d1d3425, Keyword::INTEGER, false},
    {"LATERAL", 7, 0x4379557c, Keyword::LATERAL, false},
    {"NATURAL", 7, 0x94fcecf6, Keyword::NATURAL, false},
    {"NOTHING", 7, 0xcf511daa, Keyword::NOTHING, false},
    {"NUMERIC", 7, 0x98309408, Keyword::NUMERIC, false},
    {"PRIMARY", 7, 0x7ebe45b9, Keyword::PRIMARY, true},
    {"REINDEX", 7, 0xc55291ec, Keyword::REINDEX, false},
    {"RELEASE", 7, 0xb680a59e, Keyword::RELEASE, false},
    {"REPLACE", 7, 0x826d4ca3, Keyword::REPLACE, false},
    {"RESTART", 7, 0x519ab00c, Keyword::RESTART, false},
    {"SESSION", 7, 0xb4004d7, Keyword::SESSION, false},
    {"TRIGGER", 7, 0xfb910ae3, Keyword::TRIGGER, false},
    {"UNPIVOT", 7, 0xd4f4dfa2, Keyword::UNPIVOT, false},
    {"VARCHAR", 7, 0x787d3cd2, Keyword::VARCHAR, false},
    {"VARYING", 7, 0xe8db4e9b, Keyword::VARYING, false},
    {"VIRTUAL", 7, 0xbbef575c, Keyword::VIRTUAL, false},
    {"CASCADED", 8, 0xd05d9723, Keyword::CASCADED, false},
    {"CONFLICT", 8, 0xa9eba04f, Keyword::CONFLICT, false},
    {"DATABASE", 8, 0x55247b78, Keyword::DATABASE, false},
    {"DISTINCT", 8, 0x5df0578d, Keyword::DISTINCT, true},
    {"GROUPING", 8, 0xffe809ac, Keyword::GROUPING, false},
    {"INTERVAL", 8, 0x3a6cd678, Keyword::INTERVAL, false},
    {"MAXVALUE", 8, 0xe6f29226, Keyword::MAXVALUE, false},
    {"MINVALUE", 8, 0x384e748c, Keyword::MINVALUE, false},
    {"RESTRICT", 8, 0x511012e5, Keyword::RESTRICT, false},
    {"ROLLBACK", 8, 0xeb2ebdbd, Keyword::ROLLBACK, true},
    {"SEQUENCE", 8, 0xbbf885e8, Keyword::SEQUENCE, false},
    {"SMALLINT", 8, 0x87216b55, Keyword::SMALLINT, false},
    {"CHARACTER", 9, 0x394b87f0, Keyword::CHARACTER, false},
    {"COMMITTED", 9, 0xfb1fa339, Keyword::COMMITTED, false},
    {"FOLLOWING", 9, 0xb4c25af8, Keyword::FOLLOWING, false},
    {"GENERATED", 9, 0x3306b1a2, Keyword::GENERATED, false},
    {"INCREMENT", 9, 0xe4fb099e, Keyword::INCREMENT, false},
    {"INTERSECT", 9, 0x9de03e06, Keyword::INTERSECT, true},
    {"ISOLATION", 9, 0x942ae17, Keyword::ISOLATION, false},
    {"PARTITION", 9, 0x4c75ce69, Keyword::PARTITION, false},
    {"PRECEDING", 9, 0x47b0c492, Keyword::PRECEDING, false},
    {"PRECISION", 9, 0x9fc732f7, Keyword::PRECISION, false},
    {"RECURSIVE", 9, 0xce8af1d7, Keyword::RECURSIVE, false},
    {"RETURNING", 9, 0x592bb459, Keyword::RETURNING, false},
    {"SAVEPOINT", 9, 0xc6a3127e, Keyword::SAVEPOINT, false},
    {"STATEMENT", 9, 0xe7bcaef6, Keyword::STATEMENT, false},
    {"TEMPORARY", 9, 0xf245025c, Keyword::TEMPORARY, false},
    {"TIMESTAMP", 9, 0x3ec4ad43, Keyword::TIMESTAMP, false},
    {"UNBOUNDED", 9, 0x1be19cb, Keyword::UNBOUNDED, false},
    {"CONSTRAINT", 10, 0x995c895a, Keyword::CONSTRAINT, false},
    {"DEFERRABLE", 10, 0x43bc76b7, Keyword::DEFERRABLE, false},
    {"REFERENCES", 10, 0x5462484b, Keyword::REFERENCES, true},
    {"REPEATABLE", 10, 0xafeaaf2e, Keyword::REPEATABLE, false},
    {"TRANSACTION", 11, 0x37efb583, Keyword::TRANSACTION, true},
    {"UNCOMMITTED", 11, 0x128ff0b0, Keyword::UNCOMMITTED, false},
    {"SERIALIZABLE", 12, 0x5629dc12, Keyword::SERIALIZABLE, false},
    {"AUTHORIZATION", 13, 0xfa06895e, Keyword::AUTHORIZATION, false}
}};

// Length-based lookup tables for O(log n) search
struct LengthBucket {
    size_t start;
    size_t count;
};

inline constexpr std::array<LengthBucket, 12> LENGTH_BUCKETS = {{
    {0, 11},  // length 2
    {11, 12},  // length 3
    {23, 44},  // length 4
    {67, 38},  // length 5
    {105, 36},  // length 6
    {141, 30},  // length 7
    {171, 12},  // length 8
    {183, 17},  // length 9
    {200, 4},  // length 10
    {204, 2},  // length 11
    {206, 1},  // length 12
    {207, 1},  // length 13
}};

// Fast lookup function
[[nodiscard]] inline Keyword find_keyword(std::string_view text) noexcept {
    if (text.empty() || text.length() > 32) return Keyword::UNKNOWN;
    
    // Convert to uppercase for comparison
    char upper[32];
    for (size_t i = 0; i < text.length(); ++i) {
        upper[i] = (text[i] >= 'a' && text[i] <= 'z') ? 
                   (text[i] - 'a' + 'A') : text[i];
    }
    std::string_view upper_text(upper, text.length());
    
    // Binary search in the keyword table
    auto it = std::lower_bound(KEYWORDS.begin(), KEYWORDS.end(), upper_text,
        [](const KeywordEntry& entry, std::string_view val) {
            if (entry.length != val.length()) return entry.length < val.length();
            return entry.text < val;
        });
    
    if (it != KEYWORDS.end() && it->text == upper_text) {
        return it->id;
    }
    
    return Keyword::UNKNOWN;
}

// SIMD-optimized keyword matching
template<typename Processor>
[[nodiscard]] inline bool is_keyword_simd(Processor& proc,
                                          const std::byte* data,
                                          size_t size,
                                          Keyword& out_keyword) noexcept {
    if (size == 0 || size > 32) {
        out_keyword = Keyword::UNKNOWN;
        return false;
    }
    
    // Find the length bucket
    for (const auto& bucket : LENGTH_BUCKETS) {
        if (KEYWORDS[bucket.start].length == size) {
            // Check keywords in this bucket
            for (size_t i = 0; i < bucket.count; ++i) {
                const auto& entry = KEYWORDS[bucket.start + i];
                if (proc.matches_keyword(data, size, entry.text.data(), entry.length)) {
                    out_keyword = entry.id;
                    return true;
                }
            }
            break;
        }
        if (KEYWORDS[bucket.start].length > size) break;
    }
    
    out_keyword = Keyword::UNKNOWN;
    return false;
}

// Keyword name lookup
[[nodiscard]] inline std::string_view keyword_name(Keyword kw) noexcept {
    if (kw == Keyword::UNKNOWN) return "UNKNOWN";
    size_t idx = static_cast<size_t>(kw) - 1;
    if (idx < KEYWORDS.size()) {
        return KEYWORDS[idx].text;
    }
    return "INVALID";
}

}  // namespace db25
