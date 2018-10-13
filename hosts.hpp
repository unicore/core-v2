#include <algorithm>
#include <cmath>
#include <eosiolib/transaction.hpp> 
#include <eosio.system/native.hpp>
#include <exchange_state.hpp>
#include <exchange_state.cpp>
#include <eosiolib/crypto.h>

namespace eosio {
	
    // @abi table account
    struct account{
        account_name username;
        std::string purpose;
        eosio::time_point_sec registered_at;
        bool activated;
        bool payed = false;
        account_name box_account;
        uint64_t primary_key()const { return username; }
        
        EOSLIB_SERIALIZE( account, (username)(purpose)(registered_at)(activated)(payed)(box_account))
    };

    typedef eosio::multi_index <N(account), account> account_index;
    

    // @abi action
    struct upgrade{
        account_name username;
        std::string purpose;
        EOSLIB_SERIALIZE( upgrade, (username)(purpose))
    };

    struct wait_weight {
	  uint32_t wait_sec;
	  weight_type weight;
	};

	struct authority {
	  uint32_t threshold;
	  vector<eosiosystem::key_weight> keys;
	  vector<eosiosystem::permission_level_weight> accounts;
	  vector<wait_weight> waits;
	};

	// eosiosystem::native::newaccount Doesn't seem to want to take authorities.
	struct call {
	  struct eosio {
	    void newaccount(account_name creator, account_name name,
	                    authority owner, authority active);
	  };
	};

	asset buyrambytes(uint32_t bytes) {
	  rammarket market(N(eosio), N(eosio));
	  auto itr = market.find(S(4,RAMCORE));
	  eosio_assert(itr != market.end(), "RAMCORE market not found");
	  
	  auto tmp = *itr;

	  return tmp.convert(asset(bytes, S(0, RAM)), CORE_SYMBOL);
}

}