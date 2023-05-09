#pragma once

#include <algorithm>
#include <cmath>
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/time.hpp>
#include <eosio/multi_index.hpp>
#include <eosio/contract.hpp>
#include <eosio/action.hpp>
#include <eosio/system.hpp>
#include "eosio.token.hpp"
#include <eosio/print.hpp>
#include <eosio/datastream.hpp>


#include "hosts.hpp"
#include "ref.hpp"
#include "conditions.hpp"
#include "consts.hpp"

/**
\defgroup public_consts CONSTS
\brief Константы контракта
*/

/**
\defgroup public_actions ACTIONS
\brief Методы действий контракта
*/

/**
\defgroup public_tables TABLES
\brief Структуры таблиц контракта
*/

/**
\defgroup public_subcodes SUBCODES
\brief Субкоды входящих переводов
*/

/**
\defgroup public_modules MODULES
\brief Подмодули
*/

// namespace eosio {


class [[eosio::contract]] unicore2 : public eosio::contract {
    public:
        unicore2(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds ): eosio::contract(receiver, code, ds)
        {}
        void apply(uint64_t receiver, uint64_t code, uint64_t action);
        
        [[eosio::action]] void changemode(eosio::name host, eosio::name mode);

        [[eosio::action]] void setparams(eosio::name host, eosio::name chost, uint64_t size_of_pool,
            uint64_t quants_precision, uint64_t overlap, uint64_t profit_growth, uint64_t base_rate,
            uint64_t loss_percent, uint64_t compensator_percent, uint64_t pool_limit, uint64_t pool_timeout, uint64_t priority_seconds);

        [[eosio::action]] void start(eosio::name host, eosio::name chost); 
        [[eosio::action]] void withdraw(eosio::name username, eosio::name host, uint64_t balance_id);
        
        [[eosio::action]] void priorenter(eosio::name username, eosio::name host, uint64_t balance_id);
        [[eosio::action]] void refreshbal(eosio::name username, eosio::name host, uint64_t balance_id, uint64_t partrefresh);
        
        [[eosio::action]] void setstartdate(eosio::name host, eosio::time_point_sec start_at); 
        
        static void pay_for_upgrade(eosio::name username, eosio::asset amount, eosio::name code);
        
        static void refresh_state(eosio::name host);

        [[eosio::action]] void init(uint64_t system_income);

        [[eosio::action]] void refreshst(eosio::name username, eosio::name host);

        static void deposit ( eosio::name username, eosio::name host, eosio::asset amount, eosio::name code, std::string message );
        
        static void emit(eosio::name host, eosio::asset host_income, eosio::asset max_income);
        
        static void burn_action(eosio::name username, eosio::name host, eosio::asset quantity, eosio::name code, eosio::name status);

        static void next_pool( eosio::name host);
        //HOST
        [[eosio::action]] void setarch(eosio::name host, eosio::name architect);
        [[eosio::action]] void upgrade(eosio::name username, eosio::name platform, std::string title, std::string purpose, uint64_t total_shares, eosio::asset quote_amount, eosio::name quote_token_contract, eosio::asset root_token, eosio::name root_token_contract, bool voting_only_up, uint64_t consensus_percent, uint64_t referral_percent, uint64_t dacs_percent, uint64_t cfund_percent, uint64_t hfund_percent, std::vector<uint64_t> levels, uint64_t emission_percent, uint64_t gtop, std::string meta);
        [[eosio::action]] void cchildhost(eosio::name parent_host, eosio::name chost);
        [[eosio::action]] void edithost(eosio::name architect, eosio::name host, eosio::name platform, eosio::string title, eosio::string purpose, eosio::string manifest, eosio::name power_market_id, eosio::string meta);
        [[eosio::action]] void setlevels(eosio::name host, std::vector<uint64_t> levels);
        [[eosio::action]] void settiming(eosio::name host, uint64_t pool_timeout, uint64_t priority_seconds);
        [[eosio::action]] void setflows(eosio::name host, uint64_t ref_percent, uint64_t dacs_percent, uint64_t cfund_percent, uint64_t hfund_percent);

        //DACS
        [[eosio::action]] void adddac(eosio::name username, eosio::name host, uint64_t weight, eosio::name limit_type, eosio::asset income_limit, std::string title, std::string descriptor);
        [[eosio::action]] void rmdac(eosio::name username, eosio::name host);

