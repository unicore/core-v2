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
      asset    supply;

      struct connector {
         asset balance;
         double weight = .5;

         EOSLIB_SERIALIZE( connector, (balance)(weight) )
      };

      connector base;
      connector quote;

      eosio::symbol primary_key()const { return supply.symbol; }

      asset convert_to_exchange( connector& c, asset in ); 
      asset convert_from_exchange( connector& c, asset in );
      asset convert( asset from, symbol to );

      EOSLIB_SERIALIZE( exchange_state, (supply)(base)(quote) )
   };

   typedef eosio::multi_index<"powermarket"_n, exchange_state> powermarket;

} /// namespace eosiosystem
