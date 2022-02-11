// namespace eosio {
  #define QUARKS_IN_QUANTS 1000000
  #define ONE_PERCENT 10000
  #define HUNDR_PERCENT 1000000
  #define SYS_PERCENT 100000
  #define TOTAL_SEGMENTS 1000000000
  #define MAX_CORE_HISTORY_LENGTH 1000

  static constexpr eosio::symbol _SYM     = eosio::symbol(eosio::symbol_code("FLOWER"), 4);
  static constexpr eosio::symbol _POWER     = eosio::symbol(eosio::symbol_code("POWER"), 0);
  static constexpr eosio::symbol _USDT     = eosio::symbol(eosio::symbol_code("USDT"), 4);

  static constexpr uint64_t _DATA_ORDER_EXPIRATION = 86400;
  static constexpr uint64_t _SHARES_VESTING_DURATION = 604800;
  static constexpr uint64_t _TOTAL_VOTES = 7;
  static constexpr uint64_t _MAX_LEVELS = 26;
  static constexpr uint64_t _VOTING_TIME_EXPIRATION = 86400; //

  static constexpr eosio::name _me = "unicore"_n;
  static constexpr eosio::name _partners = "part"_n;
  static constexpr eosio::name _curator = "curator"_n;
  static constexpr eosio::name _registrator = "registrator"_n;
  static constexpr eosio::name _gateway = "gateway"_n;
  static constexpr eosio::name _saving = "eosio.saving"_n;
  static constexpr eosio::name _core_host = "core"_n;
  static constexpr eosio::name _p2p = "p2p"_n;
// }