        static void spread_to_dacs(eosio::name host, eosio::asset amount, eosio::name contract);

        static void spread_to_funds(eosio::name code, eosio::name host, eosio::asset quantity, std::string comment);

        static void spread_to_refs(eosio::name host, eosio::name username, eosio::asset spread_amount, eosio::asset from_amount, eosio::name token_contract);

        [[eosio::action]] void withdrdacinc(eosio::name username, eosio::name host);
        
        //CONDITIONS
        [[eosio::action]] void setcondition(eosio::name host, eosio::string key, uint64_t value);
        [[eosio::action]] void rmcondition(eosio::name host, uint64_t key); 
        static uint64_t getcondition(eosio::name host, eosio::string key);

        static void add_balance(eosio::name payer, eosio::asset quantity, eosio::name contract);   
        static void sub_balance(eosio::name username, eosio::asset quantity, eosio::name contract);
    

        std::string symbol_to_string(eosio::asset val) const;
        
        [[eosio::action]] void withrbenefit(eosio::name username, eosio::name host, uint64_t id);
        
        static void fill_pool(eosio::name host);
        static void fill_master_pool(eosio::name host, eosio::name username, eosio::asset amount);

        static uint64_t get_global_id(eosio::name key);

};

    
    /**
     * @brief      Таблица промежуточного хранения балансов пользователей.
     * @ingroup public_tables
     * @table balance
     * @contract _me
     * @scope username
     * @details Таблица баланса пользователя пополняется им путём совершения перевода на аккаунт контракта p2p. При создании ордера используется баланс пользователя из этой таблицы. Чтобы исключить необходимость пользователю контролировать свой баланс в контракте p2p, терминал доступа вызывает транзакцию с одновременно двумя действиями: перевод на аккаунт p2p и создание ордера на ту же сумму. 
     */

    struct [[eosio::table, eosio::contract("unicore2")]] ubalance {
        uint64_t id;                    /*!< идентификатор баланса */
        eosio::name contract;           /*!< имя контракта токена */
        eosio::asset quantity;          /*!< количество токенов на балансе */

        uint64_t primary_key() const {return id;} /*!< return id - primary_key */
        uint128_t byconsym() const {return combine_ids(contract.value, quantity.symbol.code().raw());} /*!< return combine_ids(contract, quantity) - комбинированный secondary_index 2 */

        EOSLIB_SERIALIZE(ubalance, (id)(contract)(quantity))
    };


    typedef eosio::multi_index<"ubalance"_n, ubalance,
    
      eosio::indexed_by< "byconsym"_n, eosio::const_mem_fun<ubalance, uint128_t, &ubalance::byconsym>>
    
    > ubalances_index;




/*!
   \brief Структура для хранения живой очереди приоритетных участников
*/
    struct [[eosio::table, eosio::contract("unicore")]] tail {
        uint64_t id;
        eosio::name username;
        eosio::time_point_sec enter_at;
        eosio::asset amount;
        uint64_t exit_pool_id;

        uint64_t primary_key() const {return id;}
        uint64_t byenter() const {return enter_at.sec_since_epoch();}
        uint64_t byamount() const {return amount.amount;}

        uint64_t byusername() const {return username.value;}


        EOSLIB_SERIALIZE(tail, (id)(username)(enter_at)(amount)(exit_pool_id));
    
    };

    
    typedef eosio::multi_index<"tail"_n, tail,
        eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<tail, uint64_t, &tail::byusername>>,
        eosio::indexed_by<"byexpr"_n, eosio::const_mem_fun<tail, uint64_t, &tail::byenter>>,
        eosio::indexed_by<"byamount"_n, eosio::const_mem_fun<tail, uint64_t, &tail::byamount>>
    > tail_index;
    



    /**
     * @brief      Таблица хранения счётчиков
     * @contract _me
     * @scope nft | _me
     * @table counts
     * @ingroup public_tables
     * @details Таблица хранит категории событий хоста. 
     */
    struct [[eosio::table, eosio::contract("unicore2")]] counts {
      eosio::name key;
      eosio::name secondary_key;
      uint64_t value;
      double   double_value;

      uint64_t primary_key() const {return key.value;}       /*!< return id - primary_key */
      uint128_t keyskey() const {return combine_ids(key.value, secondary_key.value);} /*!< (contract, blocked_now.symbol) - комбинированный secondary_key для получения курса по имени выходного контракта и символу */
      uint128_t keyvalue() const {return combine_ids(key.value, value);} /*!< (contract, blocked_now.symbol) - комбинированный secondary_key для получения курса по имени выходного контракта и символу */
      

      EOSLIB_SERIALIZE(counts, (key)(secondary_key)(value)(double_value))
    };

    typedef eosio::multi_index< "counts"_n, counts,
        eosio::indexed_by<"keyskey"_n, eosio::const_mem_fun<counts, uint128_t, &counts::keyskey>>,
        eosio::indexed_by<"keyvalue"_n, eosio::const_mem_fun<counts, uint128_t, &counts::keyvalue>>
    > counts_index;


