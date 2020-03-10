#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/datastream.hpp>
#include <eosiolib/serialize.hpp>

namespace eosio {
   using eosio::asset;
   using eosio::symbol_type;

   typedef double real_type;

   /**
    *  Uses Bancor math to create a 50/50 relay between two asset types. The state of the
    *  bancor exchange is entirely contained within this struct. There are no external
    *  side effects associated with using this API.
    */

   // @abi table powermarket i64
   struct exchange_state {
      uint64_t id;
      std::string name;
      uint64_t vesting_seconds = 0;
      asset    supply;


      struct connector {
         asset balance;
         double weight = 1;
         account_name contract;
         
         std::string emission_function;
         uint64_t emission_percent;

         EOSLIB_SERIALIZE( connector, (balance)(weight)(contract)(emission_function)(emission_percent) )
      };

      connector base;
      connector quote;

      uint64_t primary_key()const { return id; }
      
      asset convert_to_exchange( connector& c, asset in ); 
      asset convert_from_exchange( connector& c, asset in );
      asset convert( asset from, symbol_type to );

      EOSLIB_SERIALIZE( exchange_state, (id)(name)(vesting_seconds)(supply)(base)(quote) )
   };

   typedef eosio::multi_index<N(powermarket), exchange_state> powermarket;

} /// namespace eosiosystem
