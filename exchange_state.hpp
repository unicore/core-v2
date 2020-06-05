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
         
         EOSLIB_SERIALIZE( connector, (balance)(weight)(contract))
      };

      connector base;
      connector quote;

      uint64_t primary_key()const { return id; }
      
      asset convert_to_exchange( connector& reserve, const asset& payment );
      asset convert_from_exchange( connector& reserve, const asset& tokens );
      asset convert( const asset& from, const symbol_type& to );

      asset direct_convert( const asset& from, const symbol_type& to );

      static int64_t get_bancor_output( int64_t inp_reserve,
                                        int64_t out_reserve,
                                        int64_t inp );
      static int64_t get_bancor_input( int64_t out_reserve,
                                       int64_t inp_reserve,
                                       int64_t out );

      EOSLIB_SERIALIZE( exchange_state, (id)(name)(vesting_seconds)(supply)(base)(quote) )
   };

   typedef eosio::multi_index<N(powermarket), exchange_state> powermarket;

} /// namespace eosiosystem