// }



    struct [[eosio::table, eosio::contract("unicore2")]] spreads {
        uint64_t id;
        eosio::time_point_sec timestamp;
        std::string comment;
        eosio::asset total;
        eosio::asset ref_amount;
        eosio::asset dacs_amount;
        eosio::asset hfund_amount;
        eosio::asset cfund_amount;
        eosio::asset sys_amount;

        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(spreads, (id)(timestamp)(comment)(total)(ref_amount)(dacs_amount)(hfund_amount)(cfund_amount)(sys_amount))
    };

    typedef eosio::multi_index<"spreads"_n, spreads> spreads_index;


/*!
   \brief Структура системного процента, изымаего протокол из обращения при каждом полу-обороте Двойной Спирали каждого хоста.
*/

    struct [[eosio::table, eosio::contract("unicore2")]] gpercents {
        eosio::name key;
        uint64_t value;
        uint64_t primary_key() const {return key.value;}

        EOSLIB_SERIALIZE(gpercents, (key)(value))
    };

    typedef eosio::multi_index<"gpercents"_n, gpercents> gpercents_index;

/*!
   \brief Структура балансов пользователя у всех хостов Двойной Спирали
*/
    
    struct [[eosio::table, eosio::contract("unicore2")]] balance {
        uint64_t id;
        eosio::name owner;
        eosio::name host;
        eosio::name chost;
        eosio::name status = "process"_n;
        uint64_t cycle_num;
        uint64_t pool_num;
        uint64_t global_pool_id;
        uint64_t quants_for_sale;
        uint64_t next_quants_for_sale;
        uint64_t last_recalculated_win_pool_id = 1;
        bool win = false; //true if win, false if lose or nominal
        int64_t root_percent;
        int64_t convert_percent;
        std::string pool_color;
        eosio::asset available; 
        eosio::asset purchase_amount;
        eosio::asset compensator_amount;
        eosio::asset start_convert_amount;
        eosio::asset if_convert; 
        eosio::asset if_convert_to_power;
        
        eosio::asset ref_amount; 
        eosio::asset dac_amount;
        eosio::asset cfund_amount;
        eosio::asset hfund_amount;
        eosio::asset sys_amount;

        eosio::string meta; 

        uint64_t primary_key() const {return id;}
        uint64_t byowner() const {return owner.value;}
        uint64_t byavailable() const {return available.amount;}
        uint64_t bystatus() const {return status.value;}


        EOSLIB_SERIALIZE(balance, (id)(owner)(host)(chost)(status)(cycle_num)(pool_num)(global_pool_id)(quants_for_sale)(next_quants_for_sale)(last_recalculated_win_pool_id)(win)(root_percent)(convert_percent)(pool_color)(available)(purchase_amount)(compensator_amount)(start_convert_amount)(if_convert)(if_convert_to_power)(ref_amount)(dac_amount)(cfund_amount)(hfund_amount)(sys_amount)(meta))
    
        eosio::name get_ahost() const {
            if (host == chost)
                return host;
            else
                return chost;
        }
    };

    typedef eosio::multi_index<"balance"_n, balance,
        eosio::indexed_by<"byowner"_n, eosio::const_mem_fun<balance, uint64_t, &balance::byowner>>,
        eosio::indexed_by<"byavailable"_n, eosio::const_mem_fun<balance, uint64_t, &balance::byavailable>>,
        eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<balance, uint64_t, &balance::bystatus>>
    > balance_index;





