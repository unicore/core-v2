#define QUARKS_IN_QUANTS 1000000
#define ONE_PERCENT 10000
#define HUNDR_PERCENT 1000000
#define SYS_PERCENT 100000
#define TOTAL_SEGMENTS 1000000000

static constexpr eosio::symbol _SYM     = eosio::symbol(eosio::symbol_code("FLOWER"), 4);
static constexpr eosio::symbol _POWER     = eosio::symbol(eosio::symbol_code("POWER"), 0);

static constexpr uint64_t _MAX_LEVELS = 7;

static constexpr eosio::name _me = "unicore"_n;
static constexpr eosio::name _partners = "part"_n;
static constexpr eosio::name _saving = "eosio.saving"_n;

constexpr uint64_t DEPOSIT = 100;
constexpr uint64_t DEPOSIT_FOR_SOMEONE = 101;
constexpr uint64_t PAY_FOR_HOST = 110;
constexpr uint64_t BURN_QUANTS = 800;
constexpr uint64_t SPREAD_TO_REFS = 111;
constexpr uint64_t SPREAD_TO_FUNDS = 112;
constexpr uint64_t SPREAD_TO_DACS = 222;
constexpr uint64_t ADD_INTERNAL_BALANCE = 700;