/*!
   \brief Структура для учёта развития циклов хоста Двойной Спирали.
*/

    struct [[eosio::table, eosio::contract("unicore2")]] cycle {
        uint64_t id;
        eosio::name ahost;
        uint64_t start_at_global_pool_id;
        uint64_t finish_at_global_pool_id;
        eosio::asset emitted;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(cycle, (id)(ahost)(start_at_global_pool_id)(finish_at_global_pool_id)(emitted));
    };

    typedef eosio::multi_index<"cycle"_n, cycle> cycle_index;
    


/*!
   \brief Структура для учёта распределения бассейнов внутренней учетной единицы хоста Двойной Спирали.
*/
    struct [[eosio::table, eosio::contract("unicore2")]] pool {
        uint64_t id;
        eosio::name ahost;
        uint64_t cycle_num;
        uint64_t pool_num;
        std::string color;
        uint64_t total_quants; 
        uint64_t remain_quants;
        uint64_t creserved_quants; 
        eosio::asset filled;
        eosio::asset remain;
        uint64_t filled_percent;
        eosio::asset pool_cost;
        eosio::asset quant_cost;
        eosio::time_point_sec pool_started_at;
        eosio::time_point_sec priority_until;
        eosio::time_point_sec pool_expired_at;
        eosio::string meta;

        uint64_t primary_key() const {return id;}
        uint64_t bycycle() const {return cycle_num;}
        
        EOSLIB_SERIALIZE(pool,(id)(ahost)(cycle_num)(pool_num)(color)(total_quants)(remain_quants)(creserved_quants)(filled)(remain)(filled_percent)(pool_cost)(quant_cost)(pool_started_at)(priority_until)(pool_expired_at)(meta))
    };

    typedef eosio::multi_index<"pool"_n, pool> pool_index;
    


/*!
   \brief Структура для учёта распределения бассейнов внутренней учетной единицы хоста Двойной Спирали.
*/
    struct [[eosio::table, eosio::contract("unicore2")]] masterpool {
        uint64_t cycle_num;
        uint64_t current_pool_num;
        eosio::asset filled_balance; 
        eosio::asset remain_balance;

        uint64_t primary_key() const {return cycle_num;}
        
        EOSLIB_SERIALIZE(masterpool,(cycle_num)(current_pool_num)(filled_balance)(remain_balance))
    };

    typedef eosio::multi_index<"masterpool"_n, masterpool> masterpool_index;



/*!
   \brief Структура для учёта распределения бассейнов внутренней учетной единицы хоста Двойной Спирали.
*/
    struct [[eosio::table, eosio::contract("unicore2")]] masterbal {
        uint64_t id;
        eosio::name username;
        eosio::asset amount;

        uint64_t primary_key() const {return id;}
        
        EOSLIB_SERIALIZE(masterbal,(id)(username)(amount))
    };

    typedef eosio::multi_index<"masterbal"_n, masterbal> masterbal_index;
    
    

/*!
   \brief Структура хостов, платформ и их сайтов. 
*/

    struct  [[eosio::table, eosio::contract("unicore2")]] ahosts{
        eosio::name username;
        eosio::string website;
        eosio::checksum256 hash;
        bool is_host = false;
        eosio::name type;
        uint64_t votes;
        std::string title;
        std::string purpose;
        std::string manifest;
        bool comments_is_enabled = false;
        std::string meta;

        uint64_t primary_key() const{return username.value;}
        uint64_t byvotes() const {return votes;}

        eosio::checksum256 byuuid() const { return hash; }
        
        eosio::checksum256 hashit(std::string str) const
        {
            return eosio::sha256(const_cast<char*>(str.c_str()), str.size());
        }
          

        EOSLIB_SERIALIZE(ahosts, (username)(website)(hash)(is_host)(type)(votes)(title)(purpose)(manifest)(comments_is_enabled)(meta))
    };

    typedef eosio::multi_index<"ahosts"_n, ahosts,
        eosio::indexed_by<"byvotes"_n, eosio::const_mem_fun<ahosts, uint64_t, &ahosts::byvotes>>,
        eosio::indexed_by<"byuuid"_n, eosio::const_mem_fun<ahosts, eosio::checksum256, &ahosts::byuuid>>
    > ahosts_index;




